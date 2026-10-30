// PROBLEM: element-wise vector operations: square + add, scale, reverse, max of two vectors
// PATTERN: Rule 1 - own box -> #pragma omp parallel for
// TIP: always use a SEPARATE output vector (reverse into b, not into a itself)
#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    long n = 10000000;
    vector<long> a(n), b(n);
    for (long i = 0; i < n; i++) {
        a[i] = i % 10;
        b[i] = 1;
    }
    vector<long> sq1(n), sq2(n), rev1(n), rev2(n), mx1(n), mx2(n);

    // serial
    double start = omp_get_wtime();
    for (long i = 0; i < n; i++) {
        sq1[i] = a[i] * a[i] + b[i];       // square + add
        rev1[n - 1 - i] = a[i];            // reverse into a new vector
        mx1[i] = max(a[i], b[i]);          // bigger of the two
    }
    double ts = omp_get_wtime() - start;
    cout << "serial time = " << ts << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        start = omp_get_wtime();
        #pragma omp parallel for
        for (long i = 0; i < n; i++) {
            sq2[i] = a[i] * a[i] + b[i];
            rev2[n - 1 - i] = a[i];        // still its own box: each i writes a different place
            mx2[i] = max(a[i], b[i]);
        }
        double tp = omp_get_wtime() - start;
        bool ok = (sq1 == sq2) && (rev1 == rev2) && (mx1 == mx2);
        cout << "threads = " << t << "   time = " << tp << "   speedup = " << ts / tp
             << "   efficiency = " << ts / tp / t << "   " << (ok ? "MATCH" : "MISMATCH") << "\n";
    }
    cout << "sq[7] = " << sq2[7] << " (7*7+1 = 50)\n";
}
