// PROBLEM: (a) Fibonacci with tasks   (b) recursive (divide and conquer) array sum with tasks
// RECURSION TEMPLATE: base case -> cutoff -> task 1 -> task 2 -> taskwait -> combine
// shared(x): the task writes its answer into x; without shared, it writes into a copy that's thrown away
// CUTOFF MATTERS: too small a cutoff = too many tiny tasks = slower than serial
#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

long fibSerial(int n) {
    if (n < 2) return n;
    return fibSerial(n - 1) + fibSerial(n - 2);
}

long fibParallel(int n) {
    if (n < 25) return fibSerial(n);              // cutoff
    long x, y;
    #pragma omp task shared(x)
    x = fibParallel(n - 1);
    #pragma omp task shared(y)
    y = fibParallel(n - 2);
    #pragma omp taskwait
    return x + y;
}

long sumSerial(vector<long> &a, long lo, long hi) {      // sum of a[lo..hi-1]
    long s = 0;
    for (long i = lo; i < hi; i++) s += a[i];
    return s;
}

long sumParallel(vector<long> &a, long lo, long hi) {
    if (hi - lo < 100000) return sumSerial(a, lo, hi);    // cutoff
    long mid = (lo + hi) / 2, left, right;
    #pragma omp task shared(a, left)
    left = sumParallel(a, lo, mid);
    #pragma omp task shared(a, right)
    right = sumParallel(a, mid, hi);
    #pragma omp taskwait
    return left + right;
}

int main() {
    int n = 40;
    double start = omp_get_wtime();
    long f1 = fibSerial(n);
    double ts = omp_get_wtime() - start;

    long f2 = 0;
    start = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        f2 = fibParallel(n);
    }
    double tp = omp_get_wtime() - start;
    cout << "fib(" << n << ") = " << f2 << "  serial = " << ts << "  parallel = " << tp
         << "  speedup = " << ts / tp << "  " << (f1 == f2 ? "MATCH" : "MISMATCH") << "\n";

    long m = 10000000;
    vector<long> a(m);
    for (long i = 0; i < m; i++) a[i] = i % 100;
    long s1 = sumSerial(a, 0, m), s2 = 0;
    #pragma omp parallel
    {
        #pragma omp single
        s2 = sumParallel(a, 0, m);
    }
    cout << "recursive sum = " << s2 << "  " << (s1 == s2 ? "MATCH" : "MISMATCH") << "\n";
}
