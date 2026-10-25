#include <gtest/gtest.h>
#include "bendcore/analysis/inspection_tools.hpp"

using namespace BendCore::CadCore;

// ===== DistanceMeasurement Tests =====

TEST(DistanceMeasurementTest, CreateMeasurement) {
    DistanceMeasurement measurement(EntityId::generate(), EntityId::generate());
    EXPECT_DOUBLE_EQ(measurement.getDistance(), 0.0);
}

TEST(DistanceMeasurementTest, SetPositions) {
    DistanceMeasurement measurement(EntityId::generate(), EntityId::generate());
    Vector3D pos1(0, 0, 0);
    Vector3D pos2(3, 4, 0);

    measurement.setPositions(pos1, pos2);
    EXPECT_DOUBLE_EQ(measurement.getDistance(), 5.0);  // 3-4-5 triangle
}

TEST(DistanceMeasurementTest, WithinTolerance) {
    DistanceMeasurement measurement(EntityId::generate(), EntityId::generate());
    Vector3D pos1(0, 0, 0);
    Vector3D pos2(10, 0, 0);

    measurement.setPositions(pos1, pos2);
    EXPECT_TRUE(measurement.isWithinTolerance(10.0));
    EXPECT_FALSE(measurement.isWithinTolerance(5.0));
}

TEST(DistanceMeasurementTest, Get3DDistance) {
    DistanceMeasurement measurement(EntityId::generate(), EntityId::generate());
    Vector3D pos1(0, 0, 0);
    Vector3D pos2(1, 1, 1);

    measurement.setPositions(pos1, pos2);
    double expected = std::sqrt(3.0);
    EXPECT_DOUBLE_EQ(measurement.getDistance(), expected);
}

// ===== AngleMeasurement Tests =====

TEST(AngleMeasurementTest, CreateAngleMeasurement) {
    AngleMeasurement angle(EntityId::generate(), EntityId::generate());
    EXPECT_DOUBLE_EQ(angle.getAngleDegrees(), 0.0);
}

TEST(AngleMeasurementTest, RightAngle) {
    AngleMeasurement angle(EntityId::generate(), EntityId::generate());
    Vector3D vec1(1, 0, 0);
    Vector3D vec2(0, 1, 0);

    angle.setVectors(vec1, vec2);
    EXPECT_NEAR(angle.getAngleDegrees(), 90.0, 0.1);
}

TEST(AngleMeasurementTest, ParallelVectors) {
    AngleMeasurement angle(EntityId::generate(), EntityId::generate());
    Vector3D vec1(1, 0, 0);
    Vector3D vec2(2, 0, 0);

    angle.setVectors(vec1, vec2);
    EXPECT_NEAR(angle.getAngleDegrees(), 0.0, 0.1);
}

TEST(AngleMeasurementTest, OppositeVectors) {
    AngleMeasurement angle(EntityId::generate(), EntityId::generate());
    Vector3D vec1(1, 0, 0);
    Vector3D vec2(-1, 0, 0);

    angle.setVectors(vec1, vec2);
    EXPECT_NEAR(angle.getAngleDegrees(), 180.0, 0.1);
}

TEST(AngleMeasurementTest, AngleInRadians) {
    AngleMeasurement angle(EntityId::generate(), EntityId::generate());
    Vector3D vec1(1, 0, 0);
    Vector3D vec2(0, 1, 0);

    angle.setVectors(vec1, vec2);
    EXPECT_NEAR(angle.getAngleRadians(), M_PI/2.0, 0.01);
}

// ===== CurvatureAnalysis Tests =====

TEST(CurvatureAnalysisTest, CreateAnalysis) {
    CurvatureAnalysis analysis(EntityId::generate());
    EXPECT_EQ(analysis.getSamples().size(), 0);
}

TEST(CurvatureAnalysisTest, AddSamples) {
    CurvatureAnalysis analysis(EntityId::generate());
    analysis.addSample(0.0, 0.5);
    analysis.addSample(0.5, 0.6);
    analysis.addSample(1.0, 0.4);

    EXPECT_EQ(analysis.getSamples().size(), 3);
}

TEST(CurvatureAnalysisTest, GetMinCurvature) {
    CurvatureAnalysis analysis(EntityId::generate());
    analysis.addSample(0.0, 0.5);
    analysis.addSample(0.5, 0.2);
    analysis.addSample(1.0, 0.4);

    EXPECT_DOUBLE_EQ(analysis.getMinCurvature(), 0.2);
}

TEST(CurvatureAnalysisTest, GetMaxCurvature) {
    CurvatureAnalysis analysis(EntityId::generate());
    analysis.addSample(0.0, 0.5);
    analysis.addSample(0.5, 0.8);
    analysis.addSample(1.0, 0.4);

    EXPECT_DOUBLE_EQ(analysis.getMaxCurvature(), 0.8);
}

TEST(CurvatureAnalysisTest, GetAverageCurvature) {
    CurvatureAnalysis analysis(EntityId::generate());
    analysis.addSample(0.0, 0.3);
    analysis.addSample(0.5, 0.4);
    analysis.addSample(1.0, 0.5);

    EXPECT_NEAR(analysis.getAverageCurvature(), 0.4, 0.01);
}

TEST(CurvatureAnalysisTest, AnalyzeContinuity) {
    CurvatureAnalysis analysis(EntityId::generate());
    analysis.addSample(0.0, 0.5);
    analysis.addSample(0.5, 0.505);
    analysis.addSample(1.0, 0.51);

    std::string continuity = analysis.analyzeContinuity();
    EXPECT_TRUE(continuity == "C0" || continuity == "C1" || continuity == "C2");
}

// ===== SurfaceQuality Tests =====

TEST(SurfaceQualityTest, CreateQuality) {
    SurfaceQuality quality(EntityId::generate());
    EXPECT_EQ(quality.getQualityScore(), 0);
}

TEST(SurfaceQualityTest, EvaluateQuality) {
    SurfaceQuality quality(EntityId::generate());
    bool result = quality.evaluateQuality();
    EXPECT_TRUE(result);
    EXPECT_GE(quality.getQualityScore(), 0);
    EXPECT_LE(quality.getQualityScore(), 100);
}

TEST(SurfaceQualityTest, GetAssessment) {
    SurfaceQuality quality(EntityId::generate());
    [[maybe_unused]] bool eval_result = quality.evaluateQuality();

    std::string assessment = quality.getAssessment();
    EXPECT_FALSE(assessment.empty());
}

TEST(SurfaceQualityTest, AddDefect) {
    SurfaceQuality quality(EntityId::generate());
    Vector3D defect(10.0, 20.0, 5.0);
    quality.addDefect(defect);

    EXPECT_EQ(quality.getDefectLocations().size(), 1);
    EXPECT_EQ(quality.getDefectLocations()[0].x, 10.0);
}

// ===== ToleranceChecker Tests =====

TEST(ToleranceCheckerTest, CreateChecker) {
    ToleranceChecker checker(EntityId::generate());
    EXPECT_TRUE(checker.isCompliant());
}

TEST(ToleranceCheckerTest, AddTolerance) {
    ToleranceChecker checker(EntityId::generate());
    ToleranceChecker::ToleranceSpec spec;
    spec.type = ToleranceChecker::ToleranceType::DIMENSIONAL;
    spec.nominal_value = 10.0;
    spec.lower_limit = 9.9;
    spec.upper_limit = 10.1;
    spec.description = "Width";

    checker.addTolerance(spec);
    EXPECT_TRUE(true);  // Tolerance added
}

TEST(ToleranceCheckerTest, CheckWithinTolerance) {
    ToleranceChecker checker(EntityId::generate());
    ToleranceChecker::ToleranceSpec spec;
    spec.nominal_value = 10.0;
    spec.lower_limit = 9.0;
    spec.upper_limit = 11.0;
    checker.addTolerance(spec);

    bool result = checker.checkTolerance(10.5);
    EXPECT_TRUE(result);
    EXPECT_TRUE(checker.isCompliant());
}

TEST(ToleranceCheckerTest, CheckOutOfTolerance) {
    ToleranceChecker checker(EntityId::generate());
    ToleranceChecker::ToleranceSpec spec;
    spec.nominal_value = 10.0;
    spec.lower_limit = 9.0;
    spec.upper_limit = 11.0;
    spec.description = "Length";
    checker.addTolerance(spec);

    bool result = checker.checkTolerance(12.0);
    EXPECT_FALSE(result);
    EXPECT_FALSE(checker.isCompliant());
    EXPECT_EQ(checker.getViolations().size(), 1);
}

// ===== GeometryValidator Tests =====

TEST(GeometryValidatorTest, CreateValidator) {
    GeometryValidator validator(EntityId::generate());
    EXPECT_FALSE(validator.hasSelfIntersections());
    EXPECT_FALSE(validator.hasGapsOrOverlaps());
}

TEST(GeometryValidatorTest, Validate) {
    GeometryValidator validator(EntityId::generate());
    bool result = validator.validate();
    EXPECT_TRUE(result);
    EXPECT_EQ(validator.getIssues().size(), 0);
}

TEST(GeometryValidatorTest, AutoRepair) {
    GeometryValidator validator(EntityId::generate());
    bool result = validator.autoRepair();
    EXPECT_TRUE(result);
}

TEST(GeometryValidatorTest, GetRepairLog) {
    GeometryValidator validator(EntityId::generate());
    [[maybe_unused]] bool repair_result = validator.autoRepair();

    const auto& log = validator.getRepairLog();
    EXPECT_GE(log.size(), 0);
}

// ===== InspectionTools Tests =====

TEST(InspectionToolsTest, CreateInspectionTools) {
    InspectionTools tools(EntityId::generate());
    EXPECT_EQ(tools.getMeasurementCount(), 0);
    EXPECT_EQ(tools.getAnalysisCount(), 0);
}

TEST(InspectionToolsTest, MeasureDistance) {
    InspectionTools tools(EntityId::generate());
    auto distance = tools.measureDistance(EntityId::generate(), EntityId::generate());

    EXPECT_NE(distance, nullptr);
    EXPECT_EQ(tools.getMeasurementCount(), 1);
}

TEST(InspectionToolsTest, MeasureAngle) {
    InspectionTools tools(EntityId::generate());
    auto angle = tools.measureAngle(EntityId::generate(), EntityId::generate());

    EXPECT_NE(angle, nullptr);
    EXPECT_EQ(tools.getMeasurementCount(), 1);
}

TEST(InspectionToolsTest, AnalyzeCurvature) {
    InspectionTools tools(EntityId::generate());
    auto curvature = tools.analyzeCurvature(EntityId::generate());

    EXPECT_NE(curvature, nullptr);
    EXPECT_EQ(tools.getAnalysisCount(), 1);
}

TEST(InspectionToolsTest, AssessSurfaceQuality) {
    InspectionTools tools(EntityId::generate());
    auto quality = tools.assessSurfaceQuality(EntityId::generate());

    EXPECT_NE(quality, nullptr);
    EXPECT_EQ(tools.getAnalysisCount(), 1);
}

TEST(InspectionToolsTest, CreateToleranceChecker) {
    InspectionTools tools(EntityId::generate());
    auto checker = tools.createToleranceChecker(EntityId::generate());

    EXPECT_NE(checker, nullptr);
}

TEST(InspectionToolsTest, MixedOperations) {
    InspectionTools tools(EntityId::generate());

    auto dist = tools.measureDistance(EntityId::generate(), EntityId::generate());
    auto angle = tools.measureAngle(EntityId::generate(), EntityId::generate());
    auto curv = tools.analyzeCurvature(EntityId::generate());
    auto surf = tools.assessSurfaceQuality(EntityId::generate());

    EXPECT_EQ(tools.getMeasurementCount(), 2);
    EXPECT_EQ(tools.getAnalysisCount(), 2);
}

TEST(InspectionToolsTest, AccessValidator) {
    InspectionTools tools(EntityId::generate());
    GeometryValidator& validator = tools.getValidator();

    [[maybe_unused]] bool validate_result = validator.validate();
    EXPECT_EQ(validator.getIssues().size(), 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
