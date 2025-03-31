#include <iostream>
#include <ostream>

#include "algorithm.h"
#include "graph.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using std::cout,std::endl;
using namespace graphs;
using namespace algorithms;

TEST_SUITE("graphs") {
    TEST_CASE("Create") {
        cout << "Testing graph creation" << endl;
        auto g = new Graph(3);
        CHECK_EQ(g->n, 3);
        delete g;
        g = new Graph(10);
        CHECK_EQ(g->n, 10);
        delete g;
    }

    TEST_CASE("Directed") {
        cout << "Testing edges (directed)" << endl;

        auto g = new Graph(3, true);

        CHECK(g->directed);

        g->addEdge(2, 0);
        CHECK(g->hasEdge(2,0));
        CHECK(!g->hasEdge(0,2));
        g->addEdge(0, 2);
        CHECK(g->hasEdge(0,2));
        CHECK_EQ(g->m(), 2);

        delete g;
    }

    TEST_CASE("Undirected") {
        cout << "Testing edges (undirected)" << endl;

        auto g = new Graph(4);

        CHECK(!g->directed);

        CHECK_THROWS(g->addEdge(-1,2));
        CHECK_THROWS(g->addEdge(0,g->n + 1));

        g->addEdge(2, 3);
        CHECK(g->hasEdge(2,3));
        CHECK(g->hasEdge(3,2));
        CHECK_EQ(g->m(), 1); // undirected bidirectional edges count as 1 edge
        g->deleteEdge(2, 3);
        CHECK(!g->hasEdge(2,3));

        delete g;
    }
}

TEST_SUITE("algorithms") {
    TEST_CASE("Graph") {
        cout << endl << "Testing algorithm ";

        const auto g = new Graph(6);
        g->addEdge(0, 2, 5);
        g->addEdge(0, 3, 1);
        g->addEdge(1, 3, 10);
        g->addEdge(1, 5);
        g->addEdge(2, 3);
        g->addEdge(2, 4);
        g->addEdge(3, 4);
        g->addEdge(4, 5);
        g->addEdge(5, 0, 0);

        constexpr auto src = 3, expected_mst_cost = 4;

        SUBCASE("BFS") {
            cout << "BFS" << endl;
            const auto bfs = algorithm::bfs(g, src);

            CHECK_EQ(bfs->m(), g->n-1);
            CHECK_EQ(bfs->n, g->n);
            CHECK(!bfs->directed);

            // check path
            CHECK(bfs->hasEdge(3,1));
            CHECK(bfs->hasEdge(3,2));
            CHECK(bfs->hasEdge(3,4));
            CHECK(bfs->hasEdge(0,5));
            CHECK(bfs->hasEdge(3,0));

            // check neighbour priority
            // check that all src neighbours are in bfs
            auto n = bfs->neighbour_list[src].head;
            while (n) {
                CHECK(bfs->hasEdge(src,n->val.vertex));
                n = n->next;
            }

            // bfs->print_graph();

            delete bfs;
        }

        SUBCASE("DFS") {
            cout << "DFS" << endl;
            const auto dfs = algorithm::dfs(g, src);

            CHECK_EQ(dfs->m(), g->n-1);
            CHECK_EQ(dfs->n, g->n);
            CHECK(!dfs->directed);

            // check path
            CHECK(dfs->hasEdge(3,0));
            CHECK(dfs->hasEdge(0,2));
            CHECK(dfs->hasEdge(2,4));
            CHECK(dfs->hasEdge(4,5));
            CHECK(dfs->hasEdge(5,1));

            // dfs->print_graph();

            delete dfs;
        }

        SUBCASE("Djikstra") {
            cout << "Djikstra from (" << src << ")" << endl;
            const auto djikstra = algorithm::djikstra(g, src);

            // check path
            CHECK_EQ(djikstra[0], 1);
            CHECK_EQ(djikstra[1], 2); // 3)--10-->(1   >    3)--1-->(5)--0-->(0)--1-->1
            CHECK_EQ(djikstra[2], 1);
            CHECK_EQ(djikstra[3], 0); // (3)@
            CHECK_EQ(djikstra[4], 1);
            CHECK_EQ(djikstra[5], 1); // 3)--1-->(0)--0-->(5

            CHECK_EQ(djikstra[src], 0); // @

            /**/
            cout << "dist from (" << src << "): ";
            for (int i = 0; i < g->n; i++)
                cout << "(" << i << ")" <<
                (djikstra[i] == Graph::VtxDist::INF
                     ? "INF"
                     : (std::to_string(djikstra[i]))
                ) << ", ";
            cout << endl;
            //*/

            delete djikstra;

            SUBCASE("Negative weights") {
                cout << "Testing negative weights (should be unsupported)" << endl;
                // djikstra on negative weight is unsupported
                auto neg_graph = new Graph(3);
                neg_graph->addEdge(0, 1, 1);
                neg_graph->addEdge(0, 2, 0);
                neg_graph->addEdge(1, 2, -1);
                CHECK_THROWS(algorithm::djikstra(neg_graph, 0));
                delete neg_graph;
            }
        }

        SUBCASE("Prim") {
            cout << "Prim MST (Edge Cut) from (" << src << ")" << endl;
            const auto mst_p = algorithm::prim(g, src);
            // check mst cost
            const auto cost = mst_p->weight();
            CHECK_EQ(cost, expected_mst_cost);
            cout << "cost: " << cost << endl;
            delete mst_p;
        }


        SUBCASE("Kruskal") {
            cout << "Kruskal MST (Union Find)" << endl;
            const auto mst_k = algorithm::kruskal(g);
            // check mst cost
            const auto cost = mst_k->weight();
            CHECK_EQ(cost, expected_mst_cost);
            cout << "cost: " << cost << endl;
            delete mst_k;
        }

        delete g;
    }
}
