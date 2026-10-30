// PROBLEM: parallel merge sort with tasks  (Lab 9 Task 1)
// WHY TASKS: no loop to split - it's recursion. Left and right halves don't overlap -> separate jobs
// STEPS: base case -> cutoff (small -> serial) -> task(left) -> task(right) -> taskwait -> merge
// START: #pragma omp parallel + #pragma omp single (ONE thread starts, the others pick up tasks)
// shared(a, tmp): same arrays, different parts.  firstprivate(left, mid): own copy of the numbers
// taskwait: merge needs BOTH halves fully sorted
#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
using namespace std;

void merge(vector<int> &a, vector<int> &tmp, int left, int mid, int right) {
    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        if (a[i] <= a[j]) tmp[k++] = a[i++];
        else              tmp[k++] = a[j++];
    }
    while (i <= mid)   tmp[k++] = a[i++];
    while (j <= right) tmp[k++] = a[j++];
    for (int t = left; t <= right; t++)
        a[t] = tmp[t];
}

void mergeSortSerial(vector<int> &a, vector<int> &tmp, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSortSerial(a, tmp, left, mid);
    mergeSortSerial(a, tmp, mid + 1, right);
    merge(a, tmp, left, mid, right);
}

void mergeSortParallel(vector<int> &a, vector<int> &tmp, int left, int right) {
    if (left >= right) return;
    if (right - left < 2048) {                        // cutoff
        mergeSortSerial(a, tmp, left, right);
        return;
    }
    int mid = left + (right - left) / 2;

    #pragma omp task shared(a, tmp) firstprivate(left, mid)
    mergeSortParallel(a, tmp, left, mid);

    #pragma omp task shared(a, tmp) firstprivate(mid, right)
    mergeSortParallel(a, tmp, mid + 1, right);

    #pragma omp taskwait
    merge(a, tmp, left, mid, right);
}

int main() {
    int sizes[] = {100000, 1000000, 10000000};
    for (int s = 0; s < 3; s++) {
        int n = sizes[s];
        vector<int> a(n);
        for (int i = 0; i < n; i++) a[i] = rand();

        vector<int> ser = a, tmp(n);
        double start = omp_get_wtime();
        mergeSortSerial(ser, tmp, 0, n - 1);
        double ts = omp_get_wtime() - start;
        cout << "\nN = " << n << "  serial time = " << ts << "\n";

        for (int t = 1; t <= 8; t = t * 2) {
            omp_set_num_threads(t);
            vector<int> p = a;                       // fresh unsorted copy
            start = omp_get_wtime();
            #pragma omp parallel
            {
                #pragma omp single
                mergeSortParallel(p, tmp, 0, n - 1);
            }
            double tp = omp_get_wtime() - start;
            bool sorted = true;
            for (int i = 0; i < n - 1; i++) if (p[i] > p[i + 1]) sorted = false;
            cout << "threads = " << t << "  time = " << tp << "  speedup = " << ts / tp
                 << "  efficiency = " << ts / tp / t << "  " << ((sorted && p == ser) ? "MATCH" : "MISMATCH") << "\n";
        }
    }
}
