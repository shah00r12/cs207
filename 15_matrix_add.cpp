// PROBLEM: C = A + B for N x N matrices  (also: subtract, scale by k)
// STORAGE: 1D vector, row by row. Box (i, j) is at M[i * cols + j]
// PATTERN: Rule 2 - each (i, j) writes its own box -> parallel for collapse(2)
// collapse(2): joins both loops into one big loop so work is divided better (matters for small N)
#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    int n = 4000;
    vector<double> A((long)n * n), B((long)n * n), C1((long)n * n), C2((long)n * n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            A[(long)i * n + j] = i + j;
            B[(long)i * n + j] = 2;
        }

    double start = omp_get_wtime();
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C1[(long)i * n + j] = A[(long)i * n + j] + B[(long)i * n + j];
    double ts = omp_get_wtime() - start;
    cout << "N = " << n << "  serial time = " << ts << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        start = omp_get_wtime();
        #pragma omp parallel for collapse(2)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                C2[(long)i * n + j] = A[(long)i * n + j] + B[(long)i * n + j];
        double tp = omp_get_wtime() - start;
        cout << "threads = " << t << "  time = " << tp << "  speedup = " << ts / tp
             << "  efficiency = " << ts / tp / t << "  " << (C1 == C2 ? "MATCH" : "MISMATCH") << "\n";
    }
    cout << "C(3,5) = " << C2[3 * n + 5] << " (3+5+2 = 10)\n";
}
