// PROBLEM: parallel bubble sort
// WHY NORMAL BUBBLE CAN'T: compare (j, j+1) then (j+1, j+2) both touch a[j+1] -> each step needs the previous
// FIX: odd-even transposition sort
//   even round: pairs (0,1)(2,3)(4,5)...   odd round: pairs (1,2)(3,4)...
//   pairs in one round never share an element -> all can swap at the same time
//   n rounds -> sorted
// SYNC: a BARRIER between rounds (next round needs this round's swaps). omp for gives it automatically.
//       NEVER add nowait. parallel is OUTSIDE the round loop (threads created once).
// DESCENDING: swap when a[j] < a[j+1]
#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
using namespace std;

int main() {
    int n = 20000;
    vector<int> a(n);
    for (int i = 0; i < n; i++)
        a[i] = rand() % 100000;

    // normal bubble sort (for comparison)
    vector<int> b = a;
    double start = omp_get_wtime();
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - 1 - i; j++)
            if (b[j] > b[j + 1]) swap(b[j], b[j + 1]);
    double tb = omp_get_wtime() - start;

    // odd-even serial
    vector<int> s = a;
    start = omp_get_wtime();
    for (int round = 0; round < n; round++) {
        int st = round % 2;
        for (int j = st; j < n - 1; j += 2)
            if (s[j] > s[j + 1]) swap(s[j], s[j + 1]);
    }
    double ts = omp_get_wtime() - start;
    cout << "N = " << n << "  normal bubble = " << tb << "  odd-even serial = " << ts << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        vector<int> p = a;
        start = omp_get_wtime();
        #pragma omp parallel
        for (int round = 0; round < n; round++) {
            int st = round % 2;
            #pragma omp for
            for (int j = st; j < n - 1; j += 2)
                if (p[j] > p[j + 1]) swap(p[j], p[j + 1]);
        }
        double tp = omp_get_wtime() - start;

        bool sorted = true;
        for (int i = 0; i < n - 1; i++) if (p[i] > p[i + 1]) sorted = false;
        cout << "threads = " << t << "  time = " << tp << "  speedup = " << ts / tp
             << "  efficiency = " << ts / tp / t << "  " << ((sorted && p == s && p == b) ? "MATCH" : "MISMATCH") << "\n";
    }
}
