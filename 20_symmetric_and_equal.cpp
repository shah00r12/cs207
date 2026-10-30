// PROBLEM: (a) is the matrix symmetric? A(i,j) == A(j,i) for all i, j
//          (b) are two arrays equal?   (c) is an array sorted?
// PATTERN: Rule 3 - one true/false for everything -> reduction(&&:ok), start ok = true
#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    int n = 1000;
    vector<long> A((long)n * n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            A[(long)i * n + j] = i + j;                // symmetric

    for (int test = 0; test < 2; test++) {
        bool ok = true;
        #pragma omp parallel for collapse(2) reduction(&&:ok)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                ok = ok && (A[(long)i * n + j] == A[(long)j * n + i]);
        cout << "symmetric? " << (ok ? "yes" : "no") << "\n";
        A[5 * n + 7] = 999;                            // break it for the second test
    }

    long m = 1000000;
    vector<long> a(m), b(m);
    for (long i = 0; i < m; i++) { a[i] = i; b[i] = i; }
    b[500] = -1;

    bool same = true, sorted = true;
    #pragma omp parallel for reduction(&&:same, sorted)
    for (long i = 0; i < m - 1; i++) {
        same = same && (a[i] == b[i]);
        sorted = sorted && (a[i] <= a[i + 1]);
    }
    cout << "a == b? " << (same ? "yes" : "no") << " (expected no)\n";
    cout << "a sorted? " << (sorted ? "yes" : "no") << " (expected yes)\n";
}
