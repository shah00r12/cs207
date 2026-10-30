// PROBLEM: count primes (and sum of primes) from 2 to N
// PATTERN: one counter -> reduction(+:count); bigger numbers take longer to check (uneven work)
//          -> schedule(dynamic, 100) balances it
// Expected: 9592 primes up to 100000, their sum = 454396537
#include <iostream>
#include <omp.h>
using namespace std;

bool isPrime(long x) {
    if (x < 2) return false;
    for (long d = 2; d * d <= x; d++)
        if (x % d == 0) return false;
    return true;
}

int main() {
    long N = 3000000;
    long c1 = 0, sum1 = 0;
    double start = omp_get_wtime();
    for (long x = 2; x <= N; x++)
        if (isPrime(x)) { c1++; sum1 += x; }
    double ts = omp_get_wtime() - start;
    cout << "N = " << N << "  serial count = " << c1 << "  time = " << ts << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);

        long c2 = 0, sum2 = 0;
        start = omp_get_wtime();
        #pragma omp parallel for reduction(+:c2, sum2) schedule(static)
        for (long x = 2; x <= N; x++)
            if (isPrime(x)) { c2++; sum2 += x; }
        double tst = omp_get_wtime() - start;

        long c3 = 0, sum3 = 0;
        start = omp_get_wtime();
        #pragma omp parallel for reduction(+:c3, sum3) schedule(dynamic, 100)
        for (long x = 2; x <= N; x++)
            if (isPrime(x)) { c3++; sum3 += x; }
        double tdy = omp_get_wtime() - start;

        bool ok = (c1 == c2 && c1 == c3 && sum1 == sum2 && sum1 == sum3);
        cout << "threads = " << t << "  static = " << tst << "  dynamic = " << tdy
             << "  speedup(dynamic) = " << ts / tdy << "  " << (ok ? "MATCH" : "MISMATCH") << "\n";
    }

    long c = 0, s = 0;
    #pragma omp parallel for reduction(+:c, s) schedule(dynamic, 100)
    for (long x = 2; x <= 100000; x++)
        if (isPrime(x)) { c++; s += x; }
    cout << "up to 100000: count = " << c << " (9592), sum = " << s << " (454396537)\n";
}
