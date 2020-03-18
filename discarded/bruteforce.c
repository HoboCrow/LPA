#include <math.h>
#include <stdio.h>
#include <string.h>
#define BUFF_SIZE 32

typedef struct point {
    int x;
    int y;
} Position;

typedef struct antena_s {
    int radius;
    int cost;
} AntenaType;

int inRange(Position ant, Position person, int range) {
    int xdist = ant.x - person.x;
    int ydist = ant.y - person.y;
    return (xdist * xdist + ydist * ydist) <= range * range;
}

int sum(int positioned[], int n, AntenaType types[]) {
    int s = 0;
    int i;
    for (i = 0; i < n; i++) {
        s += types[positioned[i]].cost;
    }
    return s;
}

void printarr(int a[], int n) {
    printf("[ ");
    int i;
    for (i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

void increment_antena_combination(int positioned[], int n, int ntypes) {
    int i;
    for (i = 0; i < n; i++) {
        if (positioned[i] == ntypes - 1) {
            positioned[i] = 0;
        } else {
            positioned[i]++;
            break;
        }
    }
    // printarr(positioned, n);
}

int main(int argc, char const *argv[]) {
    // Number of new listeners
    int listener_count;
    scanf("%d", &listener_count);
    Position listeners[listener_count];
    int i;
    for (i = 0; i < listener_count; i++)
        scanf("%d %d", &listeners[i].x, &listeners[i].y);

    // Possible antena places
    int spot_count;
    scanf("%d", &spot_count);
    Position spots[spot_count];
    for (i = 0; i < spot_count; i++)
        scanf("%d %d", &spots[i].x, &spots[i].y);

    // Antena Types
    int type_count;
    scanf("%d", &type_count);
    AntenaType types[++type_count];
    AntenaType empty = {0, 0};
    types[0] = empty;
    for (i = 1; i < type_count; i++)
        scanf("%d %d", &types[i].radius, &types[i].cost);

    int positionedAntenas[spot_count];
    int bestSolution[spot_count];
    int marked[listener_count];
    // Initialized with -1 as invalid cost
    int best_cost = -1;
    memset(positionedAntenas, 0, spot_count * sizeof(int));
    memset(bestSolution, 0, spot_count * sizeof(int));
    // Brute force
    // int solutions_found = 0;
    long lim = roundl(pow(type_count, spot_count));
    int c;
    for (c = 0; c < lim; c++) {
        memset(marked, 0, listener_count * sizeof(int));
        for (i = 0; i < spot_count; i++) {
            // nao vale a pena calcular para antenas nao posicionadas
            if (positionedAntenas[i] == 0)
                continue;

            int cost = sum(positionedAntenas, spot_count, types);
            if (best_cost != -1 && cost > best_cost)
                continue;

            Position antena_pos = spots[i];
            AntenaType antena_type = types[positionedAntenas[i]];
            int k;
            for (k = 0; k < listener_count; k++) {
                Position listener = listeners[k];
                if (marked[k])
                    continue;
                if (inRange(antena_pos, listener, antena_type.radius))
                    marked[k] = 1;
                // PROBABLY BROKEN contar uma pessoa mais que uma vez
                // FIX: array de pessoas marcadas
                // TODO fix
            }
            int valid;
            for (valid = 0; valid < listener_count; valid++) {
                if (marked[valid] != 1) {
                    valid = -1;
                    break;
                }
            }
            if (valid > -1) {
                // Possible solution
                if (best_cost < cost && best_cost != -1)
                    continue;
                best_cost = cost;
                memcpy(bestSolution, positionedAntenas,
                       spot_count * sizeof(int));
                // char filename[64];
                // snprintf(filename, 64, "./solutions/solution%d.txt",
                //          solutions_found++);
                // FILE *f = fopen(filename, "w");
                // // printf("Possible solution:\t");
                // for (i = 0; i < spot_count; i++) {
                //     // printf("%d ", positionedAntenas[i]);
                //     fprintf(f, "%d ", positionedAntenas[i]);
                // }
                // // printf("\tCost: %d\n", cost);
                // fclose(f);
                // // printf("\n");
            }
        }
        // printarr(positionedAntenas, spot_count);
        increment_antena_combination(positionedAntenas, spot_count, type_count);
    }
    // printarr(bestSolution, spot_count);
    if (best_cost == -1) {
        printf("no solution\n");
    } else {
        printf("%d\n", best_cost);
    }
    return 0;
}
