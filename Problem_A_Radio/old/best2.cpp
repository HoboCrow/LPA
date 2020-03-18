#include <bits/stdc++.h>
#include <stdio.h>

using namespace std;

class Antena {
  public:
    int cost;
    int radius;
    int index;
    Antena() {
        cost = 0;
        radius = 0;
        index = -1;
    }
    Antena(int radius, int cost, int i) {
        this->radius = radius;
        this->cost = cost;
        index = i;
    }
};
class Place;
class Listener {
  public:
    int index;
    int x;
    int y;
    vector<Place *> reached_from;
    Listener(int x, int y, int i) {
        this->x = x;
        this->y = y;
        index = i;
    }
};

class Place {
  public:
    int x;
    int y;
    int index;
    Antena *antena;
    vector<Listener *> reachable;
    Place(int x, int y, int i) {
        this->x = x;
        this->y = y;
        this->antena = NULL;
        index = i;
    }
};

int n_listeners;
int n_places;
int n_antenas;
int max_range = -1;

vector<Listener> listeners;
vector<Place> places;
vector<Antena> antenas;

int best_cost = -1;

void get_input() {
    // Listeners
    scanf("%d", &n_listeners);
    listeners.reserve(n_listeners);
    int x, y;
    for (int i = 0; i < n_listeners; i++) {
        scanf("%d %d", &x, &y);
        listeners.push_back(Listener(x, y, i));
    }

    // Places
    scanf("%d", &n_places);
    places.reserve(n_places);
    for (int i = 0; i < n_places; i++) {
        scanf("%d %d", &x, &y);
        places.push_back(Place(x, y, i));
    }

    // Antenas
    scanf("%d", &n_antenas);
    antenas.reserve(++n_antenas);
    antenas.push_back(Antena(0, 0, 0));
    int r, c;
    for (int i = 1; i < n_antenas; i++) {
        scanf("%d %d", &r, &c);
        antenas.push_back(Antena(r, c, i));
        if (r > max_range || max_range == -1)
            max_range = r;
    }
}

inline bool inRange(Place &plc, Listener &lis, int range) {
    return (lis.x - plc.x) * (lis.x - plc.x) +
               (lis.y - plc.y) * (lis.y - plc.y) <=
           range * range;
}

inline bool inDistance(int xa, int ya, int xb, int yb, int d) {
    return (xa - xb) * (xa - xb) + (ya - yb) * (ya - yb) <= d * d;
}

void set_reaches() {
#ifdef DEBUG
    printf("set_reaches() run\n");
#endif
    for (auto &listener : listeners) {
        for (auto &place : places) {
            if (inDistance(listener.x, listener.y, place.x, place.y,
                           max_range)) {
                listener.reached_from.push_back(&place);
                place.reachable.push_back(&listener);
            }
        }
    }
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

#ifdef DEBUG
int *placed_arr;
int c = 0;
#endif

void deploy(int lis_i, int cost, bool reached[], int placed[], int n_reached) {

    if (cost > best_cost && best_cost != -1)
        return;

    Listener &listener = listeners[lis_i];
    bool curr_reached[n_listeners];
    memcpy(curr_reached, reached, n_listeners * sizeof(bool));
    int curr_placed[n_places];
    memcpy(curr_placed, placed, n_places * sizeof(placed[0]));
    int curr_n_reached = n_reached;
    int curr_cost = cost;
    
    for (auto place : listener.reached_from) {
        // TODO think should i clean this? after recursive call
        if (curr_placed[place->index])
            continue;
        for (auto &antena : antenas) {
            if (antena.radius != 0) {
                curr_placed[place->index] = antena.index;
                memcpy(curr_reached, reached, n_listeners * sizeof(bool));
                // memcpy(curr_placed, placed, n_places * sizeof(placed[0]));
                curr_n_reached = n_reached;
                curr_cost = cost + antena.cost;
                if (curr_cost >= best_cost && best_cost != -1)
                    break;
                // For each person in reach by this antena at this place
                for (auto &person_in_reach : place->reachable) {
// Recalculating range multiple times
// Solutions: array of distances / antena type
#ifdef DEBUG
                    c++;
#endif
                    if (curr_reached[person_in_reach->index])
                        continue;
                    if (inRange(*place, *person_in_reach, antena.radius))
                        if (!curr_reached[person_in_reach->index]) {
                            curr_reached[person_in_reach->index] = true;
                            curr_n_reached++;
                            if (curr_n_reached == n_listeners) {
                                // This is a solution
                                if (curr_cost < best_cost || best_cost == -1) {
                                    best_cost = curr_cost;
                                    return;
                                }
                            }
                        }
                }
            }

            // !!! Probably redundant
            if (curr_cost >= best_cost && best_cost != -1)
                continue;

            while (curr_reached[lis_i + 1] && lis_i + 1 < n_listeners)
                lis_i++;
            if (lis_i + 1 < n_listeners)
                deploy(lis_i + 1, curr_cost, curr_reached, curr_placed,
                       curr_n_reached);
        }
    }
}

void sort_range_const() {
    sort(antenas.begin(), antenas.end(),
         [](const Antena &a1, const Antena &a2) -> bool {
             if (a1.cost != a2.cost)
                 return a1.cost < a2.cost;
             return a1.radius > a2.radius;
         });
}
// Antenas w
void filter_costly_antenas() {
#ifdef DEBUG
    printf("Vector in before filter:\n");
    for (auto &a : antenas)
        printf("(c:%d,r:%d)\n", a.cost, a.radius);
#endif
    uint i = 0, j;
    vector<Antena> temp(antenas);
    // ordered by ascending cost and descending range
    for (j = 1; j < temp.size(); j++) {
        if (temp[i].radius < temp[j].radius)
            antenas[++i] = temp[j];
    }
    antenas.resize(i + 1);
#ifdef DEBUG
    printf("Vector out after filter:\n");
    for (auto &a : antenas)
        printf("(c:%d,r:%d)\n", a.cost, a.radius);
#endif
}

int main(int argc, char const *argv[]) {
    get_input();
    sort_range_const();
    filter_costly_antenas();
    set_reaches();

#ifdef DEBUG
    placed_arr = new int[n_places];
    memset(placed_arr, 0, n_places * sizeof(int));

    for (auto &li : listeners) {
        printf("Listener %d %d\n", li.x, li.y);
        for (auto re : li.reached_from) {
            printf("\t %d %d\n", re->x, re->y);
        }
    }
#endif

    bool reached[n_listeners];
    memset(reached, false, n_listeners * sizeof(bool));
    int placed[n_places];
    memset(placed, 0, n_places * sizeof(placed[0]));
    deploy(0, 0, reached, placed, 0);

    // printf("Best cost: %d\n", best_cost);
    if (best_cost != -1) {
        printf("%d\n", best_cost);
    } else {
        printf("no solution\n");
    }
#ifdef DEBUG
    printf("Inner count = %d\n", c);
#endif
    return 0;
}
