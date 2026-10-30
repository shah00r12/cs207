// PROBLEM: parallel insertion sort
// ANALYSIS: normal insertion sort can NOT be parallelised directly:
//           inserting a[i] needs a[0..i-1] already sorted (each step depends on the previous).
// WORKAROUND: split the array into chunks. Each thread insertion-sorts ITS OWN chunk
//             (chunks don't overlap -> parallel for over chunks). Then merge the sorted chunks
//             pairwise (width 1, 2, 4...), each merge of a pair is independent -> parallel for again.
#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
using namespace std;

void insertionSort(vector<int> &a, int lo, int hi) {      // sorts a[lo..hi-1]
    for (int i = lo + 1; i < hi; i++) {
        int key = a[i], j = i - 1;
        while (j >= lo && a[j] > key) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

void mergeRange(vector<int> &a, vector<int> &tmp, int lo, int mid, int hi) {   // [lo,mid) + [mid,hi)
    int i = lo, j = mid, k = lo;
    while (i < mid && j < hi) tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];
    while (i < mid) tmp[k++] = a[i++];
    while (j < hi) tmp[k++] = a[j++];
    for (int t = lo; t < hi; t++) a[t] = tmp[t];
}

int main() {
    int n = 50000;
    vector<int> a(n);
    for (int i = 0; i < n; i++)
        a[i] = rand() % 100000;

    vector<int> s = a;
    double start = omp_get_wtime();
    insertionSort(s, 0, n);
    double ts = omp_get_wtime() - start;
    cout << "N = " << n << "  serial insertion sort = " << ts << "\n";

    int chunks = 16;
    for (int t = 1; t <= 8; t = t * 2) {
        omp_set_num_threads(t);
        vector<int> p = a, tmp(n);
        start = omp_get_wtime();

        // 1. sort each chunk in parallel
        #pragma omp parallel for
        for (int c = 0; c < chunks; c++) {
            int lo = (long)n * c / chunks, hi = (long)n * (c + 1) / chunks;
            insertionSort(p, lo, hi);
        }

        // 2. merge pairs of chunks: width 1, 2, 4, 8 chunks
        for (int width = 1; width < chunks; width *= 2) {
            #pragma omp parallel for
            for (int c = 0; c < chunks; c += 2 * width) {
                int lo = (long)n * c / chunks;
                int mid = (long)n * min(c + width, chunks) / chunks;
                int hi = (long)n * min(c + 2 * width, chunks) / chunks;
                mergeRange(p, tmp, lo, mid, hi);
            }
        }
        double tp = omp_get_wtime() - start;
        cout << "threads = " << t << "  time = " << tp << "  speedup = " << ts / tp
             << "  " << (p == s ? "MATCH" : "MISMATCH") << "\n";
    }
    cout << "(big speedup even with 1 thread: sorting 16 small chunks is much less work than one big insertion sort)\n";
}
