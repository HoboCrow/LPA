#include <array>
#include <bits/stdc++.h>
#include <stdio.h>

class Point {
  public:
    long long int x;
    long long int y;
    Point() {}
    Point(long long int x, long long int y) {
        this->x = x;
        this->y = y;
    }
    float incline() const { return (float)y / x; }
};

std::vector<Point> *make_point_vect(int people[], int n) {
    std::vector<Point> *vtr = new std::vector<Point>(n, Point());
    for (int i = 0; i < n; i++) {
        (*vtr)[i].x = people[i];
        (*vtr)[i].y = people[i] * people[i] * people[i];
    }
    return vtr;
}

bool check_exists_colinear(std::vector<Point> &points) {
    for (uint i = 0; i < points.size(); i++) {
        // New line paralel to x axis, passes through point i
        Point pi = points[i];
        // Copy vector
        std::vector<Point> wrking(points.size() - 1, Point());
        wrking.assign(points.begin() + i + 1, points.end());
        // Modify values as if x of point i is origin, and mirror
        // on point i all negative points
        for (uint j = 0; j < wrking.size(); j++) {
            wrking[j].x -= pi.x;
            wrking[j].y -= pi.y;
            if (wrking[j].y < 0) {
                wrking[j].x *= -1;
                wrking[j].y *= -1;
            }
        }
        // Sort by incline
        std::sort(wrking.begin(), wrking.end(),
                  [](const Point &p1, const Point &p2) -> bool {
                      return (float)p1.y / p1.x > (float)p2.y / p2.x;
                  });
        // Find 2 consecutive points with the same incline -> 3 coll true
        Point fst_point = wrking[0];
        Point snd_point;
        for (uint k = 1; k < wrking.size(); k++, fst_point = snd_point) {
            snd_point = wrking[k];
            if (fst_point.incline() == snd_point.incline()) // TODO rever float/doubel
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

        std::vector<Point> *points = make_point_vect(people, n);
        bool isSolution = check_exists_colinear(*points);
        if (isSolution)
            printf("Fair\n");
        else
            printf("Rigged\n");
    }
    return 0;
}
