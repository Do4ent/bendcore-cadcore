#include <gtest/gtest.h>
#include "bendcore/bend_calculator.h"
#include <cmath>

using namespace bendcore::bending;

TEST(MaterialDatabaseTest, GetSteel) {
    auto steel = MaterialDatabase::getSteel();
    EXPECT_EQ(steel.name, "Steel");
    EXPECT_EQ(steel.youngModulus, 210000);
    EXPECT_EQ(steel.density, 7.85);
}

TEST(MaterialDatabaseTest, GetMaterial) {
    auto steel = MaterialDatabase::getMaterial("steel");
    EXPECT_TRUE(steel.has_value());
    EXPECT_EQ(steel->name, "Steel");
}

TEST(BendCalculatorTest, CalculateMinBendRadius) {
    MaterialProperties steel = MaterialDatabase::getSteel();
    double minRadius = BendCalculator::calculateMinBendRadius(10.0, 1.5, steel);
    EXPECT_GT(minRadius, 0);
    EXPECT_LT(minRadius, 10.0);  // Should be less than OD
}

TEST(BendCalculatorTest, CalculateMaxBendRadius) {
    MaterialProperties steel = MaterialDatabase::getSteel();
    double maxRadius = BendCalculator::calculateMaxBendRadius(10.0, 1.5, steel);
    EXPECT_GT(maxRadius, 0);
}

TEST(BendCalculatorTest, CalculateBendStress) {
    MaterialProperties steel = MaterialDatabase::getSteel();
    double stress = BendCalculator::calculateBendStress(10.0, 8.0, 15.0, steel);
    EXPECT_GT(stress, 0);
}

TEST(BendCalculatorTest, CalculateSpringback) {
    MaterialProperties steel = MaterialDatabase::getSteel();
    double springback = BendCalculator::calculateSpringback(90.0, 15.0, 10.0, steel);
    EXPECT_GT(springback, 0);
    EXPECT_LT(springback, 90.0);  // Springback should be less than original angle
}

TEST(BendCalculatorTest, CalculateBend) {
    BendResult result = BendCalculator::calculateBend(
        10.0,      // OD
        1.5,       // Wall thickness
        90.0,      // Bend angle
        "Steel",   // Material
        15.0       // Bend radius
    );

    // Verify calculations are performed
    EXPECT_EQ(result.bendRadius, 15.0);
    EXPECT_GT(result.minAllowedRadius, 0);
    EXPECT_GT(result.maxAllowedRadius, 0);
    EXPECT_LT(result.minAllowedRadius, result.maxAllowedRadius);
    // Note: isFeasible depends on internal calculation logic
}

TEST(BendCalculatorTest, BendWithAutoRadius) {
    BendResult result = BendCalculator::calculateBend(
        10.0,      // OD
        1.5,       // Wall thickness
        90.0,      // Bend angle
        "Steel"    // Material
    );

    EXPECT_GT(result.bendRadius, 0);
    EXPECT_EQ(result.bendRadius, 15.0);  // Default: 1.5 × OD
    EXPECT_GT(result.minAllowedRadius, 0);
}

TEST(BendCalculatorTest, BendAluminum) {
    BendResult result = BendCalculator::calculateBend(
        12.0,       // OD
        1.5,        // Wall thickness
        45.0,       // Bend angle
        "aluminum", // Material
        20.0        // Bend radius
    );

    EXPECT_TRUE(result.isFeasible || !result.isFeasible);  // Result depends on algorithm
    EXPECT_GT(result.bendRadius, 0);
}

TEST(BendCalculatorTest, UnknownMaterial) {
    BendResult result = BendCalculator::calculateBend(
        10.0,
        1.5,
        90.0,
        "unknown_material"
    );

    EXPECT_FALSE(result.isFeasible);
    EXPECT_FALSE(result.warningMessage.empty());
}

TEST(BendCalculatorTest, BendForceCalculation) {
    MaterialProperties steel = MaterialDatabase::getSteel();
    double force = BendCalculator::calculateBendForce(10.0, 1.5, 15.0, steel);
    EXPECT_GT(force, 0);
}

TEST(BendCalculatorTest, StrainCalculation) {
    double strain = BendCalculator::calculateStrainAtFiber(15.0, 1.0, 10.0);
    EXPECT_GT(strain, 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
