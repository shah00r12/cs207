// PROBLEM: 2D convolution - slide a 3x3 filter over an image (blur / edge / sharpen)
// OUTPUT SIZE: (R - 2) x (C - 2)   [general: R - k + 1 for a k x k filter, no padding]
// out(i,j) = sum over x,y = 0..2 of img(i+x, j+y) * K[x][y], then divide
// PATTERN: Rule 2 - each output pixel is its own box -> collapse(2); local sum; image only READ
// TRAP: write into a SEPARATE output array, never into img
#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
using namespace std;

void conv(vector<double> &img, vector<double> &out, int R, int C, double K[3][3], double div) {
    int outR = R - 2, outC = C - 2;
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < outR; i++)
        for (int j = 0; j < outC; j++) {
            double sum = 0;
            for (int x = 0; x < 3; x++)
                for (int y = 0; y < 3; y++)
                    sum += img[(long)(i + x) * C + (j + y)] * K[x][y];
            out[(long)i * outC + j] = sum / div;
        }
}

void convSerial(vector<double> &img, vector<double> &out, int R, int C, double K[3][3], double div) {
    int outR = R - 2, outC = C - 2;
    for (int i = 0; i < outR; i++)
        for (int j = 0; j < outC; j++) {
            double sum = 0;
            for (int x = 0; x < 3; x++)
                for (int y = 0; y < 3; y++)
                    sum += img[(long)(i + x) * C + (j + y)] * K[x][y];
            out[(long)i * outC + j] = sum / div;
        }
}

int main() {
    double avg[3][3]     = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};          // blur, divide by 9
    double edge[3][3]    = {{0, 1, 0}, {1, -4, 1}, {0, 1, 0}};         // edges, divide by 1
    double sharpen[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};      // sharpen, divide by 1

    vector<double> img = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150,
                          160, 170, 180, 190, 200, 210, 220, 230, 240, 250};
    vector<double> out(9);

    conv(img, out, 5, 5, avg, 9);
    cout << "average filter:\n";
    for (int i = 0; i < 3; i++) { for (int j = 0; j < 3; j++) cout << out[i * 3 + j] << "\t"; cout << "\n"; }

    conv(img, out, 5, 5, edge, 1);
    cout << "edge filter (all 0 because the image changes evenly):\n";
    for (int i = 0; i < 3; i++) { for (int j = 0; j < 3; j++) cout << out[i * 3 + j] << "\t"; cout << "\n"; }

    conv(img, out, 5, 5, sharpen, 1);
    cout << "sharpen filter:\n";
    for (int i = 0; i < 3; i++) { for (int j = 0; j < 3; j++) cout << out[i * 3 + j] << "\t"; cout << "\n"; }

    // big image timing
    int n = 2000;
    vector<double> big((long)n * n), o1((long)(n - 2) * (n - 2)), o2((long)(n - 2) * (n - 2));
    for (long k = 0; k < (long)n * n; k++) big[k] = k % 256;

    double start = omp_get_wtime();
    convSerial(big, o1, n, n, avg, 9);
    double ts = omp_get_wtime() - start;
    cout << "\nN = " << n << "  serial time = " << ts << "\n";

    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        start = omp_get_wtime();
        conv(big, o2, n, n, avg, 9);
        double tp = omp_get_wtime() - start;
        bool ok = true;
        for (long k = 0; k < (long)o1.size(); k++) if (fabs(o1[k] - o2[k]) > 1e-9) ok = false;
        cout << "threads = " << t << "  time = " << tp << "  speedup = " << ts / tp
             << "  efficiency = " << ts / tp / t << "  " << (ok ? "MATCH" : "MISMATCH") << "\n";
    }
}
