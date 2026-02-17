#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#define MAX_PROCESS_SAYISI 100

int esitMi = 0;
int *esitMiPtr = &esitMi; 

typedef struct
{
    int id;
    double varisZamani;
    double yurutmeSuresi;
    double kalanYurutmeSuresi;
    double priorityValue;
    int beklemeSuresi;
    int kuyrugaSonGirisZamani;
    int kuyrugaIlkGirisMi;
} Process;

typedef struct BHNode
{
    Process *veri;
    int degree;
    struct BHNode *child;
    struct BHNode *parent;
    struct BHNode *sibling;
} BHNode;

typedef struct BinomialHeap
{
    BHNode *head;
} BinomialHeap;

int oncelikliMi(Process *p1, Process *p2)  // returns 1 if p1 has higher priority than p2
{

    if (p1 && p2)
    {
        if (p1->priorityValue == p2->priorityValue)
        {
            *esitMiPtr = 1;
            if (p1->varisZamani < p2->varisZamani)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            *esitMiPtr = 0;
            if (p1->priorityValue < p2->priorityValue)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
    else
    {
        return 0;
    }
}

double priorityValueHesaplama(Process *p, double maxYurutmeSuresi) // returns the new priority value
{

    if (p->kuyrugaIlkGirisMi) 
    {
        return p->yurutmeSuresi; // if it's the first time in the queue , return execution time
    }
    else
    {
        double c_e_sabiti = 1.0 / (exp(-(pow((2.0 * p->kalanYurutmeSuresi) / (3.0 * maxYurutmeSuresi), 3))));  // ce constant
        return c_e_sabiti * p->kalanYurutmeSuresi; // new priority value
    }
}

BHNode *nodeInit(Process *p)
{
    BHNode *node = (BHNode *)malloc(sizeof(BHNode));
    if (node == NULL)
        return NULL;
    node->veri = p;
    node->degree = 0;
    node->child = NULL;
    node->parent = NULL;
    node->sibling = NULL;
    return node;
}

BinomialHeap *heapInit(void)
{
    BinomialHeap *heap;
    heap = (BinomialHeap *)malloc(sizeof(BinomialHeap));
    if (heap == NULL)
        return NULL;
    heap->head = NULL;
    return heap;
}

BHNode *heapMerge(BinomialHeap *heap1, BinomialHeap *heap2)
{
    BHNode *head;
    BHNode *tail;
    BHNode *h1It;
    BHNode *h2It;

    if (heap1->head == NULL)
        return heap2->head;
    if (heap2->head == NULL)
        return heap1->head;

    h1It = heap1->head;
    h2It = heap2->head;

    if (h1It->degree <= h2It->degree)
    {
        head = h1It;
        h1It = h1It->sibling;
    }
    else
    {
        head = h2It;
        h2It = h2It->sibling;
    }

    tail = head;

    while (h1It != NULL && h2It != NULL)
    {
        if (h1It->degree <= h2It->degree)
        {
            tail->sibling = h1It;
            h1It = h1It->sibling;
        }
        else
        {
            tail->sibling = h2It;
            h2It = h2It->sibling;
        }
        tail = tail->sibling;
    }
    tail->sibling = (h1It != NULL) ? h1It : h2It;
    return head;
}

BHNode *heapUnion(BinomialHeap *original, BinomialHeap *uni)
{

    BHNode *new_head;
    BHNode *prev;
    BHNode *aux;
    BHNode *next;

    new_head = heapMerge(original, uni);
    original->head = NULL;
    uni->head = NULL;

    if (new_head == NULL)
        return NULL;

    prev = NULL;
    aux = new_head;
    next = aux->sibling;

    while (next != NULL)
    {
        if (aux->degree != next->degree ||
            (next->sibling != NULL && next->sibling->degree == aux->degree))
        {
            prev = aux;
            aux = next;
        }
        else
        {
            if (oncelikliMi(aux->veri, next->veri))
            {
                aux->sibling = next->sibling;
                next->parent = aux;
                next->sibling = aux->child;
                aux->child = next;
                aux->degree++;
            }
            else
            {
                if (prev == NULL)
                    new_head = next;
                else
                    prev->sibling = next;
                aux->parent = next;
                aux->sibling = next->child;
                next->child = aux;
                next->degree++;
                aux = next;
            }
        }
        next = aux->sibling;
    }
    return new_head;
}

void heapInsert(BinomialHeap *heap, Process *p) 
{

    BinomialHeap *temp;
    BHNode *node;

    node = nodeInit(p);
    if (node == NULL)
        return;

    temp = heapInit();
    if (temp == NULL)
        return;

    temp->head = node;
    heap->head = heapUnion(heap, temp);
    free(temp);
}

void heapRemove(BinomialHeap *heap, BHNode *node, BHNode *before)
{
    BinomialHeap *temp;
    BHNode *child;
    BHNode *new_head;
    BHNode *next;

    if (node == heap->head)
        heap->head = node->sibling;
    else if (before != NULL)
        before->sibling = node->sibling;

    new_head = NULL;
    child = node->child;

    while (child != NULL)
    {
        next = child->sibling;
        child->sibling = new_head;
        child->parent = NULL;
        new_head = child;
        child = next;
    }

    temp = heapInit();
    if (temp == NULL)
        return;

    temp->head = new_head;
    heap->head = heapUnion(heap, temp);
    free(temp);
}

Process *heapMin(BinomialHeap *heap)
{

    BHNode *min;
    BHNode *min_prev;
    BHNode *curr;
    BHNode *prev;

    if (heap->head == NULL)
        return NULL;

    min = heap->head;
    min_prev = NULL;
    prev = min;
    curr = min->sibling;

    while (curr != NULL)
    {
        if (oncelikliMi(curr->veri, min->veri))
        {
            min = curr;
            min_prev = prev;
        }
        prev = curr;
        curr = curr->sibling;
    }

    heapRemove(heap, min, min_prev);
    Process *donenGorev = min->veri;
    free(min);
    return donenGorev;
}

void kuyruktakiDugumuYazdir(BHNode *node) // prints the node and its children and siblings recursively
{
    if (node == NULL) 
    {
        return; // if node is NULL, return recursively
    }
    else
    {
        if (*esitMiPtr) 
        {
            printf("P%d[%.3f] ", node->veri->id, node->veri->varisZamani); // if equal , print arrival time
        }
        else
        {
            printf("P%d[%.3f] ", node->veri->id, node->veri->priorityValue); // if not equal , print priority value
        }

        kuyruktakiDugumuYazdir(node->child); // print children recursively
        kuyruktakiDugumuYazdir(node->sibling); // print siblings recursively
    }
}

void tumKuyruguYazdir(BinomialHeap *heap) // prints the entire heap
{
    if (heap->head == NULL) 
    {
        printf("Empty"); // if heap is empty , print empty
        return;
    }else{
        kuyruktakiDugumuYazdir(heap->head); // print from head
    }
    
}

double mekanizmayiBaslat(int q, Process inputProcesses[], int toplamIsSayisi, double maxYurutmeSuresi, int yazdirsinMi) // starts the mechanism and returns average waiting time
{
    Process temporaryProcessesArray[MAX_PROCESS_SAYISI]; // create a temporary array to avoid modifying the original input
    for (int i = 0; i < toplamIsSayisi; i++) // copy input processes to temporary array
    {
        temporaryProcessesArray[i] = inputProcesses[i];
        temporaryProcessesArray[i].kalanYurutmeSuresi = temporaryProcessesArray[i].yurutmeSuresi;
        temporaryProcessesArray[i].beklemeSuresi = 0;
        temporaryProcessesArray[i].kuyrugaIlkGirisMi = 1;
        temporaryProcessesArray[i].priorityValue = temporaryProcessesArray[i].yurutmeSuresi;
    }

    BinomialHeap *bh = heapInit(); // initialize binomial heap
    int suankiZaman = 0; 
    int tamamlananProcessSayisi = 0;
    Process *suandaCPUdakiProcess = NULL;
    int zamanDilimiSayaci = 0;

    if (yazdirsinMi)
    {
        printf("Selected quantum value (q) = %d\n", q);
        printf("Time\tCPU\t\t  Binomial Heap Contents\n");
    }

    while (tamamlananProcessSayisi < toplamIsSayisi) // until all processes are completed
    {
        for (int i = 0; i < toplamIsSayisi; i++) // Enqueue newly arrived processes
        {
            if (temporaryProcessesArray[i].varisZamani == suankiZaman) // newly arrived process
            {
                temporaryProcessesArray[i].kuyrugaSonGirisZamani = suankiZaman; // queue entry time
                temporaryProcessesArray[i].kuyrugaIlkGirisMi = 1; // first entry
                temporaryProcessesArray[i].priorityValue = temporaryProcessesArray[i].yurutmeSuresi; // priority value assignment
                heapInsert(bh, &temporaryProcessesArray[i]); // Enqueue to queue
            }
        }

        if (suandaCPUdakiProcess == NULL && bh->head != NULL) // If the CPU is idle and there is a process in the queue
        {
            suandaCPUdakiProcess = heapMin(bh); // Get the most prioritized process from the queue
            suandaCPUdakiProcess->beklemeSuresi += (suankiZaman - suandaCPUdakiProcess->kuyrugaSonGirisZamani); // Update waiting time
            suandaCPUdakiProcess->kuyrugaIlkGirisMi = 0; // Not the first entry
            zamanDilimiSayaci = 0; // Reset time slice counter
        }

        if (yazdirsinMi) // print or not
        {
            printf("%d\t", suankiZaman); // print time
            if (suandaCPUdakiProcess) // if there is a process in the CPU
            {
                if (*esitMiPtr) // if equal
                    printf("P%d[%.3f] \t  ", suandaCPUdakiProcess->id, suandaCPUdakiProcess->varisZamani); // if equal, print arrival time
                else // if not equal
                    printf("P%d[%.3f] \t  ", suandaCPUdakiProcess->id, suandaCPUdakiProcess->priorityValue); // if not equal, print priority value
            }
            else // if CPU is idle
            {
                printf("CPU bosta\t\t");
            }
            tumKuyruguYazdir(bh); // Print queue contents
            printf("\n");
        }

        if (suandaCPUdakiProcess != NULL) // if there is a process in the CPU
        {
            suandaCPUdakiProcess->kalanYurutmeSuresi--; // Decrease the remaining execution time of the process
            zamanDilimiSayaci++; // Increase the time slice counter

            if (suandaCPUdakiProcess->kalanYurutmeSuresi == 0) // if the process is completed
            {
                tamamlananProcessSayisi++; // Increase the completed process count
                suandaCPUdakiProcess = NULL; // Clear the CPU
                zamanDilimiSayaci = 0; // Reset the time slice counter
            }

            else if (zamanDilimiSayaci == q) // If the time slice expires
            {

                suandaCPUdakiProcess->priorityValue = priorityValueHesaplama(suandaCPUdakiProcess, maxYurutmeSuresi); // update priority value

                suandaCPUdakiProcess->kuyrugaSonGirisZamani = suankiZaman + 1; // Update queue entry time
                heapInsert(bh, suandaCPUdakiProcess); // Re-enqueue the process

                suandaCPUdakiProcess = NULL; // Clear the CPU
                zamanDilimiSayaci = 0; // Reset the time slice counter
            }
        }
        suankiZaman++; // Increase the time
    }

    double totalWT = 0;
    if (yazdirsinMi){
        printf("\nProcess Waiting Times:\n");
    }
        
    for (int i = 0; i < toplamIsSayisi; i++)
    {
        totalWT += temporaryProcessesArray[i].beklemeSuresi; // calculate total waiting time
        if (yazdirsinMi)
            printf("P%d: %d\n", temporaryProcessesArray[i].id, temporaryProcessesArray[i].beklemeSuresi); // Print each process's waiting time
    }

    double awt = totalWT / toplamIsSayisi; // calculate average waiting time
    if (yazdirsinMi) 
        printf("Average Waiting Time (AWT): %.2f\n", awt); // Print average waiting time

    free(bh); // Free memory
    return awt; // Return average waiting time
}

int main()
{
    Process input[MAX_PROCESS_SAYISI];
    int toplamIsSayisi = 0;
    double maxYurutmeSuresi = 0;

    // read input from file

    FILE *file = fopen("input.txt", "r");
    if (file == NULL)
    {
        printf("HATA: 'input.txt' dosyasi bulunamadi!\n");
        return 1;
    }

    int yurutmeSuresiGirdisi, varisZamaniGirdisi;
    while (fscanf(file, "%d,%d", &yurutmeSuresiGirdisi, &varisZamaniGirdisi) == 2 && toplamIsSayisi < MAX_PROCESS_SAYISI)
    {
        input[toplamIsSayisi].id = toplamIsSayisi + 1;
        input[toplamIsSayisi].yurutmeSuresi = yurutmeSuresiGirdisi;
        input[toplamIsSayisi].varisZamani = varisZamaniGirdisi;
        input[toplamIsSayisi].kalanYurutmeSuresi = yurutmeSuresiGirdisi;
        input[toplamIsSayisi].beklemeSuresi = 0;
        input[toplamIsSayisi].kuyrugaIlkGirisMi = 1;

        if (yurutmeSuresiGirdisi > maxYurutmeSuresi)
            maxYurutmeSuresi = yurutmeSuresiGirdisi;
        toplamIsSayisi++;
    }
    fclose(file);

    if (toplamIsSayisi == 0)
    {
        printf("Dosya bos.\n");
        return 1;
    }


    // calculate best quantum value and minimum AWT

    double minAWT = 999999.0;
    int bestQuantumValue = -1;

    for (int q = 1; q <= 10; q++) // try quantum values from 1 to 10
    {
        double awt = mekanizmayiBaslat(q, input, toplamIsSayisi, maxYurutmeSuresi, 0); // don't print
        if (awt < minAWT)
        {
            minAWT = awt;
            bestQuantumValue = q;
        }
    }

    mekanizmayiBaslat(3, input, toplamIsSayisi, maxYurutmeSuresi, 1); // print with best quantum value

    return 0;
}