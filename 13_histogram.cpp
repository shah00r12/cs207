// PROBLEM: histogram - count how many times each number 0..9 appears
// RACE: all threads do hist[a[i]]++ on the SAME 10 boxes -> lost counts
// PATTERN: Rule 4 - own local[10] per thread -> omp for -> critical merge once (FAST)
// ALSO: atomic (correct, slow), array reduction reduction(+:h[:10]) (OpenMP 4.5+, plain array only)
// CHECK: all counts add up to n
#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
using namespace std;

int main() {
    long n = 10000000;
    vector<int> a(n);
    for (long i = 0; i < n; i++)
        a[i] = rand() % 10;

    long hs[10] = {0};
    double start = omp_get_wtime();
    for (long i = 0; i < n; i++)
        hs[a[i]]++;
    double ts = omp_get_wtime() - start;
    cout << "serial histogram:";
    for (int k = 0; k < 10; k++) cout << "  " << k << ":" << hs[k];
    cout << "\nserial time = " << ts << "\n";

    // wrong version, to show the race
    omp_set_num_threads(4);
    long hw[10] = {0};
    #pragma omp parallel for
    for (long i = 0; i < n; i++)
        hw[a[i]]++;
    long tw = 0;
    for (int k = 0; k < 10; k++) tw += hw[k];
    cout << "WITHOUT protection total = " << tw << " (should be " << n << ")\n\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);

        long h1[10] = {0};
        start = omp_get_wtime();
        #pragma omp parallel for
        for (long i = 0; i < n; i++) {
            #pragma omp atomic
            h1[a[i]]++;
        }
        double ta = omp_get_wtime() - start;

        long h2[10] = {0};
        start = omp_get_wtime();
        #pragma omp parallel
        {
            long local[10] = {0};
            #pragma omp for
            for (long i = 0; i < n; i++)
                local[a[i]]++;
            #pragma omp critical
            {
                for (int k = 0; k < 10; k++)
                    h2[k] += local[k];
            }
        }
        double tl = omp_get_wtime() - start;

        long h3[10] = {0};
        start = omp_get_wtime();
        #pragma omp parallel for reduction(+:h3[:10])
        for (long i = 0; i < n; i++)
            h3[a[i]]++;
        double tr = omp_get_wtime() - start;

        bool ok = true;
        for (int k = 0; k < 10; k++)
            if (h1[k] != hs[k] || h2[k] != hs[k] || h3[k] != hs[k]) ok = false;
        cout << "threads = " << t << "  atomic = " << ta << "  local = " << tl << "  array-reduction = " << tr
             << "  speedup(local) = " << ts / tl << "  " << (ok ? "MATCH" : "MISMATCH") << "\n";
    }
}
