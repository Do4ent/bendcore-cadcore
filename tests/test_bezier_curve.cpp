#include <gtest/gtest.h>
#include <bendcore/geometry/bezier_curve.h>
#include <cmath>

using namespace bendcore;

TEST(BezierCurveTest, LinearBezier) {
    std::vector<Vector3D> controlPoints = {
        Vector3D(0, 0, 0),
        Vector3D(1, 0, 0)
    };
    BezierCurve curve(controlPoints);

    Vector3D start = curve.evaluate(0.0);
    Vector3D mid = curve.evaluate(0.5);
    Vector3D end = curve.evaluate(1.0);

    EXPECT_NEAR(start.x, 0.0, 1e-6);
    EXPECT_NEAR(mid.x, 0.5, 1e-6);
    EXPECT_NEAR(end.x, 1.0, 1e-6);
}

TEST(BezierCurveTest, QuadraticBezier) {
    std::vector<Vector3D> controlPoints = {
        Vector3D(0, 0, 0),
        Vector3D(0.5, 1, 0),
        Vector3D(1, 0, 0)
    };
    BezierCurve curve(controlPoints);

    Vector3D start = curve.evaluate(0.0);
    Vector3D mid = curve.evaluate(0.5);
    Vector3D end = curve.evaluate(1.0);

    EXPECT_NEAR(start.x, 0.0, 1e-6);
    EXPECT_NEAR(end.x, 1.0, 1e-6);
    EXPECT_GT(mid.y, 0.0);  // Should be above due to control point
}

TEST(BezierCurveTest, Discretization) {
    std::vector<Vector3D> controlPoints = {
        Vector3D(0, 0, 0),
        Vector3D(1, 0, 0)
    };
    BezierCurve curve(controlPoints);

    auto points = curve.discretize(10);
    EXPECT_EQ(points.size(), 11);  // 10 segments = 11 points
}

TEST(BezierCurveTest, CurveLength) {
    std::vector<Vector3D> controlPoints = {
        Vector3D(0, 0, 0),
        Vector3D(1, 0, 0)
    };
    BezierCurve curve(controlPoints);

    double length = curve.length();
    EXPECT_NEAR(length, 1.0, 0.01);
}

TEST(NURBSCurveTest, BasicEvaluation) {
    std::vector<Vector3D> controlPoints = {
        Vector3D(0, 0, 0),
        Vector3D(1, 1, 0),
        Vector3D(2, 0, 0)
    };
    std::vector<double> weights = {1.0, 1.0, 1.0};
    std::vector<double> knots = {0, 0, 0, 1, 1, 1};

    NURBSCurve curve(controlPoints, weights, knots, 2);

    Vector3D start = curve.evaluate(0.0);
    Vector3D end = curve.evaluate(1.0);

    EXPECT_NEAR(start.x, 0.0, 1e-6);
    EXPECT_NEAR(end.x, 2.0, 1e-6);
}

TEST(NURBSCurveTest, Discretization) {
    std::vector<Vector3D> controlPoints = {
        Vector3D(0, 0, 0),
        Vector3D(1, 1, 0),
        Vector3D(2, 0, 0)
    };
    std::vector<double> weights = {1.0, 1.0, 1.0};
    std::vector<double> knots = {0, 0, 0, 1, 1, 1};

    NURBSCurve curve(controlPoints, weights, knots, 2);
    auto points = curve.discretize(10);

    EXPECT_EQ(points.size(), 11);
}
