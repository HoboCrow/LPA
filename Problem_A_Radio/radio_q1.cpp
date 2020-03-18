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
} Place;

int n_listeners;
Listener *listeners;
int n_places;
Place *places;
int n_antenas;
Antena *antenas;

int best_cost = -1;

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
}

inline bool inRange(Place *plc, Listener *lis) {
    return (lis->x - plc->x) * (lis->x - plc->x) +
               (lis->y - plc->y) * (lis->y - plc->y) <=
           plc->antena->radius * plc->antena->radius;
}

bool isSolution(int pos) {
    bool reached[n_listeners] = {false};
    int reached_count = 0;
    for (int i = 0; i < pos; i++) {
        Place *p = &places[i];
        if (!(p->antena))
            continue;
        for (int j = 0; j < n_listeners; j++) {
            if (inRange(p, &listeners[j])) {
                if (!reached[j]) {
                    reached[j] = true;
                    reached_count++;
                }
            }
        }
    }
    return reached_count == n_listeners;
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
    printf("|\n");
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

        test++;
        printf("test: %4d pos: %d i: %d \t ", test, pos, i);
        print_current_solution_short(pos + 1);

        if (isSolution(pos + 1)) {
            if (cost < best_cost || best_cost == -1) {
                best_cost = cost;
                // printf("New best solution! Cost: %d\n", cost);
                // print_current_solution(pos + 1);
            }
        }
        if (pos + 1 < n_places)
            try_place(pos + 1, cost);
    }
}

/**
 * ATM está a fazer em sum de i=0 -> pos : antenas^i
 *
 * */

int main(int argc, char const *argv[]) {
    get_input();
    try_place(0, 0);
    printf("Best cost: %d\n", best_cost);
    return 0;
}
