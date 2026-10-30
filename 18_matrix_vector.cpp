// PROBLEM: y = A x  (matrix times vector). y[i] = row i of A times x, added up
// PATTERN: Rule 1 on ROWS only -> parallel for on i; local sum for the j loop
// TRAP: do NOT collapse(2) - all j of one row add into the same y[i] -> race
// TRAP: i = row (goes to n, writes y[i]); j = column (uses A[i*n+j] and x[j])
#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
using namespace std;

int main() {
    int n = 3;
    vector<double> A = {1, 2, 3, 4, 5, 6, 7, 8, 9}, x = {1, 1, 1}, y(n);
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        double sum = 0;
        for (int j = 0; j < n; j++)
            sum += A[i * n + j] * x[j];
        y[i] = sum;
    }
    cout << "y = " << y[0] << " " << y[1] << " " << y[2] << "  (6 15 24)\n";

    n = 5000;
    vector<double> B((long)n * n), v(n), y1(n), y2(n);
    for (long k = 0; k < (long)n * n; k++) B[k] = k % 10;
    for (int j = 0; j < n; j++) v[j] = 1;

    double start = omp_get_wtime();
    for (int i = 0; i < n; i++) {
        double sum = 0;
        for (int j = 0; j < n; j++)
            sum += B[(long)i * n + j] * v[j];
        y1[i] = sum;
    }
    double ts = omp_get_wtime() - start;
    cout << "\nN = " << n << "  serial time = " << ts << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        start = omp_get_wtime();
        #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            double sum = 0;
            for (int j = 0; j < n; j++)
                sum += B[(long)i * n + j] * v[j];
            y2[i] = sum;
        }
        double tp = omp_get_wtime() - start;
        bool ok = true;
        for (int i = 0; i < n; i++) if (fabs(y1[i] - y2[i]) > 1e-6) ok = false;
        cout << "threads = " << t << "  time = " << tp << "  speedup = " << ts / tp
             << "  efficiency = " << ts / tp / t << "  " << (ok ? "MATCH" : "MISMATCH") << "\n";
    }
}
