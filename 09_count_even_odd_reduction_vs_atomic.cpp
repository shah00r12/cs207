// PROBLEM: count even and odd numbers; compare reduction with atomic  (Lab 9 Task 3)
// PATTERN: Rule 3 - even and odd are INDEPENDENT (the if checks only a[i]) -> reduction(+:even, odd)
// CHECK: even + odd must equal n (every number is one or the other)
// WHY ATOMIC IS SLOW: threads take turns on every single ++ (millions of times)
#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
using namespace std;

int main() {
    long sizes[] = {100000, 1000000, 10000000, 100000000};
    for (int s = 0; s < 4; s++) {
        long n = sizes[s];
        vector<int> a(n);
        for (long i = 0; i < n; i++)
            a[i] = rand();

        long e1 = 0, o1 = 0;
        double start = omp_get_wtime();
        for (long i = 0; i < n; i++) {
            if (a[i] % 2 == 0) e1++;
            else o1++;
        }
        double ts = omp_get_wtime() - start;
        cout << "\nN = " << n << "  serial: even = " << e1 << ", odd = " << o1 << ", time = " << ts << "\n";

        for (int t = 1; t <= 8; t = t * 2) {
            omp_set_num_threads(t);

            long e2 = 0, o2 = 0;
            start = omp_get_wtime();
            #pragma omp parallel for reduction(+:e2, o2)
            for (long i = 0; i < n; i++) {
                if (a[i] % 2 == 0) e2++;
                else o2++;
            }
            double tr = omp_get_wtime() - start;

            long e3 = 0, o3 = 0;
            start = omp_get_wtime();
            #pragma omp parallel for
            for (long i = 0; i < n; i++) {
                if (a[i] % 2 == 0) {
                    #pragma omp atomic
                    e3++;
                } else {
                    #pragma omp atomic
                    o3++;
                }
            }
            double ta = omp_get_wtime() - start;

            bool ok = (e1 == e2 && o1 == o2 && e1 == e3 && o1 == o3 && e2 + o2 == n);
            cout << "threads = " << t << "  reduction = " << tr << " (speedup " << ts / tr << ", eff " << ts / tr / t << ")"
                 << "  atomic = " << ta << " (speedup " << ts / ta << ")  " << (ok ? "MATCH" : "MISMATCH") << "\n";
        }
    }
}
