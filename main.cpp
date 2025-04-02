//
// Created by Aviad Levine on 30/03/2025.
//


#include <iostream>

#include "graph.h"
using namespace graphs;


void algorithm_ui(const Graph *g) {
    int n, src = 0;
    do {
        std::cout << "Choose Algorithm:\n" <<
                "1. BFS\n" <<
                "2. DFS\n" <<
                "3. Djikstra\n" <<
                "4. Prim\n" <<
                "5. Kruskal\n" <<
                "0. <\n";
        std::cin >> n;
        if (n == 1 || n == 3 || n == 4) {
            std::cout << "Enter src vtx index:\n";
            std::cin >> src;
            if (!g->hasVtx(src)) {
                std::cout << "vtx doesn't exist\n";
                continue;
            }
        }
        const Graph *res = nullptr;
        try {
            switch (n) {
                case 0: break ;
                case 1: {
                    res = Algorithms::bfs(g, src);
                    break;
                }
                case 2: {
                    const auto forest = Algorithms::dfs(g, 0);
                    for (int i = 0; i < g->n && forest[i]; ++i) {
                        std::cout << "Tree [" << i << "]:\n";
                        forest[i]->print_graph();
                    }
                    for (int v = 0; v<g->n; v++)
                        delete forest[v];
                    delete[] forest;
                    break;
                }
                case 3: {
                    res = Algorithms::djikstra(g, src);
                    std::cout << "dist from (" << src << "): ";
                    for (int v = 0; v < g->n; v++) {
                        std::cout << "(" << v << ")";
                        const auto d = res->weight(src, v);
                        if (d == Graph::VtxDist::INF)std::cout << "INF";
                        else std::cout << d;
                        std::cout << ", ";
                    }
                    std::cout << "\n";
                    break;
                }
                case 4: {
                    res = Algorithms::prim(g, src);
                    break;
                }
                case 5: {
                    res = Algorithms::kruskal(g);
                    break;
                }
                default:
                    std::cout << "Wrong input\n";
            }
            if (res) res->print_graph();
            delete res;
        } catch (...) {
            delete res;
        }
    } while (n);
}

void graph_ui(Graph *g) {
    int n, u, v, w;
    do {
        std::cout << "Choose action:\n" <<
                "1. Print graph\n" <<
                "2. Run Algorithm\n" <<
                "3. Add edge\n" <<
                "4. Remove edge\n" <<
                "5. Check edge\n" <<
                "6. Check vtx\n" <<
                "0. Discard graph\n";
        std::cin >> n;
        if (n == 3 || n == 4 || n == 5) {
            std::cout << "Enter u,v indexes to specify edge:\n";
            std::cin >> u >> v;
        }
        try {
            switch (n) {
                case 0: { break; }
                case 1: {
                    g->print_graph();
                    break;
                }
                case 3: {
                    std::cout << "Enter weight:";
                    std::cin >> w;
                    std::cout << "Adding " << u << ")-" << w << "->(" << v << "\n";
                    g->addEdge(u, v, w);
                    break;
                }
                case 4: {
                    std::cout << "Removing " << u << "->" << v << "\n";
                    g->deleteEdge(u, v);
                    break;
                }
                case 5: {
                    std::cout << "edge is: " << u << ")-" << g->weight(u, v) << "->(" << v << "\n";
                    break;
                }
                case 6: {
                    std::cout << "Enter vtx index:";
                    std::cin >> u;
                    assert_graph_vtx(g, u);
                    std::cout << "vtx " << u << " has ";
                    if (g->neighbour_list[u].isEmpty()) {
                        std::cout << "no neighbours.";
                    } else {
                        std::cout << g->neighbour_list[u].length() << " neighbours: ";
                        auto neighbour = g->neighbour_list[u].head;
                        while (neighbour) {
                            v = neighbour->val.vertex, w = neighbour->val.weight;
                            std::cout << u << ")-" << w << "->(" << v << ", ";
                            neighbour = neighbour->next;
                        }
                    }
                    std::cout << "\n";
                    break;
                }
                case 2: {
                    algorithm_ui(g);
                    break;
                }
                default:
                    std::cout << "Wrong input\n";
            }
        } catch (const std::exception &e) {
            std::cout << "Exception: " << e.what() << '\n';
        }
    } while (n);
}

int main() {
    std::cout << "Hello World.\n\n";

    int n = 1;
    while (n) {
        std::cout << "Enter graph vtx # to create graph\n0 to quit\n";
        std::cin >> n;
        if (n > 0) {
            std::cout << "Creating graph " << n << "-vtx graph\n";
            const auto g = new Graph(n);
            graph_ui(g);
            std::cout << "Graph discarded\n";
            delete g;
        }
    }
    return 0;
}
