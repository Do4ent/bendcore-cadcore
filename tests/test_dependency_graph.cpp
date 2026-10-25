#include <gtest/gtest.h>
#include "bendcore/dependency/dependency_graph.hpp"

using namespace BendCore::CadCore;

TEST(DependencyGraphTest, AddDependency) {
    DependencyGraph graph;
    EntityId a = EntityId::generate();
    EntityId b = EntityId::generate();

    graph.addDependency(a, b);
    EXPECT_TRUE(graph.hasEdge(a, b));
}

TEST(DependencyGraphTest, DetectCycle) {
    DependencyGraph graph;
    EntityId a = EntityId::generate();
    EntityId b = EntityId::generate();

    graph.addDependency(a, b);
    EXPECT_FALSE(graph.hasCycle());

    graph.addDependency(b, a);
    EXPECT_TRUE(graph.hasCycle());
}

TEST(DependencyGraphTest, TopologicalSort) {
    DependencyGraph graph;
    EntityId a = EntityId::generate();
    EntityId b = EntityId::generate();
    EntityId c = EntityId::generate();

    graph.addDependency(b, a);
    graph.addDependency(c, b);

    auto order = graph.topologicalSort();
    EXPECT_EQ(order.size(), 3);
}

TEST(DependencyGraphTest, GetDependents) {
    DependencyGraph graph;
    EntityId a = EntityId::generate();
    EntityId b = EntityId::generate();

    graph.addDependency(b, a);
    auto deps = graph.getDependents(a);
    EXPECT_EQ(deps.size(), 1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
