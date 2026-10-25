#include <gtest/gtest.h>
#include "bendcore/pipe_profile.h"
#include "bendcore/pipe_fittings.h"
#include "bendcore/constraint_system.h"

using namespace bendcore::pipe;
using namespace bendcore::assembly;

TEST(PipeProfileTest, DIN2391StockProfiles) {
    auto profiles = PipeProfileLibrary::getDIN2391Profiles();
    EXPECT_GT(profiles.size(), 0);

    auto profile = profiles.front();
    EXPECT_EQ(profile.type, ProfileType::CIRCULAR);
    EXPECT_GT(profile.outerDiameter, 0);
    EXPECT_GT(profile.wallThickness, 0);
}

TEST(PipeProfileTest, FindProfileByDesignation) {
    auto result = PipeProfileLibrary::findProfile("DIN2391-10x1.5");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->outerDiameter, 10.0);
    EXPECT_EQ(result->wallThickness, 1.5);
}

TEST(PipeProfileTest, CalculateCrossSectionArea) {
    PipeProfile pipe(ProfileType::CIRCULAR, "test", 10.0, 8.0, 1.0, 100, "steel");
    double area = pipe.getCrossSectionArea();
    EXPECT_GT(area, 0);
    EXPECT_NEAR(area, 28.27, 0.1);  // π * (5² - 4²) ≈ 28.27
}

TEST(PipeProfileTest, CalculateSectionModulus) {
    PipeProfile pipe(ProfileType::CIRCULAR, "test", 10.0, 8.0, 1.0, 100, "steel");
    double modulus = pipe.getSectionModulus();
    EXPECT_GT(modulus, 0);
}

TEST(PipeFittingTest, StandardElbows) {
    auto elbows = FittingLibrary::getStandardElbows();
    EXPECT_GT(elbows.size(), 0);

    auto elbow = elbows.front();
    EXPECT_EQ(elbow.getType(), FittingType::ELBOW_90);
}

TEST(PipeFittingTest, FindFittingByDesignation) {
    auto result = FittingLibrary::findFitting("ISO-90-M10");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->getType(), FittingType::ELBOW_90);
}

TEST(PipeFittingTest, FindFittingsByType) {
    auto elbows = FittingLibrary::findFittingsByType(FittingType::ELBOW_90);
    EXPECT_GT(elbows.size(), 0);
}

TEST(ConstraintSystemTest, AddConstraints) {
    ConstraintSystem cs;
    ConstraintTarget target{"pipe1", "pipe2", 50.0};
    Constraint constraint(ConstraintType::DISTANCE, target);

    cs.addConstraint(constraint);
    EXPECT_EQ(cs.getConstraints().size(), 1);
}

TEST(ConstraintSystemTest, GetConstraintsByType) {
    ConstraintSystem cs;
    ConstraintTarget target1{"pipe1", "pipe2", 50.0};
    ConstraintTarget target2{"pipe3", "pipe4", std::nullopt};

    cs.addConstraint(Constraint(ConstraintType::DISTANCE, target1));
    cs.addConstraint(Constraint(ConstraintType::PARALLEL, target2));

    auto distances = cs.getConstraintsByType(ConstraintType::DISTANCE);
    EXPECT_EQ(distances.size(), 1);
}

TEST(ConstraintSystemTest, ValidateConstraints) {
    ConstraintSystem cs;
    ConstraintTarget target{"pipe1", "pipe2", 50.0};
    cs.addConstraint(Constraint(ConstraintType::DISTANCE, target));

    bool valid = cs.validateAllConstraints();
    EXPECT_TRUE(valid);
}

TEST(Vector3Test, BasicOperations) {
    Vector3 v(3, 4, 0);
    EXPECT_NEAR(v.length(), 5.0, 0.0001);

    Vector3 normalized = v.normalized();
    EXPECT_NEAR(normalized.length(), 1.0, 0.0001);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
