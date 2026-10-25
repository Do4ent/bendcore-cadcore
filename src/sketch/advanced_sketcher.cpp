#include "bendcore/sketch/advanced_sketcher.hpp"
#include <cmath>
#include <algorithm>

namespace BendCore::CadCore {

// ===== SmartDimensioning Implementation =====

Vector3D SmartDimensioning::suggestDimensionPosition([[maybe_unused]] const EntityId& entity_id,
                                                     const Vector3D& entity_center) noexcept {
    // Suggest position offset from entity center
    // Default: offset to the right and above
    Vector3D suggested = entity_center;
    suggested.x += 10.0;  // 10mm to the right
    suggested.y += 10.0;  // 10mm above

    return suggested;
}

std::vector<std::pair<EntityId, Vector3D>> SmartDimensioning::autoDimensionSketch(
    const std::vector<EntityId>& entities) noexcept {

    std::vector<std::pair<EntityId, Vector3D>> suggestions;

    double offset_distance = 15.0;  // Initial offset

    for (size_t i = 0; i < entities.size(); ++i) {
        // Rotate position around each entity
        double angle = (2.0 * M_PI * i) / entities.size();
        Vector3D pos(
            offset_distance * std::cos(angle),
            offset_distance * std::sin(angle),
            0.0
        );

        suggestions.push_back({entities[i], pos});
    }

    return suggestions;
}

bool SmartDimensioning::placeDimensionSmart(const EntityId& dimension_id,
                                           const Vector3D& suggested_pos) noexcept {
    // Check for collisions with existing dimensions
    for (const auto& [existing_id, existing_pos] : dimension_positions_) {
        double distance = std::sqrt(
            (suggested_pos.x - existing_pos.x) * (suggested_pos.x - existing_pos.x) +
            (suggested_pos.y - existing_pos.y) * (suggested_pos.y - existing_pos.y)
        );

        if (distance < min_spacing_) {
            // Collision detected, adjust position
            Vector3D adjusted = suggested_pos;
            adjusted.y += min_spacing_;  // Move up
            dimension_positions_[dimension_id] = adjusted;
            return true;
        }
    }

    // No collision, place at suggested position
    dimension_positions_[dimension_id] = suggested_pos;
    return true;
}

// ===== CurveTools Implementation =====

bool CurveTools::createBezierCurve(const std::vector<Vector3D>& control_points,
                                   int degree) noexcept {
    if (control_points.size() < 2) return false;

    curve_type_ = CurveType::BEZIER;
    control_points_ = control_points;
    degree_ = degree;

    return true;
}

bool CurveTools::createBSpline(const std::vector<Vector3D>& control_points,
                              const std::vector<double>& knots,
                              int degree) noexcept {
    if (control_points.size() < static_cast<size_t>(degree + 1)) return false;
    if (knots.size() != control_points.size() + static_cast<size_t>(degree + 1)) return false;

    curve_type_ = CurveType::BSPLINE;
    control_points_ = control_points;
    knots_ = knots;
    degree_ = degree;

    return true;
}

bool CurveTools::createInterpolatedCurve(const std::vector<Vector3D>& points) noexcept {
    if (points.size() < 2) return false;

    curve_type_ = CurveType::INTERPOLATED;
    control_points_ = points;

    return true;
}

Vector3D CurveTools::evaluateCurve(double parameter) const noexcept {
    if (control_points_.empty()) return Vector3D(0, 0, 0);

    // Linear interpolation between control points based on parameter
    size_t segment = static_cast<size_t>(parameter * (control_points_.size() - 1));
    if (segment >= control_points_.size() - 1) {
        return control_points_.back();
    }

    double local_t = parameter * (control_points_.size() - 1) - segment;
    const Vector3D& p0 = control_points_[segment];
    const Vector3D& p1 = control_points_[segment + 1];

    return Vector3D(
        p0.x + (p1.x - p0.x) * local_t,
        p0.y + (p1.y - p0.y) * local_t,
        p0.z + (p1.z - p0.z) * local_t
    );
}

Vector3D CurveTools::getCurveTangent(double parameter) const noexcept {
    if (control_points_.size() < 2) return Vector3D(0, 0, 0);

    // Approximate tangent using finite difference
    double dt = 0.01;
    Vector3D p1 = evaluateCurve(std::max(0.0, parameter - dt));
    Vector3D p2 = evaluateCurve(std::min(1.0, parameter + dt));

    Vector3D tangent(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);

    // Normalize
    double mag = std::sqrt(tangent.x*tangent.x + tangent.y*tangent.y + tangent.z*tangent.z);
    if (mag > 1e-9) {
        tangent.x /= mag;
        tangent.y /= mag;
        tangent.z /= mag;
    }

    return tangent;
}

double CurveTools::getCurveCurvature(double parameter) const noexcept {
    if (control_points_.size() < 3) return 0.0;

    // Approximate curvature using second derivative
    double dt = 0.01;
    Vector3D p1 = evaluateCurve(std::max(0.0, parameter - dt));
    Vector3D p2 = evaluateCurve(parameter);
    Vector3D p3 = evaluateCurve(std::min(1.0, parameter + dt));

    Vector3D t1(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
    Vector3D t2(p3.x - p2.x, p3.y - p2.y, p3.z - p2.z);

    double len1 = std::sqrt(t1.x*t1.x + t1.y*t1.y + t1.z*t1.z);
    double len2 = std::sqrt(t2.x*t2.x + t2.y*t2.y + t2.z*t2.z);

    if (len1 < 1e-9 || len2 < 1e-9) return 0.0;

    // Approximate curvature from tangent change
    double delta_len = (len2 - len1) / (2 * dt);
    double curvature = std::abs(delta_len) / (len1 + len2);

    return curvature;
}

// ===== AdvancedSketchSolver Implementation =====

bool AdvancedSketchSolver::solve() noexcept {
    // Simulate solver: assume sketch has some degree of freedom
    dof_ = 3;  // Default: 3 DOF (X, Y, rotation)
    status_ = SolverStatus::UNDER_CONSTRAINED;

    // In real implementation, solve constraint system
    // For now, return success and set status
    return true;
}

std::vector<std::string> AdvancedSketchSolver::suggestConstraints() const noexcept {
    std::vector<std::string> suggestions;

    if (status_ == SolverStatus::UNDER_CONSTRAINED) {
        if (dof_ >= 3) suggestions.push_back("Fix point position (X, Y)");
        if (dof_ >= 2) suggestions.push_back("Fix one axis");
        if (dof_ >= 1) suggestions.push_back("Add parallel or collinear constraint");
    }

    return suggestions;
}

// ===== AdvancedSketcher Implementation =====

std::shared_ptr<ConstructionGeometry> AdvancedSketcher::addConstructionGeometry(
    ConstructionGeometry::GeometryType type) noexcept {

    auto geom = std::make_shared<ConstructionGeometry>(EntityId::generate(), type);
    construction_geometry_.push_back(geom);
    return geom;
}

std::shared_ptr<GeometricRelation> AdvancedSketcher::addRelation(
    GeometricRelation::RelationType type) noexcept {

    auto relation = std::make_shared<GeometricRelation>(EntityId::generate(), type);
    relations_.push_back(relation);
    return relation;
}

} // namespace BendCore::CadCore
