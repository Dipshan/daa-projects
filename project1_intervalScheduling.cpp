// CS456 Design and Analysis of Algorithms
// Project 1 - Interval Scheduling

// Author: Deepshan Adhikari (800846035)
// deepadh@siue.edu
// September 6, 2026

// Description:
// This program compares four different interval scheduling strategies,
// by running 1000 trials with 1000 random jobs each.

// It analyzes:
// 1. Earliest Finish Time (EFT) - PROVEN OPTIMAL
// 2. Shortest Duration First (SDF) - NOT OPTIMAL
// 3. Earliest Start Time (EST) - NOT OPTIMAL
// 4. Fewest Conflicts First (FCF) - NOT OPTIMAL
//
// The program generates random intervals (jobs), applies each strategy,
// and reports the maximum, mean, and minimum number of jobs that can
// be scheduled for each strategy.

#include <iostream>  // For input/output operations
#include <vector>    // For using dynamic arrays (vectors)
#include <algorithm> // For sorting algorithms
#include <random>    // For generating random numbers
#include <iomanip>   // For formatting output (setw, setprecision)
#include <climits>   // For using INT_MAX/INT_MIN constants

using namespace std;

// Job Structure
// Represents an interval/job with start time, duration, and finish time
struct Job
{
    int start;    // Start time of the job
    int duration; // Duration/length of the job
    int finish;   // Finish time (start + duration)

    // Constructor: automatically calculates finish time
    Job(int s = 0, int d = 0) : start(s), duration(d), finish(s + d) {}
};

// Creates an array of random jobs
// Parameters: n - Number of jobs to create (default: 1000)
// Returns: Vector containing n random jobs
vector<Job> createJobArray(int n = 1000)
{
    random_device rd;                                 // Creates a random number generator using real randomness from the laptop device to get a truly random seed
    mt19937 gen(rd());                                // Creates a random number engine that generates millions of random numbers quickly
    uniform_int_distribution<> startDist(1, 9500);    // Creates a rule "give any whole number between 1 and 9500, all equally likely" - used for job start times
    uniform_int_distribution<> durationDist(10, 500); // Creates a rule "give any whole number between 10 and 500, all equally likely" - used for how long each job takes

    vector<Job> jobs;
    jobs.reserve(n); // Reserve memory for efficiency

    // Create n random jobs
    for (int i = 0; i < n; i++)
    {
        int start = startDist(gen);
        int duration = durationDist(gen);
        jobs.push_back(Job(start, duration));
    }
    return jobs;
}

// Chronological Scheduler
// Used for EFT and EST strategies where jobs are sorted by time
// Only checks against the last accepted job (greedy approach)
// Parameters: sortedJobs - Jobs sorted by finish time or start time
// Returns: Number of jobs that can be scheduled
int scheduleChronological(vector<Job> sortedJobs)
{
    int lastFinish = 0; // Finish time of the last accepted job
    int count = 0;      // Number of accepted jobs

    // Check each job in order
    for (const auto &job : sortedJobs)
    {
        // If job starts after or when last job finished, accept it
        if (job.start >= lastFinish)
        {
            count++;
            lastFinish = job.finish; // Update last finish time
        }
    }
    return count;
}

// General Scheduler
// Used for SDF and FCF strategies
// Checks compatibility with ALL previously accepted jobs
// Parameters: sortedJobs - Jobs sorted by duration or conflict count
// Returns: Number of jobs that can be scheduled
int scheduleGeneral(vector<Job> sortedJobs)
{
    vector<Job> accepted; // List of accepted jobs

    // Check each job in order
    for (const auto &job : sortedJobs)
    {
        bool compatible = true;

        // Check if job overlaps with any accepted job
        for (const auto &a : accepted)
        {
            // Check for overlap: Two jobs overlap if one starts before the other finishes and vice versa
            // job1.start < job2.finish && job2.start < job1.finish
            if (job.start < a.finish && a.start < job.finish)
            {
                compatible = false;
                break; // No need to check further
            }
        }

        // If compatible, accept the job
        if (compatible)
            accepted.push_back(job);
    }
    return accepted.size();
}

// Strategy 1: Earliest Finish Time First
// Sorts jobs by finish time and schedules greedily
// PROVEN OPTIMAL - This always gives the maximum number of jobs
int earliestFinishTimeFirst(vector<Job> jobs)
{
    // Sort by finish time (ascending)
    sort(jobs.begin(), jobs.end(), [](const Job &a, const Job &b)
         { return a.finish < b.finish; });
    return scheduleChronological(jobs);
}

// Strategy 2: Shortest Duration First
// Sorts jobs by duration and schedules non-overlapping ones
// NOT OPTIMAL - May fail to find maximum number of jobs
int shortestDurationFirst(vector<Job> jobs)
{
    // Sort by duration (shortest first)
    sort(jobs.begin(), jobs.end(), [](const Job &a, const Job &b)
         { return a.duration < b.duration; });
    return scheduleGeneral(jobs);
}

// Strategy 3: Earliest Start Time First
// Sorts jobs by start time and schedules greedily
// NOT OPTIMAL - May fail to find maximum number of jobs
int earliestStartTimeFirst(vector<Job> jobs)
{
    // Sort by start time (earliest first)
    sort(jobs.begin(), jobs.end(), [](const Job &a, const Job &b)
         { return a.start < b.start; });
    return scheduleChronological(jobs);
}

// Count Conflicts for FCF Strategy
// For each job, count how many other jobs overlap with it
// Parameters: jobs - Array of jobs
// Returns: Vector where conflicts[i] = number of jobs overlapping with job i
vector<int> countConflicts(const vector<Job> &jobs)
{
    int n = jobs.size();
    vector<int> conflicts(n, 0); // Initialize all conflicts to 0

    // Compare each pair of jobs
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            // Check if job i and job j overlap
            if (jobs[i].start < jobs[j].finish && jobs[j].start < jobs[i].finish)
            {
                conflicts[i]++; // Job i has one more conflict
                conflicts[j]++; // Job j has one more conflict
            }
        }
    }
    return conflicts;
}

// Strategy 4: Fewest Conflicts First
// Sorts jobs by number of conflicts (fewest first)
// NOT OPTIMAL - Graduate requirement, may fail to find maximum
int fewestConflictsFirst(vector<Job> jobs)
{
    // Count conflicts for each job
    vector<int> conflicts = countConflicts(jobs);

    // Create pairs of (conflict_count, job)
    vector<pair<int, Job>> jobsWithConflicts;
    for (int i = 0; i < jobs.size(); i++)
    {
        jobsWithConflicts.push_back({conflicts[i], jobs[i]});
    }

    // Sort by conflict count (fewest conflicts first)
    sort(jobsWithConflicts.begin(), jobsWithConflicts.end(),
         [](const pair<int, Job> &a, const pair<int, Job> &b)
         {
             return a.first < b.first; // Ascending order of conflicts
         });

    // Extract just the jobs (without conflict counts)
    vector<Job> sortedJobs;
    for (const auto &p : jobsWithConflicts)
    {
        sortedJobs.push_back(p.second);
    }

    return scheduleGeneral(sortedJobs);
}

// Main Function
// Runs the experiment, collects data, and displays results
int main()
{
    // Experiment setup
    const int RUNS = 1000; // Number of trials
    const int JOBS = 1000; // Jobs per trial

    // Vectors to store results for each strategy
    vector<int> eftResults, sdfResults, estResults, fcfResults;
    eftResults.reserve(RUNS);
    sdfResults.reserve(RUNS);
    estResults.reserve(RUNS);
    fcfResults.reserve(RUNS);

    // Variables to track the most notable run (largest performance gap)
    int maxDifference = 0;
    int notableRun = 0;
    int notableEFT = 0, notableSDF = 0, notableEST = 0, notableFCF = 0;

    // Display header
    cout << "\nCS456 Design and Analysis of Algorithms" << endl;
    cout << "Project 1 - Interval Scheduling" << endl;
    cout << "\nRunning 1000 trials (1000 jobs per trial)..." << endl;

    // Run experiment
    for (int run = 0; run < RUNS; run++)
    {
        // Generate random jobs for this trial
        vector<Job> jobs = createJobArray(JOBS);

        // Apply all four strategies
        int eft = earliestFinishTimeFirst(jobs);
        int sdf = shortestDurationFirst(jobs);
        int est = earliestStartTimeFirst(jobs);
        int fcf = fewestConflictsFirst(jobs);

        // Store results
        eftResults.push_back(eft);
        sdfResults.push_back(sdf);
        estResults.push_back(est);
        fcfResults.push_back(fcf);

        // Check if this run has the largest performance gap
        int difference = max(max(eft, sdf), max(est, fcf)) -
                         min(min(eft, sdf), min(est, fcf));
        if (difference > maxDifference)
        {
            maxDifference = difference;
            notableRun = run + 1;
            notableEFT = eft;
            notableSDF = sdf;
            notableEST = est;
            notableFCF = fcf;
        }
    }

    // Function to calculate high, mean and low value for a dataset
    auto summarize = [](const vector<int> &v)
    {
        int high = *max_element(v.begin(), v.end());
        double mean = 0;
        for (int x : v)
            mean += x;
        mean /= v.size();
        int low = *min_element(v.begin(), v.end());
        return make_tuple(high, mean, low);
    };

    // Get statistics for each strategy
    auto [eftHigh, eftMean, eftLow] = summarize(eftResults);
    auto [sdfHigh, sdfMean, sdfLow] = summarize(sdfResults);
    auto [estHigh, estMean, estLow] = summarize(estResults);
    auto [fcfHigh, fcfMean, fcfLow] = summarize(fcfResults);

    cout << "\nResults after 1000 trials (1000 jobs per trial):" << endl;
    cout << "\n"
         << setw(30) << "" << setw(14) << "High Value"
         << setw(14) << "Mean Value" << setw(12) << "Low Value" << endl;
    cout << string(75, '-') << endl;

    cout << left << setw(32) << "Earliest Finish Time (EFT)"
         << right << setw(10) << eftHigh
         << setw(16) << fixed << setprecision(2) << eftMean
         << setw(10) << eftLow << endl;

    cout << left << setw(32) << "Shortest Duration First (SDF)"
         << right << setw(10) << sdfHigh
         << setw(16) << fixed << setprecision(2) << sdfMean
         << setw(10) << sdfLow << endl;

    cout << left << setw(32) << "Earliest Start Time (EST)"
         << right << setw(10) << estHigh
         << setw(16) << fixed << setprecision(2) << estMean
         << setw(10) << estLow << endl;

    cout << left << setw(32) << "Fewest Conflicts First (FCF)"
         << right << setw(10) << fcfHigh
         << setw(16) << fixed << setprecision(2) << fcfMean
         << setw(10) << fcfLow << endl;

    cout << "\nRun with greatest performance gap between strategies: Run " << notableRun << endl;
    cout << "\nEFT = " << notableEFT << endl;
    cout << "SDF = " << notableSDF << endl;
    cout << "EST = " << notableEST << endl;
    cout << "FCF = " << notableFCF << endl;
    cout << "\nPerformance Gap (max - min) = " << max({notableEFT, notableSDF, notableEST, notableFCF})
         << " - " << min({notableEFT, notableSDF, notableEST, notableFCF})
         << " = " << maxDifference << endl;

    return 0;
}