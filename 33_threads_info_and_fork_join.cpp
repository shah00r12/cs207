// PROBLEM: show threads, fork-join, parallel vs parallel for, single, master, barrier
// parallel     -> EVERY thread runs the whole block (work repeated)
// parallel for -> the loop is DIVIDED among threads
// single       -> one thread runs it (others wait at the end)
// master       -> thread 0 runs it (no waiting)
// critical     -> one thread at a time (so cout lines don't mix)
#include <iostream>
#include <omp.h>
using namespace std;

int main() {
    omp_set_num_threads(4);
    cout << "Before: 1 thread\n";

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int total = omp_get_num_threads();
        #pragma omp critical
        cout << "parallel: hello from thread " << id << " of " << total << "\n";

        #pragma omp barrier                   // everyone waits here
        #pragma omp single
        cout << "single: printed once (by thread " << omp_get_thread_num() << ")\n";
    }

    #pragma omp parallel for
    for (int i = 0; i < 8; i++) {
        #pragma omp critical
        cout << "parallel for: i = " << i << " done by thread " << omp_get_thread_num() << "\n";
    }
    cout << "After: back to 1 thread. max threads = " << omp_get_max_threads() << "\n";
}
