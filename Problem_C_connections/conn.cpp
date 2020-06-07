#include <stdio.h>
#include <algorithm>
#include <forward_list>
#include <iostream>
#include <limits>
#include <list>
#include <queue>
#include <set>
#include <stack>
#include <vector>

using namespace std;

struct PairSecondCompare {
    bool operator()(pair<int, int>& a, pair<int, int>& b) const {
        return a.second < b.second;
    }
};

typedef struct edge {
    int a;
    int b;
    int cost;
} edge;

class Graph {
   private:
    vector<vector<pair<int, int>>> adjlist;
    vector<vector<int>> adjmat;
    vector<edge> edges;
    forward_list<int>::iterator closest(int s, forward_list<int>& q, int d[]) {
        int min = numeric_limits<int>::max();
        forward_list<int>::iterator min_i;

        auto it = q.begin();
        auto prev = q.before_begin();
        for (; it != q.end(); prev = it, ++it) {
            if (d[*it] < min) {
                min = d[*it];
                min_i = prev;
            }
        }
        return min_i;
    }
    // Kruskal
    vector<int> set_k;
    vector<int> rank_k;
    Graph* kruskal() {
        Graph* t = new Graph(adjlist.size());
        set_k.resize(adjlist.size());
        rank_k.resize(adjlist.size());

        for (int v = 0; v < (int)adjlist.size(); ++v) make_set(v);

        sort(edges.begin(), edges.end(),
             [](edge& a, edge& b) -> bool { return a.cost < b.cost; });

        for (auto& edge : edges) {
            int u = edge.a;
            int v = edge.b;
            if (find_set(u) != find_set(v)) {
                t->link(u, v, edge.cost);
                union_sets(u, v);
            }
        }
        return t;
    }

    void make_set(int v) {
        for (int i = 0; i < (int)adjlist.size(); ++i) {
            set_k[i] = i;
            rank_k[i] = 0;
        }
    }
    int find_set(int a) {
        if (set_k[a] != a) set_k[a] = find_set(set_k[a]);
        return set_k[a];
    }
    void union_sets(int a, int b) { link_sets(find_set(a), find_set(b)); }
    void link_sets(int a, int b) {
        if (rank_k[a] > rank_k[b])
            set_k[b] = a;
        else
            set_k[a] = b;
        if (rank_k[a] == rank_k[b]) rank_k[b]++;
    }

   public:
    Graph(int n) {
        adjlist.resize(n);
        adjmat.resize(n);
        for (auto& a : adjmat) {
            a.resize(n);
        }
    };
    ~Graph(){};

    vector<pair<int, int>> neighbors(int i) { return adjlist[i]; }
    int cost(int a, int b) { return adjmat[a][b]; }
    void link(int a, int b, int cost) {
        adjlist[a].push_back(make_pair(b, cost));
        adjlist[b].push_back(make_pair(a, cost));

        adjmat[a][b] = cost;
        adjmat[b][a] = cost;

        edges.push_back({a, b, cost});
    }
    vector<int> prev;
    int dijkstra(int s, int t) {
        // Dijkstra
        forward_list<int> q;
        prev.resize(adjlist.size());
        for (int i = 0; i < (int)adjlist.size(); i++) q.push_front(i);

        int d[adjlist.size()];
        for (int i = 0; i < (int)adjlist.size(); i++) {
            d[i] = numeric_limits<int>::max();
            prev[i] = -1;
        }
        d[s] = 0;

        while (!q.empty()) {
            forward_list<int>::iterator u_prev = closest(s, q, d);
            forward_list<int>::iterator u_iter = next(u_prev);
            int u = *u_iter;
            q.erase_after(u_prev);

            if (u == t) break;
            for (auto v : neighbors(u)) {
                if (d[v.first] > d[u] + v.second) {
                    d[v.first] = d[u] + v.second;
                    prev[v.first] = u;
                }
            }
        }

        return d[t];
    }

    stack<int> shortest_path(int a, int b) {
        dijkstra(a, b);
        stack<int> path;
        if (prev[b] != -1 || b == a) {
            while (prev[b] != -1) {
                path.push(b);
                b = prev[b];
            }
        }
        return path;
    }

    vector<int> low;
    vector<int> dfs;
    vector<int> parent;
    vector<set<int>> aps;
    vector<bool> marked;
    vector<set<int>> articulation_points() {
        low.resize(adjlist.size());
        dfs.resize(adjlist.size());
        marked.resize(adjlist.size());
        fill(dfs.begin(), dfs.end(), -1);
        parent.resize(adjlist.size());
        aps.push_back(aps_rec(0, NULL));
        for (int i = 0; i < (int)adjlist.size(); i++)
            if (!marked[i]) aps.push_back(aps_rec(i, NULL));
        return aps;
    }
    int t = 1;
    set<int> aps_rec(int v, set<int>* aps) {
        if (!aps) aps = new set<int>;
        marked[v] = true;
        low[v] = dfs[v] = t++;
        for (auto wp : neighbors(v)) {
            int w = wp.first;
            if (dfs[w] == -1) {
                parent[w] = v;
                aps_rec(w, aps);
                low[v] = min(low[v], low[w]);
                if ((dfs[v] == 1 && dfs[w] != 2) ||
                    (dfs[v] != 1 && low[w] >= dfs[v])) {
                    aps->insert(v);
                }
            } else if (parent[v] != w)
                low[v] = min(low[v], dfs[w]);
        }
        return *aps;
    }

    int total_connect_cost(vector<set<int>> aps_list) {
        int total_cost = 0;
        for (auto& aps : aps_list) {
            set<pair<int, int>> total;
            for (auto ap1 = aps.begin(); ap1 != aps.end(); ++ap1) {
                for (auto ap2 = next(ap1); ap2 != aps.end(); ap2++) {
                    stack<int> path = shortest_path(*ap1, *ap2);
                    int ant = *ap1;
                    // printf("(%d -> %d ) %d ", *ap1, *ap2, ant);
                    while (!path.empty()) {
                        if (ant > path.top())
                            total.insert(make_pair(ant, path.top()));
                        else
                            total.insert(make_pair(path.top(), ant));
                        // printf("%d ", path.top());
                        ant = path.top();
                        path.pop();
                    }
                    // printf("\n");
                }
            }
            for (auto& link : total)
                total_cost += cost(link.first, link.second);
        }
        return total_cost;
    }

    int tree_connect_cost(vector<set<int>> aps_list) {
        Graph* t = kruskal();
        return t->total_connect_cost(aps);
    }
    void print() {
        for (int i = 0; i < (int)adjlist.size(); i++) {
            printf("%d -> ", i + 1);
            for (int j = 0; j < (int)adjlist[i].size(); j++)
                printf("(%d,%d) ", adjlist[i][j].first + 1,
                       adjlist[i][j].second);
            printf("\n");
        }
    }
};

vector<Graph*> get_input() {
    vector<Graph*> graphs;
    int n;
    int a, b, cost;
    char buffer[100];
    while (fgets(buffer, 100, stdin)) {
        sscanf(buffer, "%d", &n);
        Graph* g = new Graph(n);
        while (fgets(buffer, 100, stdin) &&
               sscanf(buffer, "%d %d %d", &a, &b, &cost) == 3) {
            g->link(a - 1, b - 1, cost);
        }
        graphs.push_back(g);
    }
    return graphs;
}

int main(int argc, char const* argv[]) {
    vector<Graph*> graphs = get_input();
    for (auto g : graphs) {
        vector<set<int>> aps = g->articulation_points();
        int sum = 0;
        for (auto& ap : aps) sum += ap.size();
        if (sum)
            printf("%d ", sum);
        else {
            printf("no server\n");
            continue;
        }

        printf("%d ", g->total_connect_cost(aps));
        printf("%d", g->tree_connect_cost(aps));

        printf("\n");
    }
    return 0;
}
