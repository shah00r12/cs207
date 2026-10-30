// PROBLEM: compute pi by numerical integration of 4 / (1 + x^2) from 0 to 1
// PATTERN: one shared sum -> reduction(+:sum). x is made inside the loop (private)
#include <iostream>
#include <iomanip>
#include <cmath>
#include <omp.h>
using namespace std;

int main() {
    long steps = 100000000;
    double h = 1.0 / steps;

    double start = omp_get_wtime();
    double s1 = 0;
    for (long i = 0; i < steps; i++) {
        double x = (i + 0.5) * h;
        s1 += 4.0 / (1.0 + x * x);
    }
    double pi1 = s1 * h;
    double ts = omp_get_wtime() - start;
    cout << setprecision(12) << "serial pi = " << pi1 << "  time = " << ts << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        start = omp_get_wtime();
        double s2 = 0;
        #pragma omp parallel for reduction(+:s2)
        for (long i = 0; i < steps; i++) {
            double x = (i + 0.5) * h;
            s2 += 4.0 / (1.0 + x * x);
        }
        double pi2 = s2 * h;
        double tp = omp_get_wtime() - start;
        cout << "threads = " << t << "  pi = " << pi2 << "  time = " << tp << "  speedup = " << ts / tp
             << "  " << (fabs(pi1 - pi2) < 1e-9 ? "MATCH" : "MISMATCH") << "\n";
    }
}
