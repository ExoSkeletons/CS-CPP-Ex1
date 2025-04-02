#include <iostream>
#include <ostream>

#include "graph.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using std::cout,std::endl;
using namespace graphs;

TEST_SUITE("graphs") {
    TEST_CASE("Create") {
        cout << "Testing graph creation" << endl;
        auto g = new Graph(3);
        CHECK_EQ(g->n, 3);
        delete g;
        g = new Graph(10);
        CHECK_EQ(g->n, 10);
        delete g;
        // CHECK_THROWS(new Graph(0));
        CHECK_THROWS(new Graph(-1));
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
    TEST_CASE("null") {
        cout << endl << "Testing algorithms fail on null graph";
        CHECK_THROWS(Algorithms::bfs(nullptr, 0));
        CHECK_THROWS(Algorithms::dfs(nullptr,0));
        CHECK_THROWS(Algorithms::djikstra(nullptr, 0));
        CHECK_THROWS(Algorithms::prim(nullptr, 0));
        CHECK_THROWS(Algorithms::kruskal(nullptr));
    }

    TEST_CASE("empty") {
        cout << endl << "Testing algorithms fail on empty graph";
        auto g = new Graph(0);
        CHECK_THROWS(Algorithms::bfs(g, 0));
        CHECK_THROWS(Algorithms::dfs(g,0));
        CHECK_THROWS(Algorithms::djikstra(g, 0));
        CHECK_THROWS(Algorithms::prim(g, 0));
        CHECK_THROWS(Algorithms::kruskal(g));
        delete g;
    }

    TEST_CASE("Graph") {
        cout << endl << "Testing algorithms with graph ";

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
            const auto bfs = Algorithms::bfs(g, src);

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
            const auto dfs_forest = Algorithms::dfs(g, 0);
            const auto dfs = dfs_forest[0];

            // dfs->print_graph();

            CHECK_EQ(dfs->m(), g->n-1);
            CHECK_EQ(dfs->n, g->n);
            CHECK(!dfs->directed);

            // check path
            CHECK(dfs->hasEdge(0,2));
            CHECK(dfs->hasEdge(2,3));
            CHECK(dfs->hasEdge(3,1));
            CHECK(dfs->hasEdge(1,5));
            CHECK(dfs->hasEdge(5,4));

            // check forest (in this case) is single-tree
            for (int v = 1; v < g->n; v++)
                CHECK(dfs_forest[v]==nullptr);

            // dfs->print_graph();

            for (int v = 0; v<g->n; v++)
                delete dfs_forest[v];
            delete[] dfs_forest;
        }

        SUBCASE("Djikstra") {
            cout << "Djikstra from (" << src << ")" << endl;
            const auto djikstra = Algorithms::djikstra(g, src);

            int d[g->n];
            for (int v = 0; v < g->n; v++) {
                d[v] = djikstra->weight(src, v);
            }


            // check path
            CHECK_EQ(d[0], 1);
            CHECK_EQ(d[1], 2); // 3)--10-->(1   >    3)--1-->(5)--0-->(0)--1-->1
            CHECK_EQ(d[2], 1);
            CHECK_EQ(d[3], 0); // (3)@
            CHECK_EQ(d[4], 1);
            CHECK_EQ(d[5], 1); // 3)--1-->(0)--0-->(5

            CHECK_EQ(d[src], 0); // @

            /**/
            cout << "dist from (" << src << "): ";
            for (int i = 0; i < g->n; i++)
                cout << "(" << i << ")" <<
                (d[i] == Graph::VtxDist::INF
                     ? "INF"
                     : (std::to_string(d[i]))
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
                CHECK_THROWS(Algorithms::djikstra(neg_graph, 0));
                delete neg_graph;
            }
        }

        SUBCASE("Prim") {
            cout << "Prim MST (Edge Cut) from (" << src << ")" << endl;
            const auto mst_p = Algorithms::prim(g, src);
            // check mst cost
            const auto cost = mst_p->weight();
            CHECK_EQ(cost, expected_mst_cost);
            cout << "cost: " << cost << endl;
            delete mst_p;
        }


        SUBCASE("Kruskal") {
            cout << "Kruskal MST (Union Find)" << endl;
            const auto mst_k = Algorithms::kruskal(g);
            // check mst cost
            const auto cost = mst_k->weight();
            CHECK_EQ(cost, expected_mst_cost);
            cout << "cost: " << cost << endl;
            delete mst_k;
        }

        delete g;
    }
}
