// PROBLEM: sum of an array
// PATTERN: Rule 3 - everyone adds into ONE number -> reduction(+:sum)
// WHY: each thread gets a private sum = 0, all private sums are added once at the end
// TRAP: use long for big sums (int overflows above ~2.1 billion)
#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    long n = 10000000;
    vector<long> a(n);
    for (long i = 0; i < n; i++)
        a[i] = i % 100;

    double start = omp_get_wtime();
    long s1 = 0;
    for (long i = 0; i < n; i++)
        s1 += a[i];
    double ts = omp_get_wtime() - start;
    cout << "serial sum = " << s1 << "   time = " << ts << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        start = omp_get_wtime();
        long s2 = 0;
        #pragma omp parallel for reduction(+:s2)
        for (long i = 0; i < n; i++)
            s2 += a[i];
        double tp = omp_get_wtime() - start;
        cout << "threads = " << t << "   sum = " << s2 << "   time = " << tp << "   speedup = " << ts / tp
             << "   efficiency = " << ts / tp / t << "   " << (s1 == s2 ? "MATCH" : "MISMATCH") << "\n";
    }
}
