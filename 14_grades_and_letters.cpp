// PROBLEM: (a) count students per grade group 0-9, 10-19, ..., 90-99
//          (b) count each letter a-z in a text
// PATTERN: Rule 4 - histogram in disguise. Only the box index changes:
//          group = mark / 10        letter box = c - 'a'
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <omp.h>
using namespace std;

int main() {
    // (a) grade groups
    long n = 10000000;
    vector<int> marks(n);
    for (long i = 0; i < n; i++)
        marks[i] = rand() % 100;

    long g1[10] = {0};
    for (long i = 0; i < n; i++)
        g1[marks[i] / 10]++;

    long g2[10] = {0};
    #pragma omp parallel
    {
        long local[10] = {0};
        #pragma omp for
        for (long i = 0; i < n; i++)
            local[marks[i] / 10]++;
        #pragma omp critical
        {
            for (int k = 0; k < 10; k++)
                g2[k] += local[k];
        }
    }
    bool ok = true;
    for (int k = 0; k < 10; k++) {
        cout << k * 10 << "-" << k * 10 + 9 << " : " << g2[k] << "\n";
        if (g1[k] != g2[k]) ok = false;
    }
    cout << (ok ? "MATCH" : "MISMATCH") << "\n\n";

    // (b) letter count
    string text = "";
    for (int r = 0; r < 200000; r++)
        text += "the quick brown fox jumps over the lazy dog ";
    long m = text.size();

    long l1[26] = {0};
    for (long i = 0; i < m; i++)
        if (text[i] >= 'a' && text[i] <= 'z') l1[text[i] - 'a']++;

    long l2[26] = {0};
    #pragma omp parallel
    {
        long local[26] = {0};
        #pragma omp for
        for (long i = 0; i < m; i++)
            if (text[i] >= 'a' && text[i] <= 'z') local[text[i] - 'a']++;
        #pragma omp critical
        {
            for (int k = 0; k < 26; k++)
                l2[k] += local[k];
        }
    }
    ok = true;
    for (int k = 0; k < 26; k++) {
        if (l1[k] != l2[k]) ok = false;
    }
    cout << "letter o = " << l2['o' - 'a'] << "  letter e = " << l2['e' - 'a'] << "  " << (ok ? "MATCH" : "MISMATCH") << "\n";
}
