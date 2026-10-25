#include <gtest/gtest.h>
#include "bendcore/geometry/direct_editing.hpp"
#include "bendcore/document/entity_id.hpp"

using namespace BendCore::CadCore;

class DirectEditingTest : public ::testing::Test {
protected:
    EntityId shape_id = EntityId::generate();
    EntityId face_id = EntityId::generate();
    EntityId edge_id = EntityId::generate();
    DirectEditor editor{shape_id};
};

class FaceOffsetTest : public ::testing::Test {
protected:
    EntityId face_id = EntityId::generate();
    FaceOffset offset{face_id};
};

class ParametricDimensionTest : public ::testing::Test {
protected:
    EntityId dim_id = EntityId::generate();
};

class ParametricSketchTest : public ::testing::Test {
protected:
    EntityId sketch_id = EntityId::generate();
    ParametricSketch sketch{sketch_id};
};

// ===== FaceOffset Tests =====

TEST_F(FaceOffsetTest, CreateFaceOffset) {
    EXPECT_EQ(offset.getFaceId(), face_id);
    EXPECT_DOUBLE_EQ(offset.getOffsetDistance(), 0.0);
    EXPECT_TRUE(offset.isOutward());
}

TEST_F(FaceOffsetTest, SetOffsetDistance) {
    offset.setOffsetDistance(2.5);
    EXPECT_DOUBLE_EQ(offset.getOffsetDistance(), 2.5);
}

TEST_F(FaceOffsetTest, SetOffsetDirection) {
    offset.setOutward(false);
    EXPECT_FALSE(offset.isOutward());

    offset.setOutward(true);
    EXPECT_TRUE(offset.isOutward());
}

TEST_F(FaceOffsetTest, ApplyOffset) {
    offset.setOffsetDistance(1.5);
    bool result = offset.apply();
    EXPECT_TRUE(result);
}

TEST_F(FaceOffsetTest, UndoOffset) {
    offset.setOffsetDistance(1.5);
    [[maybe_unused]] bool result1 = offset.apply();
    bool result = offset.undo();
    EXPECT_TRUE(result);
}

TEST_F(FaceOffsetTest, ZeroOffsetFails) {
    offset.setOffsetDistance(0.0);
    bool result = offset.apply();
    EXPECT_FALSE(result);
}

// ===== EdgeEditor Tests =====

TEST_F(DirectEditingTest, CreateEdgeEditor) {
    auto editor_ptr = editor.editEdge(edge_id);
    EXPECT_EQ(editor_ptr->getEdgeId(), edge_id);
}

TEST_F(DirectEditingTest, ChamferEdge) {
    auto edge_editor = editor.editEdge(edge_id);
    bool result = edge_editor->chamfer(0.5);
    EXPECT_TRUE(result);
}

TEST_F(DirectEditingTest, FilletEdge) {
    auto edge_editor = editor.editEdge(edge_id);
    bool result = edge_editor->fillet(1.0);
    EXPECT_TRUE(result);
}

TEST_F(DirectEditingTest, FilletNegativeRadiusFails) {
    auto edge_editor = editor.editEdge(edge_id);
    bool result = edge_editor->fillet(-1.0);
    EXPECT_FALSE(result);
}

TEST_F(DirectEditingTest, DraftEdge) {
    auto edge_editor = editor.editEdge(edge_id);
    bool result = edge_editor->draft(M_PI / 6.0);  // 30 degrees
    EXPECT_TRUE(result);
}

TEST_F(DirectEditingTest, MoveEdge) {
    auto edge_editor = editor.editEdge(edge_id);
    Vector3D displacement(1.0, 2.0, 3.0);
    bool result = edge_editor->moveEdge(displacement);
    EXPECT_TRUE(result);
}

TEST_F(DirectEditingTest, MoveEdgeZeroDisplacementFails) {
    auto edge_editor = editor.editEdge(edge_id);
    Vector3D zero_displacement(0.0, 0.0, 0.0);
    bool result = edge_editor->moveEdge(zero_displacement);
    EXPECT_FALSE(result);
}

// ===== ShellOp Tests =====

TEST_F(DirectEditingTest, CreateShell) {
    auto shell = editor.createShell(1.5);
    EXPECT_DOUBLE_EQ(shell->getThickness(), 1.5);
}

TEST_F(DirectEditingTest, ShellWithRemovedFaces) {
    auto shell = editor.createShell(2.0);

    EntityId removed_face1 = EntityId::generate();
    EntityId removed_face2 = EntityId::generate();

    shell->addRemovedFace(removed_face1);
    shell->addRemovedFace(removed_face2);

    EXPECT_EQ(shell->getRemovedFaces().size(), 2);
}

TEST_F(DirectEditingTest, ApplyShell) {
    auto shell = editor.createShell(1.0);
    shell->addRemovedFace(EntityId::generate());

    bool result = shell->apply();
    EXPECT_TRUE(result);
}

TEST_F(DirectEditingTest, ShellNoRemovedFacesFails) {
    auto shell = editor.createShell(1.0);
    // No removed faces added

    bool result = shell->apply();
    EXPECT_FALSE(result);
}

// ===== ParametricDimension Tests =====

TEST_F(ParametricDimensionTest, CreateDimension) {
    ParametricDimension dim(dim_id, ParametricDimension::DimensionType::LINEAR);

    EXPECT_EQ(dim.getId(), dim_id);
    EXPECT_EQ(dim.getType(), ParametricDimension::DimensionType::LINEAR);
}

TEST_F(ParametricDimensionTest, SetDimensionValue) {
    ParametricDimension dim(dim_id, ParametricDimension::DimensionType::LINEAR);
    dim.setValue(10.5);

    EXPECT_DOUBLE_EQ(dim.getValue(), 10.5);
}

TEST_F(ParametricDimensionTest, DimensionDriving) {
    ParametricDimension dim(dim_id, ParametricDimension::DimensionType::LINEAR);

    EXPECT_TRUE(dim.isDriving());

    dim.setDriving(false);
    EXPECT_FALSE(dim.isDriving());
}

TEST_F(ParametricDimensionTest, DimensionName) {
    ParametricDimension dim(dim_id, ParametricDimension::DimensionType::LINEAR);

    dim.setName("Width");
    EXPECT_EQ(dim.getName(), "Width");
}

TEST_F(ParametricDimensionTest, AngularDimension) {
    ParametricDimension dim(dim_id, ParametricDimension::DimensionType::ANGULAR);
    dim.setValue(45.0);

    EXPECT_DOUBLE_EQ(dim.getValue(), 45.0);
}

TEST_F(ParametricDimensionTest, DiameterDimension) {
    ParametricDimension dim(dim_id, ParametricDimension::DimensionType::DIAMETER);
    dim.setValue(25.0);

    EXPECT_DOUBLE_EQ(dim.getValue(), 25.0);
}

// ===== GeometricConstraint Tests =====

TEST(GeometricConstraintTest, CreateConstraint) {
    EntityId constraint_id = EntityId::generate();
    GeometricConstraint constraint(constraint_id, GeometricConstraint::ConstraintType::COINCIDENT);

    EXPECT_EQ(constraint.getId(), constraint_id);
    EXPECT_EQ(constraint.getType(), GeometricConstraint::ConstraintType::COINCIDENT);
}

TEST(GeometricConstraintTest, AddEntities) {
    EntityId constraint_id = EntityId::generate();
    GeometricConstraint constraint(constraint_id, GeometricConstraint::ConstraintType::PARALLEL);

    EntityId entity1 = EntityId::generate();
    EntityId entity2 = EntityId::generate();

    constraint.addEntity(entity1);
    constraint.addEntity(entity2);

    EXPECT_EQ(constraint.getEntities().size(), 2);
}

TEST(GeometricConstraintTest, ConstraintSatisfaction) {
    EntityId constraint_id = EntityId::generate();
    GeometricConstraint constraint(constraint_id, GeometricConstraint::ConstraintType::EQUAL);

    EXPECT_FALSE(constraint.isSatisfied());

    constraint.setSatisfied(true);
    EXPECT_TRUE(constraint.isSatisfied());
}

// ===== ParametricSketch Tests =====

TEST_F(ParametricSketchTest, CreateSketch) {
    EXPECT_EQ(sketch.getId(), sketch_id);
    EXPECT_EQ(sketch.getDimensions().size(), 0);
    EXPECT_EQ(sketch.getConstraints().size(), 0);
}

TEST_F(ParametricSketchTest, AddDimensions) {
    auto dim1 = std::make_shared<ParametricDimension>(
        EntityId::generate(), ParametricDimension::DimensionType::LINEAR);
    auto dim2 = std::make_shared<ParametricDimension>(
        EntityId::generate(), ParametricDimension::DimensionType::ANGULAR);

    sketch.addDimension(dim1);
    sketch.addDimension(dim2);

    EXPECT_EQ(sketch.getDimensions().size(), 2);
}

TEST_F(ParametricSketchTest, AddConstraints) {
    auto constraint1 = std::make_shared<GeometricConstraint>(
        EntityId::generate(), GeometricConstraint::ConstraintType::COINCIDENT);
    auto constraint2 = std::make_shared<GeometricConstraint>(
        EntityId::generate(), GeometricConstraint::ConstraintType::PARALLEL);

    sketch.addConstraint(constraint1);
    sketch.addConstraint(constraint2);

    EXPECT_EQ(sketch.getConstraints().size(), 2);
}

TEST_F(ParametricSketchTest, SolveEmptySketch) {
    bool result = sketch.solve();
    EXPECT_TRUE(result);
}

TEST_F(ParametricSketchTest, SolveSketchWithDimensions) {
    auto dim1 = std::make_shared<ParametricDimension>(
        EntityId::generate(), ParametricDimension::DimensionType::LINEAR);
    auto dim2 = std::make_shared<ParametricDimension>(
        EntityId::generate(), ParametricDimension::DimensionType::LINEAR);

    sketch.addDimension(dim1);
    sketch.addDimension(dim2);

    bool result = sketch.solve();
    EXPECT_TRUE(result);
}

TEST_F(ParametricSketchTest, UnderconstainedSketch) {
    // Sketch with only one dimension is underconstrained
    auto dim = std::make_shared<ParametricDimension>(
        EntityId::generate(), ParametricDimension::DimensionType::LINEAR);
    sketch.addDimension(dim);

    bool result = sketch.solve();
    EXPECT_FALSE(result);
}

TEST_F(ParametricSketchTest, GetSketchStatus) {
    auto dim = std::make_shared<ParametricDimension>(
        EntityId::generate(), ParametricDimension::DimensionType::LINEAR);
    sketch.addDimension(dim);

    [[maybe_unused]] bool result = sketch.solve();
    std::string status = sketch.getStatus();

    EXPECT_FALSE(status.empty());
    EXPECT_TRUE(status.find("Sketch") != std::string::npos);
}

// ===== DirectEditor Tests =====

TEST_F(DirectEditingTest, OffsetFace) {
    auto offset = editor.offsetFace(face_id, 2.0);

    EXPECT_EQ(offset->getFaceId(), face_id);
    EXPECT_DOUBLE_EQ(offset->getOffsetDistance(), 2.0);
}

TEST_F(DirectEditingTest, TrackPendingOperations) {
    auto offset = editor.offsetFace(face_id, 1.5);
    auto edge_editor = editor.editEdge(edge_id);
    auto shell = editor.createShell(1.0);

    EXPECT_NE(offset, nullptr);
    EXPECT_NE(edge_editor, nullptr);
    EXPECT_NE(shell, nullptr);
    EXPECT_EQ(editor.getPendingOperationCount(), 3);
}

TEST_F(DirectEditingTest, ApplyAllOperations) {
    auto offset = editor.offsetFace(face_id, 1.5);
    EXPECT_NE(offset, nullptr);
    bool result = editor.applyAllOperations();

    EXPECT_TRUE(result);
    EXPECT_EQ(editor.getPendingOperationCount(), 0);
}

TEST_F(DirectEditingTest, RollbackOperations) {
    auto offset = editor.offsetFace(face_id, 1.5);
    auto edge_editor = editor.editEdge(edge_id);

    EXPECT_NE(offset, nullptr);
    EXPECT_NE(edge_editor, nullptr);
    EXPECT_EQ(editor.getPendingOperationCount(), 2);

    editor.rollbackAllOperations();
    EXPECT_EQ(editor.getPendingOperationCount(), 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
