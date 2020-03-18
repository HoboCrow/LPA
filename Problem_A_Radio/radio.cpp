#include <bits/stdc++.h>
#include <stdio.h>

#include <limits>

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
template <>
struct hash<Antena> {
    std::size_t operator()(const Antena &a) const {
        return std::hash<int>{}(a.index);
    }
};
}  // namespace std

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
template <>
struct hash<Place> {
    std::size_t operator()(const Place &p) const {
        return std::hash<int>{}(p.index);
    }
};
}  // namespace std

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
int min_cost = numeric_limits<int>::max();

vector<Listener> listeners;
vector<Place> places;
vector<Antena> antenas;

void impossible() {
    printf("no solution\n");
    exit(1);
}

#ifdef DEBUG
void output_solution(int id) {
    char buff[24];
    snprintf(buff, 24, "./prog_out/out%03d.txt", id);
    FILE *f = fopen(buff, "w");
    for (auto &plc : places) fprintf(f, "%d ", plc.occupied);
    fclose(f);
}
#endif
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
    // TODO maybe remove 0 antena and clean code
    antenas.reserve(++n_antenas);
    // antenas.push_back(Antena(0, 0, 0));
    int r, c;
    for (int i = 1; i < n_antenas; i++) {
        scanf("%d %d", &r, &c);
        antenas.push_back(Antena(r, c, i));
        if (r > max_range || max_range == -1) max_range = r;
        if (c < min_cost) min_cost = c;
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
             if (a1.cost != a2.cost) return a1.cost < a2.cost;
             return a1.radius > a2.radius;
         });
}
// Antenas w
void filter_costly_antenas() {
#ifdef DEBUG
    printf("Vector in before filter:\n");
    for (auto &a : antenas) printf("(c:%d,r:%d)\n", a.cost, a.radius);
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
    for (auto &a : antenas) printf("(c:%d,r:%d)\n", a.cost, a.radius);
#endif
}

void sort_listeners_by_least_choise() {
    sort(listeners.begin(), listeners.end(),
         [](const Listener &p1, const Listener &p2) -> bool {
             return p1.reached_from.size() < p2.reached_from.size();
         });
}

int best_cost = numeric_limits<int>::max();

#ifdef DEBUG
int c = 0;  // inner loop count. rough performance estimate
int sol_count = 0;
#endif

void mark_listener(int lis_i, int cost, int marked) {
    Listener *person = &listeners[lis_i];
    // Verificação podia estár antes da chamada recursiva

    // if (person->marked && lis_i + 1 < n_listeners) {
    //     // marked before, lets try to mark the ones ahead first
    //     mark_listener(lis_i + 1, cost, marked);
    // }

    //     int estimated = (n_listeners - marked) * min_cost + cost;
    // #ifdef DEBUG
    //     printf("Estimated worst case solution: %d vs %d\n", estimated,
    //     best_cost); if (estimated >= best_cost) printf("Not worth!\n");
    // #endif
    //     if (estimated >= best_cost) return;

    for (auto &combo : person->reached_from) {
        Place *place = combo.first;
        if (place->occupied) continue;
        vector<Antena *> types = combo.second;
        for (auto type : types) {
            place->occupied = type->index;  // Could be moved up if bool
            if (cost + type->cost >= best_cost)
                continue;  // this antena is not worth it
#ifdef DEBUG
            c++;
#endif
            // mark all the people
            for (auto &p : place->people_in_reach[type]) {
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
                            output_solution(sol_count++);
#endif
                            break;
                        }
                    }
                }
            }

            if (lis_i + 1 < n_listeners && marked < n_listeners)
                mark_listener(lis_i + 1, cost + type->cost, marked);
            // TODO consider using local variable for counting marked

            // Unmark all marked people in this iteration
            for (auto &p : place->people_in_reach[type]) {
                if (--(p->marked) == 0) marked--;
            }
        }
        // Desocupy place
        place->occupied = 0;
    }
}

int main(int argc, char const *argv[]) {
    get_input();
    if (places.size() == 0 || antenas.size() == 0 || listeners.size() == 0)
        impossible();
    // sort_cost_range();       // Doesnt seem to make a diference
    // filter_costly_antenas(); // Doesnt seem to make a diference
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
        for (auto &entry : place.people_in_reach) {
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
    if (places.size() == 0) impossible();

        // TODO maybe mark some befor, ex: only have one possibility
        // sort_listeners_by_least_choise();
#ifdef DEBUG
    printf("Min cost: %d\n", min_cost);
#endif
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