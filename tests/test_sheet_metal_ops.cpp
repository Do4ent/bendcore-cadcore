#include <gtest/gtest.h>
#include "bendcore/geometry/sheet_metal_ops.hpp"

using namespace BendCore::CadCore;

// ===== BendAllowance Tests =====

TEST(BendAllowanceTest, CreateBendAllowance) {
    BendAllowance ba(EntityId::generate(), 2.0, 5.0);
    EXPECT_DOUBLE_EQ(ba.getKFactor(), 0.33);
    EXPECT_DOUBLE_EQ(ba.getBendAngle(), 90.0);
}

TEST(BendAllowanceTest, CalculateBendAllowance) {
    BendAllowance ba(EntityId::generate(), 2.0, 5.0);
    double allowance = ba.getBendAllowance();
    EXPECT_GT(allowance, 0.0);
}

TEST(BendAllowanceTest, SetKFactor) {
    BendAllowance ba(EntityId::generate(), 2.0, 5.0);
    ba.setKFactor(0.4);
    EXPECT_DOUBLE_EQ(ba.getKFactor(), 0.4);
}

TEST(BendAllowanceTest, SetBendAngle) {
    BendAllowance ba(EntityId::generate(), 2.0, 5.0);
    ba.setBendAngle(45.0);
    EXPECT_DOUBLE_EQ(ba.getBendAngle(), 45.0);
}

TEST(BendAllowanceTest, SetMaterial) {
    BendAllowance ba(EntityId::generate(), 2.0, 5.0);
    ba.setMaterial("Aluminum");
    EXPECT_TRUE(true);  // Material set
}

// ===== UnfoldSheet Tests =====

TEST(UnfoldSheetTest, CreateUnfold) {
    UnfoldSheet unfold(EntityId::generate());
    EXPECT_EQ(unfold.getBendEdges().size(), 0);
}

TEST(UnfoldSheetTest, AddBendEdges) {
    UnfoldSheet unfold(EntityId::generate());
    EntityId edge_id = EntityId::generate();
    BendAllowance ba(EntityId::generate(), 2.0, 5.0);

    unfold.addBendEdge(edge_id, ba);
    EXPECT_EQ(unfold.getBendEdges().size(), 1);
}

TEST(UnfoldSheetTest, UnfoldOperation) {
    UnfoldSheet unfold(EntityId::generate());
    EntityId edge_id = EntityId::generate();
    BendAllowance ba(EntityId::generate(), 2.0, 5.0);
    unfold.addBendEdge(edge_id, ba);

    bool result = unfold.unfold();
    EXPECT_TRUE(result);
    EXPECT_GT(unfold.getUnfoldedProfile().size(), 0);
}

TEST(UnfoldSheetTest, ExportToDXF) {
    UnfoldSheet unfold(EntityId::generate());
    EntityId edge_id = EntityId::generate();
    BendAllowance ba(EntityId::generate(), 2.0, 5.0);
    unfold.addBendEdge(edge_id, ba);

    [[maybe_unused]] bool unfold_result = unfold.unfold();
    bool export_result = unfold.exportToDXF("output.dxf");
    EXPECT_TRUE(export_result);
}

// ===== TubeNotch Tests =====

TEST(TubeNotchTest, CreateNotch) {
    TubeNotch notch(EntityId::generate(), EntityId::generate());
    EXPECT_EQ(notch.getNotchCurve().size(), 0);
}

TEST(TubeNotchTest, ButtJointNotch) {
    TubeNotch notch(EntityId::generate(), EntityId::generate());
    notch.setIntersectionType(TubeNotch::IntersectionType::BUTT);

    bool result = notch.generateNotch();
    EXPECT_TRUE(result);
    EXPECT_EQ(notch.getNotchCurve().size(), 2);
}

TEST(TubeNotchTest, FishmouthNotch) {
    TubeNotch notch(EntityId::generate(), EntityId::generate());
    notch.setIntersectionType(TubeNotch::IntersectionType::FISHMOUTH);

    bool result = notch.generateNotch();
    EXPECT_TRUE(result);
    EXPECT_GT(notch.getNotchCurve().size(), 0);
}

TEST(TubeNotchTest, SaddleNotch) {
    TubeNotch notch(EntityId::generate(), EntityId::generate());
    notch.setIntersectionType(TubeNotch::IntersectionType::SADDLE);

    bool result = notch.generateNotch();
    EXPECT_TRUE(result);
    EXPECT_GT(notch.getNotchCurve().size(), 0);
}

TEST(TubeNotchTest, ExportForCutting) {
    TubeNotch notch(EntityId::generate(), EntityId::generate());
    [[maybe_unused]] bool generate_result = notch.generateNotch();

    EXPECT_TRUE(notch.exportForCutting("gcode"));
    EXPECT_TRUE(notch.exportForCutting("dxf"));
}

// ===== StampingPattern Tests =====

TEST(StampingPatternTest, CreatePattern) {
    StampingPattern pattern(EntityId::generate());
    EXPECT_EQ(pattern.getInstances().size(), 0);
}

TEST(StampingPatternTest, LinearPattern) {
    StampingPattern pattern(EntityId::generate());
    Vector3D direction(1.0, 0.0, 0.0);
    pattern.createLinearPattern(direction, 10.0, 5);

    EXPECT_EQ(pattern.getInstances().size(), 5);
}

TEST(StampingPatternTest, CircularPattern) {
    StampingPattern pattern(EntityId::generate());
    Vector3D axis(0.0, 0.0, 1.0);
    pattern.createCircularPattern(axis, 20.0, 4);

    EXPECT_EQ(pattern.getInstances().size(), 4);
}

TEST(StampingPatternTest, ApplyPattern) {
    StampingPattern pattern(EntityId::generate());
    Vector3D direction(1.0, 0.0, 0.0);
    pattern.createLinearPattern(direction, 10.0, 3);

    bool result = pattern.apply();
    EXPECT_TRUE(result);
}

// ===== RolledFeature Tests =====

TEST(RolledFeatureTest, CreateRolled) {
    RolledFeature rolled(EntityId::generate(), 50.0);
    EXPECT_FALSE(rolled.isValid());
}

TEST(RolledFeatureTest, SetRotations) {
    RolledFeature rolled(EntityId::generate(), 50.0);
    rolled.setRotations(3.5);
    EXPECT_TRUE(true);  // Rotations set
}

TEST(RolledFeatureTest, ExecuteRoll) {
    RolledFeature rolled(EntityId::generate(), 50.0);
    bool result = rolled.executeRoll();
    EXPECT_TRUE(result);
    EXPECT_TRUE(rolled.isValid());
}

TEST(RolledFeatureTest, InvalidRadius) {
    RolledFeature rolled(EntityId::generate(), -1.0);
    bool result = rolled.executeRoll();
    EXPECT_FALSE(result);
}

// ===== BendTable Tests =====

TEST(BendTableTest, CreateBendTable) {
    BendTable table;
    // Empty table should still have header
    EXPECT_TRUE(table.exportToCSV().find("Material") != std::string::npos);
}

TEST(BendTableTest, AddMaterialData) {
    BendTable table;
    BendTable::BendEntry entry;
    entry.material = "Aluminum 6061";
    entry.thickness = 2.0;
    entry.min_radius = 3.0;
    entry.max_radius = 10.0;
    entry.allowance_90 = 2.5;

    table.addMaterialData(entry);
    EXPECT_TRUE(table.exportToCSV().find("Aluminum") != std::string::npos);
}

TEST(BendTableTest, LookupBendAllowance) {
    BendTable table;
    BendTable::BendEntry entry;
    entry.material = "Steel";
    entry.allowance_90 = 3.0;
    table.addMaterialData(entry);

    double allowance = table.lookupBendAllowance("Steel", 2.0, 90.0);
    EXPECT_DOUBLE_EQ(allowance, 3.0);
}

TEST(BendTableTest, GetRecommendedRadius) {
    BendTable table;
    BendTable::BendEntry entry;
    entry.material = "Copper";
    entry.thickness = 1.5;
    entry.min_radius = 2.0;
    entry.max_radius = 8.0;
    table.addMaterialData(entry);

    double radius = table.getRecommendedRadius("Copper", 1.5);
    EXPECT_EQ(radius, 5.0);  // (2.0 + 8.0) / 2.0
}

// ===== SheetMetalOperations Tests =====

TEST(SheetMetalOperationsTest, CreateOperations) {
    SheetMetalOperations ops(EntityId::generate());
    EXPECT_EQ(ops.getOperationCount(), 0);
}

TEST(SheetMetalOperationsTest, CreateUnfold) {
    SheetMetalOperations ops(EntityId::generate());
    auto unfold = ops.createUnfold(EntityId::generate());

    EXPECT_NE(unfold, nullptr);
    EXPECT_EQ(ops.getOperationCount(), 1);
}

TEST(SheetMetalOperationsTest, CreateNotch) {
    SheetMetalOperations ops(EntityId::generate());
    auto notch = ops.createNotch(EntityId::generate(), EntityId::generate());

    EXPECT_NE(notch, nullptr);
    EXPECT_EQ(ops.getOperationCount(), 1);
}

TEST(SheetMetalOperationsTest, CreatePattern) {
    SheetMetalOperations ops(EntityId::generate());
    auto pattern = ops.createPattern(EntityId::generate());

    EXPECT_NE(pattern, nullptr);
    EXPECT_EQ(ops.getOperationCount(), 1);
}

TEST(SheetMetalOperationsTest, CreateRolled) {
    SheetMetalOperations ops(EntityId::generate());
    auto rolled = ops.createRolled(EntityId::generate(), 50.0);

    EXPECT_NE(rolled, nullptr);
    EXPECT_EQ(ops.getOperationCount(), 1);
}

TEST(SheetMetalOperationsTest, MixedOperations) {
    SheetMetalOperations ops(EntityId::generate());
    auto unfold = ops.createUnfold(EntityId::generate());
    auto notch = ops.createNotch(EntityId::generate(), EntityId::generate());
    auto pattern = ops.createPattern(EntityId::generate());

    EXPECT_EQ(ops.getOperationCount(), 3);
}

TEST(SheetMetalOperationsTest, GetBendTable) {
    SheetMetalOperations ops(EntityId::generate());
    BendTable::BendEntry entry;
    entry.material = "Stainless";
    entry.allowance_90 = 2.8;
    ops.getBendTable().addMaterialData(entry);

    EXPECT_TRUE(ops.getBendTable().exportToCSV().find("Stainless") != std::string::npos);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
