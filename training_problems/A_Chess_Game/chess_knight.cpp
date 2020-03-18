#include <bits/stdc++.h>
#include <stdio.h>
#include <utility>

using namespace std;

class Horse {
    struct pair_hash {
        template <class T1, class T2>
        std::size_t operator()(std::pair<T1, T2> const &pair) const {
            std::size_t h1 = std::hash<T1>()(pair.first);
            std::size_t h2 = std::hash<T2>()(pair.second);
            return h1 ^ h2;
        }
    };

  public:
    int x, y, m;
    unordered_set<std::pair<int, int>, pair_hash> move_map;
    Horse() {}
    Horse(int x, int y, int m) {
        this->x = x;
        this->y = y;
        this->m = m;
    }
    ~Horse() {}
    void setPossibleMoves() { setAtPos(x, y, m); }

  private:
    void setAtPos(int xi, int yi, int mleft) {
        mleft--;
        move_map.insert(make_pair(++xi, ++++yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
        move_map.insert(make_pair(++xi, --yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
        move_map.insert(make_pair(xi, ----yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
        move_map.insert(make_pair(--xi, --yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
        move_map.insert(make_pair(----xi, yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
        move_map.insert(make_pair(--xi, ++yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
        move_map.insert(make_pair(xi, ++++yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
        move_map.insert(make_pair(++xi, ++yi));
        if (mleft)
            setAtPos(xi, yi, mleft);
    }
};

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(std::pair<T1, T2> const &pair) const {
        std::size_t h1 = std::hash<T1>()(pair.first);
        std::size_t h2 = std::hash<T2>()(pair.second);
        return h1 ^ h2;
    }
};

int main(int argc, char const *argv[]) {
    unordered_set<std::pair<int, int>, pair_hash> map;

    int n_horses;
    scanf("%d", &n_horses);
    Horse horses[n_horses];
    for (int i = 0; i < n_horses; i++) {
        scanf("%d %d %d", &(horses[i].x), &(horses[i].y), &(horses[i].m));
        if (horses[i].m) {
            horses[i].setPossibleMoves();
            for (auto iter = horses[i].move_map.begin();
                 iter != horses[i].move_map.end(); ++iter) {
                map.insert(*iter);
            }
        }
    }
    printf("%d\n", (int)map.size());
    return 0;
}
