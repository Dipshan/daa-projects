// CS456 Design and Analysis of Algorithms
// Project 2 - Enhancing Quicksort

// Author: Deepshan Adhikari (800846035)
// deepadh@siue.edu

// This program benchmarks four pivot selection strategies for Quicksort:
//   1. Default Quicksort        - last element as pivot
//   2. Modified Quicksort       - retry until center half pivot
//   3. Random Splitter          - any random pivot
//   4. Quickselect at each step - median as pivot

// The benchmark tests five array sizes (10k, 20k, 50k, 100k, 200k) on two
// input types: random values in [1, 1,000,000] and sorted values 1..n
// Each combination is timed 10 times and the average is reported
// Any run over 10 minutes is reported as "too long"
// Any run whose child process is killed by a signal is reported as "Stack Overflow"

#include <iostream>   // console input and output (cout, endl)
#include <vector>     // dynamic array container (std::vector)
#include <random>     // random number generator (mt19937, uniform_int_distribution)
#include <chrono>     // high-resolution timer (high_resolution_clock)
#include <iomanip>    // output formatting (setw, setprecision, fixed)
#include <sstream>    // string stream for building strings (stringstream)
#include <string>     // string type (std::string)
#include <algorithm>  // swap helper (std::swap)
#include <sys/wait.h> // process utilities for fork/wait (waitpid, WIFSIGNALED)
#include <unistd.h>   // POSIX process utilities (fork, _exit)

using namespace std;
using namespace std::chrono;

// The random number generator. It is seeded from the operating system,
// so every run of the program produces different random arrays and
// different random pivot choices
static mt19937 g_rng(random_device{}());

// Any single run longer than this is reported as "too long"
static const double TIMEOUT_MS = 600000.0;

// Returns a uniformly random integer in the range [lo, hi]
static inline int randomInt(int lo, int hi)
{
    uniform_int_distribution<int> dist(lo, hi);
    return dist(g_rng);
}

// Partitions arr[low..high] around the pivot at pivotIdx
// Moves the pivot to the end, moves every element <= pivot to the front,
// and places the pivot in its final sorted position. Returns that index
static int partitionAroundPivot(vector<int> &arr, int low, int high, int pivotIdx)
{
    int pivotValue = arr[pivotIdx];
    swap(arr[pivotIdx], arr[high]); // move pivot to the end
    int i = low - 1;                // boundary of the "<=" region
    for (int j = low; j < high; ++j)
    {
        if (arr[j] <= pivotValue)
        {
            ++i;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]); // place pivot at its final index
    return i + 1;
}

// Default Quicksort.
// Always uses the last element of the current subarray as the pivot
// No depth limit and no fallback, so sorted input shows the O(n^2) case
static void defaultQuicksort(vector<int> &arr, int low, int high)
{
    if (low < high)
    {
        int pi = partitionAroundPivot(arr, low, high, high);
        defaultQuicksort(arr, low, pi - 1);
        defaultQuicksort(arr, pi + 1, high);
    }
}
// Wrapper that sorts the whole array.
static void runDefault(vector<int> &arr)
{
    if (!arr.empty())
        defaultQuicksort(arr, 0, (int)arr.size() - 1);
}

// Returns true if the pivot at pivotIdx sits in the middle half of the
// subarray when sorted: at least 25% of elements are strictly smaller
// and at most 75% are smaller-or-equal
static bool isCenterHalf(const vector<int> &arr, int low, int high, int pivotIdx)
{
    int len = high - low + 1;
    int pivotValue = arr[pivotIdx];
    int less = 0, equal = 0;
    for (int i = low; i <= high; ++i)
    {
        if (arr[i] < pivotValue)
            ++less;
        else if (arr[i] == pivotValue)
            ++equal;
    }
    return (less >= len / 4) && (less + equal <= (3 * len) / 4);
}

// Picks random pivots and returns the first one that lies in the middle half
static int findCenterHalfPivot(const vector<int> &arr, int low, int high)
{
    for (int attempt = 0; attempt < 100; ++attempt)
    {
        int idx = randomInt(low, high);
        if (isCenterHalf(arr, low, high, idx))
            return idx;
    }
    return randomInt(low, high); // fallback after too many attempts
}

// Modified Quicksort (Center Half Pivot)
// Repeatedly chooses a random pivot until one lands in the middle half,
// which guarantees balanced partitions and O(n log n) expected time on
// every input, including sorted input
static void modifiedQuicksort(vector<int> &arr, int low, int high)
{
    if (low < high)
    {
        int pivotIdx = findCenterHalfPivot(arr, low, high);
        int pi = partitionAroundPivot(arr, low, high, pivotIdx);
        modifiedQuicksort(arr, low, pi - 1);
        modifiedQuicksort(arr, pi + 1, high);
    }
}
// Wrapper that sorts the whole array
static void runModified(vector<int> &arr)
{
    if (!arr.empty())
        modifiedQuicksort(arr, 0, (int)arr.size() - 1);
}

// Random Quicksort
// Picks any random element as the pivot, with no quality check
static void randomQuicksort(vector<int> &arr, int low, int high)
{
    if (low < high)
    {
        int pivotIdx = randomInt(low, high);
        int pi = partitionAroundPivot(arr, low, high, pivotIdx);
        randomQuicksort(arr, low, pi - 1);
        randomQuicksort(arr, pi + 1, high);
    }
}
// Wrapper that sorts the whole array.
static void runRandomSplitter(vector<int> &arr)
{
    if (!arr.empty())
        randomQuicksort(arr, 0, (int)arr.size() - 1);
}

// Quickselect Pivot
// Returns the index at which the k-th smallest element of arr[low..high]
// ends up, without fully sorting the range. At each step it picks a random
// pivot, partitions, and recurses only into the side that contains the k-th element
static int quickselectIndex(vector<int> &arr, int low, int high, int k)
{
    while (low < high)
    {
        int pivotIdx = randomInt(low, high);
        int pi = partitionAroundPivot(arr, low, high, pivotIdx);
        if (pi == k)
            return pi; // k-th element is at the pivot index
        else if (pi < k)
            low = pi + 1; // k-th element is on the right
        else
            high = pi - 1; // k-th element is on the left
    }
    return low;
}

// Quickselect Pivot
// Uses Quickselect at every step to place the exact median at its final
// position, then uses that median as the pivot. This gives perfectly
// balanced partitions at the cost of an extra O(n) selection pass at every level
static void quickselectPivot(vector<int> &arr, int low, int high)
{
    if (low < high)
    {
        int medianIdx = quickselectIndex(arr, low, high, (low + high) / 2);
        int pi = partitionAroundPivot(arr, low, high, medianIdx);
        quickselectPivot(arr, low, pi - 1);
        quickselectPivot(arr, pi + 1, high);
    }
}
// Wrapper that sorts the whole array.
static void runQuickselect(vector<int> &arr)
{
    if (!arr.empty())
        quickselectPivot(arr, 0, (int)arr.size() - 1);
}

// Creates an array of the given size filled with random integers in the range [1, 1,000,000]
static vector<int> makeRandomArray(int size)
{
    vector<int> arr(size);
    uniform_int_distribution<int> dist(1, 1000000);
    for (int i = 0; i < size; ++i)
        arr[i] = dist(g_rng);
    return arr;
}

// Creates an array of the given size containing the values 1, 2, 3, ..., size
static vector<int> makeSortedArray(int size)
{
    vector<int> arr(size);
    for (int i = 0; i < size; ++i)
        arr[i] = i + 1;
    return arr;
}

// Runs one sort on a copy of the array inside a forked child process
// The copy is made before the timer starts. Returns the elapsed time in ms
static double timeOneRun(void (*sortFunc)(vector<int> &),
                         const vector<int> &base)
{
    vector<int> copy = base; // copy is made before the timer starts

    auto start = high_resolution_clock::now();

    pid_t pid = fork();
    if (pid == 0)
    {
        // Child process: run the sort, then exit immediately
        sortFunc(copy);
        _exit(0);
    }

    // Parent process: wait for the child to finish or be killed
    int status = 0;
    waitpid(pid, &status, 0);

    auto end = high_resolution_clock::now();
    double elapsedMs = duration<double, milli>(end - start).count();

    if (WIFSIGNALED(status))
        return -2.0; // child was killed
    if (elapsedMs > TIMEOUT_MS)
        return -1.0; // run exceeded 10 minutes
    return elapsedMs;
}

// Runs the sort 10 times and returns the average in milliseconds
// If any single run returns one of the sentinels (-1.0 or -2.0), that value is returned immediately
static double timeMethod(void (*sortFunc)(vector<int> &),
                         const vector<int> &base,
                         int runs)
{
    double total = 0.0;
    for (int r = 0; r < runs; ++r)
    {
        double elapsed = timeOneRun(sortFunc, base);
        if (elapsed < 0.0)
            return elapsed;
        total += elapsed;
    }
    return total / runs;
}

// Formats a timing value for the table:
//   -2.0   -> "Stack Overflow"
//   -1.0   -> "too long"
//   >= 0.0 -> the value in milliseconds
static string formatTime(double ms)
{
    if (ms == -2.0)
        return "Stack Overflow";
    if (ms < 0.0)
        return "too long";
    stringstream ss;
    ss << fixed << setprecision(1) << ms;
    return ss.str();
}

// Formats one table cell as "<random time> / <sorted time>"
static string cell(double randomMs, double sortedMs)
{
    return formatTime(randomMs) + " / " + formatTime(sortedMs);
}

int main()
{
    // The five array sizes that will be tested
    const vector<int> sizes = {10000, 20000, 50000, 100000, 200000};

    // Number of trials per method per array.
    const int RUNS = 10;

    // Print the header.
    cout << "\nQuicksort Benchmark\n";
    cout << "Values are averages over " << RUNS << " runs.\n";
    cout << "Runs over 10 minutes are reported as \"too long\".\n";
    cout << "Crashes inside a sort (e.g. stack overflow) are reported as "
            "\"Stack Overflow\".\n\n";

    // Column widths for the console table
    const int SIZE_W = 12;
    const int COL_W = 26;

    // Top line of the table header (strategy names)
    cout << left
         << setw(SIZE_W) << "Array Size" << " | "
         << setw(COL_W) << "Default Quicksort" << " | "
         << setw(COL_W) << "Center Half Pivot" << " | "
         << setw(COL_W) << "Random Pivot" << " | "
         << setw(COL_W) << "Quickselect Pivot" << "\n";

    // Second line of the header (input types).
    cout << left
         << setw(SIZE_W) << "" << " | "
         << setw(COL_W) << "(random/sorted)" << " | "
         << setw(COL_W) << "(random/sorted)" << " | "
         << setw(COL_W) << "(random/sorted)" << " | "
         << setw(COL_W) << "(random/sorted)" << "\n";

    // Separator line.
    cout << string(SIZE_W + 3 + 4 * (COL_W + 3), '-') << "\n";

    // Run the experiment once per array size
    for (int n : sizes)
    {
        // Build the two input arrays for this size.
        vector<int> randBase = makeRandomArray(n);
        vector<int> sortedBase = makeSortedArray(n);

        // Time every strategy on both input types.
        double dR = timeMethod(runDefault, randBase, RUNS);
        double dS = timeMethod(runDefault, sortedBase, RUNS);
        double mR = timeMethod(runModified, randBase, RUNS);
        double mS = timeMethod(runModified, sortedBase, RUNS);
        double rR = timeMethod(runRandomSplitter, randBase, RUNS);
        double rS = timeMethod(runRandomSplitter, sortedBase, RUNS);
        double qR = timeMethod(runQuickselect, randBase, RUNS);
        double qS = timeMethod(runQuickselect, sortedBase, RUNS);

        // Print the size with a comma for readability, e.g. 10,000
        string sizeStr = to_string(n);
        if (sizeStr.size() > 3)
            sizeStr.insert(sizeStr.size() - 3, ",");

        // Print this row immediately so progress is visible
        cout << left
             << setw(SIZE_W) << sizeStr << " | "
             << setw(COL_W) << cell(dR, dS) << " | "
             << setw(COL_W) << cell(mR, mS) << " | "
             << setw(COL_W) << cell(rR, rS) << " | "
             << setw(COL_W) << cell(qR, qS) << "\n";
    }

    return 0;
}