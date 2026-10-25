#include "bendcore/geometry/surface_features.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace BendCore::CadCore {

// ===== SurfaceProfile Implementation =====

Vector3D SurfaceProfile::getBoundingBoxMin() const noexcept {
    if (control_points_.empty()) {
        return Vector3D(0.0, 0.0, 0.0);
    }

    double min_x = control_points_[0].x;
    double min_y = control_points_[0].y;
    double min_z = control_points_[0].z;

    for (const auto& point : control_points_) {
        min_x = std::min(min_x, point.x);
        min_y = std::min(min_y, point.y);
        min_z = std::min(min_z, point.z);
    }

    return Vector3D(min_x, min_y, min_z);
}

Vector3D SurfaceProfile::getBoundingBoxMax() const noexcept {
    if (control_points_.empty()) {
        return Vector3D(0.0, 0.0, 0.0);
    }

    double max_x = control_points_[0].x;
    double max_y = control_points_[0].y;
    double max_z = control_points_[0].z;

    for (const auto& point : control_points_) {
        max_x = std::max(max_x, point.x);
        max_y = std::max(max_y, point.y);
        max_z = std::max(max_z, point.z);
    }

    return Vector3D(max_x, max_y, max_z);
}

double SurfaceProfile::getLength() const noexcept {
    if (control_points_.size() < 2) {
        return 0.0;
    }

    double total_length = 0.0;
    for (size_t i = 0; i < control_points_.size() - 1; ++i) {
        Vector3D delta = control_points_[i + 1] - control_points_[i];
        total_length += delta.magnitude();
    }

    // If closed loop, add segment from last to first
    if (type_ == ProfileType::CLOSED_LOOP) {
        Vector3D delta = control_points_[0] - control_points_.back();
        total_length += delta.magnitude();
    }

    return total_length;
}

bool SurfaceProfile::isValid() const noexcept {
    // Minimum requirements for a valid profile
    switch (type_) {
        case ProfileType::CLOSED_LOOP:
        case ProfileType::EDGE_SEQUENCE:
            return control_points_.size() >= 3;  // At least 3 points for a loop
        case ProfileType::OPEN_CURVE:
            return control_points_.size() >= 2;  // At least 2 points for a curve
        case ProfileType::POINT_CLOUD:
            return control_points_.size() >= 1;  // At least 1 point
        case ProfileType::SKETCH:
            return control_points_.size() >= 3;  // Sketch must have geometry
        default:
            return false;
    }
}

// ===== LoftSurface Implementation =====

bool LoftSurface::validate() const noexcept {
    // Need at least 2 profiles for loft
    if (profiles_.size() < 2) {
        return false;
    }

    // All profiles must be valid
    for (const auto& profile : profiles_) {
        if (!profile || !profile->isValid()) {
            return false;
        }
    }

    // For closed loft, need at least 3 profiles
    if (close_surface_ && profiles_.size() < 3) {
        return false;
    }

    return true;
}

EntityId LoftSurface::generate() const noexcept {
    if (!validate()) {
        return EntityId();  // Empty ID on validation failure
    }

    // Return new EntityId for generated loft surface
    // In real implementation with OCCT:
    //   BRepFill_Loft lofter;
    //   for (auto& profile : profiles_) {
    //       TopoDS_Wire wire = ...;
    //       lofter.AddWire(wire);
    //   }
    //   if (match_tangency_) {
    //       lofter.SetSmoothing(true);
    //   }
    //   lofter.Build();
    //   return shape_to_entity_id(lofter.Shape());

    return EntityId::generate();
}

// ===== SweepSurface Implementation =====

bool SweepSurface::validate() const noexcept {
    // Profile must exist and be valid
    if (!profile_ || !profile_->isValid()) {
        return false;
    }

    // Path must exist and be valid
    if (!path_ || !path_->isValid()) {
        return false;
    }

    // Path should be open curve (not closed loop)
    if (path_->getType() == SurfaceProfile::ProfileType::CLOSED_LOOP) {
        return false;
    }

    // Scale must be positive
    if (scale_profile_ <= 0.0) {
        return false;
    }

    return true;
}

EntityId SweepSurface::generate() const noexcept {
    if (!validate()) {
        return EntityId();  // Empty ID on validation failure
    }

    // Return new EntityId for generated sweep surface
    // In real implementation with OCCT:
    //   BRepFill_Sweep sweep;
    //   sweep.SetMode(path_wire, normal_to_path_);
    //   sweep.SetMode(profile_wire, false);
    //   if (twist_angle_ != 0.0) {
    //       sweep.SetMode(twist_angle_);
    //   }
    //   sweep.Build();
    //   return shape_to_entity_id(sweep.Shape());

    return EntityId::generate();
}

// ===== SurfaceAnalyzer Implementation =====

SurfaceQualityMetrics SurfaceAnalyzer::analyzeSurface([[maybe_unused]] const EntityId& surface_id) noexcept {
    SurfaceQualityMetrics metrics;

    // Placeholder implementation
    // Real implementation would:
    //   1. Extract surface from EntityId
    //   2. Calculate surface area
    //   3. Check curvature continuity (G0, G1, G2)
    //   4. Generate triangulation
    //   5. Analyze triangle quality
    //   6. Check manifold property
    //   7. Check if closed

    metrics.surface_area = 0.0;
    metrics.curvature_continuity = 0.0;
    metrics.triangle_count = 0;
    metrics.degenerate_triangles = 0;
    metrics.min_triangle_quality = 1.0;
    metrics.is_manifold = true;
    metrics.is_closed = false;

    return metrics;
}

bool SurfaceAnalyzer::validateSurface([[maybe_unused]] const EntityId& surface_id) noexcept {
    // Placeholder implementation
    // Real implementation would verify:
    //   1. Surface is valid B-Rep geometry
    //   2. No degenerate faces
    //   3. Proper edge/vertex connectivity
    //   4. No self-intersections
    //   5. Proper topology (manifold)

    return true;  // Assume valid for placeholder
}

bool SurfaceAnalyzer::repairSurface([[maybe_unused]] const EntityId& surface_id) noexcept {
    // Placeholder implementation
    // Real implementation would use OCCT ShapeHealer:
    //   ShapeFix_ShapeTolerance fixer;
    //   fixer.LimitTolerance(shape, tol);
    //   ShapeFix_Wire wire_fixer;
    //   wire_fixer.FixSmall();
    //   wire_fixer.FixIntersectingWires();

    return true;  // Assume successful repair for placeholder
}

} // namespace BendCore::CadCore
