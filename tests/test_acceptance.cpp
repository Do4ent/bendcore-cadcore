#include <gtest/gtest.h>
#include "bendcore/document/entity_id.hpp"
#include "bendcore/document/cad_document.hpp"
#include "bendcore/dependency/dependency_graph.hpp"
#include "bendcore/parameters/parameter_store.hpp"

using namespace BendCore::CadCore;

/**
 * Acceptance tests for product requirements
 */

// Requirement: System must support stable entity identification
TEST(AcceptanceTest, StableEntityIdentification) {
    EntityId id1 = EntityId::generate();
    EntityId id2 = EntityId::parse(id1.toString());

    EXPECT_EQ(id1, id2);
    EXPECT_TRUE(EntityId::isValid(id1.toString()));
}

// Requirement: Document must track all entities
TEST(AcceptanceTest, EntityTracking) {
    CadDocument doc;
    std::vector<EntityId> ids;

    for (int i = 0; i < 10; i++) {
        Entity entity;
        entity.id = EntityId::generate();
        entity.state = LifecycleState::ACTIVE;
        doc.addEntity(entity);
        ids.push_back(entity.id);
    }

    for (const auto& id : ids) {
        EXPECT_TRUE(doc.hasEntity(id));
    }
}

// Requirement: Dependency graph must detect cycles
TEST(AcceptanceTest, CyclePrevention) {
    DependencyGraph graph;
    EntityId a = EntityId::generate();
    EntityId b = EntityId::generate();

    graph.addDependency(a, b);
    EXPECT_FALSE(graph.hasCycle());

    graph.addDependency(b, a);
    EXPECT_TRUE(graph.hasCycle());
}

// Requirement: Parameters must support unit conversion
TEST(AcceptanceTest, UnitConversion) {
    double mm_value = Units::toMM(1.0, "cm");
    EXPECT_DOUBLE_EQ(mm_value, 10.0);

    double in_value = Units::fromMM(25.4, "in");
    EXPECT_DOUBLE_EQ(in_value, 1.0);
}

// Requirement: System must maintain revision history
TEST(AcceptanceTest, RevisionTracking) {
    CadDocument doc;
    uint64_t rev1 = doc.getRevision();

    Entity entity;
    entity.id = EntityId::generate();
    entity.state = LifecycleState::ACTIVE;
    doc.addEntity(entity);

    uint64_t rev2 = doc.getRevision();
    EXPECT_GT(rev2, rev1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
