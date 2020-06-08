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
    int N;
    vector<vector<pair<int, int>>> adjlist;
    vector<vector<int>> adjmat;
    vector<edge> edges;
    forward_list<int>::iterator closest(int s, forward_list<int>& q,
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
    // Kruskal
    vector<int> set_k;
    vector<int> rank_k;
    Graph* kruskal() {
        Graph* t = new Graph(N);
        set_k.resize(N);
        rank_k.resize(N);

        for (int v = 0; v < N; ++v) make_set(v);
        // Regenerate edges.. this is a mess :/
        edges.clear();
        for (int i = 0; i < N; i++) {
            for (auto& n : neighbors(i))
                if (i < n.first)
                    edges.push_back({i, n.first, cost(i, n.first)});
        }
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

   public:
    Graph() : Graph(0) {}
    Graph(int n) {
        adjlist.resize(n);
        adjmat.resize(n);
        N = n;
        for (auto& a : adjmat) {
            a.resize(n);
        }
    };
    ~Graph(){};
    vector<pair<int, int>> neighbors(int i) { return adjlist[i]; }
    int cost(int a, int b) { return adjmat[a][b]; }
    void resize(int n) {
        adjlist.resize(n);
        adjmat.resize(n);
        for (auto& a : adjmat) a.resize(n);
        N = n;
    }
    void link(int a, int b, int cost) {
        if (N < max(a, b)) resize(max(a, b));

        adjlist[a].push_back(make_pair(b, cost));
        adjlist[b].push_back(make_pair(a, cost));

        adjmat[a][b] = cost;
        adjmat[b][a] = cost;

        // edges.push_back({a, b, cost});
    }
    vector<int> prev;
    vector<int> dijkstra(int s, int t) {
        // Dijkstra
        forward_list<int> q;
        prev.resize(N);
        for (int i = 0; i < N; i++) q.push_front(i);

        vector<int> d(N);
        for (int i = 0; i < N; i++) {
            d[i] = numeric_limits<int>::max();
            prev[i] = -1;
        }
        d[s] = 0;

        while (!q.empty()) {
            forward_list<int>::iterator u_prev = closest(s, q, d);
            forward_list<int>::iterator u_iter = next(u_prev);
            int u = *u_iter;
            q.erase_after(u_prev);

            if (t != -1 && u == t) break;
            for (auto v : neighbors(u)) {
                // if (d[v.first] > d[u] + v.second) {
                //     d[v.first] = d[u] + v.second;
                //     prev[v.first] = u;
                // }
                if (d[v.first] > d[u] + cost(v.first, u)) {
                    d[v.first] = d[u] + cost(v.first, u);
                    prev[v.first] = u;
                }
            }
        }

        return d;
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

    vector<deque<int>> possible_paths(int a, int b, int cost) {
        pp_paths.clear();
        pp_path.clear();
        pp_dfs(a, b, cost);
        return pp_paths;
    }

    void pp_dfs(int a, int b, int cost) {
        if (cost < 0) return;

        if (a == b) {
            pp_paths.push_back(pp_path);
            return;
        }

        for (auto& n : neighbors(a)) {
            pp_path.push_back(n.first);
            pp_dfs(n.first, b, cost - this->cost(a, n.first));
            pp_path.pop_back();
        }
    }

    vector<int> low;
    vector<int> dfs;
    vector<int> parent;
    set<int> aps;
    vector<bool> marked;
    vector<int> sub_graph_nodes;
    vector<Graph*> connected_graphs;
    void gen_connected_graphs() {
        low.resize(N);
        dfs.resize(N);
        marked.resize(N);
        parent.resize(N);
        fill(dfs.begin(), dfs.end(), -1);

        for (int i = 0; i < N; i++) {
            Graph* subGraph = new Graph();
            sub_graph_nodes.clear();
            set<int> aps;
            if (!marked[i]) {
                t = 1;
                aps = aps_rec(i, NULL);
                subGraph->resize(sub_graph_nodes.size());
                subGraph->aps = aps;

                for (auto n : sub_graph_nodes) {
                    for (auto neighbor : neighbors(n)) {
                        subGraph->link(n, neighbor.first,
                                       cost(n, neighbor.first));
                    }
                }
                connected_graphs.push_back(subGraph);
            }
        }
    }
    int t = 1;
    set<int> aps_rec(int v, set<int>* aps) {
        if (!aps) aps = new set<int>;
        marked[v] = true;
        sub_graph_nodes.push_back(v);
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

    deque<int> path_most_aps(vector<deque<int>> paths,
                             vector<set<int>> aps_list) {
        int max = 0;
        deque<int> max_path;
        for (auto& p : paths) {
            // count aps int path
            int sum = 0;
            for (auto& aps : aps_list) {
                vector<int> common(aps.size());
                deque<int> path_copy(p);  // TODO move up
                // sort(aps.begin(), aps.end());
                sort(path_copy.begin(), path_copy.end());
                auto end =
                    set_intersection(path_copy.begin(), path_copy.end(),
                                     aps.begin(), aps.end(), common.begin());
                for (auto start = common.begin(); start != end; start++) sum++;
            }
            if (sum > max) {
                max = sum;
                max_path = p;
            }
        }
        return max_path;
    }

    int total_connect_cost(vector<set<int>> aps_list) {
        int total_cost = 0;
        set<pair<int, int>> total;
        for (auto& aps : aps_list) {
            for (auto ap1 = aps.begin(); ap1 != aps.end(); ++ap1) {
                for (auto ap2 = next(ap1); ap2 != aps.end(); ++ap2) {
                    // TODO pensar em usar matriz para marcar os caminhos?
                    total_cost += dijkstra(*ap1, *ap2)[*ap2];
                }
            }
        }
        return total_cost;
    }

    int total_connect_cost_no_repeat(vector<set<int>> aps_list) {
        int total_cost = 0;
        for (auto& aps : aps_list) {
            set<pair<int, int>> total;
            for (auto ap1 = aps.begin(); ap1 != aps.end(); ++ap1) {
                for (auto ap2 = next(ap1); ap2 != aps.end(); ++ap2) {
                    deque<int> path = shortest_path(*ap1, *ap2);
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
            for (auto& link : total)
                total_cost += cost(link.first, link.second);
        }
        return total_cost;
    }

    void update_path_cost(vector<vector<int>>& adjmat, deque<int> path,
                          int val) {
        while (path.size() != 1) {
            int a = path.back();
            path.pop_back();
            int b = path.back();
            adjmat[a][b]--;
            adjmat[b][a]--;
        }
    }

    int tree_connect_cost(vector<set<int>> aps_list) {
        auto original_adjmat = this->adjmat;
        auto updated_adjmat = adjmat;
        for (auto& aps : aps_list) {
            for (auto ap1 = aps.begin(); ap1 != aps.end(); ++ap1) {
                for (auto ap2 = next(ap1); ap2 != aps.end(); ++ap2) {
                    printf("%d \n", *ap1);
                    vector<int> dist = dijkstra(*ap1, -1);
                    // vector<deque<int>> paths =
                    //     possible_paths(*ap1, *ap2, dist[*ap2]);
                    // for (auto& path : paths) {
                    //     path.push_front(*ap1);
                    //     update_path_cost(updated_adjmat, path, -1);
                    // }
                }
            }
        }
        this->adjmat = updated_adjmat;
        Graph* t = kruskal();
        this->adjmat = original_adjmat;
        t->adjmat = original_adjmat;
        return t->total_connect_cost_no_repeat(aps_list);
    }
    void print() {
        for (int i = 0; i < N; i++) {
            printf("%d -> ", i + 1);
            for (int j = 0; j < (int)adjlist[i].size(); j++)
                printf("(%d,%d) ", adjlist[i][j].first + 1,
                       adjmat[i][adjlist[i][j].first]);
            printf("\n");
        }
    }
};

vector<Graph*> get_input() {
    vector<Graph*> graphs;
    int n;
    int a, b, cost;
    char buffer[100];
    buffer[0] = '\0';
    char* ret = NULL;
    while ((ret = fgets(buffer, 100, stdin))) {
        sscanf(buffer, "%d", &n);
        Graph* g = new Graph(n);
        while ((ret = fgets(buffer, 100, stdin)) &&
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
        g->gen_connected_graphs();
        int sum = 0;
        for (auto& cg : g->connected_graphs) sum += cg->aps.size();

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
