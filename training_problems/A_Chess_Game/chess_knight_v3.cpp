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

bool worth_jump(int x, int y, int m, int move[2]) {
    return m > map[xoff + x + move[0]][yoff + y + move[1]];
}

void jump_from(int xi, int yi, int mleft) {
    for (int i = 0; i < 8; i++) {
        int *mv = jump_dirs[i];
        if (out_of_range(xi, yi, mv) || !worth_jump(xi, yi, mleft, mv))
            continue;
        // worth jump, not yet counted
        if (map[xoff + xi + mv[0]][yoff + yi + mv[1]] == 0)
            n_spots++; // only count one time
        map[xoff + xi + mv[0]][yoff + yi + mv[1]] = mleft;
        if (mleft > 1)
            jump_from(xi + mv[0], yi + mv[1], mleft - 1);
    }
}

int main(int argc, char const *argv[]) {
    memset(map, 0, 401 * 401 * sizeof(int));
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
    //         bool t = map[xoff - 20 + i][yoff - 20 + j];
    //         if (t) {
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
