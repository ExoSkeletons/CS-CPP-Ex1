//
// Created by Aviad Levine on 22/03/2025.
//

#include "graph.h"

#include <iostream>
#include <stdexcept>

using std::cout, std::endl;

using namespace ds;

namespace graphs {
    void Graph::VtxDist::relax(const int d) {
        if ((weight == INF && d != INF) || weight > d) weight = d;
    }

    Graph::Graph(const int n, const bool directed) : n(n), directed(directed) {
        if (n <= 0) throw std::invalid_argument("n must be positive");

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
        if (u < 0 || v < 0 || u >= n || v >= n) throw std::invalid_argument("both u and v must be positive");
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
        if (u < 0 || v < 0 || u >= n || v >= n) throw std::invalid_argument("both u and v must be positive");

        if (!hasEdge(u, v)) return;

        neighbour_list[u].removeValue(v);
        e--;
        if (!directed) {
            neighbour_list[v].removeValue(u);
            e--;
        }
    }

    bool Graph::hasEdge(const int u, const int v) const {
        if (u < 0 || v < 0 || u >= n || v >= n) throw std::invalid_argument("both u and v must be positive");

        if (u == v) return true;

        return neighbour_list[u].contains(Edge(v)) || (!directed && neighbour_list[v].contains(Edge(u)));
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

    void Graph::print_graph(const bool all_vtx) const {
        for (int i = 0; i < n; i++) {
            auto neighbour = neighbour_list[i].head;
            if (!neighbour && !all_vtx) continue;

            if (!neighbour)
                cout << i << " " << "NONE" << endl;
            else {
                while (neighbour) {
                    cout << i << ")-" << neighbour->val.weight << "->(" << neighbour->val.vertex << "\t";
                    neighbour = neighbour->next;
                }
                cout << endl;
            }
        }
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
} // graphs
