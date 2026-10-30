// PROBLEM: parallel quick sort with tasks
// IDEA: partition FIRST (serial: smaller left of pivot, bigger right), then the two sides
//       don't overlap -> task(left) + task(right) -> taskwait. Nothing to combine after.
// vs MERGE SORT: merge sort does its work AFTER the split (merge), quick sort BEFORE (partition)
// partition: i = low - 1 marks the end of the "small" area (empty at the start)
// WEAKNESS: bad pivot -> one side gets almost everything -> unequal tasks -> poor speedup
#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
using namespace std;

int partitionArr(vector<int> &a, int low, int high) {
    int pivot = a[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (a[j] < pivot) {
            i++;
            swap(a[i], a[j]);
        }
    }
    swap(a[i + 1], a[high]);
    return i + 1;
}

void quickSerial(vector<int> &a, int low, int high) {
    if (low >= high) return;
    int p = partitionArr(a, low, high);
    quickSerial(a, low, p - 1);
    quickSerial(a, p + 1, high);
}

void quickParallel(vector<int> &a, int low, int high) {
    if (low >= high) return;
    if (high - low < 2048) {
        quickSerial(a, low, high);
        return;
    }
    int p = partitionArr(a, low, high);

    #pragma omp task shared(a) firstprivate(low, p)
    quickParallel(a, low, p - 1);

    #pragma omp task shared(a) firstprivate(p, high)
    quickParallel(a, p + 1, high);

    #pragma omp taskwait
}

int main() {
    int sizes[] = {100000, 1000000, 10000000};
    for (int s = 0; s < 3; s++) {
        int n = sizes[s];
        vector<int> a(n);
        for (int i = 0; i < n; i++) a[i] = rand();

        vector<int> ser = a;
        double start = omp_get_wtime();
        quickSerial(ser, 0, n - 1);
        double ts = omp_get_wtime() - start;
        cout << "\nN = " << n << "  serial time = " << ts << "\n";

        for (int t = 1; t <= 8; t = t * 2) {
            omp_set_num_threads(t);
            vector<int> p = a;
            start = omp_get_wtime();
            #pragma omp parallel
            {
                #pragma omp single
                quickParallel(p, 0, n - 1);
            }
            double tp = omp_get_wtime() - start;
            bool sorted = true;
            for (int i = 0; i < n - 1; i++) if (p[i] > p[i + 1]) sorted = false;
            cout << "threads = " << t << "  time = " << tp << "  speedup = " << ts / tp
                 << "  efficiency = " << ts / tp / t << "  " << ((sorted && p == ser) ? "MATCH" : "MISMATCH") << "\n";
        }
    }
}
