// PROBLEM: prefix sum (running total): out[i] = a[0] + a[1] + ... + a[i]
// ANALYSIS: out[i] = out[i-1] + a[i] -> each step needs the previous one -> parallel for gives WRONG answers
// FIX (block method): split into one block per thread
//   pass 1: each thread does a running total of its own block (parallel)
//   middle: one thread adds up the block totals (single)
//   pass 2: each thread adds the total of all earlier blocks to its elements (parallel)
// barrier between the passes. Also shows manual loop bounds with omp_get_thread_num().
#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    long n = 10000000;
    vector<long> a(n), s(n), p(n);
    for (long i = 0; i < n; i++) a[i] = i % 7;

    // serial
    s[0] = a[0];
    for (long i = 1; i < n; i++)
        s[i] = s[i - 1] + a[i];

    // WRONG: plain parallel for (shows the dependency problem)
    vector<long> w(n);
    w[0] = a[0];
    #pragma omp parallel for
    for (long i = 1; i < n; i++)
        w[i] = w[i - 1] + a[i];
    cout << "plain parallel for: " << (w == s ? "MATCH (lucky)" : "WRONG (dependency!)") << "\n";

    // correct block method
    vector<long> block(omp_get_max_threads() + 1, 0);
    #pragma omp parallel
    {
        int t = omp_get_thread_num(), nt = omp_get_num_threads();
        long lo = n * t / nt, hi = n * (t + 1) / nt;      // my block

        long run = 0;
        for (long i = lo; i < hi; i++) {                  // pass 1
            run += a[i];
            p[i] = run;
        }
        block[t + 1] = run;

        #pragma omp barrier
        #pragma omp single
        for (int k = 1; k <= nt; k++)                     // block totals
            block[k] += block[k - 1];
        // implicit barrier after single

        long off = block[t];
        for (long i = lo; i < hi; i++)                    // pass 2
            p[i] += off;
    }
    cout << "block method: " << (p == s ? "MATCH" : "MISMATCH") << "  last value = " << p[n - 1] << "\n";
}
