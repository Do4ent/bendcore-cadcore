#include <gtest/gtest.h>
#include "bendcore/dependency/rebuild_engine.hpp"

using namespace BendCore::CadCore;

TEST(RebuildEngineTest, MarkChanged) {
    DependencyGraph graph;
    RebuildEngine engine(graph);

    EntityId a = EntityId::generate();
    engine.markChanged(a);
    EXPECT_EQ(engine.getChangedCount(), 1);
}

TEST(RebuildEngineTest, ClearChanged) {
    DependencyGraph graph;
    RebuildEngine engine(graph);

    EntityId a = EntityId::generate();
    engine.markChanged(a);
    engine.clearChanged();
    EXPECT_EQ(engine.getChangedCount(), 0);
}

TEST(RebuildEngineTest, RebuildOrder) {
    DependencyGraph graph;
    RebuildEngine engine(graph);

    EntityId a = EntityId::generate();
    EntityId b = EntityId::generate();

    graph.addDependency(b, a);
    engine.markChanged(a);

    auto order = engine.getRebuildOrder();
    EXPECT_GE(order.size(), 1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
