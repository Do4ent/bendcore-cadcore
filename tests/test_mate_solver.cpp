#include <gtest/gtest.h>
#include "bendcore/assembly/mate_solver.hpp"
#include "bendcore/document/entity_id.hpp"

using namespace BendCore::CadCore;

class MateSolverTest : public ::testing::Test {
protected:
    MateSolver solver;
    EntityId component1 = EntityId::generate();
    EntityId component2 = EntityId::generate();
};

// Test: Create mate constraint
TEST_F(MateSolverTest, CreateMateConstraint) {
    MateConstraint mate(EntityId::generate(), MateConstraint::MateType::FIXED);
    mate.setEntity1(component1);

    EXPECT_EQ(mate.getEntity1(), component1);
    EXPECT_EQ(mate.getType(), MateConstraint::MateType::FIXED);
}

// Test: Add constraint
TEST_F(MateSolverTest, AddConstraint) {
    MateConstraint mate(EntityId::generate(), MateConstraint::MateType::COINCIDENT);
    mate.setEntity1(component1);
    mate.setEntity2(component2);

    solver.addConstraint(mate);

    EXPECT_EQ(solver.getConstraints().size(), 1);
}

// Test: Fixed constraint
TEST_F(MateSolverTest, FixedConstraint) {
    MateConstraint mate(EntityId::generate(), MateConstraint::MateType::FIXED);
    mate.setEntity1(component1);
    mate.setEntity2(component2);

    solver.addConstraint(mate);
    bool result = solver.solve();

    EXPECT_TRUE(result);
    auto pos = solver.getComponentPosition(component1);
    EXPECT_DOUBLE_EQ(pos.x, 0.0);
    EXPECT_DOUBLE_EQ(pos.y, 0.0);
    EXPECT_DOUBLE_EQ(pos.z, 0.0);
}

// Test: Coincident constraint
TEST_F(MateSolverTest, CoincidentConstraint) {
    MateConstraint mate(EntityId::generate(), MateConstraint::MateType::COINCIDENT);
    mate.setEntity1(component1);
    mate.setEntity2(component2);

    solver.addConstraint(mate);
    solver.solve();

    auto pos1 = solver.getComponentPosition(component1);
    auto pos2 = solver.getComponentPosition(component2);

    EXPECT_DOUBLE_EQ(pos1.x, pos2.x);
    EXPECT_DOUBLE_EQ(pos1.y, pos2.y);
    EXPECT_DOUBLE_EQ(pos1.z, pos2.z);
}

// Test: Distance constraint
TEST_F(MateSolverTest, DistanceConstraint) {
    MateConstraint mate(EntityId::generate(), MateConstraint::MateType::DISTANCE);
    mate.setEntity1(component1);
    mate.setEntity2(component2);
    mate.setValue(10.0);  // 10 units distance

    solver.addConstraint(mate);
    solver.solve();

    auto pos1 = solver.getComponentPosition(component1);
    auto pos2 = solver.getComponentPosition(component2);

    double distance = std::sqrt(
        (pos2.x - pos1.x) * (pos2.x - pos1.x) +
        (pos2.y - pos1.y) * (pos2.y - pos1.y) +
        (pos2.z - pos1.z) * (pos2.z - pos1.z)
    );

    EXPECT_DOUBLE_EQ(distance, 10.0);
}

// Test: Parallel constraint
TEST_F(MateSolverTest, ParallelConstraint) {
    MateConstraint mate(EntityId::generate(), MateConstraint::MateType::PARALLEL);
    mate.setEntity1(component1);
    mate.setEntity2(component2);

    solver.addConstraint(mate);
    solver.solve();

    auto orient1 = solver.getComponentOrientation(component1);
    auto orient2 = solver.getComponentOrientation(component2);

    EXPECT_DOUBLE_EQ(orient1.x, orient2.x);
    EXPECT_DOUBLE_EQ(orient1.y, orient2.y);
    EXPECT_DOUBLE_EQ(orient1.z, orient2.z);
}

// Test: Remove constraint
TEST_F(MateSolverTest, RemoveConstraint) {
    EntityId constraint_id = EntityId::generate();
    MateConstraint mate(constraint_id, MateConstraint::MateType::FIXED);
    mate.setEntity1(component1);

    solver.addConstraint(mate);
    EXPECT_EQ(solver.getConstraints().size(), 1);

    bool removed = solver.removeConstraint(constraint_id);
    EXPECT_TRUE(removed);
    EXPECT_EQ(solver.getConstraints().size(), 0);
}

// Test: Multiple constraints
TEST_F(MateSolverTest, MultipleConstraints) {
    EntityId component3 = EntityId::generate();

    // Constraint 1: component1 fixed
    MateConstraint mate1(EntityId::generate(), MateConstraint::MateType::FIXED);
    mate1.setEntity1(component1);
    solver.addConstraint(mate1);

    // Constraint 2: component2 coincident with component1
    MateConstraint mate2(EntityId::generate(), MateConstraint::MateType::COINCIDENT);
    mate2.setEntity1(component1);
    mate2.setEntity2(component2);
    solver.addConstraint(mate2);

    // Constraint 3: component3 at distance from component2
    MateConstraint mate3(EntityId::generate(), MateConstraint::MateType::DISTANCE);
    mate3.setEntity1(component2);
    mate3.setEntity2(component3);
    mate3.setValue(5.0);
    solver.addConstraint(mate3);

    solver.solve();

    // All constraints added
    EXPECT_EQ(solver.getConstraints().size(), 3);

    // Verify positions
    auto pos1 = solver.getComponentPosition(component1);
    auto pos2 = solver.getComponentPosition(component2);
    auto pos3 = solver.getComponentPosition(component3);

    // component1 and component2 should be at origin
    EXPECT_DOUBLE_EQ(pos1.x, 0.0);
    EXPECT_DOUBLE_EQ(pos2.x, 0.0);

    // component3 should be 5 units away on X axis
    EXPECT_DOUBLE_EQ(pos3.x, 5.0);
}

// Test: Constraint validation
TEST_F(MateSolverTest, ConstraintValidation) {
    MateConstraint invalid_mate(EntityId::generate(), MateConstraint::MateType::DISTANCE);
    // Missing entities
    EXPECT_FALSE(invalid_mate.validate());

    invalid_mate.setEntity1(component1);
    invalid_mate.setEntity2(component2);
    // Missing value for DISTANCE
    EXPECT_FALSE(invalid_mate.validate());

    invalid_mate.setValue(10.0);
    // Now valid
    EXPECT_TRUE(invalid_mate.validate());
}

// Test: Under-constrained detection
TEST_F(MateSolverTest, UnderConstrainedDetection) {
    MateConstraint mate(EntityId::generate(), MateConstraint::MateType::FIXED);
    mate.setEntity1(component1);
    solver.addConstraint(mate);

    solver.solve();

    // component2 is not constrained - assembly is under-constrained
    EXPECT_TRUE(solver.isUnderConstrained());
}

// Test: Diagnose
TEST_F(MateSolverTest, Diagnose) {
    MateConstraint mate(EntityId::generate(), MateConstraint::MateType::FIXED);
    mate.setEntity1(component1);
    solver.addConstraint(mate);

    solver.solve();

    std::string diagnosis = solver.diagnose();
    EXPECT_FALSE(diagnosis.empty());
    EXPECT_TRUE(diagnosis.find("constraint") != std::string::npos);
}

// Test: Clear constraints
TEST_F(MateSolverTest, ClearConstraints) {
    MateConstraint mate(EntityId::generate(), MateConstraint::MateType::FIXED);
    mate.setEntity1(component1);

    solver.addConstraint(mate);
    EXPECT_EQ(solver.getConstraints().size(), 1);

    solver.clear();
    EXPECT_EQ(solver.getConstraints().size(), 0);
}

// Test: Angle constraint
TEST_F(MateSolverTest, AngleConstraint) {
    const double TEST_ANGLE = M_PI / 4.0;  // 45 degrees

    MateConstraint mate(EntityId::generate(), MateConstraint::MateType::ANGLE);
    mate.setEntity1(component1);
    mate.setEntity2(component2);
    mate.setValue(TEST_ANGLE);

    solver.addConstraint(mate);
    solver.solve();

    auto orient2 = solver.getComponentOrientation(component2);
    EXPECT_NEAR(orient2.z, TEST_ANGLE, 1e-6);
}

// Test: Symmetric constraint
TEST_F(MateSolverTest, SymmetricConstraint) {
    MateConstraint mate(EntityId::generate(), MateConstraint::MateType::SYMMETRIC);
    mate.setEntity1(component1);
    mate.setEntity2(component2);

    solver.addConstraint(mate);
    solver.solve();

    auto pos1 = solver.getComponentPosition(component1);
    auto pos2 = solver.getComponentPosition(component2);

    // Symmetric about YZ plane: x coordinates should be negatives
    EXPECT_DOUBLE_EQ(pos1.x, -pos2.x);
    EXPECT_DOUBLE_EQ(pos1.y, pos2.y);
    EXPECT_DOUBLE_EQ(pos1.z, pos2.z);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
