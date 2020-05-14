#include <math.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <iostream>
#include <map>
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
    for (auto point : points) printf("x: %lf\ty: %lf\n", point->x, point->y);
}

void getInput() {
    scanf("%d %d", &n, &k);
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
}

double *areas;

double cutArea(int hi, int lo) {
    return points[hi]->x * (points[hi]->y - points[lo]->y);
}
inline double getArea(int i, int j) {
    // printf("Accessing read (i:%d j:%d) %d/%d\n", i, j, i * n + j, k * n);
    return areas[i * n + j];
}

inline void setArea(int i, int j, double v) {
    // printf("Accessing write (i:%d j:%d) %d/%d\n", i, j, i * n + j, k * n);
    areas[i * n + j] = v;
}

void printAreas() {
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < n; j++) printf("%.12f ", getArea(i, j));
        printf("\n");
    }
}

int main(int argc, char const *argv[]) {
    getInput();
    sort_points_by_y();
    // printPoints();
    // Create matrix to hold data
    areas = (double *)malloc(k * n * sizeof *areas);

    // preencher a primeira linha da matriz com as areas
    for (int i = 0; i < n; i++) {
        setArea(0, i, points[i]->area());
    }
// i -> numero de pontos a escolher
// j -> posicao a partir da qual esoclhemos os i pontos
#ifdef DEBUG
    printPoints();
#endif

    for (int i = 1; i < k; i++) {
        for (int j = n - i - 1; j >= 0; j--) {
            setArea(i, j, 0);
            for (int p = j + 1; p <= n - i; p++) {
                double a = getArea(i - 1, p) + cutArea(j, p);
                if (a > getArea(i, j)) {
                    setArea(i, j, a);
                }
            }
        }
    }
#ifdef DEBUG
    printAreas();
#endif

    max_area = 0;
    for (int i = 0; i < n - k + 1; i++) {
        if (getArea(k - 1, i) > max_area) max_area = getArea(k - 1, i);
    }
    printf("%.12f\n", max_area);

    free(areas);
    for (auto p : points) delete p;
    return 0;
}
