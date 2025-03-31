//
// Created by Aviad Levine on 23/03/2025.
//

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "graph.h"
#include "data_structures.h"

using namespace ds;
using namespace graphs;

namespace algorithms {
    class algorithm {
        static void dfs_recursive(const Graph *graph, int u, Graph *result, LinkedList<int> *visited);

    public:
        static Graph *bfs(const Graph *graph, int src);

        static Graph *dfs(const Graph *graph, const int src) {
            const auto result = new Graph(graph, false);
            LinkedList<int> visited;

            dfs_recursive(graph, src, result, &visited);

            return result;
        }

        static int *djikstra(const Graph *graph, int src);

        static Graph *prim(const Graph *graph, int src);

        static Graph *kruskal(const Graph *graph);
    };
} // algorithms

#endif //ALGORITHMS_H
