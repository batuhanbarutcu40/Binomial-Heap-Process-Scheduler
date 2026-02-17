Project Description:
  * This repository contains a C implementation of a CPU Process Scheduler using a Binomial Heap data structure. The project simulates a Round Robin-like scheduling mechanism but with a dynamic priority calculation.

Key Features:
  * Binomial Heap Implementation: Efficient priority queue management for processes.
  * Dynamic Priority: Uses a custom mathematical formula (including $e$ and power constants) to recalculate priorities after each time quantum.
  * Automated Optimization: Automatically tests different quantum values (1-10) to find the best Average Waiting Time (AWT).
