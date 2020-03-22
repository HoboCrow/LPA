#include <array>
#include <bits/stdc++.h>
#include <stdio.h>

using namespace std;

bool isPossible(int people[], int n) {
    sort(people, people + n);
    int st, en;
    for (int i = 1; i < n - 2; i++) {
        st = 0;
        en = n - 1;
        while (st < i && i < en) {
            // printf("\n");
            // for (int k = 0; k < n; k++)
            //     printf("%3d", people[k]);
            // printf("\n");
            // printf(" %*c  %*c  %*c ", 3 * st + 1, 's', 3 * (i - st - 1) + 1,
            //        'i', 3 * (en - i - 1) + 1, 'e');
            int sum = people[i] + people[st] + people[en];
            // printf("\tSum: %d\n", sum);
            if (sum > 0)
                en--;
            else if (sum < 0)
                st++;
            else
                return true;
        }
    }
    return false;
}

int main(int argc, char const *argv[]) {
    int n;
    while (scanf("%d", &n) != EOF) {
        int people[n];
        for (int i = 0; i < n; i++)
            scanf("%d", &(people[i]));
        scanf("%*d"); // Ler o 0

        bool isSolution = isPossible(people, n);

        if (isSolution)
            printf("Fair\n");
        else
            printf("Rigged\n");
    }
    return 0;
}
