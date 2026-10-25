#include <gtest/gtest.h>
#include "bendcore/document/entity_tree.hpp"

using namespace BendCore::CadCore;

class EntityTreeTest : public ::testing::Test {
protected:
    CadDocument doc;
    EntityTree tree{doc};

    void SetUp() override {
        doc.clear();
    }
};

// CAD-CORE-004: Test 1 - Create root objects
TEST_F(EntityTreeTest, CreateRootObjects) {
    EntityId root1 = tree.createRoot(EntityType::PART, "Part_001");
    EntityId root2 = tree.createRoot(EntityType::ASSEMBLY, "Assembly_001");

    EXPECT_NE(root1, root2);
    EXPECT_TRUE(doc.hasEntity(root1));
    EXPECT_TRUE(doc.hasEntity(root2));

    // Verify they're roots
    EXPECT_TRUE(tree.getParent(root1).isNull());
    EXPECT_TRUE(tree.getParent(root2).isNull());
}

// CAD-CORE-004: Test 2 - Add children to parent
TEST_F(EntityTreeTest, AddChildren) {
    EntityId part = tree.createRoot(EntityType::PART, "Part_001");
    EntityId body1 = tree.addChild(part, EntityType::BODY, "Body_001");
    EntityId body2 = tree.addChild(part, EntityType::BODY, "Body_002");

    EXPECT_EQ(tree.getParent(body1), part);
    EXPECT_EQ(tree.getParent(body2), part);

    auto children = tree.getChildren(part);
    EXPECT_EQ(children.size(), 2);
}

// CAD-CORE-004: Test 3 - No cycles allowed
TEST_F(EntityTreeTest, NoCyclesAllowed) {
    EntityId part = tree.createRoot(EntityType::PART, "Part_001");
    EntityId body = tree.addChild(part, EntityType::BODY, "Body_001");

    // Try to set body as parent of part - should fail
    EXPECT_THROW(tree.setParent(part, body), std::invalid_argument);
}

// CAD-CORE-004: Test 4 - Can't assign object as own parent
TEST_F(EntityTreeTest, CantAssignSelfAsParent) {
    EntityId part = tree.createRoot(EntityType::PART, "Part_001");

    EXPECT_THROW(tree.setParent(part, part), std::invalid_argument);
}

// CAD-CORE-004: Test 5 - Can't put parent inside own child
TEST_F(EntityTreeTest, CantPutParentInChild) {
    EntityId part = tree.createRoot(EntityType::PART, "Part_001");
    EntityId body = tree.addChild(part, EntityType::BODY, "Body_001");
    EntityId sketch = tree.addChild(body, EntityType::SKETCH, "Sketch_001");

    // Try to move part under sketch (would make sketch's parent its descendant)
    EXPECT_THROW(tree.setParent(part, sketch), std::invalid_argument);
}

// CAD-CORE-004: Test 6 - Moving doesn't change ID
TEST_F(EntityTreeTest, MovingDoesntChangeId) {
    EntityId part1 = tree.createRoot(EntityType::PART, "Part_001");
    EntityId part2 = tree.createRoot(EntityType::PART, "Part_002");
    EntityId body = tree.addChild(part1, EntityType::BODY, "Body_001");

    EntityId original_id = body;

    // Move body from part1 to part2
    tree.move(body, part2);

    EXPECT_EQ(original_id, body);
    EXPECT_EQ(tree.getParent(body), part2);
}

// CAD-CORE-004: Test 7 - Get entity path
TEST_F(EntityTreeTest, GetEntityPath) {
    EntityId part = tree.createRoot(EntityType::PART, "Part_001");
    EntityId body = tree.addChild(part, EntityType::BODY, "Body_001");
    EntityId sketch = tree.addChild(body, EntityType::SKETCH, "Sketch_001");

    auto path = tree.getPath(sketch);
    EXPECT_EQ(path.size(), 3);
    EXPECT_EQ(path[0], part);
    EXPECT_EQ(path[1], body);
    EXPECT_EQ(path[2], sketch);
}

// CAD-CORE-004: Test 8 - Get path string
TEST_F(EntityTreeTest, GetPathString) {
    EntityId part = tree.createRoot(EntityType::PART, "Part_001");
    EntityId body = tree.addChild(part, EntityType::BODY, "Body_001");

    std::string path_str = tree.getPathString(body);
    EXPECT_TRUE(path_str.find("Part_001") != std::string::npos);
    EXPECT_TRUE(path_str.find("Body_001") != std::string::npos);
}

// CAD-CORE-004: Test 9 - Delete subtree
TEST_F(EntityTreeTest, DeleteSubtree) {
    EntityId part = tree.createRoot(EntityType::PART, "Part_001");
    EntityId body = tree.addChild(part, EntityType::BODY, "Body_001");
    EntityId sketch = tree.addChild(body, EntityType::SKETCH, "Sketch_001");

    EXPECT_EQ(tree.getChildren(part).size(), 1);
    EXPECT_EQ(tree.getChildren(body).size(), 1);

    // Delete body subtree
    tree.deleteSubtree(body);

    EXPECT_EQ(tree.getChildren(part).size(), 0);
    EXPECT_FALSE(doc.hasEntity(body));
    EXPECT_FALSE(doc.hasEntity(sketch));
    EXPECT_TRUE(doc.hasEntity(part));
}

// CAD-CORE-004: Test 10 - Validate tree (no cycles)
TEST_F(EntityTreeTest, ValidateTree) {
    EntityId part = tree.createRoot(EntityType::PART, "Part_001");
    (void)tree.addChild(part, EntityType::BODY, "Body_001");  // body

    std::string error = tree.validate();
    EXPECT_TRUE(error.empty()) << "Error: " << error;
}

// CAD-CORE-004: Test 11 - Get depth
TEST_F(EntityTreeTest, GetDepth) {
    EntityId part = tree.createRoot(EntityType::PART, "Part_001");
    EntityId body = tree.addChild(part, EntityType::BODY, "Body_001");
    EntityId sketch = tree.addChild(body, EntityType::SKETCH, "Sketch_001");

    EXPECT_EQ(tree.getDepth(part), 0);      // Root
    EXPECT_EQ(tree.getDepth(body), 1);      // Level 1
    EXPECT_EQ(tree.getDepth(sketch), 2);    // Level 2
}

// CAD-CORE-004: Test 12 - Get descendants
TEST_F(EntityTreeTest, GetDescendants) {
    EntityId part = tree.createRoot(EntityType::PART, "Part_001");
    EntityId body1 = tree.addChild(part, EntityType::BODY, "Body_001");
    EntityId body2 = tree.addChild(part, EntityType::BODY, "Body_002");
    EntityId sketch = tree.addChild(body1, EntityType::SKETCH, "Sketch_001");

    auto descendants = tree.getDescendants(part);
    EXPECT_EQ(descendants.size(), 3);
    EXPECT_TRUE(std::find(descendants.begin(), descendants.end(), body1) != descendants.end());
    EXPECT_TRUE(std::find(descendants.begin(), descendants.end(), body2) != descendants.end());
    EXPECT_TRUE(std::find(descendants.begin(), descendants.end(), sketch) != descendants.end());
}

// CAD-CORE-004: Test 13 - Check ancestor/descendant relationship
TEST_F(EntityTreeTest, AncestorDescendantRelationships) {
    EntityId part = tree.createRoot(EntityType::PART, "Part_001");
    EntityId body = tree.addChild(part, EntityType::BODY, "Body_001");
    EntityId sketch = tree.addChild(body, EntityType::SKETCH, "Sketch_001");

    EXPECT_TRUE(tree.isAncestor(part, sketch));
    EXPECT_TRUE(tree.isDescendant(sketch, part));
    EXPECT_FALSE(tree.isAncestor(sketch, part));
    EXPECT_FALSE(tree.isDescendant(part, sketch));
}

// CAD-CORE-004: Test 14 - Get all roots
TEST_F(EntityTreeTest, GetAllRoots) {
    EntityId part1 = tree.createRoot(EntityType::PART, "Part_001");
    EntityId part2 = tree.createRoot(EntityType::PART, "Part_002");
    EntityId assembly = tree.createRoot(EntityType::ASSEMBLY, "Assembly_001");

    // Add children (shouldn't be in roots)
    EntityId body = tree.addChild(part1, EntityType::BODY, "Body_001");

    auto roots = tree.getRoots();
    EXPECT_EQ(roots.size(), 3);
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), part1) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), part2) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), assembly) != roots.end());
    EXPECT_TRUE(std::find(roots.begin(), roots.end(), body) == roots.end());
}

// CAD-CORE-004: Test 15 - Complex hierarchy
TEST_F(EntityTreeTest, ComplexHierarchy) {
    EntityId part = tree.createRoot(EntityType::PART, "Part_001");
    EntityId body1 = tree.addChild(part, EntityType::BODY, "Body_001");
    EntityId body2 = tree.addChild(part, EntityType::BODY, "Body_002");
    (void)tree.addChild(body1, EntityType::SKETCH, "Sketch_001");  // sketch1
    (void)tree.addChild(body1, EntityType::SKETCH, "Sketch_002");  // sketch2
    (void)tree.addChild(body2, EntityType::SKETCH, "Sketch_003");  // sketch3

    EXPECT_EQ(tree.getChildren(part).size(), 2);
    EXPECT_EQ(tree.getChildren(body1).size(), 2);
    EXPECT_EQ(tree.getChildren(body2).size(), 1);

    auto descendants = tree.getDescendants(part);
    EXPECT_EQ(descendants.size(), 5);
}

// CAD-CORE-004: Test 16 - Detect cycles
TEST_F(EntityTreeTest, DetectCycles) {
    (void)tree.createRoot(EntityType::PART, "Part_001");  // part

    // Valid tree - no cycles
    EXPECT_FALSE(tree.hasCycles());
}

// CAD-CORE-004: Test 17 - Move between branches
TEST_F(EntityTreeTest, MoveBetweenBranches) {
    EntityId part = tree.createRoot(EntityType::PART, "Part_001");
    EntityId body1 = tree.addChild(part, EntityType::BODY, "Body_001");
    EntityId body2 = tree.addChild(part, EntityType::BODY, "Body_002");
    EntityId sketch = tree.addChild(body1, EntityType::SKETCH, "Sketch_001");

    EXPECT_EQ(tree.getParent(sketch), body1);
    EXPECT_EQ(tree.getChildren(body1).size(), 1);
    EXPECT_EQ(tree.getChildren(body2).size(), 0);

    // Move sketch to body2
    tree.move(sketch, body2);

    EXPECT_EQ(tree.getParent(sketch), body2);
    EXPECT_EQ(tree.getChildren(body1).size(), 0);
    EXPECT_EQ(tree.getChildren(body2).size(), 1);
}

// CAD-CORE-004: Test 18 - Hierarchy recovery after operations
TEST_F(EntityTreeTest, HierarchyAfterOperations) {
    EntityId part = tree.createRoot(EntityType::PART, "Part_001");
    EntityId body = tree.addChild(part, EntityType::BODY, "Body_001");
    EntityId sketch = tree.addChild(body, EntityType::SKETCH, "Sketch_001");

    // Get path, then validate
    auto path = tree.getPath(sketch);
    EXPECT_EQ(path.size(), 3);

    // Validate after operations
    std::string error = tree.validate();
    EXPECT_TRUE(error.empty());
}

// CAD-CORE-004: Test 19 - Large hierarchy
TEST_F(EntityTreeTest, LargeHierarchy) {
    EntityId root = tree.createRoot(EntityType::PART, "Root");
    const int BRANCHES = 10;
    const int DEPTH = 5;

    for (int b = 0; b < BRANCHES; ++b) {
        EntityId branch = tree.addChild(root, EntityType::BODY, "Branch_" + std::to_string(b));
        EntityId current = branch;

        for (int d = 0; d < DEPTH - 1; ++d) {
            current = tree.addChild(current, EntityType::SKETCH,
                                   "Level_" + std::to_string(d));
        }
    }

    auto descendants = tree.getDescendants(root);
    EXPECT_EQ(descendants.size(), BRANCHES * DEPTH);
}

// CAD-CORE-004: Test 20 - Path consistency
TEST_F(EntityTreeTest, PathConsistency) {
    EntityId part = tree.createRoot(EntityType::PART, "Part_001");
    EntityId body = tree.addChild(part, EntityType::BODY, "Body_001");
    EntityId sketch = tree.addChild(body, EntityType::SKETCH, "Sketch_001");

    auto path = tree.getPath(sketch);

    // Walking up from sketch should match path
    EntityId current = sketch;
    std::vector<EntityId> reverse_walk;
    while (!current.isNull()) {
        reverse_walk.push_back(current);
        current = tree.getParent(current);
    }

    std::reverse(reverse_walk.begin(), reverse_walk.end());
    EXPECT_EQ(path, reverse_walk);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
