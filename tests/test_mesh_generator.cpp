#include <gtest/gtest.h>
#include <bendcore/geometry/mesh_generator.h>
#include <cmath>

using namespace bendcore;

TEST(MeshGeneratorTest, BoxMesh) {
    auto mesh = MeshGenerator::generateFromBox(2.0f, 2.0f, 2.0f);

    EXPECT_GT(mesh->getVertexCount(), 0);
    EXPECT_GT(mesh->getTriangleCount(), 0);
    EXPECT_EQ(mesh->getTriangleCount(), 12);  // 2 triangles per face
}

TEST(MeshGeneratorTest, BoxMeshOptimization) {
    auto mesh = MeshGenerator::generateFromBox(1.0f, 1.0f, 1.0f);

    size_t originalCount = mesh->getVertexCount();
    mesh->optimize();
    size_t optimizedCount = mesh->getVertexCount();

    EXPECT_LE(optimizedCount, originalCount);
}

TEST(MeshGeneratorTest, BoxMeshNormals) {
    auto mesh = MeshGenerator::generateFromBox(1.0f, 1.0f, 1.0f);
    mesh->computeNormals();

    const auto& vertices = mesh->getVertices();
    for (const auto& v : vertices) {
        float len = std::sqrt(v.nx * v.nx + v.ny * v.ny + v.nz * v.nz);
        EXPECT_GT(len, 0.0f);
    }
}

TEST(MeshGeneratorTest, SphereMesh) {
    auto mesh = MeshGenerator::generateFromSphere(1.0f);

    EXPECT_GT(mesh->getVertexCount(), 0);
    EXPECT_GT(mesh->getTriangleCount(), 0);
}

TEST(MeshGeneratorTest, CylinderMesh) {
    auto mesh = MeshGenerator::generateFromCylinder(1.0f, 2.0f);

    EXPECT_GT(mesh->getVertexCount(), 0);
    EXPECT_GT(mesh->getTriangleCount(), 0);
}

TEST(MeshGeneratorTest, ConeMesh) {
    auto mesh = MeshGenerator::generateFromCone(1.0f, 2.0f);

    EXPECT_NE(mesh, nullptr);
}

TEST(MeshGeneratorTest, TorusMesh) {
    auto mesh = MeshGenerator::generateFromTorus(2.0f, 0.5f);

    EXPECT_NE(mesh, nullptr);
}

TEST(MeshGeneratorTest, QualitySettings) {
    auto lowMesh = MeshGenerator::generateFromSphere(1.0f, MeshGenerator::Quality::Low);
    auto highMesh = MeshGenerator::generateFromSphere(1.0f, MeshGenerator::Quality::High);

    EXPECT_LT(lowMesh->getTriangleCount(), highMesh->getTriangleCount());
}

TEST(MeshTest, Mesh) {
    Mesh mesh;

    Vertex v1 = {0, 0, 0, 0, 1, 0, 0, 0};
    Vertex v2 = {1, 0, 0, 0, 1, 0, 1, 0};
    Vertex v3 = {0, 1, 0, 0, 1, 0, 0, 1};

    mesh.addVertex(v1);
    mesh.addVertex(v2);
    mesh.addVertex(v3);

    Triangle tri = {0, 1, 2};
    mesh.addTriangle(tri);

    EXPECT_EQ(mesh.getVertexCount(), 3);
    EXPECT_EQ(mesh.getTriangleCount(), 1);
}
