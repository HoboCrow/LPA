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
    vector<int> servers;
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
    int dists[1000][1000];
    // Kruskal
    vector<int> set_k;
    vector<int> rank_k;

    pair<vector<arc>, int> kruskal(vector<int> &G) {
        set_k.resize(N);
        rank_k.resize(N);
        vector<arc> links;
        int total = 0;
        int N = G.size();

        for (int v = 0; v < N; ++v) make_set(G[v]);

        vector<arc> arcs;
        for (int i = 0; i < N - 1; i++) {
            for (int j = i + 1; j < N; j++) {
                int sv1 = G[i];
                int sv2 = G[j];
                arcs.push_back({sv1, sv2, dists[sv1][sv2]});
            }
        }
        sort(arcs.begin(), arcs.end(),
             [](arc &a, arc &b) -> bool { return a.cost < b.cost; });

        for (auto &arc : arcs) {
            int u = arc.a;
            int v = arc.b;
            if (find_set(arc.a) != find_set(arc.b)) {
                links.push_back(arc);
                total += arc.cost;
                union_sets(u, v);
            }
        }
        // printf("Dists in Kruskall : %d\n", temp);
        return make_pair(links, total);
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
    vector<int> aps;
    int t = 1;
    vector<int> sub_graph_nodes;
    vector<bool> marked;
    int min_index;
    vector<int> articulation_points_rec(int v, vector<int> *aps) {
        if (!aps) {
            aps = new vector<int>;
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
                    aps->push_back(v);
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
    vector<lig> neighbors(int i) { return adjlist[i]; }
    vector<group> groups;
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
        // vector<int> aps = articulation_points_rec(0, NULL);
        FW();

        for (int i = 0; i < N; i++) {
            if (!marked[i]) {
                sub_graph_nodes.clear();
                t = 1;
                vector<int> aps = articulation_points_rec(i, NULL);
                sort(aps.begin(), aps.end());
                auto end = unique(aps.begin(), aps.end());
                aps.resize(distance(aps.begin(), end));
                groups.push_back({aps, sub_graph_nodes});
            }
        }
    }

    int total_connect_cost() {
        // printf("\n");
        int total = 0;
        for (auto &group : groups) {
            auto servers = group.servers;
            int N = servers.size();
            if (N < 2) continue;
            for (int i = 0; i < N - 1; i++)
                for (int j = i + 1; j < N; j++) {
                    int sv1 = servers[i];
                    int sv2 = servers[j];
                    total += dists[sv1][sv2];
                }
        }
        return total;
    }

    int tree_connect_cost() {
        int total = 0;
        for (auto group : groups) total += kruskal(group.servers).second;

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

        for (auto &aps : g->groups) sum += aps.servers.size();

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