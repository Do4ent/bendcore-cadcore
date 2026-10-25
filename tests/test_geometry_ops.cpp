#include <gtest/gtest.h>
#include <bendcore/math/geometry_ops.h>
#include <cmath>

using namespace bendcore::math;

class GeometryOpsTest : public ::testing::Test {
protected:
    Point3D origin{0, 0, 0};
    Point3D p1{1, 0, 0};
    Point3D p2{0, 1, 0};
    Point3D p3{0, 0, 1};
};

TEST_F(GeometryOpsTest, PointDistance) {
    double dist = origin.distance_to(p1);
    EXPECT_NEAR(dist, 1.0, 1e-10);
}

TEST_F(GeometryOpsTest, PointLength) {
    Point3D p{3, 4, 0};
    double len = p.length();
    EXPECT_NEAR(len, 5.0, 1e-10);
}

TEST_F(GeometryOpsTest, Vector3DLength) {
    Vector3D v{3, 4, 0};
    double len = v.length();
    EXPECT_NEAR(len, 5.0, 1e-10);
}

TEST_F(GeometryOpsTest, VectorNormalized) {
    Vector3D v{3, 4, 0};
    Vector3D normalized = v.normalized();

    EXPECT_NEAR(normalized.x, 0.6, 1e-10);
    EXPECT_NEAR(normalized.y, 0.8, 1e-10);
    EXPECT_NEAR(normalized.z, 0.0, 1e-10);
}

TEST_F(GeometryOpsTest, VectorDot) {
    Vector3D a{1, 2, 3};
    Vector3D b{4, 5, 6};

    double dot = a.dot(b);
    EXPECT_NEAR(dot, 32.0, 1e-10);  // 1*4 + 2*5 + 3*6
}

TEST_F(GeometryOpsTest, VectorCross) {
    Vector3D a{1, 0, 0};
    Vector3D b{0, 1, 0};

    Vector3D c = a.cross(b);

    EXPECT_NEAR(c.x, 0.0, 1e-10);
    EXPECT_NEAR(c.y, 0.0, 1e-10);
    EXPECT_NEAR(c.z, 1.0, 1e-10);
}

TEST_F(GeometryOpsTest, TriangleArea) {
    Point3D a{0, 0, 0};
    Point3D b{1, 0, 0};
    Point3D c{0, 1, 0};

    double area = GeometryOps::triangle_area(a, b, c);

    EXPECT_NEAR(area, 0.5, 1e-10);
}

TEST_F(GeometryOpsTest, PolygonArea) {
    std::vector<Point3D> square{
        {0, 0, 0},
        {1, 0, 0},
        {1, 1, 0},
        {0, 1, 0}
    };

    double area = GeometryOps::polygon_area(square);

    EXPECT_NEAR(area, 1.0, 1e-10);
}

TEST_F(GeometryOpsTest, ConvexHull2D) {
    std::vector<Point3D> points{
        {0, 0, 0},
        {1, 0, 0},
        {1, 1, 0},
        {0, 1, 0},
        {0.5, 0.5, 0}  // Inside point
    };

    std::vector<Point3D> hull = GeometryOps::convex_hull_2d(points);

    EXPECT_EQ(hull.size(), 4);  // Should have 4 points in hull
}

TEST_F(GeometryOpsTest, PointToSegmentDistance) {
    Point3D p{0.5, 1, 0};
    Point3D seg_start{0, 0, 0};
    Point3D seg_end{1, 0, 0};

    double dist = GeometryOps::point_to_segment_distance(p, seg_start, seg_end);

    EXPECT_NEAR(dist, 1.0, 1e-10);
}

TEST_F(GeometryOpsTest, ClosestPointOnSegment) {
    Point3D p{0.5, 1, 0};
    Point3D seg_start{0, 0, 0};
    Point3D seg_end{1, 0, 0};

    Point3D closest = GeometryOps::closest_point_on_segment(p, seg_start, seg_end);

    EXPECT_NEAR(closest.x, 0.5, 1e-10);
    EXPECT_NEAR(closest.y, 0.0, 1e-10);
    EXPECT_NEAR(closest.z, 0.0, 1e-10);
}

TEST_F(GeometryOpsTest, PolygonCentroid) {
    std::vector<Point3D> square{
        {0, 0, 0},
        {1, 0, 0},
        {1, 1, 0},
        {0, 1, 0}
    };

    Point3D centroid = GeometryOps::polygon_centroid(square);

    EXPECT_NEAR(centroid.x, 0.5, 1e-10);
    EXPECT_NEAR(centroid.y, 0.5, 1e-10);
    EXPECT_NEAR(centroid.z, 0.0, 1e-10);
}

TEST_F(GeometryOpsTest, PolygonOrientation) {
    std::vector<Point3D> ccw_square{
        {0, 0, 0},
        {1, 0, 0},
        {1, 1, 0},
        {0, 1, 0}
    };

    bool is_ccw = GeometryOps::is_counterclockwise(ccw_square);

    EXPECT_TRUE(is_ccw);
}

TEST_F(GeometryOpsTest, PolygonTriangulation) {
    std::vector<Point3D> polygon{
        {0, 0, 0},
        {2, 0, 0},
        {2, 2, 0},
        {0, 2, 0}
    };

    auto triangles = GeometryOps::triangulate_polygon(polygon);

    EXPECT_EQ(triangles.size(), 2);  // 4 points = 2 triangles
}

TEST_F(GeometryOpsTest, PointInPolygon2D) {
    std::vector<Point3D> square{
        {0, 0, 0},
        {1, 0, 0},
        {1, 1, 0},
        {0, 1, 0}
    };

    Point3D inside{0.5, 0.5, 0};
    Point3D outside{2, 2, 0};

    EXPECT_TRUE(GeometryOps::point_in_polygon_2d(inside, square));
    EXPECT_FALSE(GeometryOps::point_in_polygon_2d(outside, square));
}
