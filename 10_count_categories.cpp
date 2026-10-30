// PROBLEM: count positive / negative / zero, AND multiples of 3 / multiples of 5
// PATTERN: Rule 3 - counters are independent -> reduction(+:pos, neg, zero)
// NOTE: "multiples of 3 AND multiples of 5" = two separate ifs (15 counts for both)
//       "multiples of 3 and 5" (both at once) = one if with && -> multiples of 15
#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    long n = 1000000;
    vector<long> a(n);
    for (long i = 0; i < n; i++)
        a[i] = i % 21 - 10;                  // -10 to 10

    long pos = 0, neg = 0, zero = 0;
    #pragma omp parallel for reduction(+:pos, neg, zero)
    for (long i = 0; i < n; i++) {
        if (a[i] > 0) pos++;
        else if (a[i] < 0) neg++;
        else zero++;
    }
    cout << "positive = " << pos << "  negative = " << neg << "  zero = " << zero
         << "  total = " << pos + neg + zero << " (should be " << n << ")\n";

    vector<long> b(n);
    for (long i = 0; i < n; i++) b[i] = i;
    long c3 = 0, c5 = 0, c15 = 0;
    #pragma omp parallel for reduction(+:c3, c5, c15)
    for (long i = 0; i < n; i++) {
        if (b[i] % 3 == 0) c3++;               // separate ifs
        if (b[i] % 5 == 0) c5++;
        if (b[i] % 3 == 0 && b[i] % 5 == 0) c15++;
    }
    cout << "multiples of 3 = " << c3 << "  multiples of 5 = " << c5 << "  multiples of both = " << c15 << "\n";
}
