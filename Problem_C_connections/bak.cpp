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
    bool operator()(pair<int, int> &a, pair<int, int> &b) const {
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
    // Graph info
    int N;
    vector<int> V;
    vector<vector<pair<int, int>>> adjlist;
    vector<vector<int>> adjmat;
    vector<edge> edges;

    // Kruskal
    vector<int> set_k;
    vector<int> rank_k;
    Graph *kruskal(vector<vector<int>> costs) {
        Graph *t = new Graph();
        t->resize(N);
        set_k.resize(N);
        rank_k.resize(N);

        for (int v = 0; v < N; ++v) make_set(v);
        // Regenerate edges.. this is a mess :/
        vector<edge> edges;
        for (int i = 0; i < N; i++) {
            for (auto &n : neighbors(i))
                if (i < n.first)
                    edges.push_back({i, n.first, costs[i][n.first]});
        }
        sort(edges.begin(), edges.end(),
             [](edge &a, edge &b) -> bool { return a.cost < b.cost; });

        for (auto &edge : edges) {
            int u = edge.a;
            int v = edge.b;
            if (find_set(u) != find_set(v)) {
                t->link(u, v, cost(u, v));
                union_sets(u, v);
            }
        }
        t->gen_connected_graphs();
        // Hacky :(
        // gen_coonected_graphs generates wrong APs for the tree
        // they should have been in a server variable, sparate from aps
        // or the subdivision should be separate from the AP generation
        auto g = sub_graphs.begin();
        auto tr = t->sub_graphs.begin();
        while (g != sub_graphs.end()) {
            (*tr)->aps = (*g)->aps;
            ++g;
            ++tr;
        }
        return t;
    }
    void make_set(int v) {
        for (int i = 0; i < N; ++i) {
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

    // Articulation points
    vector<int> low;
    vector<int> dfs;
    vector<bool> marked;
    vector<int> parent;
    set<int> aps;
    int t = 1;
    set<int> articulation_points_rec(int v, set<int> *aps) {
        if (!aps) aps = new set<int>;
        marked[v] = true;
        sub_graph_nodes.push_back(v);
        low[v] = dfs[v] = t++;
        for (auto wp : neighbors(v)) {
            int w = wp.first;
            if (dfs[w] == -1) {
                parent[w] = v;
                articulation_points_rec(w, aps);
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

    // Dijkstra
    vector<int> prev;
    vector<int> dijkstra(int s, int t) {
        // Dijkstra
        forward_list<int> Q;
        prev.resize(N);
        for (auto node : V) Q.push_front(node);
        vector<int> dist(N);

        for (int i = 0; i < N; i++) {
            dist[i] = numeric_limits<int>::max();
            prev[i] = -1;
        }
        dist[s] = 0;

        while (!Q.empty()) {
            forward_list<int>::iterator u_prev = closest(s, Q, dist);
            forward_list<int>::iterator u_iter = next(u_prev);
            int u = *u_iter;
            Q.erase_after(u_prev);

            if (t != -1 && u == t) break;
            for (auto v : neighbors(u)) {
                // if (dist[v.first] > dist[u] + v.second) {
                //     dist[v.first] = dist[u] + v.second;
                //     prev[v.first] = u;
                // }
                if (dist[v.first] > dist[u] + cost(v.first, u)) {
                    dist[v.first] = dist[u] + cost(v.first, u);
                    prev[v.first] = u;
                }
            }
        }

        return dist;
    }
    forward_list<int>::iterator closest(int s, forward_list<int> &q,
                                        vector<int> d) {
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

   public:
    Graph() : Graph(0) {}
    Graph(int n) {
        adjlist.resize(n);
        adjmat.resize(n);
        for (auto &a : adjmat) {
            a.resize(n);
        }
        N = n;
    };
    Graph(const Graph &old) {
        N = old.N;
        V = old.V;
        adjlist = old.adjlist;
        adjmat = old.adjmat;
        edges = old.edges;
        aps = old.aps;
        sub_graphs = old.sub_graphs;
    }
    ~Graph(){};
    vector<pair<int, int>> neighbors(int i) { return adjlist[i]; }
    int cost(int a, int b) { return adjmat[a][b]; }
    void resize(int n) {
        adjlist.resize(n);
        adjmat.resize(n);
        for (auto &a : adjmat) a.resize(n);
        N = n;
    }
    void link(int a, int b, int cost) {
        if (N < max(a, b) + 1) resize(max(a, b) + 1);

        adjlist[a].push_back(make_pair(b, cost));
        adjlist[b].push_back(make_pair(a, cost));

        if (adjmat[a][b] != 0) {
            printf("Matrix already has a value! Buggy code!\n");
            exit(1);
        }

        adjmat[a][b] = cost;
        adjmat[b][a] = cost;

        // TODO add to edges
    }
    deque<int> shortest_path(int a, int b) {
        dijkstra(a, b);
        deque<int> path;
        if (prev[b] != -1 || b == a) {
            while (prev[b] != -1) {
                path.push_back(b);
                b = prev[b];
            }
        }
        return path;
    }

    vector<deque<int>> pp_paths;
    deque<int> pp_path;
    // int count = 0;
    vector<deque<int>> possible_paths(int a, int b, int cost) {
        pp_paths.clear();
        pp_path.clear();
        pp_dfs(a, b, cost);
        // printf("Count (%d %d) : %d\n", a, b, count);
        return pp_paths;
    }
    void pp_dfs(int a, int b, int cost) {
        // count++;
        if (cost < 0) return;

        if (a == b) {
            pp_paths.push_back(pp_path);
            return;
        }

        for (auto &n : neighbors(a)) {
            pp_path.push_back(n.first);
            pp_dfs(n.first, b, cost - this->cost(a, n.first));
            pp_path.pop_back();
        }
    }

    vector<int> sub_graph_nodes;
    vector<Graph *> sub_graphs;
    void gen_connected_graphs() {
        low.resize(N);
        dfs.resize(N);
        marked.resize(N);
        parent.resize(N);
        fill(dfs.begin(), dfs.end(), -1);

        for (int i = 0; i < N; i++) {
            if (!marked[i]) {
                Graph *subGraph = new Graph();
                sub_graph_nodes.clear();
                t = 1;
                set<int> aps = articulation_points_rec(i, NULL);
                subGraph->resize(sub_graph_nodes.size());
                subGraph->aps = aps;
                subGraph->V = sub_graph_nodes;
                for (auto n : sub_graph_nodes)
                    for (auto neighbor : neighbors(n)) {
                        if (n < neighbor.first)
                            subGraph->link(n, neighbor.first,
                                           cost(n, neighbor.first));
                    }

                sub_graphs.push_back(subGraph);
            }
        }
    }

    int total_connect_cost() {
        int total_cost = 0;
        set<pair<int, int>> total;
        for (auto subGraph : sub_graphs) {
            auto aps = subGraph->aps;
            for (auto ap1 = aps.begin(); ap1 != aps.end(); ++ap1) {
                auto distances = subGraph->dijkstra(*ap1, -1);
                for (auto ap2 = next(ap1); ap2 != aps.end(); ++ap2) {
                    // TODO pensar em usar matriz para marcar os caminhos?
                    total_cost += distances[*ap2];
                }
            }
        }
        return total_cost;
    }

    int total_connect_cost_tree() {
        int total_cost = 0;
        set<pair<int, int>> total;
        for (auto subGraph : sub_graphs) {
            auto aps = subGraph->aps;
            for (auto ap1 = aps.begin(); ap1 != aps.end(); ++ap1) {
                auto distances = subGraph->dijkstra(*ap1, -1);
                for (auto ap2 = next(ap1); ap2 != aps.end(); ++ap2) {
                    // TODO pensar em usar matriz para marcar os caminhos?
                    total_cost += distances[*ap2];
                }
            }
        }
        return total_cost;
    }

    int total_connect_cost_no_repeat() {
        int total_cost = 0;
        for (auto subGraph : sub_graphs) {
            auto aps = subGraph->articulation_points();
            set<pair<int, int>> total;
            for (auto ap1 = aps.begin(); ap1 != aps.end(); ++ap1) {
                for (auto ap2 = next(ap1); ap2 != aps.end(); ++ap2) {
                    // Move this up TODO
                    deque<int> path = subGraph->shortest_path(*ap1, *ap2);
                    int ant = *ap1;
                    while (!path.empty()) {
                        if (ant > path.back())
                            total.insert(make_pair(ant, path.back()));
                        else
                            total.insert(make_pair(path.back(), ant));
                        ant = path.back();
                        path.pop_back();
                    }
                }
            }
            for (auto &link : total)
                total_cost += cost(link.first, link.second);
        }
        return total_cost;
    }

    void update_path_cost(vector<vector<int>> &adjmat, deque<int> path,
                          int val) {
        while (path.size() != 1) {
            int a = path.back();
            path.pop_back();
            int b = path.back();
            adjmat[a][b]--;
            adjmat[b][a]--;
        }
    }
    set<int> articulation_points() { return aps; }
    int tree_connect_cost() {
        // Calculate decreased costs
        // vector < vector<vector<deque<paths>>> paths;

        auto decreased_costs = adjmat;
        for (auto subGraph : sub_graphs) {
            auto aps = subGraph->articulation_points();
            for (auto ap1 = aps.begin(); ap1 != aps.end(); ++ap1) {
                vector<int> distances = subGraph->dijkstra(*ap1, -1);
                // vector<int> distances(aps.size());

                for (auto ap2 = next(ap1); ap2 != aps.end(); ++ap2) {
                    // // -1 on Dijstra means calculate all distances
                    vector<deque<int>> paths =
                        possible_paths(*ap1, *ap2, distances[*ap2]);
                    for (auto &path : paths) {
                        path.push_front(*ap1);
                        update_path_cost(decreased_costs, path, -1);
                    }
                }
            }
        }
        Graph *t = kruskal(decreased_costs);
        return t->total_connect_cost();
        // return 0;
    }

    void print() {
        for (int i = 0; i < N; i++) {
            printf("%d -> ", i + 1);
            for (int j = 0; j < N; j++)
                printf("(%d,%d) ", adjlist[i][j].first + 1,
                       adjmat[i][adjlist[i][j].first]);
            printf("\n");
        }
    }
};

vector<Graph *> get_input() {
    vector<Graph *> graphs;
    int n;
    int a, b, cost;
    char buffer[100];
    buffer[0] = '\0';
    char *ret = NULL;
    while ((ret = fgets(buffer, 100, stdin))) {
        sscanf(buffer, "%d", &n);
        Graph *g = new Graph(n);
        while ((ret = fgets(buffer, 100, stdin)) &&
               sscanf(buffer, "%d %d %d", &a, &b, &cost) == 3) {
            g->link(a - 1, b - 1, cost);
        }
        graphs.push_back(g);
    }
    return graphs;
}

int main(int argc, char const *argv[]) {
    vector<Graph *> graphs = get_input();
    for (auto g : graphs) {
        g->gen_connected_graphs();
        int sum = 0;
        for (auto &cg : g->sub_graphs) sum += cg->articulation_points().size();

        if (sum)
            printf("%d ", sum);
        else {
            printf("no server\n");
            continue;
        }
        // TODO quick 1 0 0 solution
        printf("%d ", g->total_connect_cost());
        printf("%d", g->tree_connect_cost());

        printf("\n");
    }
    return 0;
}
