#include <bits/stdc++.h>
#include <stdio.h>
#include <utility>

// using namespace std;

int map[401][401];
const int xoff = 200;
const int yoff = 200;
int n_spots = 0;
int n_horses;

int jump_dirs[][2] = {{1, 2},   {2, 1},   {2, -1}, {1, -2},
                      {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}};

typedef struct horse {
    int x;
    int y;
    int m;
} Horse;

bool out_of_range(int x, int y, int move[2]) {
    if (x + move[0] > 200 || x + move[0] < -200 || y + move[1] > 200 ||
        y + move[1] < -200)
        return true;
    return false;
}

inline int get_map(int x, int y) { return map[xoff + x][yoff + y]; }

inline void set_map(int x, int y, int m) { map[xoff + x][yoff + y] = m; }

inline bool worth_jump(int x, int y, int m, int move[2]) {
    return m > get_map(x + move[0], y + move[1]);
}

void jump_from(int xi, int yi, int mleft) {
    // Mark starting spot
    int curr = get_map(xi, yi);
    if (curr == -1) // If unmarked
        n_spots++;  // Only count first marking
    if (curr < mleft) {
        set_map(xi, yi, mleft);
    } else      // Already marked with >= jumps, not worth
        return; // Probaby redundant -> worth_jump() below should rule this out
    if (mleft < 1) // Cant jump
        return;
    for (int i = 0; i < 8; i++) {
        int *mv = jump_dirs[i];
        if (out_of_range(xi, yi, mv) || !worth_jump(xi, yi, mleft - 1, mv))
            continue;
        jump_from(xi + mv[0], yi + mv[1], mleft - 1);
    }
}

int main(int argc, char const *argv[]) {
    memset(map, -1, 401 * 401 * sizeof(int));
    scanf("%d", &n_horses);
    Horse horses[n_horses];

    for (int i = 0; i < n_horses; i++) {
        scanf("%d %d %d", &(horses[i].x), &(horses[i].y), &(horses[i].m));
        jump_from(horses[i].x, horses[i].y, horses[i].m);
    }

    // char board[40][40];
    // memset(board, '_', 40 * 40);
    // for (int i = 0; i < n_horses; i++) {
    //     Horse h = horses[i];
    //     board[20 + h.x][20 + h.y] = 'H';
    // }

    // for (int i = 0; i < 40; i++) {
    //     for (int j = 0; j < 40; j++) {
    //         int t = map[xoff - 20 + i][yoff - 20 + j];
    //         if (t > -1) {
    //             board[i][j] = 'X';
    //         }
    //     }
    // }

    // for (int i = 0; i < 40; i++) {
    //     for (int j = 0; j < 40; j++) {
    //         printf("%c ", board[i][j]);
    //     }
    //     printf("\n");
    // }

    printf("%d\n", n_spots);
    return 0;
}
