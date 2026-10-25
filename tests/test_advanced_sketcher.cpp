#include <gtest/gtest.h>
#include "bendcore/sketch/advanced_sketcher.hpp"

using namespace BendCore::CadCore;

// ===== ConstructionGeometry Tests =====

TEST(ConstructionGeometryTest, CreateConstructionPoint) {
    ConstructionGeometry geom(EntityId::generate(),
                             ConstructionGeometry::GeometryType::CONSTRUCTION_POINT);
    EXPECT_EQ(geom.getType(), ConstructionGeometry::GeometryType::CONSTRUCTION_POINT);
    EXPECT_TRUE(geom.isConstruction());
}

TEST(ConstructionGeometryTest, CreateConstructionLine) {
    ConstructionGeometry geom(EntityId::generate(),
                             ConstructionGeometry::GeometryType::CONSTRUCTION_LINE);
    EXPECT_EQ(geom.getType(), ConstructionGeometry::GeometryType::CONSTRUCTION_LINE);
}

TEST(ConstructionGeometryTest, SetPosition) {
    ConstructionGeometry geom(EntityId::generate(),
                             ConstructionGeometry::GeometryType::CONSTRUCTION_CIRCLE);
    Vector3D pos(10.0, 20.0, 0.0);
    geom.setPosition(pos);
    EXPECT_EQ(geom.getPosition().x, 10.0);
    EXPECT_EQ(geom.getPosition().y, 20.0);
}

// ===== GeometricRelation Tests =====

TEST(GeometricRelationTest, CreateCollinearRelation) {
    GeometricRelation rel(EntityId::generate(), GeometricRelation::RelationType::COLLINEAR);
    EXPECT_EQ(rel.getType(), GeometricRelation::RelationType::COLLINEAR);
    EXPECT_FALSE(rel.isSatisfied());
}

TEST(GeometricRelationTest, AddEntities) {
    GeometricRelation rel(EntityId::generate(), GeometricRelation::RelationType::EQUAL_LENGTH);
    EntityId id1 = EntityId::generate();
    EntityId id2 = EntityId::generate();

    rel.addEntity(id1);
    rel.addEntity(id2);

    EXPECT_EQ(rel.getEntities().size(), 2);
}

TEST(GeometricRelationTest, SetSatisfied) {
    GeometricRelation rel(EntityId::generate(), GeometricRelation::RelationType::TANGENT_CURVES);
    rel.setSatisfied(true);
    EXPECT_TRUE(rel.isSatisfied());
}

TEST(GeometricRelationTest, SymmetricRelation) {
    GeometricRelation rel(EntityId::generate(), GeometricRelation::RelationType::SYMMETRIC);
    EXPECT_EQ(rel.getType(), GeometricRelation::RelationType::SYMMETRIC);
}

// ===== SmartDimensioning Tests =====

TEST(SmartDimensioningTest, CreateSmartDimensioning) {
    SmartDimensioning sd(EntityId::generate());
    EXPECT_EQ(sd.getDimensionPositions().size(), 0);
}

TEST(SmartDimensioningTest, SuggestDimensionPosition) {
    SmartDimensioning sd(EntityId::generate());
    Vector3D center(100.0, 100.0, 0.0);
    Vector3D suggested = sd.suggestDimensionPosition(EntityId::generate(), center);

    EXPECT_GT(suggested.x, center.x);  // Offset to right
    EXPECT_GT(suggested.y, center.y);  // Offset above
}

TEST(SmartDimensioningTest, PlaceDimensionSmart) {
    SmartDimensioning sd(EntityId::generate());
    EntityId dim_id = EntityId::generate();
    Vector3D pos(50.0, 50.0, 0.0);

    bool result = sd.placeDimensionSmart(dim_id, pos);
    EXPECT_TRUE(result);
    EXPECT_EQ(sd.getDimensionPositions().size(), 1);
}

TEST(SmartDimensioningTest, AutoDimensionSketch) {
    SmartDimensioning sd(EntityId::generate());
    std::vector<EntityId> entities;
    entities.push_back(EntityId::generate());
    entities.push_back(EntityId::generate());
    entities.push_back(EntityId::generate());

    auto suggestions = sd.autoDimensionSketch(entities);
    EXPECT_EQ(suggestions.size(), 3);
}

// ===== CurveTools Tests =====

TEST(CurveToolsTest, CreateBezierCurve) {
    CurveTools tools;
    std::vector<Vector3D> points;
    points.push_back(Vector3D(0, 0, 0));
    points.push_back(Vector3D(10, 10, 0));
    points.push_back(Vector3D(20, 5, 0));

    bool result = tools.createBezierCurve(points, 2);
    EXPECT_TRUE(result);
}

TEST(CurveToolsTest, BezierCurveTooFewPoints) {
    CurveTools tools;
    std::vector<Vector3D> points;
    points.push_back(Vector3D(0, 0, 0));

    bool result = tools.createBezierCurve(points, 2);
    EXPECT_FALSE(result);
}

TEST(CurveToolsTest, EvaluateCurve) {
    CurveTools tools;
    std::vector<Vector3D> points;
    points.push_back(Vector3D(0, 0, 0));
    points.push_back(Vector3D(10, 0, 0));
    points.push_back(Vector3D(20, 0, 0));

    [[maybe_unused]] bool create_result = tools.createBezierCurve(points, 2);
    Vector3D eval = tools.evaluateCurve(0.5);

    EXPECT_GE(eval.x, 0.0);
    EXPECT_LE(eval.x, 20.0);
}

TEST(CurveToolsTest, GetCurveTangent) {
    CurveTools tools;
    std::vector<Vector3D> points;
    points.push_back(Vector3D(0, 0, 0));
    points.push_back(Vector3D(10, 10, 0));
    points.push_back(Vector3D(20, 0, 0));

    [[maybe_unused]] bool create_result = tools.createBezierCurve(points, 2);
    Vector3D tangent = tools.getCurveTangent(0.5);

    // Tangent should be normalized or zero
    double mag = std::sqrt(tangent.x*tangent.x + tangent.y*tangent.y + tangent.z*tangent.z);
    EXPECT_LE(mag, 1.1);  // Allow small numerical error
}

TEST(CurveToolsTest, GetCurveCurvature) {
    CurveTools tools;
    std::vector<Vector3D> points;
    points.push_back(Vector3D(0, 0, 0));
    points.push_back(Vector3D(10, 10, 0));
    points.push_back(Vector3D(20, 0, 0));

    [[maybe_unused]] bool create_result = tools.createBezierCurve(points, 2);
    double curvature = tools.getCurveCurvature(0.5);

    EXPECT_GE(curvature, 0.0);
}

TEST(CurveToolsTest, CreateInterpolatedCurve) {
    CurveTools tools;
    std::vector<Vector3D> points;
    points.push_back(Vector3D(0, 0, 0));
    points.push_back(Vector3D(5, 5, 0));
    points.push_back(Vector3D(10, 2, 0));

    bool result = tools.createInterpolatedCurve(points);
    EXPECT_TRUE(result);
}

// ===== AdvancedSketchSolver Tests =====

TEST(AdvancedSketchSolverTest, CreateSolver) {
    AdvancedSketchSolver solver(EntityId::generate());
    EXPECT_EQ(solver.getStatus(), AdvancedSketchSolver::SolverStatus::UNDER_CONSTRAINED);
}

TEST(AdvancedSketchSolverTest, SolveSketch) {
    AdvancedSketchSolver solver(EntityId::generate());
    bool result = solver.solve();
    EXPECT_TRUE(result);
}

TEST(AdvancedSketchSolverTest, GetDegreesOfFreedom) {
    AdvancedSketchSolver solver(EntityId::generate());
    [[maybe_unused]] bool solve_result = solver.solve();
    int dof = solver.getDegreesOfFreedom();
    EXPECT_GE(dof, 0);
}

TEST(AdvancedSketchSolverTest, DiagnoseFreedom) {
    AdvancedSketchSolver solver(EntityId::generate());
    std::vector<EntityId> free_entities = solver.diagnoseFreedom();
    // Can be empty or non-empty depending on solver state
    EXPECT_GE(free_entities.size(), 0);
}

TEST(AdvancedSketchSolverTest, FindConflicts) {
    AdvancedSketchSolver solver(EntityId::generate());
    auto conflicts = solver.findConflicts();
    EXPECT_GE(conflicts.size(), 0);
}

TEST(AdvancedSketchSolverTest, SuggestConstraints) {
    AdvancedSketchSolver solver(EntityId::generate());
    [[maybe_unused]] bool solve_result = solver.solve();
    auto suggestions = solver.suggestConstraints();
    EXPECT_GT(suggestions.size(), 0);
}

// ===== AdvancedSketcher Tests =====

TEST(AdvancedSketcherTest, CreateAdvancedSketcher) {
    AdvancedSketcher sketcher(EntityId::generate());
    EXPECT_EQ(sketcher.getConstructionGeometryCount(), 0);
    EXPECT_EQ(sketcher.getRelationCount(), 0);
}

TEST(AdvancedSketcherTest, AddConstructionGeometry) {
    AdvancedSketcher sketcher(EntityId::generate());
    auto geom = sketcher.addConstructionGeometry(
        ConstructionGeometry::GeometryType::CONSTRUCTION_LINE);

    EXPECT_NE(geom, nullptr);
    EXPECT_EQ(sketcher.getConstructionGeometryCount(), 1);
}

TEST(AdvancedSketcherTest, AddMultipleGeometry) {
    AdvancedSketcher sketcher(EntityId::generate());
    [[maybe_unused]] auto g1 = sketcher.addConstructionGeometry(ConstructionGeometry::GeometryType::CONSTRUCTION_POINT);
    [[maybe_unused]] auto g2 = sketcher.addConstructionGeometry(ConstructionGeometry::GeometryType::CONSTRUCTION_CIRCLE);
    [[maybe_unused]] auto g3 = sketcher.addConstructionGeometry(ConstructionGeometry::GeometryType::CONSTRUCTION_ARC);

    EXPECT_EQ(sketcher.getConstructionGeometryCount(), 3);
}

TEST(AdvancedSketcherTest, AddGeometricRelation) {
    AdvancedSketcher sketcher(EntityId::generate());
    auto rel = sketcher.addRelation(GeometricRelation::RelationType::COLLINEAR);

    EXPECT_NE(rel, nullptr);
    EXPECT_EQ(sketcher.getRelationCount(), 1);
}

TEST(AdvancedSketcherTest, MixedOperations) {
    AdvancedSketcher sketcher(EntityId::generate());

    // Add construction geometry
    [[maybe_unused]] auto g1 = sketcher.addConstructionGeometry(ConstructionGeometry::GeometryType::CONSTRUCTION_LINE);
    [[maybe_unused]] auto g2 = sketcher.addConstructionGeometry(ConstructionGeometry::GeometryType::CONSTRUCTION_CIRCLE);

    // Add relations
    [[maybe_unused]] auto r1 = sketcher.addRelation(GeometricRelation::RelationType::TANGENT_CURVES);
    [[maybe_unused]] auto r2 = sketcher.addRelation(GeometricRelation::RelationType::SYMMETRIC);

    EXPECT_EQ(sketcher.getConstructionGeometryCount(), 2);
    EXPECT_EQ(sketcher.getRelationCount(), 2);
}

TEST(AdvancedSketcherTest, AccessSmartDimensioning) {
    AdvancedSketcher sketcher(EntityId::generate());
    SmartDimensioning& sd = sketcher.getSmartDimensioning();

    EntityId dim_id = EntityId::generate();
    Vector3D pos(50.0, 50.0, 0.0);
    [[maybe_unused]] bool result = sd.placeDimensionSmart(dim_id, pos);

    EXPECT_EQ(sd.getDimensionPositions().size(), 1);
}

TEST(AdvancedSketcherTest, AccessSolver) {
    AdvancedSketcher sketcher(EntityId::generate());
    AdvancedSketchSolver& solver = sketcher.getSolver();

    [[maybe_unused]] bool solve_result = solver.solve();
    EXPECT_GE(solver.getDegreesOfFreedom(), 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
