// PROBLEM: parallel selection sort
// ANALYSIS: the OUTER loop (round i) can't be parallel - round i needs rounds 0..i-1 done.
//           the INNER work "find the smallest in a[i..n-1] and its position" CAN be parallel
//           -> that's min + position = own copy + critical, done in every round
// NOTE: n rounds each create threads -> big overhead, only helps for large n
#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <omp.h>
using namespace std;

int main() {
    int n = 20000;
    vector<int> a(n);
    for (int i = 0; i < n; i++)
        a[i] = rand() % 100000;

    // serial
    vector<int> s = a;
    double start = omp_get_wtime();
    for (int i = 0; i < n - 1; i++) {
        int pos = i;
        for (int j = i + 1; j < n; j++)
            if (s[j] < s[pos]) pos = j;
        swap(s[i], s[pos]);
    }
    double ts = omp_get_wtime() - start;
    cout << "N = " << n << "  serial time = " << ts << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        vector<int> p = a;
        start = omp_get_wtime();
        for (int i = 0; i < n - 1; i++) {              // rounds: serial
            int best = INT_MAX, bestPos = -1;
            #pragma omp parallel
            {
                int myBest = INT_MAX, myPos = -1;
                #pragma omp for
                for (int j = i; j < n; j++) {          // find min: parallel
                    if (p[j] < myBest) {
                        myBest = p[j];
                        myPos = j;
                    }
                }
                #pragma omp critical
                {
                    if (myBest < best || (myBest == best && myPos < bestPos)) {
                        best = myBest;
                        bestPos = myPos;
                    }
                }
            }
            swap(p[i], p[bestPos]);
        }
        double tp = omp_get_wtime() - start;
        bool sorted = true;
        for (int i = 0; i < n - 1; i++) if (p[i] > p[i + 1]) sorted = false;
        cout << "threads = " << t << "  time = " << tp << "  speedup = " << ts / tp
             << "  " << ((sorted && p == s) ? "MATCH" : "MISMATCH") << "\n";
    }
}
