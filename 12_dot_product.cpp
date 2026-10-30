// PROBLEM: dot product of two vectors = a[0]*b[0] + a[1]*b[1] + ...
// PATTERN: Rule 3 - one shared sum -> reduction(+:dot)
// CHECK: double -> compare with fabs(x - y) < 1e-6 (adding order can change the last digit)
#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
using namespace std;

int main() {
    long n = 10000000;
    vector<double> a(n), b(n);
    for (long i = 0; i < n; i++) {
        a[i] = i % 10;
        b[i] = 2;
    }

    double start = omp_get_wtime();
    double d1 = 0;
    for (long i = 0; i < n; i++)
        d1 += a[i] * b[i];
    double ts = omp_get_wtime() - start;
    cout << "serial dot = " << d1 << "  time = " << ts << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        start = omp_get_wtime();
        double d2 = 0;
        #pragma omp parallel for reduction(+:d2)
        for (long i = 0; i < n; i++)
            d2 += a[i] * b[i];
        double tp = omp_get_wtime() - start;
        cout << "threads = " << t << "  dot = " << d2 << "  time = " << tp << "  speedup = " << ts / tp
             << "  " << (fabs(d1 - d2) < 1e-6 ? "MATCH" : "MISMATCH") << "\n";
    }
}
