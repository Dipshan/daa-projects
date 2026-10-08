# CS456 Design and Analysis of Algorithms - Project Portfolio

**Author:** Deepshan Adhikari (800846035)  
**Email:** deepadh@siue.edu  
**Course:** CS456 Design and Analysis of Algorithms  
**Institution:** Southern Illinois University Edwardsville (SIUE)

## Table of Contents

| #   | Section                                                
| :-- | :---------------------------------------------------------------
| 1   | [Overview](#overview)
| 2   | [Repository Structure](#repository-structure)
| 3   | [Project 1: Interval Scheduling](#project-1-interval-scheduling)
| 4   | [Project 2: Enhancing Quicksort](#project-2-enhancing-quicksort)
| 5   | [Project 3: Counting Inversions](#project-3-counting-inversions)
| 6   | [Project 4: Network Flow](#project-4-network-flow)
| 7   | [References](#references)
| 8   | [License](#license)



## Overview

This repository contains four projects completed for CS456 Design and Analysis of Algorithms. Each project explores a fundamental algorithm design paradigm through implementation, experimentation, and analysis.

| Project | Topic               | Paradigm                                 |
| :-----: | :------------------ | :--------------------------------------- |
|    1    | Interval Scheduling | Greedy Algorithms                        |
|    2    | Enhancing Quicksort | Divide & Conquer / Randomized Algorithms |
|    3    | Counting Inversions | Divide & Conquer / Sorting               |
|    4    | Network Flow        | Network Flow / Optimization              |


## Repository Structure

```
├── project1_intervalScheduling.cpp                           # Project 1: Interval Scheduling
├── project2_enhancingQuicksort.cpp                           # Project 2: Enhancing Quicksort
├── project3_countingInversions.cpp                           # Project 3: Counting Inversions
├── Project1_Interval_Scheduling_Deepshan_Adhikari.pdf        # Project 1 Report
├── Project2_Enhancing_Quicksort_Deepshan_Adhikari.pdf        # Project 2 Report
├── Project3_Counting_Inversions_Deepshan_Adhikari.pdf        # Project 3 Report
├── Project4_Network_Flow_Report_Deepshan_Adhikari.pdf        # Project 4 Report
└── README.md
```

## Project 1: Interval Scheduling

**File:** `project1_intervalScheduling.cpp`  
**Report:** `Project1_Interval_Scheduling_Deepshan_Adhikari.pdf`

### Description

Compares four different interval scheduling strategies by running 1000 trials with 1000 random jobs each. The program generates random intervals (jobs) and evaluates how many jobs each strategy can schedule without overlaps.

### Strategies Implemented

|  #  | Strategy                      | Optimal | Description                                                                                                                                                         |
| :-: | :---------------------------- | :-----: | :------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
|  1  | Earliest Finish Time (EFT)    |   Yes   | Sorts jobs by finish time and greedily selects non-overlapping jobs. Always yields the maximum number of scheduled jobs (Theorem 4.1, Kleinberg & Tardos).          |
|  2  | Shortest Duration First (SDF) |   No    | Sorts jobs by duration (shortest first). May fail because a short job can block two longer non-overlapping jobs.                                                    |
|  3  | Earliest Start Time (EST)     |   No    | Sorts jobs by start time (earliest first). May fail because an early-starting job may finish late and block many others.                                            |
|  4  | Fewest Conflicts First (FCF)  |   No    | Counts how many other jobs overlap with each job. Sorts by fewest conflicts first. May fail because low-conflict jobs are not guaranteed to be mutually compatible. |

### Key Results

| Strategy                      | High |  Mean  | Low |
| :---------------------------- | :--: | :----: | :-: |
| Earliest Finish Time (EFT)    | 115  | 100.86 | 88  |
| Shortest Duration First (SDF) | 113  | 98.78  | 86  |
| Earliest Start Time (EST)     |  49  | 36.58  | 28  |
| Fewest Conflicts First (FCF)  | 115  | 99.47  | 86  |

**Observations:**

- EFT performed best with a mean of 100.86, confirming its optimality guarantee.
- EST was the weakest by a large margin, averaging only 36.58 jobs.
- SDF and FCF came within about 1–2 jobs of EFT on random inputs, showing they are strong heuristics despite lacking optimality proofs.
- Run 113 showed the greatest performance gap: EFT = 115, SDF = 113, EST = 36, FCF = 114, gap of 79 jobs.

### Counterexample (EST Not Optimal)

Consider three jobs:

| Job | Start | Finish | Duration |
| :-: | :---: | :----: | :------: |
|  A  |   1   |  100   |    99    |
|  B  |   2   |   5    |    3     |
|  C  |   6   |   9    |    3     |

- EST picks Job A first (starts at time 1), blocking both B and C, resulting in only 1 job.
- EFT picks Job B first (finishes at time 5), then Job C (starts at time 6), resulting in 2 jobs (optimal).

This demonstrates that EST fails because an early-starting job can have a very late finish time, blocking other jobs that could have been scheduled.

### Why Correctness Proofs Matter?

All four strategies sound reasonable on paper, and on random data three of the four performed close to each other. But only EFT comes with a guarantee that holds for every possible input. The other three can still fail badly on certain inputs, as shown in the counterexample above. Without proof, there is no way to know ahead of time which strategy is safe to depend on.

### How to Compile & Run

```bash
g++ -std=c++17 -o intervalScheduling intervalScheduling.cpp
./intervalScheduling
```

---

## Project 2: Enhancing Quicksort

**File:** `project2_enhancingQuicksort.cpp`  
**Report:** `Project2_Enhancing_Quicksort_Deepshan_Adhikari.pdf`

### Description

Benchmarks four pivot selection strategies for Quicksort across five array sizes (10k, 20k, 50k, 100k, 200k) on two input types: random values in [1, 1,000,000] and sorted values 1..n. Each combination is timed 10 times and the average is reported.

### Strategies Implemented

|  #  | Strategy          | Pivot Choice                 |         Worst Case          |
| :-: | :---------------- | :--------------------------- | :-------------------------: |
|  1  | Default Quicksort | Last element                 |           O(n^2)            |
|  2  | Center Half Pivot | Random pivot in middle half  |     O(n log n) expected     |
|  3  | Random Pivot      | Any random element           | O(n^2) with low probability |
|  4  | Quickselect Pivot | Exact median via Quickselect |     O(n log n) expected     |

### Theoretical Time Complexity

| Strategy          | Best Case  | Average Case |         Worst Case          |
| :---------------- | :--------: | :----------: | :-------------------------: |
| Default Quicksort | O(n log n) |  O(n log n)  |           O(n^2)            |
| Center Half Pivot | O(n log n) |  O(n log n)  |     O(n log n) expected     |
| Random Pivot      | O(n log n) |  O(n log n)  | O(n^2) with low probability |
| Quickselect Pivot | O(n log n) |  O(n log n)  |     O(n log n) expected     |

### Key Results

Average running time (ms) over 10 runs:

| Array Size | Default (rand/sorted) | Center Half (rand/sorted) | Random Pivot (rand/sorted) | Quickselect (rand/sorted) |
| :--------: | :-------------------: | :-----------------------: | :------------------------: | :-----------------------: |
|   10,000   |      2.9 / 193.4      |         3.1 / 2.0         |         1.5 / 1.2          |         3.4 / 2.9         |
|   20,000   |      2.2 / 773.5      |         6.6 / 4.0         |         3.0 / 2.3          |         6.1 / 5.3         |
|   50,000   |     5.3 / 4924.2      |        18.0 / 9.7         |         7.6 / 5.6          |        17.5 / 14.5        |
|  100,000   |    11.3 / 20183.2     |        43.3 / 21.1        |        16.1 / 11.8         |        36.6 / 31.5        |
|  200,000   | 26.0 / Stack Overflow |       110.4 / 42.0        |        33.5 / 24.1         |        76.2 / 65.5        |

### Key Findings

|  #  | Finding                                                                                                                                                                                                                                                                                                     |
| :-: | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|  1  | On random input, all four strategies perform about the same. Default and Random Pivot are slightly faster; Center Half and Quickselect have extra overhead.                                                                                                                                                 |
|  2  | On sorted input, Default Quicksort slows to O(n^2) and becomes thousands of times slower as n grows. At 200,000 elements, recursion depth exceeded the OS stack limit, causing a stack overflow.                                                                                                            |
|  3  | Center Half, Random Pivot, and Quickselect all avoid the worst case and stay fast on both input types.                                                                                                                                                                                                      |
|  4  | Surprising observation: Center Half Pivot was consistently faster on sorted input than on random input at every size (e.g., 42.0 ms vs. 110.4 ms at 200,000). On sorted input, almost any random element is already in the middle half, so the validation loop succeeds on the first try almost every time. |
|  5  | Recommendation: Random Pivot Quicksort is the best choice for a production sorting library. It is fast on random input, robust on sorted input, and simple to implement.                                                                                                                                    |

### How to Compile & Run

```bash
g++ -std=c++17 -o enhancingQuicksort enhancingQuicksort.cpp
./enhancingQuicksort
```

> **Note:** This program uses POSIX system calls (`fork`, `waitpid`, `_exit`), so it requires a Unix-like environment (Linux, macOS, WSL).

---

## Project 3: Counting Inversions

**File:** `project3_countingInversions.cpp`  
**Report:** `Project3_Counting_Inversions_Deepshan_Adhikari.pdf`

### Description

Implements and compares three algorithms for counting inversions and measures how inversion count relates to Insertion Sort runtime.

An inversion is a pair (i, j) such that i < j and A[i] > A[j]. Inversions measure how far an array is from being sorted:

| Array Type     |  Inversion Count   |
| :------------- | :----------------: |
| Sorted         |         0          |
| Random         |       ~n^2/4       |
| Reverse-sorted | n(n-1)/2 (maximum) |

### Algorithms Implemented

|  #  | Algorithm                | Time Complexity | Description                                                                                               |
| :-: | :----------------------- | :-------------: | :-------------------------------------------------------------------------------------------------------- |
|  1  | Naive Inversion Counting |     O(n^2)      | Checks every pair (i, j) with i < j and A[i] > A[j]. Used to verify the divide-and-conquer count.         |
|  2  | Divide-and-Conquer Count |   O(n log n)    | Modified MergeSort from Kleinberg & Tardos, Section 5.3. Counts cross inversions during the merge step.   |
|  3  | Insertion Sort           |    O(n + I)     | Each shift removes exactly one inversion. Total running time is O(n + I), where I is the inversion count. |

### Time Complexity

| Algorithm                   | Best Case  | Average Case | Worst Case |
| :-------------------------- | :--------: | :----------: | :--------: |
| Naive Inversion Counting    |   O(n^2)   |    O(n^2)    |   O(n^2)   |
| Divide-and-Conquer Counting | O(n log n) |  O(n log n)  | O(n log n) |
| Insertion Sort              |    O(n)    |    O(n^2)    |   O(n^2)   |

### Experiment 1: Inversions and Sort Time Across All Dataset Types and Sizes

### Results (average of 10 trials):

| Array Size | Dataset Type   | Inversion Count | Average Insertion Sort Time (ms) |
| :--------: | :------------- | --------------: | -------------------------------: |
|   10,000   | Sorted         |               0 |                           0.0050 |
|   10,000   | Random         |      24,975,835 |                           6.0751 |
|   10,000   | Reverse Sorted |      49,995,000 |                          12.1160 |
|   20,000   | Sorted         |               0 |                           0.0063 |
|   20,000   | Random         |      99,743,900 |                          24.2566 |
|   20,000   | Reverse Sorted |     199,990,000 |                          48.5599 |
|   50,000   | Sorted         |               0 |                           0.0157 |
|   50,000   | Random         |     624,115,765 |                         153.3427 |
|   50,000   | Reverse Sorted |   1,249,975,000 |                         309.4103 |
|  100,000   | Sorted         |               0 |                           0.0320 |
|  100,000   | Random         |   2,497,316,064 |                         623.6994 |
|  100,000   | Reverse Sorted |   4,999,950,000 |                       1,242.8737 |
|  200,000   | Sorted         |               0 |                           0.0644 |
|  200,000   | Random         |  10,001,760,621 |                       2,576.7234 |
|  200,000   | Reverse Sorted |  19,999,900,000 |                       4,975.7102 |

### Observations:

|  #  | Observation                                                                                                                                             |
| :-: | :------------------------------------------------------------------------------------------------------------------------------------------------------ |
|  1  | Sorted arrays have 0 inversions and sort almost instantly (0.0050 ms to 0.0644 ms as size grows from 10k to 200k).                                      |
|  2  | Random arrays have ~n^2/4 inversions; reverse-sorted arrays have the maximum n(n-1)/2.                                                                  |
|  3  | Reverse Sorted is always about twice as slow as Random, consistent with having about twice as many inversions.                                          |
|  4  | At n = 200,000, Sorted took 0.0644 ms while Reverse Sorted took 4,975.7102 ms, about 77,000 times slower, even though both arrays have the same length. |
|  5  | Sorted times roughly double when size doubles, matching O(n) behavior.                                                                                  |
|  6  | Random and Reverse Sorted times roughly quadruple when size doubles, matching O(n^2) behavior.                                                          |

### Experiment 2: Effect of Increasing Disorder on Sort Time (n = 100,000)

Results (average of 10 trials):

| Swap % | Inversion Count | Average Insertion Sort Time (ms) |
| :----: | --------------: | -------------------------------: |
|   1%   |      65,763,636 |                          17.0898 |
|   5%   |     308,973,391 |                          78.2087 |
|  10%   |     579,443,814 |                         167.3136 |
|  25%   |   1,180,900,301 |                         303.9001 |
|  50%   |   1,771,633,117 |                         447.6027 |

### Observations:

|  #  | Observation                                                                                                                                     |
| :-: | :---------------------------------------------------------------------------------------------------------------------------------------------- |
|  1  | At 1% swaps (1,000 random swaps), the array had about 65.8 million inversions and took 17.0898 ms.                                              |
|  2  | At 50% swaps (50,000 swaps), the array had about 1.77 billion inversions and took 447.6027 ms.                                                  |
|  3  | Even at 50%, the inversion count is well below the maximum of about 5 billion for n = 100,000, because many random swaps cancel each other out. |
|  4  | The swap percentage refers to the number of swaps as a fraction of the array size, not the fraction of maximum possible inversions.             |

### Key Findings

|  #  | Finding                                                                                                                                                                                                                                                             |
| :-: | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
|  1  | Inversion count strongly affects Insertion Sort performance. At n = 200,000, Sorted took 0.0644 ms while Reverse Sorted took 4,975.7102 ms, about 77,000 times slower.                                                                                              |
|  2  | Growth rates match theory: Sorted times roughly double when size doubles (O(n)); Random and Reverse Sorted times roughly quadruple (O(n^2)).                                                                                                                        |
|  3  | Time per inversion is nearly constant across all five disorder levels in Experiment 2. When each time is divided by its inversion count, the result stays close to a constant, meaning the cost per inversion does not change as the array becomes more disordered. |
|  4  | Inversion count is a reliable predictor of Insertion Sort runtime at a fixed array size.                                                                                                                                                                            |
|  5  | The divide-and-conquer method matched the naive method on every array tested, confirming correct implementation.                                                                                                                                                    |

### How to Compile & Run

```bash
g++ -std=c++17 -O2 -o countingInversions countingInversions.cpp
./countingInversions
```

> **Warning:** The naive O(n^2) verification runs on every array up to 200,000 elements. Expect long runtimes for the largest sizes. You can use the `-O2` optimization flag when compiling to significantly reduce runtime. The command above already includes `-O2`, which enables compiler optimizations that speed up the naive O(n^2) verification.

---

## Project 4: Network Flow

**File:** `Project4_Network_Flow_Report_Deepshan_Adhikari.pdf`

### Description

A research report analyzing the paper "Multi-objective Predictive Taxi Dispatch via Network Flow Optimization" by Kim et al. (IEEE Access, 2020). The report models taxi dispatch as a Minimum Cost Maximum Flow (MCMF) problem.

### Key Concepts Covered

|  #  | Concept                      | Description                                                                                                 |
| :-: | :--------------------------- | :---------------------------------------------------------------------------------------------------------- |
|  1  | Flow Networks                | Directed graph G = (V, E) with capacities, source, and sink                                                 |
|  2  | Max-Flow Min-Cut Theorem     | Maximum flow value equals minimum cut capacity                                                              |
|  3  | MCMF Algorithm               | Finds a flow that maximizes served requests while minimizing repositioning cost                             |
|  4  | Hexagonal Cell Model         | The city is divided into hexagonal cells; each cell has a vehicle-side node V_i and a request-side node W_j |
|  5  | Multi-Objective Optimization | Balances maximizing served requests against minimizing relocation cost                                      |

### Network Model Components

| Taxi Dispatch                 |         | Network Flow Problem        |
| :---------------------------- | :-----: | :-------------------------- |
| d_i,t (number of drivers)     | maps to | Flow capacity from S to V_i |
| r_i,t (number of requests)    | maps to | Flow capacity from W_i to T |
| c_i to j,t (reposition costs) | maps to | Flow costs                  |

### Worked Example

A 3-cell example (A, B, C) with:

| Cell | Drivers | Requests |
| :--: | :-----: | :------: |
|  A   |    2    |    0     |
|  B   |    1    |    2     |
|  C   |    0    |    1     |

Movement costs: V_A to W_A = 0, V_A to W_B = 2, V_B to W_A = 2, V_B to W_B = 0, V_B to W_C = 3, V_C to W_B = 3, V_C to W_C = 0.

Result: Maximum flow of 3 (all requests served) with minimum total cost of 7:

| Assignment | Count | Cost Each | Total |
| :--------- | :---: | :-------: | :---: |
| A to B     |   2   |     2     |   4   |
| B to C     |   1   |     3     |   3   |
| Total      |   3   |           |   7   |

### Augmenting Path and Residual Graph

| Step                  | Detail                                                                                                                      |
| :-------------------- | :-------------------------------------------------------------------------------------------------------------------------- |
| First augmenting path | S to V_B to W_B to T with cost 0                                                                                            |
| Bottleneck            | min(1, infinity, 2) = 1                                                                                                     |
| After augmentation    | Forward edge S to V_B is saturated (residual 0). Reverse edges V_B to S, W_B to V_B, and T to W_B have residual capacity 1. |
| Note                  | Reverse edges allow the algorithm to cancel flow if a better solution is found later.                                       |

### Research Paper Analysis

| Aspect               | Details                                                                                                                            |
| :------------------- | :--------------------------------------------------------------------------------------------------------------------------------- |
| Objective            | Predictive taxi dispatch with receding horizon optimization                                                                        |
| Why Network Flow     | Natural graph structure, global optimality, integer solutions                                                                      |
| Advantages           | Integer assignments, time-greedy property, 880 times faster LP version, 97.16% of oracle performance                               |
| Disadvantages        | Homogeneous taxi assumption, prediction inaccuracy, no real-time traffic, discretization errors                                    |
| Limitations          | Does not account for real-time traffic variations, driver behavior (refusing assignments), or special events causing demand spikes |
| Reported degradation | Days 3, 4, and 13 due to special events held in Seoul                                                                              |


## References

|  #  | Reference                                                                                                                                                                 |
| :-: | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
|  1  | Kleinberg, Jon & Tardos, Eva. (2006). Algorithm Design. Pearson Addison-Wesley.                                                                                           |
|  2  | Kim, B., Kim, J., Huh, S., You, S., & Yang, I. (2020). "Multi-objective predictive taxi dispatch via network flow optimization." IEEE Access, vol. 8, pp. 21437–21452.    |
|  3  | Ford, L. R., Jr. & Fulkerson, D. R. (1956). "Maximal flow through a network." Canadian Journal of Mathematics, vol. 8, pp. 399–404.                                       |
|  4  | Edmonds, J. & Karp, R. M. (1972). "Theoretical improvements in algorithmic efficiency for network flow problems." Journal of the ACM, vol. 19, no. 2, pp. 248–264.        |
|  5  | Busacker, R. G. & Gowen, P. J. (1960). "A procedure for determining a family of minimum-cost network flow patterns." Research Analysis Corporation, Tech. Rep. ORO-TP-15. |

---

## License

These projects were completed as academic coursework for CS456 at SIUE. Please respect academic integrity policies if referencing this work.
