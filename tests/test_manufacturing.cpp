#include <gtest/gtest.h>
#include "bendcore/manufacturing.h"

using namespace bendcore::manufacturing;

TEST(BendOperationTest, CreateBendOperation) {
    BendOperation op;
    op.sequenceNumber = 1;
    op.bendAngle = 90.0;
    op.bendRadius = 15.0;
    op.bendType = "free";
    op.toolSpeed = 10.0;

    EXPECT_EQ(op.sequenceNumber, 1);
    EXPECT_EQ(op.bendAngle, 90.0);
    EXPECT_EQ(op.bendRadius, 15.0);
}

TEST(ManufacturingSheetTest, CreateSheet) {
    ManufacturingSheet sheet;
    sheet.partNumber = "P001";
    sheet.partName = "Test Pipe Assembly";
    sheet.materialCost = 50.0;
    sheet.laborCost = 75.0;
    sheet.machineTime = 30.0;

    EXPECT_EQ(sheet.partNumber, "P001");
    EXPECT_EQ(sheet.machineTime, 30.0);
}

TEST(NCCodeGeneratorTest, GenerateBendingCode) {
    BendOperation op;
    op.sequenceNumber = 1;
    op.bendAngle = 90.0;
    op.bendRadius = 15.0;
    op.toolPosition = 50.0;
    op.toolSpeed = 10.0;
    op.bendType = "free";

    std::string code = NCCodeGenerator::generateBendingCode(op);
    EXPECT_FALSE(code.empty());
    EXPECT_TRUE(code.find("O1001") != std::string::npos);
    EXPECT_TRUE(code.find("G00") != std::string::npos);
    EXPECT_TRUE(code.find("90") != std::string::npos);
}

TEST(NCCodeGeneratorTest, GenerateCuttingCode) {
    CutOperation op;
    op.sequenceNumber = 1;
    op.cutPosition = 50.0;
    op.cutLength = 10.0;
    op.toolSpeed = 5.0;
    op.cutType = "straight";

    std::string code = NCCodeGenerator::generateCuttingCode(op);
    EXPECT_FALSE(code.empty());
    EXPECT_TRUE(code.find("O2001") != std::string::npos);
}

TEST(NCCodeGeneratorTest, GenerateCompleteProgram) {
    ManufacturingSheet sheet;
    sheet.partNumber = "P001";
    sheet.partName = "Test Assembly";
    sheet.machineTime = 30.0;

    BendOperation bend;
    bend.sequenceNumber = 1;
    bend.bendAngle = 90.0;
    bend.bendRadius = 15.0;
    bend.toolPosition = 50.0;
    bend.toolSpeed = 10.0;
    sheet.bendOperations.push_back(bend);

    std::string program = NCCodeGenerator::generateCompleteProgram(sheet);
    EXPECT_FALSE(program.empty());
    EXPECT_TRUE(program.find("BendCore CAD") != std::string::npos);
    EXPECT_TRUE(program.find("M30") != std::string::npos);
}

TEST(TubeUnrollerTest, CalculateUnfolding) {
    std::vector<double> angles = {90.0, 90.0};
    std::vector<double> radii = {15.0, 15.0};

    auto result = TubeUnroller::calculateUnfolding(10.0, 1.5, angles, radii);

    EXPECT_GT(result.unfoldedLength, 0);
    EXPECT_GT(result.totalLengthNeeded, 0);
    EXPECT_EQ(result.bendMarks.size(), 2);
    EXPECT_EQ(result.bendAngles.size(), 2);
}

TEST(ManufacturingCostCalculatorTest, CalculateCost) {
    std::vector<BendOperation> bends;
    BendOperation bend1;
    bend1.bendAngle = 90.0;
    bend1.bendRadius = 15.0;
    bends.push_back(bend1);

    std::vector<CutOperation> cuts;
    CutOperation cut1;
    cut1.cutLength = 10.0;
    cuts.push_back(cut1);

    auto cost = ManufacturingCostCalculator::calculateCost(
        100.0,      // tube length
        10.0,       // OD
        1.5,        // wall thickness
        0.5,        // material price per kg
        bends,
        cuts
    );

    EXPECT_GT(cost.totalCost, 0);
    EXPECT_GT(cost.materialCost, 0);
    EXPECT_GT(cost.bendingCost, 0);
}

TEST(CollisionCheckerTest, NoCollisions) {
    ManufacturingSheet sheet;
    BendOperation bend;
    bend.toolPosition = 100.0;
    bend.bendRadius = 20.0;
    sheet.bendOperations.push_back(bend);

    auto report = CollisionChecker::checkMachineCollisions(
        sheet,
        3000.0,  // workspace X
        2000.0,  // workspace Y
        1500.0   // workspace Z
    );

    EXPECT_FALSE(report.hasCollisions);
    EXPECT_FALSE(report.recommendation.empty());
}

TEST(CollisionCheckerTest, DetectCollisions) {
    ManufacturingSheet sheet;
    BendOperation bend;
    bend.toolPosition = 5000.0;  // Outside workspace
    bend.bendRadius = 20.0;
    sheet.bendOperations.push_back(bend);

    auto report = CollisionChecker::checkMachineCollisions(
        sheet,
        3000.0,  // workspace X
        2000.0,  // workspace Y
        1500.0   // workspace Z
    );

    EXPECT_TRUE(report.hasCollisions);
    EXPECT_GT(report.collisionLocations.size(), 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
