#include <gtest/gtest.h>
#include <bendcore/math/matrix_ops.h>
#include <cmath>

using namespace bendcore::math;

class MatrixOpsTest : public ::testing::Test {
protected:
    MatrixOps::Matrix3d identity() {
        return MatrixOps::Matrix3d::Identity();
    }
};

TEST_F(MatrixOpsTest, MatrixMultiplication) {
    MatrixOps::Matrix3d A = identity();
    MatrixOps::Matrix3d B = identity();

    MatrixOps::Matrix3d C = MatrixOps::multiply(A, B);

    EXPECT_TRUE(C.isIdentity(1e-10));
}

TEST_F(MatrixOpsTest, MatrixVectorMultiplication) {
    MatrixOps::Matrix3d M = identity();
    MatrixOps::Vector3d v(1.0, 2.0, 3.0);

    MatrixOps::Vector3d result = MatrixOps::multiply(M, v);

    EXPECT_NEAR(result(0), 1.0, 1e-10);
    EXPECT_NEAR(result(1), 2.0, 1e-10);
    EXPECT_NEAR(result(2), 3.0, 1e-10);
}

TEST_F(MatrixOpsTest, MatrixDeterminant) {
    MatrixOps::Matrix3d M = identity();
    double det = MatrixOps::determinant(M);

    EXPECT_NEAR(det, 1.0, 1e-10);
}

TEST_F(MatrixOpsTest, MatrixTranspose) {
    MatrixOps::Matrix3d M;
    M.m[0][0] = 1; M.m[0][1] = 2; M.m[0][2] = 3;
    M.m[1][0] = 4; M.m[1][1] = 5; M.m[1][2] = 6;
    M.m[2][0] = 7; M.m[2][1] = 8; M.m[2][2] = 9;

    MatrixOps::Matrix3d MT = MatrixOps::transpose(M);

    EXPECT_NEAR(MT.m[0][1], 4.0, 1e-10);
    EXPECT_NEAR(MT.m[1][0], 2.0, 1e-10);
}

TEST_F(MatrixOpsTest, RotationMatrix) {
    MatrixOps::Vector3d axis(0, 0, 1);  // Z axis
    double angle = M_PI / 2;  // 90 degrees

    MatrixOps::Matrix3d R = MatrixOps::rotation_matrix(axis, angle);

    MatrixOps::Vector3d v(1, 0, 0);
    MatrixOps::Vector3d rotated = MatrixOps::multiply(R, v);

    EXPECT_NEAR(rotated(0), 0.0, 1e-10);
    EXPECT_NEAR(rotated(1), 1.0, 1e-10);
    EXPECT_NEAR(rotated(2), 0.0, 1e-10);
}

TEST_F(MatrixOpsTest, AngleBetweenVectors) {
    MatrixOps::Vector3d v1(1, 0, 0);
    MatrixOps::Vector3d v2(0, 1, 0);

    double angle = MatrixOps::angle_between(v1, v2);

    EXPECT_NEAR(angle, M_PI / 2, 1e-10);
}

TEST_F(MatrixOpsTest, VectorNormalization) {
    MatrixOps::Vector3d v(3, 4, 0);
    MatrixOps::Vector3d normalized = MatrixOps::normalize(v);

    EXPECT_NEAR(MatrixOps::magnitude(normalized), 1.0, 1e-10);
}

TEST_F(MatrixOpsTest, CrossProduct) {
    MatrixOps::Vector3d a(1, 0, 0);
    MatrixOps::Vector3d b(0, 1, 0);

    MatrixOps::Vector3d c = MatrixOps::cross(a, b);

    EXPECT_NEAR(c(0), 0.0, 1e-10);
    EXPECT_NEAR(c(1), 0.0, 1e-10);
    EXPECT_NEAR(c(2), 1.0, 1e-10);
}

TEST_F(MatrixOpsTest, DotProduct) {
    MatrixOps::Vector3d a(1, 2, 3);
    MatrixOps::Vector3d b(4, 5, 6);

    double dot = MatrixOps::dot(a, b);

    EXPECT_NEAR(dot, 32.0, 1e-10);  // 1*4 + 2*5 + 3*6 = 32
}

TEST_F(MatrixOpsTest, VectorMagnitude) {
    MatrixOps::Vector3d v(3, 4, 0);
    double mag = MatrixOps::magnitude(v);

    EXPECT_NEAR(mag, 5.0, 1e-10);
}

TEST_F(MatrixOpsTest, MatrixInverse) {
    MatrixOps::Matrix3d M;
    M.m[0][0] = 2; M.m[0][1] = 0; M.m[0][2] = 0;
    M.m[1][0] = 0; M.m[1][1] = 2; M.m[1][2] = 0;
    M.m[2][0] = 0; M.m[2][1] = 0; M.m[2][2] = 2;

    MatrixOps::Matrix3d Minv = MatrixOps::inverse(M);

    MatrixOps::Matrix3d identity_product = MatrixOps::multiply(M, Minv);

    EXPECT_TRUE(identity_product.isIdentity(1e-10));
}
