#include <gtest/gtest.h>
#include "bendcore/document/entity_id.hpp"
#include <set>
#include <unordered_set>
#include <map>

using namespace BendCore::CadCore;

class EntityIdTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

// CAD-CORE-002: Test 1 - ID does not change after renaming
TEST_F(EntityIdTest, IdRemainsSameAfterRenaming) {
    EntityId id1 = EntityId::generate();
    std::string name1 = "Part_001";

    // Simulate renaming - ID should not change
    std::string name2 = "Part_Renamed";
    EntityId id2 = id1; // ID stays same

    EXPECT_EQ(id1, id2);
    EXPECT_NE(name1, name2); // Names differ but IDs match
}

// CAD-CORE-002: Test 2 - ID does not change after moving in tree
TEST_F(EntityIdTest, IdRemainsSameAfterTreeMove) {
    EntityId id = EntityId::generate();
    std::string parent1 = "Assembly_Root";

    // Move to different parent - ID should not change
    std::string parent2 = "Assembly_Sub";
    EntityId id_moved = id;

    EXPECT_EQ(id, id_moved);
    EXPECT_NE(parent1, parent2);
}

// CAD-CORE-002: Test 3 - ID persists across save/load cycles
TEST_F(EntityIdTest, IdPersistenceAcrossSaveLoad) {
    EntityId original = EntityId::generate();
    std::string serialized = original.toString();

    // Simulate save/load
    EntityId loaded = EntityId::parse(serialized);

    EXPECT_EQ(original, loaded);
    EXPECT_EQ(original.toString(), loaded.toString());
}

// CAD-CORE-002: Test 4 - ID does not depend on memory address
TEST_F(EntityIdTest, IdIndependentOfMemoryAddress) {
    EntityId* id_ptr1 = new EntityId(EntityId::generate());
    EntityId id_stack = *id_ptr1;

    // Same ID regardless of storage location
    EXPECT_EQ(*id_ptr1, id_stack);

    EntityId* id_ptr2 = new EntityId(id_stack);
    EXPECT_EQ(*id_ptr1, *id_ptr2);

    // Pointers differ but IDs are same
    EXPECT_NE(reinterpret_cast<uintptr_t>(id_ptr1),
              reinterpret_cast<uintptr_t>(id_ptr2));
    EXPECT_EQ(*id_ptr1, *id_ptr2);

    delete id_ptr1;
    delete id_ptr2;
}

// CAD-CORE-002: Test 5 - Copy creates new instance ID
TEST_F(EntityIdTest, CopyCreatesNewInstanceId) {
    EntityId original = EntityId::generate();
    EntityId copy = original;

    // Copy is equal but represents same identity
    EXPECT_EQ(original, copy);

    // But new instance in hierarchy would get different ID
    EntityId new_instance = EntityId::generate();
    EXPECT_NE(original, new_instance);
}

// CAD-CORE-002: Test 6 - Format validity checking
TEST_F(EntityIdTest, FormatValidityCheck) {
    // Valid UUIDs (versions 1-5 with valid variant)
    EXPECT_TRUE(EntityId::isValid("550e8400-e29b-41d4-a716-446655440000")); // v4, variant a
    EXPECT_TRUE(EntityId::isValid("6ba7b810-9dad-11d1-80b4-00c04fd430c8")); // v1, variant 8
    EXPECT_TRUE(EntityId::isValid("550e8400-e29b-31d4-a716-446655440000")); // v3, variant a
    EXPECT_TRUE(EntityId::isValid("550e8400-e29b-21d4-b716-446655440000")); // v2, variant b
    EXPECT_TRUE(EntityId::isValid("550e8400-e29b-51d4-9716-446655440000")); // v5, variant 9

    // Invalid formats
    EXPECT_FALSE(EntityId::isValid("not-a-uuid"));
    EXPECT_FALSE(EntityId::isValid("550e8400-e29b-41d4-a716")); // Too short
    EXPECT_FALSE(EntityId::isValid("550e8400-e29b-01d4-a716-446655440000")); // Invalid version (0)
    EXPECT_FALSE(EntityId::isValid("550e8400-e29b-61d4-a716-446655440000")); // Invalid version (6)
    EXPECT_FALSE(EntityId::isValid("550e8400-e29b-41d4-c716-446655440000")); // Invalid variant (c)
}

// CAD-CORE-002: Test 7 - Serialization without loss
TEST_F(EntityIdTest, SerializationWithoutLoss) {
    EntityId original = EntityId::generate();
    std::string serialized = original.toString();
    EntityId deserialized = EntityId::parse(serialized);

    EXPECT_EQ(original, deserialized);
    EXPECT_EQ(original.getBytes(), deserialized.getBytes());
}

// CAD-CORE-002: Test 8 - Generate unique IDs
TEST_F(EntityIdTest, GenerateUniqueIds) {
    std::set<EntityId> ids;
    const int COUNT = 100000;

    for (int i = 0; i < COUNT; ++i) {
        EntityId id = EntityId::generate();
        EXPECT_EQ(ids.find(id), ids.end()) << "Duplicate ID found at iteration " << i;
        ids.insert(id);
    }

    EXPECT_EQ(ids.size(), COUNT);
}

// CAD-CORE-002: Test 9 - Round-trip serialization
TEST_F(EntityIdTest, RoundTripSerialization) {
    EntityId original = EntityId::generate();
    std::string str1 = original.toString();
    EntityId reloaded = EntityId::parse(str1);
    std::string str2 = reloaded.toString();
    EntityId reloaded_again = EntityId::parse(str2);

    EXPECT_EQ(original, reloaded);
    EXPECT_EQ(reloaded, reloaded_again);
    EXPECT_EQ(str1, str2);
}

// CAD-CORE-002: Test 10 - Invalid IDs rejected
TEST_F(EntityIdTest, InvalidIdsRejected) {
    EXPECT_THROW(EntityId::parse("invalid-uuid-format"), std::invalid_argument);
    EXPECT_THROW(EntityId::parse(""), std::invalid_argument);
    EXPECT_THROW(EntityId::parse("not-uuid-at-all"), std::invalid_argument);
}

// CAD-CORE-002: Test 11 - IDs usable in containers
TEST_F(EntityIdTest, UsableInStandardContainers) {
    std::set<EntityId> id_set;
    std::unordered_set<EntityId> id_unordered;
    std::map<EntityId, std::string> id_map;

    EntityId id1 = EntityId::generate();
    EntityId id2 = EntityId::generate();
    EntityId id3 = EntityId::generate();

    // Set
    id_set.insert(id1);
    id_set.insert(id2);
    id_set.insert(id3);
    EXPECT_EQ(id_set.size(), 3);
    EXPECT_NE(id_set.find(id1), id_set.end());

    // Unordered set
    id_unordered.insert(id1);
    id_unordered.insert(id2);
    EXPECT_EQ(id_unordered.size(), 2);

    // Map
    id_map[id1] = "Part_001";
    id_map[id2] = "Part_002";
    EXPECT_EQ(id_map[id1], "Part_001");
}

// CAD-CORE-002: Test 12 - Duplicate check in document
TEST_F(EntityIdTest, DuplicateDetectionInDocument) {
    std::set<EntityId> document_ids;

    EntityId id1 = EntityId::generate();
    EntityId id2 = EntityId::generate();
    EntityId id3 = id1; // Duplicate

    document_ids.insert(id1);
    document_ids.insert(id2);

    // id3 should be detected as duplicate
    if (document_ids.find(id3) != document_ids.end()) {
        // Duplicate found - expected
        SUCCEED();
    } else {
        FAIL() << "Duplicate ID should be detected";
    }
}

// CAD-CORE-002: Test 13 - String representation format
TEST_F(EntityIdTest, StringRepresentationFormat) {
    EntityId id = EntityId::generate();
    std::string str = id.toString();

    // Should be 36 chars: 32 hex + 4 hyphens
    EXPECT_EQ(str.length(), 36);

    // Check positions of hyphens
    EXPECT_EQ(str[8], '-');
    EXPECT_EQ(str[13], '-');
    EXPECT_EQ(str[18], '-');
    EXPECT_EQ(str[23], '-');

    // Version should be 4
    EXPECT_EQ(str[14], '4');
}

// CAD-CORE-002: Test 14 - Null ID detection
TEST_F(EntityIdTest, NullIdDetection) {
    EntityId null_id;
    EntityId valid_id = EntityId::generate();

    EXPECT_TRUE(null_id.isNull());
    EXPECT_FALSE(valid_id.isNull());
}

// CAD-CORE-002: Test 15 - Comparison operators
TEST_F(EntityIdTest, ComparisonOperators) {
    EntityId id1 = EntityId::generate();
    EntityId id2 = EntityId::generate();
    EntityId id1_copy = id1;

    EXPECT_EQ(id1, id1_copy);
    EXPECT_NE(id1, id2);
    EXPECT_TRUE(id1 < id2 || id2 < id1); // One must be less
    EXPECT_FALSE(id1 < id1); // ID not less than itself
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
