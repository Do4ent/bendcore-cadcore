#include <gtest/gtest.h>
#include "bendcore/geometry/occt_wrapper.hpp"
#include "bendcore/document/entity_id.hpp"

using namespace BendCore::CadCore;

class OccWrapperTest : public ::testing::Test {
protected:
    OccWrapper::StepExporter step_exporter;
    OccWrapper::IgesExporter iges_exporter;
    OccWrapper::StepImporter step_importer;
    OccWrapper::ShapeHealer shape_healer;
    OccWrapper::SurfaceOps surface_ops;
    OccWrapper::BooleanOps boolean_ops;
};

// Test: Check OCCT availability
TEST_F(OccWrapperTest, OccAvailability) {
    bool available = OccWrapper::isAvailable();
    // Should return false when OCCT is not linked
    EXPECT_FALSE(available);
}

// Test: Get OCCT version string
TEST_F(OccWrapperTest, GetVersion) {
    std::string version = OccWrapper::getVersion();
    EXPECT_FALSE(version.empty());
}

// Test: STEP export shape (graceful fallback)
TEST_F(OccWrapperTest, StepExportShape) {
    EntityId shape_id = EntityId::generate();
    bool result = step_exporter.exportShape("/tmp/test_shape.step", shape_id);
    // Should return false without OCCT
    EXPECT_FALSE(result);
}

// Test: STEP export assembly (graceful fallback)
TEST_F(OccWrapperTest, StepExportAssembly) {
    std::vector<EntityId> components;
    components.push_back(EntityId::generate());
    components.push_back(EntityId::generate());

    bool result = step_exporter.exportAssembly("/tmp/test_assembly.step", components);
    EXPECT_FALSE(result);
}

// Test: IGES export shape (graceful fallback)
TEST_F(OccWrapperTest, IgesExportShape) {
    EntityId shape_id = EntityId::generate();
    bool result = iges_exporter.exportShape("/tmp/test_shape.iges", shape_id);
    EXPECT_FALSE(result);
}

// Test: STEP import shape (graceful fallback)
TEST_F(OccWrapperTest, StepImportShape) {
    std::vector<EntityId> result = step_importer.importShape("/tmp/nonexistent.step");
    // Should return empty vector without OCCT
    EXPECT_TRUE(result.empty());
    EXPECT_TRUE(step_importer.hasErrors());
}

// Test: Shape healing (graceful fallback)
TEST_F(OccWrapperTest, HealShape) {
    EntityId shape_id = EntityId::generate();
    bool result = shape_healer.healShape(shape_id);
    EXPECT_FALSE(result);
}

// Test: Remove faces (graceful fallback)
TEST_F(OccWrapperTest, RemoveFaces) {
    EntityId shape_id = EntityId::generate();
    std::vector<size_t> face_indices = {0, 1, 2};
    bool result = shape_healer.removeFaces(shape_id, face_indices);
    EXPECT_FALSE(result);
}

// Test: Offset face (graceful fallback)
TEST_F(OccWrapperTest, OffsetFace) {
    EntityId shape_id = EntityId::generate();
    bool result = shape_healer.offsetFace(shape_id, 0, 1.5);
    EXPECT_FALSE(result);
}

// Test: Loft profiles (graceful fallback)
TEST_F(OccWrapperTest, LoftProfiles) {
    std::vector<EntityId> profiles;
    profiles.push_back(EntityId::generate());
    profiles.push_back(EntityId::generate());

    EntityId result = surface_ops.loftProfiles(profiles);
    EXPECT_EQ(result, EntityId());  // Should return empty EntityId
}

// Test: Sweep profile (graceful fallback)
TEST_F(OccWrapperTest, SweepProfile) {
    EntityId profile = EntityId::generate();
    EntityId path = EntityId::generate();

    EntityId result = surface_ops.sweepProfile(profile, path);
    EXPECT_EQ(result, EntityId());  // Should return empty EntityId
}

// Test: Boolean union (graceful fallback)
TEST_F(OccWrapperTest, BooleanUnion) {
    EntityId shape1 = EntityId::generate();
    EntityId shape2 = EntityId::generate();

    EntityId result = boolean_ops.unite(shape1, shape2);
    EXPECT_EQ(result, EntityId());  // Should return empty EntityId
}

// Test: Boolean cut (graceful fallback)
TEST_F(OccWrapperTest, BooleanCut) {
    EntityId shape1 = EntityId::generate();
    EntityId shape2 = EntityId::generate();

    EntityId result = boolean_ops.cut(shape1, shape2);
    EXPECT_EQ(result, EntityId());  // Should return empty EntityId
}

// Test: Boolean intersection (graceful fallback)
TEST_F(OccWrapperTest, BooleanIntersect) {
    EntityId shape1 = EntityId::generate();
    EntityId shape2 = EntityId::generate();

    EntityId result = boolean_ops.intersect(shape1, shape2);
    EXPECT_EQ(result, EntityId());  // Should return empty EntityId
}

// Test: Multiple shape operations
TEST_F(OccWrapperTest, MultipleOperations) {
    EntityId shape1 = EntityId::generate();
    EntityId shape2 = EntityId::generate();
    EntityId shape3 = EntityId::generate();

    // These should all gracefully fail without OCCT
    EntityId union_result = boolean_ops.unite(shape1, shape2);
    EntityId cut_result = boolean_ops.cut(union_result, shape3);
    EntityId intersect_result = boolean_ops.intersect(union_result, cut_result);

    EXPECT_EQ(union_result, EntityId());
    EXPECT_EQ(cut_result, EntityId());
    EXPECT_EQ(intersect_result, EntityId());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
