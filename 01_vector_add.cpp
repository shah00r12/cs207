// PROBLEM: c[i] = a[i] + b[i]  (vector addition)
// PATTERN: Rule 1 - each i writes its OWN box c[i]  ->  #pragma omp parallel for
// WHY: no two threads write the same place, a and b are only read -> no race
// VARIANTS (same code, change one line): c[i]=a[i]-b[i], c[i]=a[i]*b[i], c[i]=5*a[i],
//          c[i]=a[i]*a[i], c[i]=k*a[i]+b[i] (SAXPY), c[n-1-i]=a[i] (reverse)
#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    long n = 10000000;
    vector<double> a(n), b(n), c1(n), c2(n);
    for (long i = 0; i < n; i++) {
        a[i] = i;
        b[i] = 2 * i;
    }

    // serial
    double start = omp_get_wtime();
    for (long i = 0; i < n; i++)
        c1[i] = a[i] + b[i];
    double ts = omp_get_wtime() - start;
    cout << "N = " << n << "   serial time = " << ts << "\n";

    // parallel with 1, 2, 4, 8 threads
    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        start = omp_get_wtime();
        #pragma omp parallel for
        for (long i = 0; i < n; i++)
            c2[i] = a[i] + b[i];
        double tp = omp_get_wtime() - start;

        cout << "threads = " << t << "   time = " << tp << "   speedup = " << ts / tp
             << "   efficiency = " << ts / tp / t << "   " << (c1 == c2 ? "MATCH" : "MISMATCH") << "\n";
    }
    cout << "c[7] = " << c2[7] << "\n";
}
