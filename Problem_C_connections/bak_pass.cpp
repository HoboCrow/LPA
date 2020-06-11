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

struct arc {
    int a;
    int b;
    int cost;
};

struct lig {
    int v;
    int cost;
};

struct group {
    vector<int> aps;
    vector<int> nodes;
};

class Graph {
   private:
    // Graph info
    int N;
    vector<int> V;
    vector<arc> A;
    vector<vector<lig>> adjlist;
    vector<vector<int>> adjmat;
    vector<group> groups;
    int dists[1000][1000];
    // Kruskal
    vector<int> set_k;
    vector<int> rank_k;
    int temp;
    Graph *kruskal(int dists[][1000]) {
        temp = 0;
        Graph *t = new Graph();
        t->resize(N);
        set_k.resize(N);
        rank_k.resize(N);

        for (int v = 0; v < N; ++v) make_set(v);
        // Regenerate edges.. this is a mess :/
        vector<arc> arcs;
        for (auto sv1 = servers.begin(); sv1 != prev(servers.end()); sv1++) {
            for (auto sv2 = next(sv1); sv2 != servers.end(); sv2++) {
                arcs.push_back({*sv1, *sv2, dists[*sv1][*sv2]});
            }
        }
        sort(arcs.begin(), arcs.end(),
             [](arc &a, arc &b) -> bool { return a.cost < b.cost; });

        for (auto &arc : arcs) {
            int u = arc.a;
            int v = arc.b;
            if (find_set(u) != find_set(v)) {
                t->link(u, v, arc.cost);
                temp += dists[u][v];
                union_sets(u, v);
            }
        }
        printf("Dists in Kruskall : %d\n", temp);
        return t;
    }
    void make_set(int v) {
        // TODO verificar se corre
        // for (int i = 0; i < N; ++i) {
        set_k[v] = v;
        rank_k[v] = 0;
        // }
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
    vector<int> parent;
    set<int> aps;
    int t = 1;
    vector<int> sub_graph_nodes;
    vector<bool> marked;
    int min_index;
    set<int> articulation_points_rec(int v, set<int> *aps) {
        if (!aps) {
            aps = new set<int>;
            min_index = numeric_limits<int>::max();
        }
        min_index = 0;  // min(min_index, v);
        marked[v] = true;
        sub_graph_nodes.push_back(v);
        low[v] = dfs[v] = t++;
        for (auto wp : neighbors(v)) {
            int w = wp.v;
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

   public:
    Graph() {}
    Graph(int n) { resize(n); }
    ~Graph(){};
    vector<Graph *> sub_graphs;
    vector<vector<int>> aps_groups;
    set<int> servers;
    vector<lig> neighbors(int i) { return adjlist[i]; }
    int cost(int a, int b) { return adjmat[a][b]; }
    void resize(int n) {
        adjlist.resize(n);
        adjmat.resize(n);
        for (auto &a : adjmat) a.resize(n);
        N = n;
    }
    void link(int a, int b, int cost) {
        if (N < max(a, b) + 1) resize(max(a, b) + 1);

        adjlist[a].push_back({b, cost});
        adjlist[b].push_back({a, cost});

        if (adjmat[a][b] != 0) {
            printf("Matrix already has a value! Buggy code!\n");
            exit(1);
        }

        adjmat[a][b] = cost;
        adjmat[b][a] = cost;

        A.push_back({a, b, cost});
        A.push_back({b, a, cost});
        // TODO add to arcs
    }

    void gen_connected_graphs() {
        low.resize(N);
        dfs.resize(N);
        marked.resize(N);
        parent.resize(N);
        fill(dfs.begin(), dfs.end(), -1);
        // set<int> aps = articulation_points_rec(0, NULL);
        FW();

        for (int i = 0; i < N; i++) {
            if (!marked[i]) {
                Graph *subGraph = new Graph();
                sub_graph_nodes.clear();
                t = 1;
                set<int> aps = articulation_points_rec(i, NULL);
                subGraph->resize(sub_graph_nodes.size());
                subGraph->aps = aps;
                subGraph->servers = aps;
                subGraph->V = sub_graph_nodes;
                for (auto n : sub_graph_nodes)
                    for (auto neighbor : neighbors(n)) {
                        if (n < neighbor.v)
                            subGraph->link(n - min_index,
                                           neighbor.v - min_index,
                                           cost(n, neighbor.v));
                    }
                // subGraph->FW();
                sub_graphs.push_back(subGraph);
            }
        }
    }

    int total_connect_cost() {
        // printf("\n");
        int total = 0;
        for (auto &sub : sub_graphs) {
            if (sub->servers.size() < 2) continue;
            for (auto s1 = sub->servers.begin(); s1 != prev(sub->servers.end());
                 s1++) {
                for (auto s2 = next(s1); s2 != sub->servers.end(); s2++) {
                    total += dists[*s1][*s2];
                    // printf("%d <-> %d : %d (%d)\n", *s1 + 1, *s2 + 1,
                    //        sub->dists[*s1][*s2], total);
                }
            }
        }
        return total;
    }

    int tree_connect_cost() {
        for (auto &sub : sub_graphs) {
            if (sub->servers.size() < 2) continue;
            Graph *t = sub->kruskal(dists);
            t->servers = sub->servers;
            sub = t;
            // t->print();
        }
        FW();

        int total = 0;
        for (auto &sub : sub_graphs) {
            if (sub->servers.size() < 2) continue;
            // Total nodes except the first
            set<int> unlinked(next(sub->servers.begin()), sub->servers.end());
            // Only the first node
            set<int> linked;
            linked.insert(*(sub->servers.begin()));
            while (!unlinked.empty()) {
                // chose unlinked with least dist to a node in the linked set

                int min_dist = numeric_limits<int>::max();
                int min_i;
                for (auto &li : linked) {
                    for (auto &un : unlinked) {
                        int d = dists[li][un];
                        if (d < min_dist) {
                            min_dist = d;
                            min_i = un;
                        }
                    }
                }
                unlinked.erase(min_i);
                total += min_dist;
                linked.insert(min_i);
            }
        }
        return total;
    }

    void FW() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                int c = cost(i, j);
                dists[i][j] = c > 0 ? c : numeric_limits<int>::max() / 2 - 1;
            }
        }
        // TODO perguntar porque adjlist check dá TLE
        for (int k = 0; k < N; k++) {
            // if (adjlist[k].size() == 0) continue;
            for (int i = 0; i < N; i++) {
                // if (adjlist[i].size() == 0) continue;
                for (int j = 0; j <= i; j++) {
                    // if (adjlist[j].size() == 0) continue;
                    int d = dists[i][k] + dists[k][j];
                    if (dists[i][j] > d) dists[i][j] = dists[j][i] = d;
                    // dists[i][j] = 0;  // dists[j][i] =
                    // //        min(dists[i][j], dists[i][k] + dists[k][j]);
                }
            }
        }
    }
    void print() {
        printf("Graph:\n");
        for (int i = 0; i < N; i++) {
            printf("%d -> ", i + 1);
            for (int j = 0; j < (int)adjlist[i].size(); j++)
                printf("(%d,%d) ", adjlist[i][j].v + 1,
                       adjmat[i][adjlist[i][j].v]);
            printf("\n");
        }
        printf("FW distances:\n");
        int i;
        printf("     ");
        for (i = 0; i < N; i++) printf("%2d ", i + 1);
        printf("\n");
        i = 0;
        for (auto &line : dists) {
            printf("%2d | ", i++ + 1);
            for (auto &col : line) {
                printf("%2d ", col);
            }
            printf("\n");
        }
    }
};

vector<Graph *> get_input() {
    vector<Graph *> graphs;
    int n;
    int a, b, cost;
    char buffer[100];
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
        // // g->print();
        for (auto &cg : g->sub_graphs) {
            // printf("SUB\n");
            sum += cg->servers.size();
            // cg->resize(10);
            // cg->print();
        }

        if (sum == 1) {
            printf("1 0 0\n");
            continue;
        } else if (sum == 0) {
            printf("no server\n");
            continue;
        }
        printf("%d ", sum);
        printf("%d ", g->total_connect_cost());
        printf("%d", g->tree_connect_cost());

        printf("\n");
    }
    return 0;
}
