// Implementation for ACMOJ Problem 1936 (Guess)
// Interactive functions provided by judge:
//   int query(int x, int y, int z);
// We must implement:
//   int guess(int n, int Taskid);

#include <bits/stdc++.h>
using namespace std;

static const int MOD = 998244353;
static const int MUL = 233;

extern int query(int x, int y, int z);

// Helper to compute hash of array A[1..n]
static int hash_sum(const vector<long long>& A) {
    long long ret = 0;
    for (int i = (int)A.size() - 1; i >= 1; --i) {
        ret = (ret + A[i]) % MOD;
        ret = (ret * MUL) % MOD;
    }
    return (int)ret;
}

// Find indices of global minimum and maximum using triple queries.
// Returns pair {min_index, max_index} and provides also two non-extreme indices from the first four via mid1, mid2.
static pair<int,int> find_extrema(int n, int &mid1, int &mid2) {
    // Use first four indices: 1,2,3,4
    int idx[4] = {1,2,3,4};
    long long s[4];
    // s[i] = sum for triple formed by the other three (omit idx[i])
    s[0] = query(idx[1], idx[2], idx[3]);
    s[1] = query(idx[0], idx[2], idx[3]);
    s[2] = query(idx[0], idx[1], idx[3]);
    s[3] = query(idx[0], idx[1], idx[2]);
    // The maximum s corresponds to omitting the smallest element (min index)
    // The minimum s corresponds to omitting the largest element (max index)
    int omit_for_max = int(max_element(s, s+4) - s);
    int omit_for_min = int(min_element(s, s+4) - s);
    int m = -1, M = -1;
    // omitted in max(s) => smallest
    m = idx[omit_for_max];
    // omitted in min(s) => largest
    M = idx[omit_for_min];
    // mid1 and mid2 are the remaining two indices
    vector<int> rem;
    for (int i = 0; i < 4; ++i) if (i != omit_for_max && i != omit_for_min) rem.push_back(idx[i]);
    mid1 = rem[0];
    mid2 = rem[1];

    // Use a fixed pivot 'a' = mid1 to test other indices and update m and M
    int a = mid1;
    long long base_min = query(M, a, m);
    long long base_max = query(m, a, M);
    for (int t = 1; t <= n; ++t) {
        if (t == m || t == M || t == a) continue;
        long long cur_min = query(M, a, t);
        if (cur_min < base_min) {
            m = t;
            base_min = cur_min;
            base_max = query(m, a, M);
        } else {
            long long cur_max = query(m, a, t);
            if (cur_max > base_max) {
                M = t;
                base_max = cur_max;
                base_min = query(M, a, m);
            }
        }
    }
    return {m, M};
}

int guess(int n, int Taskid) {
    (void)Taskid; // unused as per interactive specification
    // A is 1-indexed; A[0] unused to match query indices
    vector<long long> A(n+1, 0);

    // Step 1: find indices of global min and max, and two middle indices from first four
    int mid1 = -1, mid2 = -1;
    auto p = find_extrema(n, mid1, mid2);
    int m = p.first;   // index of global min
    int M = p.second;  // index of global max

    // Ensure mid1 and mid2 are distinct from m and M; if not, pick alternatives
    if (mid1 == m || mid1 == M) {
        // pick the smallest index not equal to m or M
        for (int i = 1; i <= n; ++i) if (i != m && i != M) { mid1 = i; break; }
    }
    if (mid2 == m || mid2 == M || mid2 == mid1) {
        for (int i = 1; i <= n; ++i) if (i != m && i != M && i != mid1) { mid2 = i; break; }
    }

    // Choose a third non-extreme index c different from m, M, mid1, mid2
    int c = -1;
    for (int i = 1; i <= n; ++i) {
        if (i != m && i != M && i != mid1 && i != mid2) { c = i; break; }
    }
    // Problem guarantees n >= 5
    // Step 2: compute S = A_m + A_M using any third index (use mid1)
    long long S = query(m, M, mid1);

    // Step 3: compute pairwise sums among mid1 (u), mid2 (v), c
    int u = mid1, v = mid2;
    long long sum_uv = query(m, u, v) + query(M, u, v) - S;
    long long sum_uc = query(m, u, c) + query(M, u, c) - S;
    long long sum_vc = query(m, v, c) + query(M, v, c) - S;

    // Solve for A[u], A[v], A[c]
    // a = (ab + ac - bc) / 2
    A[u] = (sum_uv + sum_uc - sum_vc) / 2;
    A[v] = sum_uv - A[u];
    A[c] = sum_uc - A[u];

    // Step 4: compute A[m] and A[M]
    A[m] = query(m, u, v) - max(A[u], A[v]);
    A[M] = query(M, u, v) - min(A[u], A[v]);

    // Step 5: compute the rest
    // Use pivot u to recover others; attempt single query via (m,u,t).
    for (int t = 1; t <= n; ++t) {
        if (t == m || t == M || t == u || t == v || t == c) continue;
        long long r = query(m, u, t) - A[m]; // equals max(A[u], A[t])
        if (r > A[u]) {
            A[t] = r; // A[t] > A[u]
        } else {
            // A[t] <= A[u], use (M, u, t)
            A[t] = query(M, u, t) - A[M];
        }
    }

    // Return hash of A
    return hash_sum(A);
}
