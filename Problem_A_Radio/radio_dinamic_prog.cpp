#include <bits/stdc++.h>
#include <stdio.h>

typedef struct {
    int cost;
    int radius;
} Antena;
typedef struct {
    int x;
    int y;
} Listener;
typedef struct {
    int x;
    int y;
    Antena *antena;
    int ant_type;
} Place;

int n_listeners;
int n_places;
int n_antenas;

Listener *listeners;
Place *places;
Antena *antenas;

int best_cost = -1;

std::pair<bool, bool> *in_range_arr;

void get_input() {
    // Listeners
    scanf("%d", &n_listeners);
    listeners = new Listener[n_listeners];
    for (int i = 0; i < n_listeners; i++)
        scanf("%d %d", &(listeners[i].x), &(listeners[i].y));

    // Places
    scanf("%d", &n_places);
    places = new Place[n_places];
    for (int i = 0; i < n_places; i++) {
        scanf("%d %d", &(places[i].x), &(places[i].y));
        places[i].antena = NULL;
    }

    // Antenas
    scanf("%d", &n_antenas);
    antenas = new Antena[++n_antenas];
    antenas[0] = {0, 0};
    for (int i = 1; i < n_antenas; i++)
        scanf("%d %d", &(antenas[i].radius), &(antenas[i].cost));

    // Initialize stuff
    in_range_arr =
        new std::pair<bool, bool>[n_antenas * n_places * n_listeners];
    std::fill_n(in_range_arr, n_antenas * n_places * n_listeners,
                std::make_pair(false, false));
}

inline void setCalculated(int ant, int plc, int lis, bool val) {
    in_range_arr[n_antenas * ant + n_listeners * plc + lis] =
        std::make_pair(true, val);
}

inline std::pair<bool, bool> isCalculated(int ant, int plc, int lis) {
    return in_range_arr[n_antenas * ant + n_listeners * plc + lis];
}

inline bool inRange(int plc_index, int listerner_index) {
    Place *plc = &places[plc_index];
    std::pair<bool, bool> result =
        isCalculated(plc->ant_type, plc_index, listerner_index);
    //   printf("Calculating %d %d %d\t", plc->ant_type, plc_index,
    //   listerner_index);
    if (result.first)
        return result.second;

    Listener *lis = &listeners[listerner_index];
    bool calculated = (lis->x - plc->x) * (lis->x - plc->x) +
                          (lis->y - plc->y) * (lis->y - plc->y) <=
                      plc->antena->radius * plc->antena->radius;
    setCalculated(plc->ant_type, plc_index, listerner_index, calculated);
    return calculated;
}

bool isSolution(int pos) {
#ifdef DEBUG
    printf("\tisSolRun ");
#endif
    bool reached[n_listeners];
    memset(reached, false, n_listeners * sizeof(bool));

    int reached_count = 0;
    for (int i = 0; i < pos; i++) {
        Place *p = &places[i];
        if (!(p->antena)) // No antena, not worth
            continue;

        for (int j = 0; j < n_listeners; j++) {
            if (inRange(i, j)) {
                if (!reached[j]) {
                    reached[j] = true;
                    if (++reached_count == n_listeners)
                        return true;
                }
            }
        }
    }
    return false;
}

void print_current_solution(int pos) {
    for (int i = 0; i < pos; i++) {
        Place p = places[i];
        if (p.antena)
            printf("\t%d %d | %d %d\n", p.x, p.y, p.antena->cost,
                   p.antena->radius);
        else
            printf("\t%d %d no antena\n", p.x, p.y);
    }
}

void print_current_solution_short(int pos) {
    for (int i = 0; i < pos; i++) {
        Place p = places[i];
        if (p.antena)
            printf("|%d", p.antena->cost);
        else
            printf("| ");
    }
    // printf("|\n");
}

int test = 0;
void try_place(int pos, int current_cost) {
    // printf("Place #%d\n", pos);
    Place *place = &places[pos];
    for (int i = 0; i < n_antenas; i++) {
        int cost = current_cost;
        // printf("\tAntena #%d\n", i);
        if (i == 0) {
            place->antena = NULL;
        } else {
            Antena *antena = &antenas[i];
            place->antena = antena;
            cost += antena->cost;
        }
#ifdef DEBUG
        test++;
        printf("test: %4d pos: %d i: %d cost: %d  \t ", test, pos, i, cost);
        print_current_solution_short(pos + 1);
#endif
        // >= -> antes tinha só >
        if (cost >= best_cost && best_cost != -1) {
#ifdef DEBUG
            printf("\tNOT WORTH\n");
#endif
            break;
        }
        // not worth checking for solution if i == 0 aka no antena
        if (i != 0 && isSolution(pos + 1)) {
            if (cost < best_cost || best_cost == -1) {
                best_cost = cost;
#ifdef DEBUG
                printf(" BEST SOL ");
#endif
                // printf("New best solution! Cost: %d\n", cost);
                // print_current_solution(pos + 1);
            }
#ifdef DEBUG
            printf(" NOT WORTH CONT\n");
#endif
            break;
        }
#ifdef DEBUG
        printf("\n");
#endif
        if (pos + 1 < n_places)
            try_place(pos + 1, cost);
    }
}

void sort_antenas() {
    qsort(antenas, n_antenas, sizeof(Antena),
          [](const void *a, const void *b) -> int {
              Antena *a1 = (Antena *)a;
              Antena *a2 = (Antena *)b;
              return a1->cost - a2->cost;
          });
}

int main(int argc, char const *argv[]) {
    get_input();
    sort_antenas();
    try_place(0, 0);
    // printf("Best cost: %d\n", best_cost);
    if (best_cost != -1) {
        printf("%d\n", best_cost);
    } else {
        printf("no solution\n");
    }
    return 0;
}
