#include <algorithm>
#include <iostream>
#include <map>
#include <math.h>
#include <stdio.h>
#include <vector>
using namespace std;

class Point {
  public:
    double x, y;
    Point(){};
    Point(double x, double y) {
        this->x = x;
        this->y = y;
    }
    double area() { return x * y; }
};

vector<Point *> points;
double *shared_area;
double max_area = 0;
int k;
int n;

void printPoints() {
    for (auto point : points)
        printf("x: %lf\ty: %lf\n", point->x, point->y);
}

inline double getShared(int i, int j) { return shared_area[i * n + j]; }
inline void setShared(int i, int j, double value) {
    shared_area[i * n + j] = value;
}

void getInput() {
    scanf("%d %d", &n, &k);
    shared_area = new double[n * n];
    points.reserve(n);
    double x, y;
    for (int i = 0; i < n; i++) {
        scanf("%lf %lf", &x, &y);
        points.push_back(new Point(x, y));
    }
}

void sort_points_by_y() {
    sort(
        points.begin(), points.end(),
        [](const Point *p1, const Point *p2) -> bool { return p1->y > p2->y; });
    printPoints();
}

int main(int argc, char const *argv[]) {
    getInput();
    sort_points_by_y();
    return 0;
}
