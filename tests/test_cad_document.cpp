#include <gtest/gtest.h>
#include "bendcore/document/cad_document.hpp"

using namespace BendCore::CadCore;

class CadDocumentTest : public ::testing::Test {
protected:
    CadDocument doc;

    void SetUp() override {
        doc.clear();
    }
};

// CAD-CORE-003: Test 1 - Create empty document
TEST_F(CadDocumentTest, CreateEmptyDocument) {
    EXPECT_FALSE(doc.getDocumentId().isNull());
    EXPECT_EQ(doc.getRevision(), 0);
    EXPECT_EQ(doc.getEntityCount(), 0);
    EXPECT_EQ(doc.getActiveEntityCount(), 0);
}

// CAD-CORE-003: Test 2 - Add and retrieve entity
TEST_F(CadDocumentTest, AddAndRetrieveEntity) {
    Entity part(EntityType::PART, "Part_001");
    EntityId part_id = part.id;

    doc.addEntity(part);

    EXPECT_EQ(doc.getEntityCount(), 1);
    EXPECT_TRUE(doc.hasEntity(part_id));
    EXPECT_EQ(doc.getEntity(part_id).name, "Part_001");
}

// CAD-CORE-003: Test 3 - Entity IDs are unique
TEST_F(CadDocumentTest, EntityIdsAreUnique) {
    Entity e1(EntityType::PART, "Part_001");
    Entity e2(EntityType::PART, "Part_002");

    EntityId id1 = e1.id;
    EntityId id2 = e2.id;

    doc.addEntity(e1);
    doc.addEntity(e2);

    EXPECT_NE(id1, id2);
    EXPECT_EQ(doc.getEntityCount(), 2);
}

// CAD-CORE-003: Test 4 - Duplicate ID rejected
TEST_F(CadDocumentTest, DuplicateIdRejected) {
    Entity e1(EntityType::PART, "Part_001");
    Entity e2 = e1;  // Same ID
    e2.name = "Part_002";

    doc.addEntity(e1);
    EXPECT_THROW(doc.addEntity(e2), std::invalid_argument);
}

// CAD-CORE-003: Test 5 - Invalid operations don't corrupt document
TEST_F(CadDocumentTest, InvalidOperationsDontCorruptDocument) {
    Entity part(EntityType::PART, "Part_001");
    EntityId part_id = part.id;

    doc.addEntity(part);
    EXPECT_EQ(doc.getActiveEntityCount(), 1);

    // Try to add entity with invalid parent
    Entity child(EntityType::BODY, "Body_001");
    child.parent_id = EntityId::generate();  // Non-existent parent
    EXPECT_THROW(doc.addEntity(child), std::invalid_argument);

    // Document should still be valid
    EXPECT_EQ(doc.getActiveEntityCount(), 1);
    EXPECT_TRUE(doc.hasEntity(part_id));
}

// CAD-CORE-003: Test 6 - Sequential changes increase revision
TEST_F(CadDocumentTest, SequentialChangesIncreaseRevision) {
    uint64_t rev0 = doc.getRevision();

    Entity e1(EntityType::PART, "Part_001");
    doc.addEntity(e1);
    uint64_t rev1 = doc.getRevision();
    EXPECT_GT(rev1, rev0);

    Entity e2(EntityType::PART, "Part_002");
    doc.addEntity(e2);
    uint64_t rev2 = doc.getRevision();
    EXPECT_GT(rev2, rev1);
}

// CAD-CORE-003: Test 7 - 10,000 sequential changes pass
TEST_F(CadDocumentTest, LargeSequentialChanges) {
    const int COUNT = 10000;
    std::vector<EntityId> ids;

    for (int i = 0; i < COUNT; ++i) {
        Entity part(EntityType::PART, "Part_" + std::to_string(i));
        EntityId id = part.id;
        doc.addEntity(part);
        ids.push_back(id);
    }

    EXPECT_EQ(doc.getActiveEntityCount(), COUNT);

    // Verify all are still retrievable
    for (const auto& id : ids) {
        EXPECT_TRUE(doc.hasEntity(id));
    }
}

// CAD-CORE-003: Test 8 - Remove entity marks as deleted
TEST_F(CadDocumentTest, RemoveEntityMarksDeleted) {
    Entity part(EntityType::PART, "Part_001");
    EntityId part_id = part.id;

    doc.addEntity(part);
    EXPECT_EQ(doc.getActiveEntityCount(), 1);
    EXPECT_TRUE(doc.hasEntity(part_id));

    doc.removeEntity(part_id);
    EXPECT_EQ(doc.getActiveEntityCount(), 0);
    EXPECT_FALSE(doc.hasEntity(part_id));

    // But entity still in storage
    EXPECT_EQ(doc.getEntityCount(), 1);
}

// CAD-CORE-003: Test 9 - Document maintains internal references
TEST_F(CadDocumentTest, DocumentMaintainsInternalReferences) {
    Entity part(EntityType::PART, "Part_001");
    Entity body(EntityType::BODY, "Body_001");
    body.parent_id = part.id;

    doc.addEntity(part);
    doc.addEntity(body);

    EXPECT_EQ(doc.getEntity(body.id).parent_id, part.id);
}

// CAD-CORE-003: Test 10 - Query entities by type
TEST_F(CadDocumentTest, QueryEntitiesByType) {
    Entity part(EntityType::PART, "Part_001");
    Entity body1(EntityType::BODY, "Body_001");
    Entity body2(EntityType::BODY, "Body_002");

    doc.addEntity(part);
    doc.addEntity(body1);
    doc.addEntity(body2);

    auto parts = doc.getEntitiesByType(EntityType::PART);
    auto bodies = doc.getEntitiesByType(EntityType::BODY);

    EXPECT_EQ(parts.size(), 1);
    EXPECT_EQ(bodies.size(), 2);
}

// CAD-CORE-003: Test 11 - Parameters storage and retrieval
TEST_F(CadDocumentTest, ParametersStorageAndRetrieval) {
    Entity part(EntityType::PART, "Part_001");
    EntityId part_id = part.id;
    doc.addEntity(part);

    Parameter p1;
    p1.name = "Length";
    p1.value = 100.0;
    p1.units = "mm";
    p1.owner_id = part_id;

    doc.addParameter(part_id, p1);

    auto retrieved = doc.getParameter(part_id, "Length");
    EXPECT_EQ(retrieved.value, 100.0);
    EXPECT_EQ(retrieved.units, "mm");
}

// CAD-CORE-003: Test 12 - References tracking
TEST_F(CadDocumentTest, ReferencesTracking) {
    Entity target(EntityType::BODY, "Body_001");
    EntityId target_id = target.id;
    doc.addEntity(target);

    Reference ref;
    ref.target_id = target_id;
    ref.description = "Used in assembly";

    doc.addReference(ref);

    auto refs = doc.getReferences(target_id);
    EXPECT_EQ(refs.size(), 1);
    EXPECT_EQ(refs[0].description, "Used in assembly");
}

// CAD-CORE-003: Test 13 - Validate document
TEST_F(CadDocumentTest, ValidateDocument) {
    Entity part(EntityType::PART, "Part_001");
    doc.addEntity(part);

    std::string error = doc.validate();
    EXPECT_TRUE(error.empty()) << "Error: " << error;
}

// CAD-CORE-003: Test 14 - Detect duplicate IDs
TEST_F(CadDocumentTest, DetectDuplicateIds) {
    Entity e1(EntityType::PART, "Part_001");
    doc.addEntity(e1);

    EXPECT_FALSE(doc.hasDuplicateIds());
}

// CAD-CORE-003: Test 15 - Non-existent entity throws
TEST_F(CadDocumentTest, NonExistentEntityThrows) {
    EntityId non_existent = EntityId::generate();
    EXPECT_THROW(doc.getEntity(non_existent), std::invalid_argument);
    EXPECT_FALSE(doc.hasEntity(non_existent));
}

// CAD-CORE-003: Test 16 - Clear document
TEST_F(CadDocumentTest, ClearDocument) {
    Entity part(EntityType::PART, "Part_001");
    doc.addEntity(part);
    EXPECT_GT(doc.getEntityCount(), 0);

    doc.clear();
    EXPECT_EQ(doc.getEntityCount(), 0);
    EXPECT_EQ(doc.getRevision(), 0);
}

// CAD-CORE-003: Test 17 - Metadata storage
TEST_F(CadDocumentTest, MetadataStorage) {
    Entity part(EntityType::PART, "Part_001");
    part.metadata["creator"] = "user@example.com";
    part.metadata["date"] = "2026-10-25";

    EntityId part_id = part.id;
    doc.addEntity(part);

    auto& retrieved = doc.getEntity(part_id);
    EXPECT_EQ(retrieved.metadata["creator"], "user@example.com");
    EXPECT_EQ(retrieved.metadata["date"], "2026-10-25");
}

// CAD-CORE-003: Test 18 - Hierarchy validation
TEST_F(CadDocumentTest, HierarchyValidation) {
    Entity part(EntityType::PART, "Part_001");
    Entity body(EntityType::BODY, "Body_001");

    // Set body parent to non-existent part
    body.parent_id = part.id;

    // Adding child without parent in document throws
    EXPECT_THROW(doc.addEntity(body), std::invalid_argument);

    // But after adding parent, it should work
    doc.addEntity(part);
    EXPECT_NO_THROW(doc.addEntity(body));
}

// CAD-CORE-003: Test 19 - Get all entity IDs
TEST_F(CadDocumentTest, GetAllEntityIds) {
    Entity e1(EntityType::PART, "Part_001");
    Entity e2(EntityType::BODY, "Body_001");
    Entity e3(EntityType::SKETCH, "Sketch_001");

    EntityId id1 = e1.id;
    EntityId id2 = e2.id;
    EntityId id3 = e3.id;

    doc.addEntity(e1);
    doc.addEntity(e2);
    doc.addEntity(e3);

    auto ids = doc.getEntityIds();
    EXPECT_EQ(ids.size(), 3);
    EXPECT_TRUE(std::find(ids.begin(), ids.end(), id1) != ids.end());
    EXPECT_TRUE(std::find(ids.begin(), ids.end(), id2) != ids.end());
    EXPECT_TRUE(std::find(ids.begin(), ids.end(), id3) != ids.end());
}

// CAD-CORE-003: Test 20 - Document revision is document-scoped
TEST_F(CadDocumentTest, DocumentRevisionScopedCorrectly) {
    CadDocument doc1;
    CadDocument doc2;

    Entity e1(EntityType::PART, "Part_001");
    Entity e2(EntityType::PART, "Part_001");

    doc1.addEntity(e1);
    doc2.addEntity(e2);

    EXPECT_NE(doc1.getDocumentId(), doc2.getDocumentId());
    EXPECT_GT(doc1.getRevision(), 0);
    EXPECT_GT(doc2.getRevision(), 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
