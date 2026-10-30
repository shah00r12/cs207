// PROBLEM: biggest number AND its position
// PATTERN: Rule 4 - value and position are DEPENDENT (position must come from the same thread
//          as the winning value) -> reduction can't do it.
//          parallel { own copy -> omp for -> critical merge once per thread }
// TIE-BREAK: (myMax == big && myPos < pos) keeps the FIRST position, same as serial
// TRAPS: { } on the if (2 lines), { } after critical, spell "critical"
#include <iostream>
#include <vector>
#include <climits>
#include <omp.h>
using namespace std;

int main() {
    long n = 10000000;
    vector<long> a(n);
    for (long i = 0; i < n; i++)
        a[i] = i % 100;
    a[5555555] = 500;
    a[8888888] = 500;                      // second copy: answer must still be 5555555

    // serial
    double start = omp_get_wtime();
    long big1 = INT_MIN, pos1 = -1;
    for (long i = 0; i < n; i++) {
        if (a[i] > big1) {
            big1 = a[i];
            pos1 = i;
        }
    }
    double ts = omp_get_wtime() - start;
    cout << "serial: " << big1 << " at " << pos1 << "  time = " << ts << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        start = omp_get_wtime();
        long big2 = INT_MIN, pos2 = -1;
        #pragma omp parallel
        {
            long myBig = INT_MIN, myPos = -1;      // own copy (private)
            #pragma omp for
            for (long i = 0; i < n; i++) {
                if (a[i] > myBig) {
                    myBig = a[i];
                    myPos = i;
                }
            }
            #pragma omp critical                   // one thread at a time
            {
                if (myBig > big2 || (myBig == big2 && myPos < pos2)) {
                    big2 = myBig;
                    pos2 = myPos;
                }
            }
        }
        double tp = omp_get_wtime() - start;
        bool ok = (big1 == big2 && pos1 == pos2);
        cout << "threads = " << t << "  " << big2 << " at " << pos2 << "  time = " << tp
             << "  speedup = " << ts / tp << "  " << (ok ? "MATCH" : "MISMATCH") << "\n";
    }
}
