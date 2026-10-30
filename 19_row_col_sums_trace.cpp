// PROBLEM: sum of each row, sum of each column, trace (sum of the diagonal), total sum of a matrix
// row sums:  parallel for on i (each row writes rowSum[i]); j loop inside with local sum
// col sums:  parallel for on j (each column writes colSum[j]); i loop inside
// trace:     one number -> reduction(+:tr) over A[i*n+i]
// total:     one number -> reduction(+:total) with collapse(2)
#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    int n = 4;
    vector<long> A(n * n);
    for (int k = 0; k < n * n; k++) A[k] = k + 1;       // 1..16

    vector<long> rowSum(n), colSum(n);
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        long s = 0;
        for (int j = 0; j < n; j++) s += A[i * n + j];
        rowSum[i] = s;
    }
    #pragma omp parallel for
    for (int j = 0; j < n; j++) {
        long s = 0;
        for (int i = 0; i < n; i++) s += A[i * n + j];
        colSum[j] = s;
    }

    long tr = 0;
    #pragma omp parallel for reduction(+:tr)
    for (int i = 0; i < n; i++)
        tr += A[i * n + i];

    long total = 0;
    #pragma omp parallel for collapse(2) reduction(+:total)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            total += A[i * n + j];

    cout << "row sums: ";
    for (int i = 0; i < n; i++) cout << rowSum[i] << " ";
    cout << " (10 26 42 58)\ncol sums: ";
    for (int j = 0; j < n; j++) cout << colSum[j] << " ";
    cout << " (28 32 36 40)\ntrace = " << tr << " (1+6+11+16 = 34)\ntotal = " << total << " (136)\n";
}
