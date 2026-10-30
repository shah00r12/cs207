// PROBLEM: biggest, smallest and range (max - min) in ONE loop
// PATTERN: Rule 3 - max and min are INDEPENDENT -> two reductions on one line
//          reduction(max:mx) reduction(min:mn)
// START VALUES: mx = INT_MIN, mn = INT_MAX (need <climits>)
#include <iostream>
#include <vector>
#include <climits>
#include <omp.h>
using namespace std;

int main() {
    long n = 10000000;
    vector<long> a(n);
    for (long i = 0; i < n; i++)
        a[i] = i % 1000;
    a[123] = -50;
    a[777777] = 5000;

    double start = omp_get_wtime();
    long mx1 = INT_MIN, mn1 = INT_MAX;
    for (long i = 0; i < n; i++) {
        if (a[i] > mx1) mx1 = a[i];
        if (a[i] < mn1) mn1 = a[i];
    }
    double ts = omp_get_wtime() - start;
    cout << "serial: max = " << mx1 << "  min = " << mn1 << "  range = " << mx1 - mn1 << "  time = " << ts << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        start = omp_get_wtime();
        long mx2 = INT_MIN, mn2 = INT_MAX;
        #pragma omp parallel for reduction(max:mx2) reduction(min:mn2)
        for (long i = 0; i < n; i++) {
            if (a[i] > mx2) mx2 = a[i];
            if (a[i] < mn2) mn2 = a[i];
        }
        double tp = omp_get_wtime() - start;
        bool ok = (mx1 == mx2 && mn1 == mn2);
        cout << "threads = " << t << "  max = " << mx2 << "  min = " << mn2 << "  range = " << mx2 - mn2
             << "  time = " << tp << "  speedup = " << ts / tp << "  " << (ok ? "MATCH" : "MISMATCH") << "\n";
    }
}
