#include <gtest/gtest.h>
#include "bendcore/geometry/geometry_engine.hpp"
#include <cmath>

using namespace BendCore::CadCore;

TEST(GeometryEngineTest, Vector3DMagnitude) {
    Vector3D v(3.0, 4.0, 0.0);
    EXPECT_DOUBLE_EQ(v.magnitude(), 5.0);
}

TEST(GeometryEngineTest, Vector3DDot) {
    Vector3D a(1.0, 2.0, 3.0);
    Vector3D b(4.0, 5.0, 6.0);
    EXPECT_DOUBLE_EQ(a.dot(b), 32.0);
}

TEST(GeometryEngineTest, Vector3DCross) {
    Vector3D a(1.0, 0.0, 0.0);
    Vector3D b(0.0, 1.0, 0.0);
    Vector3D c = a.cross(b);
    EXPECT_DOUBLE_EQ(c.z, 1.0);
}

TEST(GeometryEngineTest, Matrix4x4Identity) {
    Matrix4x4 m = Matrix4x4::identity();
    Vector3D v(1.0, 2.0, 3.0);
    Vector3D result = m * v;
    EXPECT_TRUE(result == v);
}

TEST(GeometryEngineTest, Matrix4x4Translate) {
    Matrix4x4 m = Matrix4x4::translate(Vector3D(10.0, 20.0, 30.0));
    Vector3D v(1.0, 2.0, 3.0);
    Vector3D result = m * v;
    EXPECT_TRUE(result == Vector3D(11.0, 22.0, 33.0));
}

TEST(GeometryEngineTest, SphereVolume) {
    Sphere s(Vector3D(0, 0, 0), 1.0);
    double vol = s.volume();
    EXPECT_NEAR(vol, 4.0 / 3.0 * M_PI, 1e-6);
}

TEST(GeometryEngineTest, BoxVolume) {
    Box b(Vector3D(0, 0, 0), Vector3D(2.0, 3.0, 4.0));
    EXPECT_DOUBLE_EQ(b.volume(), 24.0);
}

TEST(GeometryEngineTest, PointInBox) {
    EXPECT_TRUE(GeometryEngine::pointInBox(
        Vector3D(1.0, 1.0, 1.0),
        Vector3D(0, 0, 0),
        Vector3D(2.0, 2.0, 2.0)
    ));
}

TEST(GeometryEngineTest, SpheresIntersect) {
    EXPECT_TRUE(GeometryEngine::spheresIntersect(
        Vector3D(0, 0, 0), 1.0,
        Vector3D(1.5, 0, 0), 1.0
    ));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
