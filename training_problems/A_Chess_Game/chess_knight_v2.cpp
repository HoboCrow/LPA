#include <bits/stdc++.h>
#include <stdio.h>
#include <utility>

using namespace std;

class Horse {
  public:
    int x, y, m;
    vector<pair<int, int>> move_map;
    Horse() {}
    Horse(int x, int y, int m) {
        this->x = x;
        this->y = y;
        this->m = m;
    }
    ~Horse() {}
    void setPossibleMoves() {
        move_map.reserve(8 * m);
        setAtPos(x, y, m);
    }

  private:
    void setAtPos(int xi, int yi, int mleft) {
        mleft--;
        move_map.push_back(make_pair(++xi, ++++yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
        move_map.push_back(make_pair(++xi, --yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
        move_map.push_back(make_pair(xi, ----yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
        move_map.push_back(make_pair(--xi, --yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
        move_map.push_back(make_pair(----xi, yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
        move_map.push_back(make_pair(--xi, ++yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
        move_map.push_back(make_pair(xi, ++++yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
        move_map.push_back(make_pair(++xi, ++yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
    }
};

int main(int argc, char const *argv[]) {
    bool map[401][401] = {false};
    const int xoff = 200;
    const int yoff = 200;
    int count = 0;
    int n_horses;
    scanf("%d", &n_horses);
    Horse horses[n_horses];
    for (int i = 0; i < n_horses; i++) {
        scanf("%d %d %d", &(horses[i].x), &(horses[i].y), &(horses[i].m));
        if (horses[i].m) {
            horses[i].setPossibleMoves();
            for (auto iter = horses[i].move_map.begin();
                 iter != horses[i].move_map.end(); ++iter) {
                if (!map[xoff + (*iter).first][yoff + (*iter).second]) {
                    map[xoff + (*iter).first][yoff + (*iter).second] = true;
                    count++;
                }
            }
        }
    }

    char board[40][40];
    memset(board, '_', 40 * 40);
    for (int i = 0; i < n_horses; i++) {
        Horse h = horses[i];
        board[20 + h.x][20 + h.y] = 'H';
    }

    for (int i = 0; i < 40; i++) {
        for (int j = 0; j < 40; j++) {
            bool t = map[xoff - 20 + i][yoff - 20 + j];
            if (t) {
                board[i][j] = 'X';
            }
        }
    }

    for (int i = 0; i < 40; i++) {
        for (int j = 0; j < 40; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }

    printf("%d\n", count);
    return 0;
}
