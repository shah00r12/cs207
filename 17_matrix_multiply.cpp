// PROBLEM: C = A x B (N x N).  C(i,j) = sum over k of A(i,k) * B(k,j)
// PATTERN: Rule 2 - collapse(2) on i and j (each writes its own C box); local sum; k loop inside
// NEVER collapse/parallelise k: all k steps add into the SAME C(i,j) -> race
// BONUS: i-k-j loop order is much faster (inner loop walks memory row by row = cache friendly)
#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
using namespace std;

int main() {
    // small example
    int n = 2;
    vector<double> A = {1, 2, 3, 4}, B = {5, 6, 7, 8}, C(4);
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++)
                sum += A[i * n + k] * B[k * n + j];
            C[i * n + j] = sum;
        }
    cout << "2x2 result: " << C[0] << " " << C[1] << " / " << C[2] << " " << C[3] << "  (19 22 / 43 50)\n";

    // big
    n = 500;
    vector<double> X(n * n), Y(n * n), C1(n * n), C2(n * n), C3(n * n);
    for (int k = 0; k < n * n; k++) {
        X[k] = k % 10;
        Y[k] = k % 7;
    }

    double start = omp_get_wtime();
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++)
                sum += X[i * n + k] * Y[k * n + j];
            C1[i * n + j] = sum;
        }
    double ts = omp_get_wtime() - start;
    cout << "\nN = " << n << "  serial time = " << ts << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        start = omp_get_wtime();
        #pragma omp parallel for collapse(2)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                double sum = 0;
                for (int k = 0; k < n; k++)
                    sum += X[i * n + k] * Y[k * n + j];
                C2[i * n + j] = sum;
            }
        double tp = omp_get_wtime() - start;

        // i-k-j order (bonus, faster)
        for (int k = 0; k < n * n; k++) C3[k] = 0;
        double s2 = omp_get_wtime();
        #pragma omp parallel for
        for (int i = 0; i < n; i++)
            for (int k = 0; k < n; k++) {
                double aik = X[i * n + k];
                for (int j = 0; j < n; j++)
                    C3[i * n + j] += aik * Y[k * n + j];
            }
        double tk = omp_get_wtime() - s2;

        bool ok = true;
        for (int k = 0; k < n * n; k++)
            if (fabs(C1[k] - C2[k]) > 1e-6 || fabs(C1[k] - C3[k]) > 1e-6) ok = false;
        cout << "threads = " << t << "  time = " << tp << "  speedup = " << ts / tp
             << "  efficiency = " << ts / tp / t << "  ikj time = " << tk << "  " << (ok ? "MATCH" : "MISMATCH") << "\n";
    }
}
