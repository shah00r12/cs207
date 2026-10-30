// PROBLEM: show the race condition in a parallel sum, and the 4 ways to fix it
// WRONG:  parallel for without reduction -> threads overwrite each other's updates
// FIX 1: reduction (best)          FIX 2: critical (correct, very slow)
// FIX 3: atomic (correct, slow)    FIX 4: own copy + critical once (fast)
// WHY RACE: sum += a[i] is read -> add -> write. Two threads read the same old value, one update is lost.
#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    long n = 10000000;
    vector<long> a(n);
    for (long i = 0; i < n; i++)
        a[i] = i % 100;
    omp_set_num_threads(4);

    long s0 = 0;
    for (long i = 0; i < n; i++) s0 += a[i];
    cout << "serial      = " << s0 << "\n";

    // WRONG (race)
    for (int run = 0; run < 3; run++) {
        long w = 0;
        #pragma omp parallel for
        for (long i = 0; i < n; i++)
            w += a[i];
        cout << "wrong run " << run << " = " << w << (w == s0 ? "" : "   <-- WRONG") << "\n";
    }

    // FIX 1: reduction
    double start = omp_get_wtime();
    long s1 = 0;
    #pragma omp parallel for reduction(+:s1)
    for (long i = 0; i < n; i++)
        s1 += a[i];
    double t1 = omp_get_wtime() - start;

    // FIX 2: critical
    start = omp_get_wtime();
    long s2 = 0;
    #pragma omp parallel for
    for (long i = 0; i < n; i++) {
        #pragma omp critical
        s2 += a[i];
    }
    double t2 = omp_get_wtime() - start;

    // FIX 3: atomic
    start = omp_get_wtime();
    long s3 = 0;
    #pragma omp parallel for
    for (long i = 0; i < n; i++) {
        #pragma omp atomic
        s3 += a[i];
    }
    double t3 = omp_get_wtime() - start;

    // FIX 4: own copy, add once
    start = omp_get_wtime();
    long s4 = 0;
    #pragma omp parallel
    {
        long my = 0;
        #pragma omp for
        for (long i = 0; i < n; i++)
            my += a[i];
        #pragma omp critical
        s4 += my;
    }
    double t4 = omp_get_wtime() - start;

    cout << "reduction   = " << s1 << "   time = " << t1 << "\n";
    cout << "critical    = " << s2 << "   time = " << t2 << "\n";
    cout << "atomic      = " << s3 << "   time = " << t3 << "\n";
    cout << "own copy    = " << s4 << "   time = " << t4 << "\n";
    cout << ((s1 == s0 && s2 == s0 && s3 == s0 && s4 == s0) ? "ALL FIXES MATCH" : "MISMATCH") << "\n";
}
