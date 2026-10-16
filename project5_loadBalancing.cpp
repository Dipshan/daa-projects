// CS456 Design and Analysis of Algorithms
// Project 5 - Load Balancing

// This program implements three algorithms:
//   1. List Scheduling          - O(n*m) linear scan, 2-approximation
//   2. Sorted List Scheduling   - O(n log n + n*m), 3/2-approximation
//   3. Local Search             - Iterative improving-swap heuristic

// The experiment uses 10 machines and 200 jobs with random processing
// times between 1 and 25. For each trial, the lower bound on the
// makespan is computed as ceil(total processing time / machines).
// All three algorithms run on the same job set. The whole experiment
// is repeated 10 times, tracking the high, low, and average makespan
// for each algorithm as well as the running times.

#include <iostream>  // prints experiment results
#include <vector>    // stores job lists, machine assignments, and loads
#include <algorithm> // provides sort, max_element, and min_element
#include <random>    // provides the random number generator used to build jobs
#include <chrono>    // provides a monotonic timer used to measure runtime
#include <numeric>   // provides accumulate, used to sum job times for the lower bound
#include <iomanip>   // provides setw and setprecision for aligned output

using namespace std;
using namespace chrono;

// Problem constants. Ten machines and two hundred jobs, as required.
const int M = 10;      // number of machines
const int N = 200;     // number of jobs per trial
const int TRIALS = 10; // number of independent repetitions of the experiment

// Algorithm 1: List Scheduling
// Assign each job in the given order to the machine with the smallest
// current load. Returns the makespan, the maximum load on any machine.
// This is the classic 2-approximation greedy algorithm.
// Note: the job sets J(i) from the pseudocode are not stored because
// only the loads matter for computing the makespan.
int listScheduling(const vector<int> &jobs)
{
    vector<int> load(M, 0); // load[i] is the total time on machine i

    for (int jobTime : jobs)
    {
        // Find the machine with the smallest current load.
        int bestMachine = 0;
        for (int i = 1; i < M; i++)
        {
            if (load[i] < load[bestMachine])
                bestMachine = i;
        }
        load[bestMachine] += jobTime;
    }

    // The makespan is the largest load across all machines.
    return *max_element(load.begin(), load.end());
}

// Algorithm 2: Sorted List Scheduling
// Sort jobs in descending order first, then apply List Scheduling.
// Placing the largest jobs first gives the LPT rule. Its standard bound
// is stronger than 3/2 for m >= 2; 3/2 is a valid, looser guarantee.
int sortedListScheduling(vector<int> jobs)
{
    sort(jobs.rbegin(), jobs.rend()); // descending order
    return listScheduling(jobs);
}

// Algorithm 3: Local Search
// Randomly partition the jobs among the machines. Then repeatedly look
// for a swap of two jobs between two machines that lowers the maximum
// load of those two machines. When an improving swap is found, perform
// it immediately and restart the search from the beginning. Stop when
// a full pass finds no improving swap.
int localSearch(const vector<int> &jobs, mt19937 &rng)
{
    vector<vector<int>> machine(M); // jobs assigned to each machine
    vector<int> load(M, 0);         // total load on each machine

    // Random initial partition: place each job on a random machine.
    uniform_int_distribution<int> machineDist(0, M - 1);
    for (int jobTime : jobs)
    {
        int i = machineDist(rng);
        machine[i].push_back(jobTime);
        load[i] += jobTime;
    }

    bool improved = true;
    while (improved)
    {
        improved = false;

        // Try every pair of machines.
        for (int a = 0; a < M; a++)
        {
            for (int b = a + 1; b < M; b++)
            {
                int oldMax = max(load[a], load[b]);

                // Try every pair of jobs between the two machines.
                for (size_t x = 0; x < machine[a].size(); x++)
                {
                    for (size_t y = 0; y < machine[b].size(); y++)
                    {
                        int jobA = machine[a][x];
                        int jobB = machine[b][y];
                        int newA = load[a] - jobA + jobB;
                        int newB = load[b] - jobB + jobA;

                        // Accept the swap only if it lowers the max load
                        // of the two machines involved.
                        if (max(newA, newB) < oldMax)
                        {
                            machine[a][x] = jobB;
                            machine[b][y] = jobA;
                            load[a] = newA;
                            load[b] = newB;
                            improved = true;

                            // Restart the search from the beginning
                            // after performing an improving swap. The
                            // goto breaks out of four nested loops at once.
                            goto restart;
                        }
                    }
                }
            }
        }
    restart:; // label for the goto above; restarts the while loop
    }

    return *max_element(load.begin(), load.end());
}

// Statistics helper
// Holds the highest, lowest, and average value of a set of results.
struct Stats
{
    int high;
    int low;
    double avg;
};

// Computes the high, low, and average of a vector of integers.
Stats summarize(const vector<int> &values)
{
    int high = *max_element(values.begin(), values.end());
    int low = *min_element(values.begin(), values.end());
    double avg = accumulate(values.begin(), values.end(), 0.0) / values.size();
    return {high, low, avg};
}

int main()
{
    mt19937 jobsRng(42);
    mt19937 localSearchRng(7);

    uniform_int_distribution<int> dist(1, 25);

    vector<int> lowerBoundList;
    vector<int> listResults;
    vector<int> sortedResults;
    vector<int> localResults;

    vector<double> listTime;
    vector<double> sortedTime;
    vector<double> localTime;

    for (int trial = 0; trial < TRIALS; trial++)
    {
        vector<int> jobs(N);
        for (int &t : jobs)
            t = dist(jobsRng);

        int total = accumulate(jobs.begin(), jobs.end(), 0);
        lowerBoundList.push_back((total + M - 1) / M);

        auto startTime = steady_clock::now();
        int result1 = listScheduling(jobs);
        auto endTime = steady_clock::now();
        listResults.push_back(result1);
        listTime.push_back(duration_cast<microseconds>(endTime - startTime).count() / 1000.0);

        startTime = steady_clock::now();
        int result2 = sortedListScheduling(jobs);
        endTime = steady_clock::now();
        sortedResults.push_back(result2);
        sortedTime.push_back(duration_cast<microseconds>(endTime - startTime).count() / 1000.0);

        startTime = steady_clock::now();
        int result3 = localSearch(jobs, localSearchRng);
        endTime = steady_clock::now();
        localResults.push_back(result3);
        localTime.push_back(duration_cast<microseconds>(endTime - startTime).count() / 1000.0);
    }

    // Per-trial table
    cout << "\nPer-Trial Results\n";
    cout << "------------------------------------------------------------\n";
    cout << left << setw(8) << "Trial"
         << setw(14) << "Lower Bound"
         << setw(10) << "List"
         << setw(10) << "Sorted"
         << setw(10) << "Local" << "\n";
    cout << "------------------------------------------------------------\n";

    for (int i = 0; i < TRIALS; i++)
    {
        cout << left << setw(8) << (i + 1)
             << setw(14) << fixed << setprecision(1) << (double)lowerBoundList[i]
             << setw(10) << listResults[i]
             << setw(10) << sortedResults[i]
             << setw(10) << localResults[i] << "\n";
    }
    cout << "------------------------------------------------------------\n";

    // Makespan summary
    Stats lb = summarize(lowerBoundList);
    Stats ls = summarize(listResults);
    Stats sl = summarize(sortedResults);
    Stats lo = summarize(localResults);

    cout << "\nMakespan Summary\n";
    cout << "------------------------------------------------------------\n";
    cout << left << setw(10) << ""
         << setw(14) << "Lower Bound"
         << setw(10) << "List"
         << setw(10) << "Sorted"
         << setw(10) << "Local" << "\n";
    cout << left << setw(10) << "Highest"
         << setw(14) << lb.high
         << setw(10) << ls.high
         << setw(10) << sl.high
         << setw(10) << lo.high << "\n";
    cout << left << setw(10) << "Average"
         << setw(14) << fixed << setprecision(1) << lb.avg
         << setw(10) << ls.avg
         << setw(10) << sl.avg
         << setw(10) << lo.avg << "\n";
    cout << left << setw(10) << "Lowest"
         << setw(14) << lb.low
         << setw(10) << ls.low
         << setw(10) << sl.low
         << setw(10) << lo.low << "\n";
    cout << "------------------------------------------------------------\n";

    // Approximation ratio table
    double worstList = 0, avgList = 0;
    double worstSorted = 0, avgSorted = 0;
    double worstLocal = 0, avgLocal = 0;

    for (int i = 0; i < TRIALS; i++)
    {
        double lbd = (double)lowerBoundList[i];
        double rList = listResults[i] / lbd;
        double rSorted = sortedResults[i] / lbd;
        double rLocal = localResults[i] / lbd;

        worstList = max(worstList, rList);
        worstSorted = max(worstSorted, rSorted);
        worstLocal = max(worstLocal, rLocal);

        avgList += rList;
        avgSorted += rSorted;
        avgLocal += rLocal;
    }
    avgList /= TRIALS;
    avgSorted /= TRIALS;
    avgLocal /= TRIALS;

    cout << "\nObserved Makespan / Lower Bound\n";
    cout << "------------------------------------------------------------\n";
    cout << left << setw(14) << "Algorithm"
         << setw(14) << "Worst"
         << setw(14) << "Average"
         << setw(14) << "Guarantee" << "\n";
    cout << left << setw(14) << "List"
         << setw(14) << fixed << setprecision(4) << worstList
         << setw(14) << avgList
         << setw(14) << "2" << "\n";
    cout << left << setw(14) << "Sorted"
         << setw(14) << worstSorted
         << setw(14) << avgSorted
         << setw(14) << "3/2" << "\n";
    cout << left << setw(14) << "Local"
         << setw(14) << worstLocal
         << setw(14) << avgLocal
         << setw(14) << "none" << "\n";
    cout << "------------------------------------------------------------\n";

    // Running time summary
    auto averageTime = [](const vector<double> &values)
    {
        return accumulate(values.begin(), values.end(), 0.0) / values.size();
    };

    cout << "\nAverage Running Times (in milliseconds)\n";
    cout << "------------------------------------------------------------\n";
    cout << left << setw(30) << "List Scheduling"
         << fixed << setprecision(4) << averageTime(listTime) << "\n";
    cout << left << setw(30) << "Sorted List Scheduling"
         << fixed << setprecision(4) << averageTime(sortedTime) << "\n";
    cout << left << setw(30) << "Local Search"
         << fixed << setprecision(4) << averageTime(localTime) << "\n";
    cout << "------------------------------------------------------------\n";

    return 0;
}