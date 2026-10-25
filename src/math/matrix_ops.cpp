#include <bendcore/math/matrix_ops.h>
#include <cmath>
#include <stdexcept>

#ifndef HAVE_EIGEN
namespace bendcore::math {
    double Matrix3d::determinant() const {
        return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
               m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
               m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    }

    Matrix3d Matrix3d::transpose() const {
        Matrix3d T;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                T.m[i][j] = m[j][i];
            }
        }
        return T;
    }

    bool Matrix3d::isIdentity(double eps) const {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                double expected = (i == j) ? 1.0 : 0.0;
                if (std::abs(m[i][j] - expected) > eps) return false;
            }
        }
        return true;
    }

    double Vector3d::norm() const {
        return std::sqrt(m[0]*m[0] + m[1]*m[1] + m[2]*m[2]);
    }

    Vector3d Vector3d::normalized() const {
        double n = norm();
        if (n < 1e-10) return Vector3d(0, 0, 0);
        return Vector3d(m[0]/n, m[1]/n, m[2]/n);
    }

    double Vector3d::dot(const Vector3d& v) const {
        return m[0]*v.m[0] + m[1]*v.m[1] + m[2]*v.m[2];
    }

    Vector3d Vector3d::cross(const Vector3d& v) const {
        return Vector3d(
            m[1]*v.m[2] - m[2]*v.m[1],
            m[2]*v.m[0] - m[0]*v.m[2],
            m[0]*v.m[1] - m[1]*v.m[0]
        );
    }

    Vector3d Vector3d::operator*(double scalar) const {
        return Vector3d(m[0]*scalar, m[1]*scalar, m[2]*scalar);
    }
}
#endif

namespace bendcore::math {

Matrix3d MatrixOps::multiply(const Matrix3d& A, const Matrix3d& B) noexcept {
    #ifdef HAVE_EIGEN
    return A * B;
    #else
    Matrix3d C;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            C.m[i][j] = 0;
            for (int k = 0; k < 3; ++k) {
                C.m[i][j] += A.m[i][k] * B.m[k][j];
            }
        }
    }
    return C;
    #endif
}

Vector3d MatrixOps::multiply(const Matrix3d& M, const Vector3d& v) noexcept {
    #ifdef HAVE_EIGEN
    return M * v;
    #else
    Vector3d result;
    for (int i = 0; i < 3; ++i) {
        result(i) = M.m[i][0]*v(0) + M.m[i][1]*v(1) + M.m[i][2]*v(2);
    }
    return result;
    #endif
}

Vector3d MatrixOps::transform(const Matrix3d& rotation, const Vector3d& point) noexcept {
    return multiply(rotation, point);
}

Matrix3d MatrixOps::inverse(const Matrix3d& M) {
    double det = determinant(M);
    if (std::abs(det) < 1e-10) {
        throw std::runtime_error("Matrix is not invertible");
    }

    #ifdef HAVE_EIGEN
    return M.inverse();
    #else
    Matrix3d Minv;
    Minv.m[0][0] = (M.m[1][1]*M.m[2][2] - M.m[1][2]*M.m[2][1]) / det;
    Minv.m[0][1] = (M.m[0][2]*M.m[2][1] - M.m[0][1]*M.m[2][2]) / det;
    Minv.m[0][2] = (M.m[0][1]*M.m[1][2] - M.m[0][2]*M.m[1][1]) / det;
    Minv.m[1][0] = (M.m[1][2]*M.m[2][0] - M.m[1][0]*M.m[2][2]) / det;
    Minv.m[1][1] = (M.m[0][0]*M.m[2][2] - M.m[0][2]*M.m[2][0]) / det;
    Minv.m[1][2] = (M.m[0][2]*M.m[1][0] - M.m[0][0]*M.m[1][2]) / det;
    Minv.m[2][0] = (M.m[1][0]*M.m[2][1] - M.m[1][1]*M.m[2][0]) / det;
    Minv.m[2][1] = (M.m[0][1]*M.m[2][0] - M.m[0][0]*M.m[2][1]) / det;
    Minv.m[2][2] = (M.m[0][0]*M.m[1][1] - M.m[0][1]*M.m[1][0]) / det;
    return Minv;
    #endif
}

double MatrixOps::determinant(const Matrix3d& M) noexcept {
    #ifdef HAVE_EIGEN
    return M.determinant();
    #else
    return M.determinant();
    #endif
}

Matrix3d MatrixOps::transpose(const Matrix3d& M) noexcept {
    #ifdef HAVE_EIGEN
    return M.transpose();
    #else
    return M.transpose();
    #endif
}

std::pair<VectorXd, MatrixXd> MatrixOps::eigen_decomposition(const Matrix3d&) {
    #ifdef HAVE_EIGEN
    throw std::runtime_error("Eigen decomposition requires Eigen library");
    #else
    throw std::runtime_error("Eigen decomposition requires Eigen library");
    #endif
}

VectorXd MatrixOps::solve_linear_system(const MatrixXd&, const VectorXd&) {
    throw std::runtime_error("Linear system solving requires Eigen library");
}

std::pair<MatrixXd, MatrixXd> MatrixOps::qr_decomposition(const MatrixXd&) {
    throw std::runtime_error("QR decomposition requires Eigen library");
}

Matrix3d MatrixOps::rotation_matrix(const Vector3d& axis, double angle_radians) {
    Vector3d k = axis.normalized();
    double cos_a = std::cos(angle_radians);
    double sin_a = std::sin(angle_radians);

    // Rodrigues formula without Eigen
    Matrix3d K;
    K.m[0][0] = 0; K.m[0][1] = -k(2); K.m[0][2] = k(1);
    K.m[1][0] = k(2); K.m[1][1] = 0; K.m[1][2] = -k(0);
    K.m[2][0] = -k(1); K.m[2][1] = k(0); K.m[2][2] = 0;

    Matrix3d I = Matrix3d::Identity();
    Matrix3d K2 = multiply(K, K);

    Matrix3d R = I;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            R.m[i][j] += sin_a * K.m[i][j] + (1 - cos_a) * K2.m[i][j];
        }
    }
    return R;
}

double MatrixOps::angle_between(const Vector3d& v1, const Vector3d& v2) noexcept {
    double cos_angle = v1.normalized().dot(v2.normalized());
    cos_angle = std::clamp(cos_angle, -1.0, 1.0);
    return std::acos(cos_angle);
}

Vector3d MatrixOps::normalize(const Vector3d& v) {
    double mag = v.norm();
    if (mag < 1e-10) {
        throw std::runtime_error("Cannot normalize zero vector");
    }
    return v.normalized();
}

Vector3d MatrixOps::cross(const Vector3d& a, const Vector3d& b) noexcept {
    return a.cross(b);
}

double MatrixOps::dot(const Vector3d& a, const Vector3d& b) noexcept {
    return a.dot(b);
}

double MatrixOps::magnitude(const Vector3d& v) noexcept {
    return v.norm();
}

VectorXd MatrixOps::least_squares(const MatrixXd&, const VectorXd&) {
    throw std::runtime_error("Least squares requires Eigen library");
}

std::tuple<MatrixXd, VectorXd, MatrixXd> MatrixOps::svd_decomposition(const MatrixXd&) {
    throw std::runtime_error("SVD decomposition requires Eigen library");
}

double MatrixOps::condition_number(const MatrixXd&) {
    return 1.0;  // Fallback
}

} // namespace bendcore::math
