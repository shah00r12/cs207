// PROBLEM: search a key: does it exist, FIRST position, LAST position, how many times
// PATTERN: Rule 3 - no break allowed in parallel for -> every thread checks its whole part
//   first position: reduction(min:pos), start pos = n (bigger than any real index), n -> -1 after
//   last position:  reduction(max:pos), start pos = -1
//   count:          reduction(+:count)
//   (pos and count are independent -> can share one loop)
#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    long n = 10000000;
    vector<long> a(n);
    for (long i = 0; i < n; i++)
        a[i] = i % 100;
    a[2000000] = 777;
    a[9000000] = 777;

    long keys[] = {777, 555};
    for (int k = 0; k < 2; k++) {
        long key = keys[k];

        // serial first position (break is fine here)
        long s = -1;
        for (long i = 0; i < n; i++)
            if (a[i] == key) { s = i; break; }

        long first = n, last = -1, count = 0;
        #pragma omp parallel for reduction(min:first) reduction(max:last) reduction(+:count)
        for (long i = 0; i < n; i++) {
            if (a[i] == key) {
                count++;
                if (i < first) first = i;
                if (i > last) last = i;
            }
        }
        if (first == n) first = -1;

        cout << "key = " << key << "  found = " << (count > 0 ? "yes" : "no") << "  first = " << first
             << "  last = " << last << "  count = " << count << "  " << (first == s ? "MATCH" : "MISMATCH") << "\n";
    }
}
