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
    void print() { printf("Antena %d r:%d c:%d\n", index, radius, cost); }
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
    void print() {
        printf("Place %d x:%d y:%d ocup:%d\n", index, x, y, occupied);
    }
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

    void print() { printf("Person %d x:%d y%d m:%d\n", index, x, y, marked); }
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
    antenas.reserve(n_antenas);
    int r, c;
    for (int i = 0; i < n_antenas; i++) {
        scanf("%d %d", &r, &c);
        antenas.push_back(Antena(r, c, i));
        if (r > max_range)
            max_range = r;
        if (c < min_cost)
            min_cost = c;
    }
}

inline bool inRange(Place &plc, Listener &lis, int range) {
    return (lis.x - plc.x) * (lis.x - plc.x) +
               (lis.y - plc.y) * (lis.y - plc.y) <=
           range * range;
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



int best_cost = numeric_limits<int>::max();
void mark_listener(int lis_i, int cost, int marked) {
    Listener *person = &listeners[lis_i];

    if (person->marked && lis_i + 1 < n_listeners) {
        mark_listener(lis_i + 1, cost, marked);
        return;
    }

    //     int estimated = (n_listeners - marked) * min_cost + cost;
    // 
    //     if (estimated >= best_cost) return;

    for (auto &combo : person->reached_from) {
        Place *place = combo.first;

        if (place->occupied)
            continue; // occupied with a type that does not reach this person

        vector<Antena *> types = combo.second;
        for (auto type : types) {
            if (cost + type->cost >= best_cost)
                continue;                      // this antena is not worth it
            place->occupied = type->index + 1; // Could be moved up if bool


            // mark all the people
            for (auto &p : place->people_in_reach[type]) {
                if (p->marked++ == 0) {
                    marked++;
                    if (marked == n_listeners) {
                        // This is a solution
                        if (cost + type->cost < best_cost) {
                            // Best solution so far
                            best_cost = cost + type->cost;

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
                if (--(p->marked) == 0)
                    marked--;
            }
            place->occupied = 0;
        }
    }
}



bool listener_compare(const Listener &p1, const Listener &p2) {
    int sum1 = 0, sum2 = 0;
    for (auto &options : p1.reached_from)
        sum1 += options.second.size();
    for (auto &options : p2.reached_from)
        sum2 += options.second.size();
    return sum1 < sum2;
}

void sort_listeners_by_least_choise() {
    // Listeners with least options at the begining
    sort(listeners.begin(), listeners.end(), listener_compare);
}

int main(int argc, char const *argv[]) {
    get_input();
    if (places.size() == 0 || antenas.size() == 0 || listeners.size() == 0)
        impossible();
    set_reaches();

    sort_listeners_by_least_choise();

    mark_listener(0, 0, 0);

    if (best_cost != numeric_limits<int>::max()) {
        printf("%d\n", best_cost);
    } else {
        printf("no solution\n");
    }


    return 0;
}