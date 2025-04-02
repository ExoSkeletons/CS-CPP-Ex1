//
// Created by Aviad Levine on 22/03/2025.
//

#include "graph.h"

#include <iostream>
#include <stdexcept>

using namespace ds;

namespace graphs {
    /* Graph */

    void Graph::VtxDist::relax(const int d) {
        if ((weight == INF && d != INF) || weight > d) weight = d;
    }

    Graph::Graph(const int n, const bool directed) : n(n), directed(directed) {
        if (n < 0) throw std::invalid_argument("n must be positive");

        neighbour_list = new LinkedList<Edge>[n];
    }

    Graph::Graph(const Graph *copy, const bool copy_edges): Graph(copy->n, copy->directed) {
        if (copy_edges)
            for (int i = 0; i < n && i < copy->n; i++)
                neighbour_list[i].addLast(&copy->neighbour_list[i]);
    }

    int Graph::m() const {
        return static_cast<int>(static_cast<float>(e) * (directed ? 1 : .5f));
    }

    void Graph::addEdge(const int u, const int v, const int weight) {
        assert_graph_vtx(this, u);
        assert_graph_vtx(this, v);
        // if (weight < 0) throw std::invalid_argument("weight must be positive");

        if (hasEdge(u, v)) return;

        neighbour_list[u].addLast(Edge(v, weight)); // u -> v
        e++;
        if (!directed) {
            e++;
            neighbour_list[v].addLast(Edge(u, weight));
        } // v -> u
    }

    void Graph::deleteEdge(const int u, const int v) {
        assert_graph_vtx(this, u);
        assert_graph_vtx(this, v);

        if (!hasEdge(u, v)) return;

        neighbour_list[u].removeValue(v);
        e--;
        if (!directed) {
            neighbour_list[v].removeValue(u);
            e--;
        }
    }

    bool Graph::hasEdge(const int u, const int v) const {
        assert_graph_vtx(this, u);
        assert_graph_vtx(this, v);

        if (u == v) return true;

        return neighbour_list[u].contains(Edge(v)) || (!directed && neighbour_list[v].contains(Edge(u)));
    }

    bool Graph::hasVtx(const int u) const {
        return 0 <= u && u < n;
    }

    int Graph::weight(const int u, const int v) const {
        assert_graph_vtx(this, u);
        assert_graph_vtx(this, v);

        if (u == v) return 0;
        if (!hasEdge(u, v)) throw std::out_of_range("No such edge " + std::to_string(u) + "->" + std::to_string(v));

        auto neighbour = neighbour_list[u].head;
        while (neighbour) {
            if (neighbour->val.vertex == v)
                return neighbour->val.weight;
            neighbour = neighbour->next;
        }

        return VtxDist::INF;
    }

    int Graph::weight() const {
        int sum = 0;
        for (int i = 0; i < n; i++) {
            auto neighbour = neighbour_list[i].head;
            while (neighbour) {
                sum += neighbour->val.weight;
                neighbour = neighbour->next;
            }
        }
        return static_cast<int>(static_cast<float>(sum) * (directed ? 1 : .5f));
    }

    bool Graph::hasNegativeWeights() const {
        for (int u = 0; u < n; u++) {
            auto n = neighbour_list[u].head;
            while (n) {
                if (n->val.weight < 0)
                    return true;
                n = n->next;
            }
        }
        return false;
    }

    void Graph::print_graph() const {
        std::cout << *this;
    }

    /* Algorithms */

    Graph *Algorithms::bfs(const Graph *graph, const int src) {
        assert_graph(graph);
        assert_graph_vtx(graph, src);

        const auto result = new Graph(graph, false);
        Queue<int> gray, marked;

        gray.insert(src);
        while (!gray.isEmpty()) {
            if (const auto u = gray.pop(); !marked.contains(u)) {
                marked.insert(u);

                auto neighbour = graph->neighbour_list[u].head;
                while (neighbour) {
                    if (const auto v = neighbour->val.vertex; !marked.contains(v) && !gray.contains(v)) {
                        gray.insert(neighbour->val.vertex);
                        result->addEdge(u, v);
                    }
                    neighbour = neighbour->next;
                }
            }
        }

        return result;
    }

    void Algorithms::dfs_recursive(const Graph *graph, const int u, Graph *result, bool *visited) {
        visited[u] = true;
        auto neighbour = graph->neighbour_list[u].head;
        while (neighbour) {
            const auto v = neighbour->val.vertex;
            if (!visited[v]) {
                result->addEdge(u, v);
                dfs_recursive(graph, v, result, visited);
            }
            neighbour = neighbour->next;
        }
    }

    Graph **Algorithms::dfs(const Graph *graph, const int src) {
        assert_graph(graph);

        bool visited[graph->n] = {false};
        const auto result = new Graph *[graph->n];
        for (int i = 0; i < graph->n; ++i) result[i] = nullptr;

        // start forest creation from src
        for (int v = src; v < graph->n && !visited[v]; v++) {
            if (v >= graph->n) v = 0; // in case of overflow loop back to 0 node

            result[v] = new Graph(graph, false);
            dfs_recursive(graph, v, result[v], visited);
        }

        return result;
    }

    Graph *Algorithms::djikstra(const Graph *graph, const int src) {
        assert_graph(graph);
        assert_graph_vtx(graph, src);
        assert_graph_non_negative(graph);

        // shortest distances result table. nodes who have non-inf values here are finalised.
        int s_dist[graph->n] = {};
        for (int i = 0; i < graph->n; i++) s_dist[i] = Graph::VtxDist::INF;
        s_dist[src] = 0;
        // shortest distances result tree.
        const auto sp_result_graph = new Graph(graph, false);

        // min heap containing each node's shortest distance *so far* from src.
        Queue<Graph::VtxDist> d;
        for (int i = 0; i < graph->n; i++)
            if (i != src)
                d.insert(Graph::VtxDist(src, i));
        d.insert(Graph::VtxDist(src, src, 0));

        // Find the shortest path for all vertices
        for (int i = 0; i < graph->n; i++) {
            // pop u with the minimal distance to src (first iteration pops src, sets result[src] to 0)
            const Graph::VtxDist su = d.popMin();
            const int u = su.vertex(), su_d = su.dist();
            // node is fully relaxed, mark final & add to result (the shortest path tree)
            s_dist[u] = su_d;
            sp_result_graph->addEdge(src, u, su_d);

            // relax u neighbours
            auto n = graph->neighbour_list[u].head;
            while (n) {
                const int v = n->val.vertex, uv_w = n->val.weight;
                Graph::VtxDist *sv = d.find(Graph::VtxDist(src, v));
                // relax uv if:
                //      u->v exists in g (we're iterating on u's neighbours so this is obv always true)
                //      neighbour v isn't in sp yet (marked INF)
                //      path src-->v->u is smaller than current distance src-->u
                if (s_dist[v] == Graph::VtxDist::INF)
                    sv->relax(su_d + uv_w);
                n = n->next;
            }
        }

        return sp_result_graph;
    }

    Graph *Algorithms::prim(const Graph *graph, const int src) {
        assert_graph(graph);
        assert_graph_vtx(graph, src);

        const auto bfs = Algorithms::bfs(graph, src);
        const int m = bfs->m();
        if (bfs == nullptr || m < graph->n - 1) {
            delete bfs;
            throw std::invalid_argument("Graph is not connected from src");
        }
        delete bfs;

        const auto result = new Graph(graph, false);

        bool in_mst[graph->n] = {};
        Graph::Edge *min_edges[graph->n] = {};

        in_mst[src] = true;

        while (result->m() < graph->n - 1) {
            for (int v = 0; v < graph->n; v++) {
                if (in_mst[v]) {
                    // we need IN->OUT edges.
                    // (i) is in mst, find min neighbour that is OUT:
                    Queue<Graph::Edge> q;
                    q.insert(&graph->neighbour_list[v]);
                    while (!q.isEmpty()) {
                        // we want only edges that connect to OUT of mst,
                        // so we discard edges (neighbours) that connect to vertexes we marked as IN the mst.
                        const auto min = q.popMin();
                        if (!in_mst[min.vertex]) {
                            // collect min edge for vtx #i
                            min_edges[v] = new Graph::Edge(min.vertex, min.weight);
                            break;
                        }
                    }
                }
            }
            // we have q with all minial edges from IN->OUT.
            // pop min of that and we got true min edge that crosses.
            int min_u = -1, min_w = INT_MAX;
            for (int u = 0; u < graph->n; u++) {
                if (min_edges[u])
                    if (min_w > min_edges[u]->weight) {
                        // track min edge
                        min_u = u;
                        min_w = min_edges[u]->weight;
                    }
            }
            if (min_u != -1) {
                // found new min edge to connect
                const auto v = min_edges[min_u]->vertex;
                // add new edge to mst
                result->addEdge(min_u, v, min_w);
                // mark added neighbour node as IN mst
                in_mst[v] = true;
            }
            // free & reset min edges list
            for (int i = 0; i < graph->n; i++) {
                if (min_edges[i]) {
                    delete min_edges[i];
                    min_edges[i] = nullptr;
                }
            }
        }
        return result;
    }

    Graph *Algorithms::kruskal(const Graph *graph) {
        assert_graph(graph);

        const auto result = new Graph(graph, false);
        // Sort all edges with min queue
        Queue<Graph::VtxDist> edges;
        // add all neighbours to queue
        for (int v = 0; v < graph->n; v++) {
            auto n = graph->neighbour_list[v].head;
            while (n) {
                const auto e = Graph::VtxDist(v, n->val.vertex, n->val.weight);
                if (n->val.vertex > v) // skip duplicate edges, kruskal is undirected
                    edges.insert(e);
                n = n->next;
            }
        }

        // with kruskal we use a union set for vertex connectivity
        UnionSet vertexes(graph->n);
        int m = 0;
        while (!edges.isEmpty() && m < graph->n) {
            // try to add most minimal edge
            const auto e = edges.popMin();
            const int u = e.src, v = e.vertex(), w = e.weight;

            // check for cycle (u v are united)
            if (vertexes.find(u) != vertexes.find(v)) {
                // mark uv as added, unite nodes on edge uv
                vertexes.unite(u, v);
                // add edge count
                result->addEdge(u, v, w);
                m++;
            }
        }
        return result;
    }

    /* Friendly Operators */

    std::ostream &operator<<(std::ostream &os, const Graph &g) {
        os <<
                g.n << "-vtx, " <<
                g.m() << "-edge " <<
                (g.directed ? "" : "un") << "directed" <<
                " graph:\n";
        for (int u = 0; u < g.n; u++) {
            auto neighbour = g.neighbour_list[u].head;
            if (neighbour) {
                while (neighbour) {
                    const auto v = neighbour->val.vertex, w = neighbour->val.weight;
                    if (g.directed || v > u) // skip repeat edges for undirected
                        os << u << ")-" << w << (g.directed?"->":"-")<<"(" << v << "\t";
                    neighbour = neighbour->next;
                }
                os << "\n";
            }
        }
        return os;
    }
} // graphs
