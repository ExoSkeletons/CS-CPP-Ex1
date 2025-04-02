//
// Created by Aviad Levine on 22/03/2025.
//

#ifndef GRAPH_H
#define GRAPH_H

#include "data_structures.h"
#include <climits>

using namespace ds;

namespace graphs {
    class Graph {
        int e = 0;

    public:
        class Edge {
        public:
            int vertex;
            int weight;

            Edge(const int to_index, const int weight) {
                this->vertex = to_index;
                this->weight = weight;
            }

            Edge(const int to_index): Edge(to_index, 1) {
            }

            Edge(): Edge(0) {
            }

            bool operator==(const Edge &e) const {
                return this->vertex == e.vertex // && this->weight == e.weight
                        ;
            }

            bool operator<(const Edge &e) const {
                return this->weight < e.weight;
            }
        };

        class VtxDist : public Edge {
        public:
            static constexpr int INF = INT_MAX;

            int src;

            VtxDist(const int src, const int vertex, const int dist): Edge(vertex, dist), src(src) {
            }

            VtxDist(const int src, const int vertex) : VtxDist(src, vertex, INF) {
            }

            VtxDist(const int vertex) : VtxDist(0, vertex) {
            }

            VtxDist() : VtxDist(0, 0) {
            }

            int vertex() const {
                return this->Edge::vertex;
            }

            int dist() const { return weight; }

            void relax(int d);

            bool operator==(const VtxDist &c) const {
                return this->src == c.src && Edge::operator==(c);
            }

            auto operator<(const VtxDist &c) const {
                return weight != INF && (c.weight == INF || Edge::operator<(c));
            }
        };

        const int n;
        LinkedList<Edge> *neighbour_list;
        const bool directed;

        Graph(int n, bool directed);

        explicit Graph(const int n) : Graph(n, false) {
        }

        Graph(const Graph *copy, bool copy_edges);

        explicit Graph(const Graph *copy): Graph(copy, true) {
        }

        ~Graph() {
            delete[] neighbour_list;
        }

        int m() const;

        void addEdge(int u, int v, int weight = 1);

        void deleteEdge(int u, int v);

        bool hasEdge(int u, int v) const;

        bool hasVtx(int u) const;

        int weight(int u, int v) const;

        int weight() const;

        bool hasNegativeWeights() const;

        void print_graph() const;


        friend std::ostream &operator<<(std::ostream &os, const Graph &g);
    };

    class Algorithms {
        static void dfs_recursive(const Graph *graph, int u, Graph *result, bool *visited);

    public:
        static Graph *bfs(const Graph *graph, int src);

        static Graph **dfs(const Graph *graph, int);

        static Graph *djikstra(const Graph *graph, int src);

        static Graph *prim(const Graph *graph, int src);

        static Graph *kruskal(const Graph *graph);
    };

    std::ostream &operator<<(std::ostream &os, const Graph &g);

    static void assert_graph(const Graph *graph) {
        if (graph == nullptr) throw std::invalid_argument("graph can't be null");
        if (graph->n == 0) throw std::invalid_argument("graph is empty");
    }

    static void assert_graph_vtx(const Graph *graph, const int v) {
        if (!graph->hasVtx(v)) throw std::invalid_argument("node " + std::to_string(v) + " doesn't exist");
    }

    static void assert_graph_non_negative(const Graph *graph) {
        if (graph->hasNegativeWeights())
            throw std::invalid_argument("negative weights are not supported");
    }
} // graphs


#endif //GRAPH_H
