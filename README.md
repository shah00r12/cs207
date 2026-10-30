# OpenMP Reference: 33 Solved Problems (C++)

Every program is complete: data → serial → parallel (1, 2, 4, 8 threads) → check (MATCH) → time, speedup, efficiency.
All compile with no warnings and every check prints MATCH.

## How to compile and run

```
g++ -fopenmp -Wall file.cpp -o file
.\file            (Windows)      ./file   (Linux)
```
- No `-fopenmp` → the pragmas are ignored / "undefined reference to omp_..."
- Set threads: `omp_set_num_threads(t);` in code (used in all files), or `$env:OMP_NUM_THREADS=4` (PowerShell), `export OMP_NUM_THREADS=4` (Linux)

---

## Step 1: which pattern? Look at the LEFT side of `=`

| Each loop turn writes… | Pattern | Pragma | Files |
|---|---|---|---|
| its own box `c[i]` | 1. own box | `#pragma omp parallel for` | 01, 02, 18 |
| its own box in a matrix `C[i*n+j]` | 2. own box 2D | `#pragma omp parallel for collapse(2)` | 15, 16, 17, 21, 22 |
| one shared number (sum, count, max, min, true/false) | 3. reduction | `#pragma omp parallel for reduction(op:var)` | 03, 05, 06, 09, 10, 11, 12, 20, 30, 31 |
| a few shared boxes, OR value + position | 4. own copy + critical | `parallel { own copy; omp for; critical {merge} }` | 07, 08, 13, 14, 24 |
| each step needs the previous step | 5. phases / blocks | `parallel` outside, `omp for` inside, barrier between | 23, 32 |
| recursion (no loop) | tasks | `task` + `taskwait`, started with `parallel` + `single` | 26, 27, 28 |
| turns with uneven work | add scheduling | `schedule(dynamic, 100)` | 29, 30 |

**Dependent or independent?** Combine each variable from the threads **separately**. Still correct → independent → reduction
(sum + count, max + min, even + odd). Not correct → dependent → own copy + critical (max + its position, top 2).

**Reduction operators:** `+` sum/count · `*` product (start 1) · `max` (start INT_MIN) · `min` (start INT_MAX) · `&&` all true (start true)
Several: `reduction(+:a, b)` or `reduction(max:mx) reduction(min:mn)`

---

## File index

| # | File | Problem | Key idea |
|---|---|---|---|
| 01 | vector_add | c = a + b | parallel for |
| 02 | vector_ops | square+add, reverse, max of two vectors | parallel for, separate output |
| 03 | sum | sum of array | reduction(+) |
| 04 | sum_race_and_fixes | the race + 4 fixes | wrong / reduction / critical / atomic / own copy |
| 05 | average_and_stddev | mean and std dev | (double)sum/n, two reductions |
| 06 | max_min_range | max, min, range in one loop | reduction(max) reduction(min) |
| 07 | max_with_index | max + position (first one) | own copy + critical, tie-break |
| 08 | min_with_index | min + position, second largest | own copy + critical |
| 09 | count_even_odd_reduction_vs_atomic | Lab 9 Task 3 | reduction(+:even, odd) vs atomic |
| 10 | count_categories | +/−/0, multiples of 3 and 5 | several counters in one reduction |
| 11 | search | exists, first, last, count | min (start n), max (start −1), + |
| 12 | dot_product | a·b | reduction(+), fabs check |
| 13 | histogram | count 0–9 | atomic vs local+critical vs reduction(+:h[:10]) |
| 14 | grades_and_letters | grade groups, letter count | histogram with index mark/10, c−'a' |
| 15 | matrix_add | C = A + B | collapse(2), M[i*cols+j] |
| 16 | matrix_transpose | T = Aᵀ (rectangular) | T[j*R+i] = A[i*C+j] |
| 17 | matrix_multiply | C = A × B | collapse(2) on i,j; local sum; never split k; i-k-j bonus |
| 18 | matrix_vector | y = A x | parallel for on rows only (NO collapse) |
| 19 | row_col_sums_trace | row/col sums, trace, total | rows: parallel for; trace: reduction |
| 20 | symmetric_and_equal | symmetric? equal? sorted? | reduction(&&:ok) |
| 21 | image_brightness_threshold | +50 capped at 255, count capped; threshold | collapse(2) + reduction together |
| 22 | convolution | 3×3 blur/edge/sharpen | output (R−2)×(C−2), collapse(2), separate output |
| 23 | bubble_odd_even_sort | parallel bubble sort | even/odd rounds, barrier between, no nowait |
| 24 | selection_sort | parallel selection sort | rounds serial, find-min parallel (own copy + critical) |
| 25 | insertion_sort_chunks | parallel insertion sort | can't directly; sort chunks in parallel, then merge |
| 26 | merge_sort_tasks | Lab 9 Task 1 | task(left), task(right), taskwait, merge; cutoff |
| 27 | quick_sort_tasks | parallel quick sort | partition first, then 2 tasks, taskwait |
| 28 | fibonacci_and_recursive_sum_tasks | fib, recursive sum | task + shared(x) + taskwait + combine |
| 29 | scheduling | Lab 9 Task 2 | static vs dynamic vs guided, repeating vs growing work |
| 30 | primes_dynamic | count/sum primes | reduction + schedule(dynamic) |
| 31 | pi_integration | π = ∫ 4/(1+x²) | reduction(+) |
| 32 | prefix_sum_dependency | running total | plain parallel for is WRONG; block method with barrier/single |
| 33 | threads_info_and_fork_join | thread ids, parallel vs parallel for, single, barrier | basics |

---

## Sorting: what can be parallel?

| Sort | Directly parallel? | How |
|---|---|---|
| Bubble | No: each compare uses the previous swap | odd-even sort: pairs in a round don't overlap; barrier between rounds (23) |
| Selection | Outer rounds no; finding the min yes | each round: min + position with own copy + critical (24) |
| Insertion | No: inserting a[i] needs a[0..i−1] sorted | sort chunks in parallel, then merge chunks (25) |
| Merge | Yes: halves are separate | tasks for the halves, taskwait, merge (26) |
| Quick | Yes, after partition | partition (serial), tasks for both sides (27) |

**Can't be parallelised directly:** `a[i] = a[i-1] + …` (prefix sum, iterative Fibonacci), `while` loops,
`break` inside the loop, linked lists. → restructure (phases / blocks / tasks) or explain the dependency.

---

## Checking answers

| Answer | Check |
|---|---|
| whole number | `s1 == s2` |
| decimal | `fabs(x1 - x2) < 1e-6` |
| vector of ints / exact copies | `v1 == v2` |
| vector of doubles with math | loop with `fabs(...) > 1e-6` |
| plain array `h[10]` | loop over the boxes |
| sorted array | is sorted AND `== serial` |
| counts | even + odd == n, histogram total == n |

Speedup = Ts / Tp. Efficiency = speedup / threads.

---

## Before compiling: checklist
1. `{ }` on every `if` / `critical` with 2+ lines
2. `parallel for` / `omp for` is followed DIRECTLY by `for` (no `{`)
3. Spelling: `critical`, `reduction`, `collapse`
4. `long` for big sums/sizes; `(double)` for averages; `(long)i * n` in matrix indexes
5. Start values: sum 0, product 1, max INT_MIN, min INT_MAX, first position n
6. `#include <climits>` (INT_MIN), `<cmath>` (fabs, sqrt)
7. Separate output array (transpose, convolution, reverse)
8. In tasks: `parallel` + `single` to start; `taskwait` before combining

## Common errors

| Error | Fix |
|---|---|
| undefined reference to `omp_...` | add `-fopenmp` |
| `ignoring #pragma omp ...` | pragma spelled wrong |
| `for statement expected` | remove `{` after `parallel for` |
| `Assertion '__n < this->size()' failed` | index past the end: check `<` vs `<=`, vector sizes, i/j |
| `cannot create vector larger than max_size` | `n` not set before making the vector |
| different answer each run | race: missing reduction / critical / `{ }` |

## Viva one-liners
- **Race condition:** threads write the same variable without protection → lost updates.
- **Reduction:** private copy per thread, combined once at the end.
- **critical vs atomic:** a block one thread at a time / one small update, faster.
- **collapse(2):** merges two loops into one so the work divides better.
- **static / dynamic / guided:** fixed equal split / chunks on request / shrinking chunks.
- **task vs parallel for:** recursion/irregular work vs loops with a known size.
- **taskwait:** wait for the child tasks before combining.
- **private / firstprivate / shared:** own copy (not set) / own copy (starts with the outside value) / one copy for all.
- **single:** only one thread runs it (used to start recursion).
- **Why slower for small N:** thread/task creation costs more than the work.
- **Why not perfect speedup:** overhead, memory bandwidth, barriers, more threads than cores, serial parts (Amdahl).
