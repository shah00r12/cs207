// PROBLEM: T = transpose of A (rows become columns). A is R x C, T is C x R
// FORMULA: T(j, i) = A(i, j)  ->  T[j * R + i] = A[i * C + j]   (each matrix uses ITS OWN column count)
// PATTERN: Rule 2 - each (i, j) writes a different box of T -> collapse(2)
// TRAP: never transpose into A itself (other threads still need to read old values)
// FLIP UPSIDE DOWN variant: F[(R-1-i) * C + j] = A[i * C + j]
#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    // small example 3 x 4
    int R = 3, C = 4;
    vector<int> A(R * C), T(R * C);
    for (int k = 0; k < R * C; k++) A[k] = k + 1;

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++)
            T[j * R + i] = A[i * C + j];

    cout << "A:\n";
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) cout << A[i * C + j] << "\t";
        cout << "\n";
    }
    cout << "Transpose:\n";
    for (int i = 0; i < C; i++) {
        for (int j = 0; j < R; j++) cout << T[i * R + j] << "\t";
        cout << "\n";
    }

    // big square matrix
    int n = 4000;
    vector<double> B((long)n * n), T1((long)n * n), T2((long)n * n);
    for (long k = 0; k < (long)n * n; k++) B[k] = k;

    double start = omp_get_wtime();
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            T1[(long)j * n + i] = B[(long)i * n + j];
    double ts = omp_get_wtime() - start;
    cout << "\nN = " << n << "  serial time = " << ts << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        start = omp_get_wtime();
        #pragma omp parallel for collapse(2)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                T2[(long)j * n + i] = B[(long)i * n + j];
        double tp = omp_get_wtime() - start;
        cout << "threads = " << t << "  time = " << tp << "  speedup = " << ts / tp
             << "  efficiency = " << ts / tp / t << "  " << (T1 == T2 ? "MATCH" : "MISMATCH") << "\n";
    }
}
