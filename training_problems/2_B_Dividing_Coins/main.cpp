#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <numeric>

int dif(int coins[], int nc, int i, int acc) {
    if (acc == 0 || acc == 1 || i == nc) return acc;

    if (coins[i] > acc)
        return std::min(acc, dif(coins, nc, i + 1, acc));
    else {
        int r = std::min(dif(coins, nc, i + 1, acc),
                         dif(coins, nc, i + 1, acc - coins[i]));
        return std::min(acc, r);
    }
}

int dif_bu(int coins[], int n, int C) {
    bool T[n + 1][C + 1];
    for (int i = 0; i <= n; i++) T[i][0] = true;
    for (int j = 1; j <= C; j++) T[0][j] = false;
    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= C; j++) {
            if (coins[i - 1] > j)
                T[i][j] = T[i - 1][j];
            else
                T[i][j] = T[i - 1][j] | T[i - 1][j - coins[i - 1]];

            // printf("%d %d\n", i, j);
            // for (int i = 0; i <= n; i++) {
            //     for (int j = 0; j <= C; j++) printf("%3d ", T[i][j]);
            //     printf("\n");
            // }
        }

    int diff = INT_MAX;

    for (int j = C / 2; j >= 0; j--)
        if (T[n][j]) {
            diff = C - 2 * j;
            break;
        }

    return diff;
}

int main(int argc, char const *argv[]) {
    int n;
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        // Get input
        int nc;
        scanf("%d", &nc);
        int coins[nc];
        for (int i = 0; i < nc; i++) scanf("%d", &coins[i]);

        // Solve with greedy
        // std::sort(coins, coins + nc, std::greater<int>());
        // int cA = 0, cB = 0;
        // for (int i = 0; i < nc; i++) {
        //     if (cA < cB)
        //         cA += coins[i];
        //     else
        //         cB += coins[i];
        // }
        // printf("%d\n", abs(cA - cB));

        // Solve by recursion
        int s = std::accumulate(coins, coins + nc, 0);
        // int d = dif(coins, nc, 0, s / 2) + s % 2;
        // printf("%d\n", abs(d));

        std::sort(coins, coins + nc);
        int d = dif_bu(coins, nc, s);
        printf("%d\n", abs(d));

        // Solve by bottom up DP, this is an instance of the subset sum problem
        // int T[nc + 1][s / 2 + 1];

        // for (int i = 0; i < nc; i++) T[i][0] = 0;
        // for (int i = 1; i < s / 2 + 1; i++)
        //     T[0][i] = std::numeric_limits<int>::max();

        // for (int j = 1; j < s / 2; j++)
        //     for (int i = 1; i < nc; i++)
        //         if (i - coins[i] > j)
        //             T[i][j] = T[i - 1][j];
        //         else
        //             T[i][j] = std::min(T[i - 1][j], 1 + T[i][j - coins[i]]);
    }

    return 0;
}
