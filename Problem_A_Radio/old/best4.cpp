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
    int marked;
    vector<Place *> reached_from;
    Listener(int x, int y, int i) {
        this->x = x;
        this->y = y;
        index = i;
        marked = 0;
    }
};

class Place {
  public:
    int x;
    int y;
    int index;
    int occupied;
    vector<pair<Antena *, vector<Listener *> *>> reach_pairs;
    Place(int x, int y, int i) {
        this->x = x;
        this->y = y;
        index = i;
    }
};

int n_listeners;
int n_places;
int n_antenas;
int max_range = -1;

vector<Listener> listeners;
vector<Place> places;
vector<bool> occupied_places;
vector<Antena> antenas;

int best_cost = -1;

void impossible() {
    printf("no solution\n");
    exit(1);
}

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
    // occupied_places.reserve(n_places);
    // fill_n(occupied_places.begin(), n_places, false);

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
    printf("In set_reaches()\n");
#endif
    for (auto &place : places) {
        for (auto &type : antenas) {
            if (type.index == 0) {
                // null type
                place.reach_pairs.push_back(
                    make_pair<Antena *, vector<Listener *> *>(&type, NULL));
                continue;
            }
            vector<Listener *> *in_reach = new vector<Listener *>;
            for (auto &listener : listeners) {
                if (inDistance(listener.x, listener.y, place.x, place.y,
                               type.radius)) {
                    (*in_reach).push_back(&listener);
                }
            }
            if (!(*in_reach).empty()) {
                pair<Antena *, vector<Listener *> *> p(&type, in_reach);
                place.reach_pairs.push_back(p);
            } else {
                delete in_reach;
            }
        }
    }
#ifdef DEBUG
    printf("End\nList of places/antenas/people\n");
    for (auto &place : places) {
        printf("Place (%d %d):\n", place.x, place.y);
        for (auto &pair : place.reach_pairs) {
            if (pair.first->index == 0)
                continue;
            printf("\tRange %d:\n", pair.first->radius);
            for (auto listener : *pair.second) {
                printf("\t\t Listener (%d,%d)\n", listener->x, listener->y);
            }
        }
    }
#endif
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
        if (temp[i].radius < temp[j].radius) {
            antenas[++i] = temp[j];
            temp[j].index = i;
        }
    }
    antenas.resize(i + 1);
    n_antenas = i + 1;
#ifdef DEBUG
    printf("Vector out after filter:\n");
    for (auto &a : antenas)
        printf("(c:%d,r:%d)\n", a.cost, a.radius);
#endif
}
#ifdef DEBUG
int *placed_arr;
int c = 0;
#endif

void deploy_antenas(int plc_i, int cost, int reach_count) {
    Place &place = places[plc_i];

    for (auto &antena_types : place.reach_pairs) {
        Antena *antena = antena_types.first;
        vector<Listener *> *people = antena_types.second;
        if (antena->radius != 0) {

            if (cost + antena->cost > best_cost && best_cost != -1) {
                continue;
            }

            cost += antena->cost;

            for (auto &p : *people) {
                if (++(p->marked) == 1) { // first marking
                    if (++reach_count == n_listeners) {
                        // solution
                        if (cost < best_cost || best_cost == -1) {
                            best_cost = cost;
                            break;
                        }
                    }
                }
            }
        }

        // TODO recursive call
        if (plc_i + 1 < n_places && reach_count < n_listeners)
            deploy_antenas(plc_i + 1, cost, reach_count);

        // Undo cost, unmark people
        if (antena->radius != 0) {
            cost -= antena->cost;
            for (auto &p : *people) {
                if (--(p->marked) == 0)
                    reach_count--;
            }
        }
    }
}

void filter_places_no_reach() {
#ifdef DEBUG
    printf("Places before filter (%ld):\n", places.size());
    for (auto &place : places)
        printf("\tPlace %d %d\n", place.x, place.y);
#endif
    // TODO verificar atena na pos do listener
    sort(places.begin(), places.end(), [](Place &p1, Place &p2) -> bool {
        // Manda os places que só tem a antena 0 como possibilidade
        if (p1.reach_pairs.size() == 1)
            return false;
        return true;
    });

    int i = places.size() - 1;
    while (i >= 0 && places[i].reach_pairs.size() == 1)
        i--;
    places.resize(i + 1, Place(0, 0, -1));
    n_places = i + 1;

#ifdef DEBUG
    printf("Places after filter (resized to %d):\n", i + 1);
    for (auto &place : places)
        printf("\tPlace %d %d\n", place.x, place.y);
#endif
}

void sort_options_by_cost_listener_ratio() {
    for (auto &place : places) {
        sort(place.reach_pairs.begin(), place.reach_pairs.end(),
             [](pair<Antena *, vector<Listener *> *> v1,
                pair<Antena *, vector<Listener *> *> v2) -> bool {
                 if (v2.second == NULL)
                     return false;

                 return v1.second->size() > v2.second->size();
             });
    }
}

int main(int argc, char const *argv[]) {
    get_input();
    if (places.size() == 0 || antenas.size() == 0 || listeners.size() == 0)
        impossible();
    sort_range_const();
    filter_costly_antenas();
    set_reaches();
    // filter_places_no_reach();
    sort_options_by_cost_listener_ratio();
    if (places.size() == 0)
        impossible();

#ifdef DEBUG
    placed_arr = new int[n_places];
    memset(placed_arr, 0, n_places * sizeof(int));
#endif

    deploy_antenas(0, 0, 0);
    // TODO maybe mark some befor, ex: only have one possibility

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
