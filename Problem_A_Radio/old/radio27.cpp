#include <bits/stdc++.h>
#include <limits>
#include <stdio.h>

using namespace std;

class Place;
class Antena;
class Listener;

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
    bool operator==(const Antena &a) const { return a.index == index; }
};

namespace std {
template <> struct hash<Antena> {
    std::size_t operator()(const Antena &a) const {
        return std::hash<int>{}(a.index);
    }
};
} // namespace std

class Place {
  public:
    int x;
    int y;
    int index;
    int occupied;
    unordered_map<Antena *, vector<Listener *>> people_in_reach;
    Place(int x, int y, int i) {
        this->x = x;
        this->y = y;
        index = i;
    }
    bool operator==(const Place &p) const { return p.index == index; }
};
namespace std {
template <> struct hash<Place> {
    std::size_t operator()(const Place &p) const {
        return std::hash<int>{}(p.index);
    }
};
} // namespace std

class Listener {
  public:
    int index;
    int x;
    int y;
    int marked;
    unordered_map<Place *, vector<Antena *>> reached_from;
    Listener(int x, int y, int i) {
        this->x = x;
        this->y = y;
        index = i;
        marked = 0;
    }
};

int n_listeners;
int n_places;
int n_antenas;
int max_range = -1;

vector<Listener> listeners;
vector<Place> places;
vector<Antena> antenas;

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
    for (auto &person : listeners) {
        for (auto &place : places) {
            if (inRange(place, person, max_range)) {
                for (auto &type : antenas) {
                    if (inRange(place, person, type.radius)) {
                        person.reached_from[&place].push_back(&type);
                        place.people_in_reach[&type].push_back(&person);
                    }
                }
            }
        }
    }
}

void sort_cost_range() {
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

// void filter_places_no_reach() {
// #ifdef DEBUG
//     printf("Places before filter (%ld):\n", places.size());
//     for (auto &place : places)
//         printf("\tPlace %d %d\n", place.x, place.y);
// #endif
//     // TODO verificar atena na pos do listener
//     sort(places.begin(), places.end(), [](Place &p1, Place &p2) -> bool {
//         // Manda os places que só tem a antena 0 como possibilidade
//         if (p1.reach_pairs.size() == 1)
//             return false;
//         return true;
//     });

//     int i = places.size() - 1;
//     while (i >= 0 && places[i].reach_pairs.size() == 1)
//         i--;
//     places.resize(i + 1, Place(0, 0, -1));
//     n_places = i + 1;

// #ifdef DEBUG
//     printf("Places after filter (resized to %d):\n", i + 1);
//     for (auto &place : places)
//         printf("\tPlace %d %d\n", place.x, place.y);
// #endif
// }

// void sort_options_by_cost_listener_ratio() {
//     for (auto &place : places) {
//         sort(place.reach_pairs.begin(), place.reach_pairs.end(),
//              [](pair<Antena *, vector<Listener *> *> v1,
//                 pair<Antena *, vector<Listener *> *> v2) -> bool {
//                  if (v2.second == NULL)
//                      return false;

//                  return v1.second->size() > v2.second->size();
//              });
//     }
// }

int best_cost = numeric_limits<int>::max();

void mark_listener(int lis_i, int cost, int marked) {
    Listener *person = &listeners[lis_i];
    // TODO check marked
    for (auto &combo : person->reached_from) {
        Place *place = combo.first;
        if (place->occupied)
            continue;

        vector<Antena *> types = combo.second;
        for (auto type : types) {
            place->occupied = type->index; // Could be moved up if bool TODO
            if (cost + type->cost >= best_cost)
                continue;
            // mark all the people

            for (auto &p : place->people_in_reach[type]) { // TODO consider making hash
                                                   // with pointer
                if (p->marked++ == 0) {
                    marked++;
                    if (marked == n_listeners) {
                        // This is a solution
                        if (cost + type->cost < best_cost) {
                            // Best solution so far
                            best_cost = cost + type->cost;
#ifdef DEBUG
                            printf("Solution: [");
                            for (auto &plc : places)
                                printf("%d ", plc.occupied);
                            printf("]\n");
#endif
                        }
                    }
                }
            }

            // while (lis_i + 1 < n_listeners && listeners[lis_i + 1].marked)
            //     lis_i++;

            if (lis_i + 1 < n_listeners && marked < n_listeners)
                mark_listener(lis_i + 1, cost + type->cost, marked);
            // TODO consider using local variable for counting marked

            // Unmark all marked people in this iteration
            for (auto &p : place->people_in_reach[type]) {
                if (--(p->marked) == 0)
                    marked--;
            }
        }

        place->occupied = 0;
    }
}

int main(int argc, char const *argv[]) {
    get_input();
    if (places.size() == 0 || antenas.size() == 0 || listeners.size() == 0)
        impossible();
    // sort_cost_range();
    // filter_costly_antenas();
    set_reaches();
#ifdef DEBUG
    printf("People view:\n");
    for (auto &person : listeners) {
        printf("Person (%d,%d):\n", person.x, person.y);
        for (auto &entry : person.reached_from) {
            Place *place = entry.first;
            vector<Antena *> antenas = entry.second;
            printf("|- Place (%d,%d):\n", place->x, place->y);
            for (auto type : antenas) {
                printf("||- Antena: cost: %d radius %d\n", type->cost,
                       type->radius);
            }
        }
    }
    printf("Place view:\n");
    for (auto &place : places) {
        printf("Place (%d,%d):\n", place.x, place.y);
        for (auto &entry : place.reaches) {
            Antena *antena = entry.first;
            vector<Listener *> listeners = entry.second;
            printf("|- Antena cost : %d radius: %d):\n", antena->cost,
                   antena->radius);
            for (auto listener : listeners) {
                printf("||- Listener (%d,%d)\n", listener->x, listener->y);
            }
        }
    }
#endif
    // filter_places_no_reach();
    // sort_options_by_cost_listener_ratio();
    if (places.size() == 0)
        impossible();

    // TODO maybe mark some befor, ex: only have one possibility

    mark_listener(0, 0, 0);

    // printf("Best cost: %d\n", best_cost);
    if (best_cost != numeric_limits<int>::max()) {
        printf("%d\n", best_cost);
    } else {
        printf("no solution\n");
    }

#ifdef DEBUG
    printf("Inner count = %d\n", c);
#endif
    return 0;
}