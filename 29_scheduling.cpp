// PROBLEM: compare schedule(static), schedule(dynamic, chunk), schedule(guided) on UNEVEN work (Lab 9 Task 2)
// static:  equal split at the start (best when every turn costs the same, no overhead)
// dynamic: take a chunk, come back for more (balances uneven work, small overhead per chunk)
// guided:  like dynamic but chunks start big and shrink (fewer trips)
// RESULT: sir's work (i % 1000)*100 REPEATS -> static already fair, all similar.
//         work that GROWS with i -> static gives the last thread all heavy turns -> dynamic wins.
#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
using namespace std;

// mode 0 = sir's repeating work, mode 1 = work grows with i
int workOf(long i, int mode) {
    if (mode == 0) return (i % 1000) * 100;
    return i / 2;
}

int main() {
    long n = 50000;
    const char *names[] = {"repeating work (sir's)", "growing work (i / 2)"};

    for (int mode = 0; mode < 2; mode++) {
        vector<double> a(n);
        for (long i = 0; i < n; i++) a[i] = i * 0.001;
        vector<double> s = a, p1 = a, p2 = a, p3 = a;

        double start = omp_get_wtime();
        for (long i = 0; i < n; i++) {
            double x = s[i];
            int w = workOf(i, mode);
            for (int j = 0; j < w; j++) x = x * 1.0000001 + 0.0000001;
            s[i] = x;
        }
        double ts = omp_get_wtime() - start;

        start = omp_get_wtime();
        #pragma omp parallel for schedule(static)
        for (long i = 0; i < n; i++) {
            double x = p1[i];
            int w = workOf(i, mode);
            for (int j = 0; j < w; j++) x = x * 1.0000001 + 0.0000001;
            p1[i] = x;
        }
        double t1 = omp_get_wtime() - start;

        start = omp_get_wtime();
        #pragma omp parallel for schedule(dynamic, 100)
        for (long i = 0; i < n; i++) {
            double x = p2[i];
            int w = workOf(i, mode);
            for (int j = 0; j < w; j++) x = x * 1.0000001 + 0.0000001;
            p2[i] = x;
        }
        double t2 = omp_get_wtime() - start;

        start = omp_get_wtime();
        #pragma omp parallel for schedule(guided)
        for (long i = 0; i < n; i++) {
            double x = p3[i];
            int w = workOf(i, mode);
            for (int j = 0; j < w; j++) x = x * 1.0000001 + 0.0000001;
            p3[i] = x;
        }
        double t3 = omp_get_wtime() - start;

        bool ok = true;
        for (long i = 0; i < n; i++)
            if (fabs(s[i] - p1[i]) > 1e-9 || fabs(s[i] - p2[i]) > 1e-9 || fabs(s[i] - p3[i]) > 1e-9) ok = false;
        int t = omp_get_max_threads();
        cout << names[mode] << "  (threads = " << t << ")\n";
        cout << "  serial  = " << ts << "\n";
        cout << "  static  = " << t1 << "  speedup = " << ts / t1 << "  eff = " << ts / t1 / t << "\n";
        cout << "  dynamic = " << t2 << "  speedup = " << ts / t2 << "  eff = " << ts / t2 / t << "\n";
        cout << "  guided  = " << t3 << "  speedup = " << ts / t3 << "  eff = " << ts / t3 / t << "\n";
        cout << "  " << (ok ? "MATCH" : "MISMATCH") << "\n";
    }
}
