// CS456 Design and Analysis of Algorithms
// Project 3 - Counting Inversions

// This program implements three algorithms:
//   1. Naive Inversion Counting   - O(n^2)
//   2. Divide-and-Conquer Count   - O(n log n), modified MergeSort
//   3. Insertion Sort             - O(n^2), timed

// Experiment 1 tests Sorted, Random, and Reverse Sorted arrays at five sizes (10k, 20k, 50k, 100k, 200k).
// Each configuration runs 10 times.
// The reported Insertion Sort time is the average over 10 trials.

// Experiment 2 starts from a sorted array and applies random swaps at
// 1%, 5%, 10%, 25%, and 50% levels on a fixed size of 100,000. Time per
// inversion is reported in nanoseconds to test whether inversion count
// predicts Insertion Sort runtime.

#include <iostream>  // reads input from the console and prints output (cout, cerr)
#include <vector>    // stores the arrays of numbers that will be sorted and counted
#include <string>    // stores text such as "Sorted", "Random", and "Reverse Sorted"
#include <numeric>   // provides iota, which fills an array with 0, 1, 2, ... in order
#include <algorithm> // provides shuffle and swap, used to build random and partially sorted arrays
#include <random>    // provides the random number generators used to build random arrays
#include <chrono>    // provides the high-resolution timer used to measure Insertion Sort
#include <iomanip>   // provides formatting tools (fixed, setprecision, setw) for the output tables

using namespace std;
using namespace chrono;

// Shared random generator, seeded from the operating system.
// Each call advances the sequence, so random arrays differ between calls.
static mt19937 randomGenerator(random_device{}());

// Counts inversions by checking every pair (i, j) with i < j and A[i] > A[j].
// O(n^2). Used to verify the divide-and-conquer count.
long long naiveCount(const vector<int> &values)
{
    long long inversions = 0;
    int size = (int)values.size();
    for (int i = 0; i < size; i++)
    {
        for (int j = i + 1; j < size; j++)
        {
            if (values[i] > values[j])
                inversions++;
        }
    }
    return inversions;
}

// Merges two sorted halves and counts cross inversions.
// When a right-half element is chosen, every remaining left-half element
// is larger than it, so each one forms an inversion.
long long mergeAndCount(vector<int> &values, vector<int> &buffer,
                        int low, int mid, int high)
{
    int leftIndex = low;
    int rightIndex = mid + 1;
    int bufferIndex = low;
    long long inversions = 0;

    while (leftIndex <= mid && rightIndex <= high)
    {
        if (values[leftIndex] <= values[rightIndex])
        {
            buffer[bufferIndex++] = values[leftIndex++];
        }
        else
        {
            buffer[bufferIndex++] = values[rightIndex++];
            inversions += (mid - leftIndex + 1);
        }
    }

    while (leftIndex <= mid)
        buffer[bufferIndex++] = values[leftIndex++];
    while (rightIndex <= high)
        buffer[bufferIndex++] = values[rightIndex++];

    for (int index = low; index <= high; index++)
        values[index] = buffer[index];

    return inversions;
}

// Recursively counts inversions in the left half, the right half,
// and across the two halves during the merge step.
long long countInversionsRecursive(vector<int> &values, vector<int> &buffer,
                                   int low, int high)
{
    if (low >= high)
        return 0;

    int mid = low + (high - low) / 2;
    long long inversions = 0;

    inversions += countInversionsRecursive(values, buffer, low, mid);
    inversions += countInversionsRecursive(values, buffer, mid + 1, high);
    inversions += mergeAndCount(values, buffer, low, mid, high);

    return inversions;
}

// Wrapper that prepares the buffer and starts the recursion.
// The array is passed by value because the recursion rearranges it.
long long countInversions(vector<int> values)
{
    vector<int> buffer(values.size());
    return countInversionsRecursive(values, buffer, 0, (int)values.size() - 1);
}

// Standard Insertion Sort. Returns elapsed time in milliseconds.
// The number of shifts equals the number of inversions in the input,
// so runtime is proportional to the inversion count.
double insertionSort(vector<int> values)
{
    auto startTime = high_resolution_clock::now();

    int size = (int)values.size();
    for (int i = 1; i < size; i++)
    {
        int key = values[i];
        int j = i - 1;
        while (j >= 0 && values[j] > key)
        {
            values[j + 1] = values[j];
            j--;
        }
        values[j + 1] = key;
    }

    auto endTime = high_resolution_clock::now();
    return duration<double, milli>(endTime - startTime).count();
}

// Creates a sorted array of values 0 to size-1.
vector<int> makeSortedArray(int size)
{
    vector<int> values(size);
    iota(values.begin(), values.end(), 0);
    return values;
}

// Creates a randomly shuffled array of values 0 to size-1.
vector<int> makeRandomArray(int size)
{
    vector<int> values = makeSortedArray(size);
    shuffle(values.begin(), values.end(), randomGenerator);
    return values;
}

// Creates a reverse-sorted array from size down to 1.
// Has the maximum possible inversions, size*(size-1)/2.
vector<int> makeReverseArray(int size)
{
    vector<int> values(size);
    for (int i = 0; i < size; i++)
        values[i] = size - i;
    return values;
}

// Starts from a sorted array and performs size * swapPercent random swaps.
vector<int> makePartiallySortedArray(int size, double swapPercent)
{
    vector<int> values = makeSortedArray(size);
    int swapCount = (int)(size * swapPercent);
    uniform_int_distribution<int> indexPicker(0, size - 1);

    for (int s = 0; s < swapCount; s++)
    {
        int firstIndex = indexPicker(randomGenerator);
        int secondIndex = indexPicker(randomGenerator);
        swap(values[firstIndex], values[secondIndex]);
    }
    return values;
}

// Holds one row of results for the final summary.
struct ResultRow
{
    long long size;       // array size, or swap percentage for Experiment 2
    string label;         // dataset label, e.g. "Sorted" or "10%"
    long long inversions; // average inversion count
    double timeMs;        // average Insertion Sort time in milliseconds
};

// Formats a number with commas, e.g. 10000 becomes "10,000".
string formatWithCommas(long long number)
{
    string text = to_string(number);
    for (int position = (int)text.size() - 3; position > 0; position -= 3)
        text.insert(position, ",");
    return text;
}

// Prints every row that matches the given extreme value.
// Shows all ties rather than only the first one.
void printAllTies(const vector<ResultRow> &rows,
                  bool matchOnTime,
                  long long inversionTarget,
                  double timeTarget)
{
    bool firstMatch = true;
    for (const ResultRow &row : rows)
    {
        bool isMatch = matchOnTime
                           ? (row.timeMs == timeTarget)
                           : (row.inversions == inversionTarget);
        if (isMatch)
        {
            if (!firstMatch)
                cout << ", ";
            cout << row.label << " n=" << formatWithCommas(row.size);
            firstMatch = false;
        }
    }
    cout << "\n";
}

int main()
{
    const int TRIALS = 10;

    // Sizes and dataset types for Experiment 1.
    vector<int> arraySizes = {10000, 20000, 50000, 100000, 200000};
    vector<string> datasetTypes = {"Sorted", "Random", "Reverse"};

    // Largest size where the naive count is used to verify the Divide-and-Conquer count.
    // Set to 200,000 so every array is verified, as the assignment requires.
    const int VERIFICATION_LIMIT = 200000;

    // Column widths for the console tables.
    const int SIZE_COLUMN_WIDTH = 12;
    const int TYPE_COLUMN_WIDTH = 16;
    const int INVERSION_COLUMN_WIDTH = 16;
    const int TIME_COLUMN_WIDTH = 36;

    // Separator line lengths, derived from the column widths.
    const int EXPERIMENT1_LINE_LENGTH =
        SIZE_COLUMN_WIDTH + TYPE_COLUMN_WIDTH + INVERSION_COLUMN_WIDTH + TIME_COLUMN_WIDTH + 9;
    const int EXPERIMENT2_LINE_LENGTH =
        SIZE_COLUMN_WIDTH + INVERSION_COLUMN_WIDTH + TIME_COLUMN_WIDTH + 6;

    vector<ResultRow> sweepRows;    // Experiment 1 results
    vector<ResultRow> disorderRows; // Experiment 2 results

    // Experiment 1 header.
    cout << "\nProject 3 - Counting Inversions\n";
    cout << "\nExperiment 1: Inversions and Sort Time Across All Dataset Types and Sizes\n";
    cout << string(EXPERIMENT1_LINE_LENGTH, '-') << "\n";
    cout << left
         << setw(SIZE_COLUMN_WIDTH) << "Array Size" << " | "
         << setw(TYPE_COLUMN_WIDTH) << "Dataset Type" << " | "
         << setw(INVERSION_COLUMN_WIDTH) << "Inversion Count" << " | "
         << setw(TIME_COLUMN_WIDTH) << "Average Insertion Sort Time (ms)" << "\n";
    cout << string(EXPERIMENT1_LINE_LENGTH, '-') << "\n";

    // Experiment 1: for each size and dataset type, count inversions with
    // both methods, verify they match, and time Insertion Sort. Averages
    // are taken over 10 trials. A dashed line separates each size group.
    for (int size : arraySizes)
    {
        for (const string &type : datasetTypes)
        {
            long long inversionSum = 0;
            double timeSum = 0.0;

            for (int trial = 0; trial < TRIALS; trial++)
            {
                vector<int> data;
                if (type == "Sorted")
                    data = makeSortedArray(size);
                else if (type == "Random")
                    data = makeRandomArray(size);
                else
                    data = makeReverseArray(size);

                long long divideAndConquerCount = countInversions(data);

                // Verify against the naive count. The naive method is slow
                // at 200,000, but the assignment requires it to run for
                // every array.
                if (size <= VERIFICATION_LIMIT)
                {
                    long long naiveTotal = naiveCount(data);
                    if (naiveTotal != divideAndConquerCount)
                    {
                        cerr << "MISMATCH at n=" << size << " type=" << type << "\n";
                        return 1;
                    }
                }

                inversionSum += divideAndConquerCount;
                timeSum += insertionSort(data);
            }

            long long averageInversions = inversionSum / TRIALS;
            double averageTime = timeSum / TRIALS;

            // Print "Reverse Sorted" in full instead of the short label.
            string printedLabel = (type == "Reverse") ? "Reverse Sorted" : type;

            cout << left
                 << setw(SIZE_COLUMN_WIDTH) << formatWithCommas(size) << " | "
                 << setw(TYPE_COLUMN_WIDTH) << printedLabel << " | "
                 << setw(INVERSION_COLUMN_WIDTH) << averageInversions << " | "
                 << fixed << setprecision(4)
                 << setw(TIME_COLUMN_WIDTH) << averageTime << "\n";

            sweepRows.push_back({size, type, averageInversions, averageTime});
        }

        cout << string(EXPERIMENT1_LINE_LENGTH, '-') << "\n";
    }

    cout << "\nExperiment 2: Effect of Increasing Disorder on Sort Time (n = 100,000)\n";
    cout << string(EXPERIMENT2_LINE_LENGTH, '-') << "\n";
    cout << left
         << setw(SIZE_COLUMN_WIDTH) << "Swap %" << " | "
         << setw(INVERSION_COLUMN_WIDTH) << "Inversion Count" << " | "
         << setw(TIME_COLUMN_WIDTH) << "Average Insertion Sort Time (ms)" << "\n";
    cout << string(EXPERIMENT2_LINE_LENGTH, '-') << "\n";

    vector<double> swapLevels = {0.01, 0.05, 0.10, 0.25, 0.50};
    int fixedSize = 100000;

    for (double swapPercent : swapLevels)
    {
        long long inversionSum = 0;
        double timeSum = 0.0;

        for (int trial = 0; trial < TRIALS; trial++)
        {
            vector<int> data = makePartiallySortedArray(fixedSize, swapPercent);
            inversionSum += countInversions(data);
            timeSum += insertionSort(data);
        }

        long long averageInversions = inversionSum / TRIALS;
        double averageTime = timeSum / TRIALS;

        string percentageLabel = to_string((int)(swapPercent * 100)) + "%";

        cout << left
             << setw(SIZE_COLUMN_WIDTH) << percentageLabel << " | "
             << setw(INVERSION_COLUMN_WIDTH) << averageInversions << " | "
             << fixed << setprecision(4)
             << setw(TIME_COLUMN_WIDTH) << averageTime << "\n";

        disorderRows.push_back({(long long)(swapPercent * 100), percentageLabel, averageInversions, averageTime});
    }
    cout << string(EXPERIMENT2_LINE_LENGTH, '-') << "\n";

    // Summary of Experiment 1
    // Reports the minimum and maximum inversion counts and times, listing
    // every configuration that ties for the extreme value.
    cout << "\nSummary\n";

    long long fewestInversions = sweepRows[0].inversions;
    long long mostInversions = sweepRows[0].inversions;
    double fastestTime = sweepRows[0].timeMs;
    double slowestTime = sweepRows[0].timeMs;

    for (const ResultRow &row : sweepRows)
    {
        if (row.inversions < fewestInversions)
            fewestInversions = row.inversions;
        if (row.inversions > mostInversions)
            mostInversions = row.inversions;
        if (row.timeMs < fastestTime)
            fastestTime = row.timeMs;
        if (row.timeMs > slowestTime)
            slowestTime = row.timeMs;
    }

    cout << "Experiment 1:\n";
    cout << "  Fewest inversions : " << fewestInversions << " at ";
    printAllTies(sweepRows, false, fewestInversions, 0.0);
    cout << "  Most inversions   : " << mostInversions << " at ";
    printAllTies(sweepRows, false, mostInversions, 0.0);
    cout << "  Fastest sort      : " << fixed << setprecision(4) << fastestTime << " ms at ";
    printAllTies(sweepRows, true, 0, fastestTime);
    cout << "  Slowest sort      : " << fixed << setprecision(4) << slowestTime << " ms at ";
    printAllTies(sweepRows, true, 0, slowestTime);

    // Summary of Experiment 2
    // Time per inversion is shown in nanoseconds for easier comparison.
    // Compare the values across disorder levels to assess how well
    // inversion count relates to Insertion Sort runtime.
    cout << "\nExperiment 2:\n";
    cout << "  Time per inversion (in nanoseconds for easier comparison):\n";
    for (const ResultRow &row : disorderRows)
    {
        double nanosecondsPerInversion = (row.inversions > 0)
                                             ? (row.timeMs * 1e6 / row.inversions)
                                             : 0.0;
        cout << "    " << setw(6) << row.label << " -> "
             << fixed << setprecision(3) << nanosecondsPerInversion
             << " ns per inversion\n";
    }

    cout << "\nDone.\n";
    return 0;
}