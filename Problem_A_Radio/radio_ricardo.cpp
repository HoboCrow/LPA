#include <algorithm>
#include <iostream>
#include <map>
#include <math.h>
#include <stdio.h>
#include <vector>
using namespace std;

// declarações
class Listener;
class Place;
class Antena;
vector<Listener *> listeners;
vector<Place *> places;
vector<Antena *> antenas;
vector<Antena *> good_antenas;
int number_listeners;
int number_places;
int number_antenas;
int number_good_antenas;
int max_radius = 2147483647;
int best_cost = 2147483647;

class Listener {
  private:
    int x;
    int y;
    int covered;

  public:
    Listener(int x, int y) {
        SetX(x);
        SetY(y);
        SetCovered(0);
    }
    map<Place *, vector<Antena *>> covered_by;
    int GetX() { return this->x; }
    int GetY() { return this->y; }
    int GetCovered() { return this->covered; }
    int GetCoveredAndIncrement() { return this->covered++; }
    void SetX(int x) { this->x = x; }
    void SetY(int y) { this->y = y; }
    void SetCovered(int covered) { this->covered = covered; }
    void print() {
        printf("Listener with x = %d, y = %d and covered = %d .\n", GetX(),
               GetY(), GetCovered());
    }
};

class Place {
  private:
    int x;
    int y;
    int used;

  public:
    Place(int x, int y) {
        SetX(x);
        SetY(y);
        SetUsed(0);
    }
    map<Antena *, vector<Listener *>> reached_listeners;
    int GetX() { return this->x; }
    int GetY() { return this->y; }
    int GetUsed() { return this->used; }
    void SetX(int x) { this->x = x; }
    void SetY(int y) { this->y = y; }
    void SetUsed(int used) { this->used = used; }
    void print() {
        printf("Place with x = %d, y = %d and used = %d .\n", GetX(), GetY(),
               GetUsed());
    }
};

class Antena {
  private:
    int radius;
    int cost;
    int index;

  public:
    Antena(int radius, int cost) {
        SetRadius(radius);
        SetCost(cost);
        SetIndex(0);
    }
    int GetRadius() const { return this->radius; }
    int GetCost() const { return this->cost; }
    int GetIndex() const { return this->index; }
    void SetRadius(int radius) { this->radius = radius; }
    void SetCost(int cost) { this->cost = cost; }
    void SetIndex(int index) { this->index = index; }
    void print() {
        printf("Antena with cost = %d and radius = %d .\n", GetCost(),
               GetRadius());
    }
};

void no_solution() {
    printf("no solution\n");
    exit(0);
}

void read() {
    // temporarios para coordenadas x e y ou cost e range
    int temp_1, temp_2;

    // Listeners
    scanf("%d", &number_listeners);
    listeners.reserve(number_listeners);
    for (int i = 0; i < number_listeners; i++) {
        scanf("%d %d", &temp_1, &temp_2);
        listeners.push_back(new Listener(temp_1, temp_2));
    }

    // Places
    scanf("%d", &number_places);
    places.reserve(number_places);
    for (int i = 0; i < number_places; i++) {
        scanf("%d %d", &temp_1, &temp_2);
        places.push_back(new Place(temp_1, temp_2));
    }

    // Antenas
    scanf("%d", &number_antenas);
    antenas.reserve(number_antenas);
    for (int i = 0; i < number_antenas; i++) {
        scanf("%d %d", &temp_1, &temp_2);
        antenas.push_back(new Antena(temp_1, temp_2));
        if (temp_1 > max_radius)
            max_radius = temp_1;
        // if(temp_2 < min_cost) min_cost = temp_2;
    }
}

bool covers(Place &place, Listener &listener, int range) {
    return ((listener.GetX() - place.GetX()) *
            (listener.GetX() - place.GetX())) +
               ((listener.GetY() - place.GetY()) *
                (listener.GetY() - place.GetY())) <=
           (range * range);
}

void calculate_ranges() {
    for (auto listener : listeners) {
        for (auto place : places) {
            if (covers(*place, *listener, max_radius)) {
                for (auto antena : good_antenas) {
                    if (covers(*place, *listener, antena->GetRadius())) {
                        listener->covered_by[place].push_back(antena);
                        place->reached_listeners[antena].push_back(listener);
                    }
                }
            }
        }
        // Se nao houver nada que o alcance acaba
        if (!listener->covered_by.size()) {
            no_solution();
            exit(0);
        }
    }
};

bool listener_comp(const Listener *listener_1, const Listener *listener_2) {
    int list_1 = 0, list_2 = 0;
    for (auto &place : listener_1->covered_by) {
        list_1 += place.second.size();
    }
    for (auto &place : listener_2->covered_by) {
        list_2 += place.second.size();
    }
    return list_1 < list_2;
}

void sort_listeners() {
    sort(listeners.begin(), listeners.end(), listener_comp);
}

bool antenas_comp(const Antena *antena_1, const Antena *antena_2) {
    if (antena_1->GetCost() == antena_2->GetCost()) {
        return antena_1->GetRadius() > antena_2->GetRadius();
    }
    return antena_1->GetCost() < antena_2->GetCost();
}

void sort_antenas() { sort(antenas.begin(), antenas.end(), antenas_comp); }

void filter_antenas() {
    int temp_radius = -1, temp_cost = -1, i = 0;
    for (auto antena : antenas) {
        if (antena->GetCost() > temp_cost &&
            antena->GetRadius() > temp_radius) {
            temp_cost = antena->GetCost();
            temp_radius = antena->GetRadius();
            number_good_antenas += 1;
            antena->SetIndex(i);
            good_antenas.push_back(antena);
            i++;
        }
    }
}

void listeners_check(int list, int cost, int covered) {

    Listener *listener = listeners[list];
    // se a pessoa já està covered tenta a proxima
    if (listener->GetCovered() && (list + 1) < number_listeners) {
        listeners_check(list + 1, cost, covered);
        return;
    }

    // se não está covered verifica
    for (auto par : listener->covered_by) {
        Place *place = par.first;
        if (place->GetUsed()) // Se em uso com antena que nao chega à pessoa
            continue;

        for (auto antena : par.second) {
            if ((cost + antena->GetCost()) >= best_cost)
                continue; // nao vale a pena testar a antena uma vez que é mais
                          // cara
            place->SetUsed(1);

            // Marcar as pessoas apanhadas pelo conjunto lugar-antena
            for (auto lis : place->reached_listeners[antena]) {
                if (lis->GetCoveredAndIncrement() == 0) {
                    covered++;
                    if (covered == number_listeners) {
                        // encontrou uma solução
                        if (cost + antena->GetCost() < best_cost) {
                            best_cost = cost + antena->GetCost();
                        }
                    }
                }
            }

            if (list + 1 < number_listeners && covered < number_listeners)
                listeners_check(list + 1, cost + antena->GetCost(), covered);

            for (auto lis : place->reached_listeners[antena]) {
                lis->SetCovered(lis->GetCovered() - 1);
                if (lis->GetCovered() == 0)
                    covered--;
            }

            place->SetUsed(0);
        }
    }
}

int main() {
    read();
    if (!number_places || !number_antenas || !number_listeners)
        no_solution();
    sort_antenas();
    filter_antenas();
    calculate_ranges();
    sort_listeners();
    listeners_check(0, 0, 0);

    printf("%d\n", best_cost);
    return 0;
}
