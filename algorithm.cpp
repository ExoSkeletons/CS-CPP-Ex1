//
// Created by Aviad Levine on 23/03/2025.
//

#include "algorithm.h"

#include <format>
#include <stdexcept>

namespace algorithms {
    void algorithm::dfs_recursive(const Graph *graph, const int u, Graph *result,
                                  LinkedList<int> *visited) {
        visited->addLast(u);
        auto neighbour = graph->neighbour_list[u].head;
        while (neighbour) {
            if (const auto v = neighbour->val.vertex; !visited->contains(v)) {
                result->addEdge(u, v);
                dfs_recursive(graph, v, result, visited);
            }
            neighbour = neighbour->next;
        }
    }

    Graph *algorithm::bfs(const Graph *graph, const int src) {
        if (src < 0 || src >= graph->n) throw std::invalid_argument("src node doesn't exist");

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

    int *algorithm::djikstra(const Graph *graph, const int src) {
        // negative weights check
        if (graph->hasNegativeWeights())
            throw std::invalid_argument("negative weights are not supported");

        // shortest distances result list. nodes who have values here are finalised.
        const auto s_dist = new int[graph->n];
        for (int i = 0; i < graph->n; i++) s_dist[i] = Graph::VtxDist::INF;
        s_dist[src] = 0;

        // min heap containing each nodes distance so far from src
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
            // node is fully relaxed, add to result (the shortest path tree)
            s_dist[u] = su_d;

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

        return s_dist;
    }

    Graph *algorithm::prim(const Graph *graph, const int src) {
        const Graph *bfs = algorithm::bfs(graph, src);
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

    Graph *algorithm::kruskal(const Graph *graph) {
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
} // algorithms
