// PROBLEM: (a) brighten an image: +50 per pixel, cap at 255, COUNT the capped pixels
//          (b) threshold: pixel > 128 -> 255 else 0
// PATTERN: two patterns on one pragma: collapse(2) (own output pixel) + reduction(+:capped) (one counter)
// NOTE: memory-bound (very little math per pixel) -> small speedup is normal
#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    int N = 3;
    vector<int> img = {100, 200, 250, 10, 220, 205, 30, 40, 250}, out(N * N);
    long capped = 0;
    #pragma omp parallel for collapse(2) reduction(+:capped)
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            int v = img[i * N + j] + 50;
            if (v > 255) {
                v = 255;
                capped++;
            }
            out[i * N + j] = v;
        }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) cout << out[i * N + j] << "\t";
        cout << "\n";
    }
    cout << "capped = " << capped << " (3)\n";

    // big image: brightness + threshold
    N = 4000;
    vector<int> big((long)N * N), b1((long)N * N), b2((long)N * N), th((long)N * N);
    for (long k = 0; k < (long)N * N; k++) big[k] = k % 256;

    long c1 = 0;
    double start = omp_get_wtime();
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            int v = big[(long)i * N + j] + 50;
            if (v > 255) { v = 255; c1++; }
            b1[(long)i * N + j] = v;
        }
    double ts = omp_get_wtime() - start;
    cout << "\nN = " << N << "  serial time = " << ts << "  capped = " << c1 << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        long c2 = 0;
        start = omp_get_wtime();
        #pragma omp parallel for collapse(2) reduction(+:c2)
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++) {
                int v = big[(long)i * N + j] + 50;
                if (v > 255) { v = 255; c2++; }
                b2[(long)i * N + j] = v;
            }
        double tp = omp_get_wtime() - start;
        cout << "threads = " << t << "  time = " << tp << "  speedup = " << ts / tp
             << "  " << ((b1 == b2 && c1 == c2) ? "MATCH" : "MISMATCH") << "\n";
    }

    // threshold
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            th[(long)i * N + j] = (big[(long)i * N + j] > 128) ? 255 : 0;
    cout << "threshold: pixel 100 -> " << th[100] << ", pixel 200 -> " << th[200] << "\n";
}
