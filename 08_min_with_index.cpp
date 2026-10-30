// PROBLEM: smallest number AND its position (also: second largest at the bottom)
// PATTERN: Rule 4 - own copy + critical (same as max with index, flip > to <, INT_MIN to INT_MAX)
#include <iostream>
#include <vector>
#include <climits>
#include <omp.h>
using namespace std;

int main() {
    long n = 10000000;
    vector<long> a(n);
    for (long i = 0; i < n; i++)
        a[i] = i % 100 + 10;                 // 10 to 109
    a[4444444] = 1;

    long small1 = INT_MAX, pos1 = -1;
    for (long i = 0; i < n; i++) {
        if (a[i] < small1) {
            small1 = a[i];
            pos1 = i;
        }
    }

    long small2 = INT_MAX, pos2 = -1;
    #pragma omp parallel
    {
        long mySmall = INT_MAX, myPos = -1;
        #pragma omp for
        for (long i = 0; i < n; i++) {
            if (a[i] < mySmall) {
                mySmall = a[i];
                myPos = i;
            }
        }
        #pragma omp critical
        {
            if (mySmall < small2 || (mySmall == small2 && myPos < pos2)) {
                small2 = mySmall;
                pos2 = myPos;
            }
        }
    }
    cout << "serial:   " << small1 << " at " << pos1 << "\n";
    cout << "parallel: " << small2 << " at " << pos2 << "  " << (small1 == small2 && pos1 == pos2 ? "MATCH" : "MISMATCH") << "\n";

    // SECOND LARGEST (two values that depend on each other -> own copy + critical)
    a[100] = 900;
    a[200] = 800;
    long f1 = INT_MIN, sec1 = INT_MIN;
    for (long i = 0; i < n; i++) {
        if (a[i] > f1) { sec1 = f1; f1 = a[i]; }
        else if (a[i] > sec1 && a[i] != f1) sec1 = a[i];
    }

    long first = INT_MIN, second = INT_MIN;
    #pragma omp parallel
    {
        long my1 = INT_MIN, my2 = INT_MIN;
        #pragma omp for
        for (long i = 0; i < n; i++) {
            if (a[i] > my1) { my2 = my1; my1 = a[i]; }
            else if (a[i] > my2 && a[i] != my1) my2 = a[i];
        }
        #pragma omp critical
        {
            // merge my two into the global two
            long cand[2] = {my1, my2};
            for (int k = 0; k < 2; k++) {
                long v = cand[k];
                if (v > first) { second = first; first = v; }
                else if (v > second && v != first) second = v;
            }
        }
    }
    cout << "largest = " << first << ", second largest = " << second
         << "  " << (first == f1 && second == sec1 ? "MATCH" : "MISMATCH") << "\n";
}
