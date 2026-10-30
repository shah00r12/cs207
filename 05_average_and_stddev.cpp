// PROBLEM: average and standard deviation of an array
// PATTERN: Rule 3 - reduction(+) for the sum; average = sum / n AFTER the loop
//          std dev needs the average first -> a SECOND loop with another reduction
// TRAP: (double)sum / n  -> otherwise whole-number division cuts the decimals
#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
using namespace std;

int main() {
    long n = 10000000;
    vector<long> a(n);
    for (long i = 0; i < n; i++)
        a[i] = i % 100;                       // average should be 49.5

    // serial
    double start = omp_get_wtime();
    long s1 = 0;
    for (long i = 0; i < n; i++) s1 += a[i];
    double avg1 = (double)s1 / n;
    double sq1 = 0;
    for (long i = 0; i < n; i++) sq1 += (a[i] - avg1) * (a[i] - avg1);
    double sd1 = sqrt(sq1 / n);
    double ts = omp_get_wtime() - start;
    cout << "serial: avg = " << avg1 << "  sd = " << sd1 << "  time = " << ts << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        start = omp_get_wtime();
        long s2 = 0;
        #pragma omp parallel for reduction(+:s2)
        for (long i = 0; i < n; i++) s2 += a[i];
        double avg2 = (double)s2 / n;

        double sq2 = 0;
        #pragma omp parallel for reduction(+:sq2)
        for (long i = 0; i < n; i++) sq2 += (a[i] - avg2) * (a[i] - avg2);
        double sd2 = sqrt(sq2 / n);
        double tp = omp_get_wtime() - start;

        bool ok = fabs(avg1 - avg2) < 1e-9 && fabs(sd1 - sd2) < 1e-6;
        cout << "threads = " << t << "  avg = " << avg2 << "  sd = " << sd2 << "  time = " << tp
             << "  speedup = " << ts / tp << "  " << (ok ? "MATCH" : "MISMATCH") << "\n";
    }
}
