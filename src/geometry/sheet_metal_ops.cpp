#include "bendcore/geometry/sheet_metal_ops.hpp"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace BendCore::CadCore {

// ===== BendAllowance Implementation =====

double BendAllowance::getBendAllowance() const noexcept {
    // Formula: L = π/2 * (R + K*T) * (A/90)
    // where: R = bend radius, K = K-factor, T = thickness, A = bend angle
    [[maybe_unused]] double angle_rad = bend_angle_ * M_PI / 180.0;
    return (M_PI / 2.0) * (radius_ + k_factor_ * thickness_) * (bend_angle_ / 90.0);
}

// ===== UnfoldSheet Implementation =====

bool UnfoldSheet::unfold() noexcept {
    if (bend_edges_.empty()) return false;

    unfolded_profile_.clear();

    // Simulate unfolding by straightening bend edges
    for (const auto& [edge_id, allowance] : bend_edges_) {
        double allowance_value = allowance->getBendAllowance();
        // In real implementation, use edge geometry to compute unfolded positions
        unfolded_profile_.push_back(Vector3D(allowance_value, 0.0, 0.0));
    }

    return !unfolded_profile_.empty();
}

bool UnfoldSheet::exportToDXF([[maybe_unused]] const std::string& filename) const noexcept {
    if (unfolded_profile_.empty()) return false;

    // In real implementation, write DXF file with unfolded profile
    // For now, just return success if profile exists
    return true;
}

// ===== TubeNotch Implementation =====

bool TubeNotch::generateNotch() noexcept {
    notch_curve_.clear();

    // Generate notch curve based on intersection type
    switch (intersection_type_) {
        case IntersectionType::BUTT:
            // Butt joint: simple perpendicular cut
            notch_curve_.push_back(Vector3D(0.0, 0.0, 0.0));
            notch_curve_.push_back(Vector3D(10.0, 0.0, 0.0));
            break;

        case IntersectionType::FISHMOUTH:
            // Fish mouth: elliptical cut profile
            for (double angle = 0.0; angle <= 2.0 * M_PI; angle += M_PI / 8.0) {
                double x = 20.0 * std::cos(angle);
                double z = 15.0 * std::sin(angle);
                notch_curve_.push_back(Vector3D(x, 0.0, z));
            }
            break;

        case IntersectionType::SADDLE:
            // Saddle: curved transition
            for (int i = 0; i <= 10; ++i) {
                double t = i / 10.0;
                double x = 20.0 * t;
                double z = 10.0 * std::sin(M_PI * t);
                notch_curve_.push_back(Vector3D(x, 0.0, z));
            }
            break;

        default:
            notch_curve_.push_back(Vector3D(0.0, 0.0, 0.0));
    }

    return !notch_curve_.empty();
}

bool TubeNotch::exportForCutting(const std::string& format) const noexcept {
    if (notch_curve_.empty()) return false;

    // In real implementation, export to CNC code or DXF
    if (format == "gcode" || format == "dxf" || format == "step") {
        return true;
    }
    return false;
}

// ===== StampingPattern Implementation =====

void StampingPattern::createLinearPattern(const Vector3D& direction,
                                         double spacing, size_t count) noexcept {
    instances_.clear();
    pattern_type_ = PatternType::LINEAR;

    Vector3D normalized = direction;
    double mag = std::sqrt(normalized.x * normalized.x +
                          normalized.y * normalized.y +
                          normalized.z * normalized.z);
    if (mag > 1e-9) {
        normalized.x /= mag;
        normalized.y /= mag;
        normalized.z /= mag;
    }

    for (size_t i = 0; i < count; ++i) {
        Vector3D instance = Vector3D(
            normalized.x * spacing * i,
            normalized.y * spacing * i,
            normalized.z * spacing * i
        );
        instances_.push_back(instance);
    }
}

void StampingPattern::createCircularPattern([[maybe_unused]] const Vector3D& axis,
                                           double radius, size_t count) noexcept {
    instances_.clear();
    pattern_type_ = PatternType::CIRCULAR;

    double angle_step = 2.0 * M_PI / count;

    for (size_t i = 0; i < count; ++i) {
        double angle = angle_step * i;
        double x = radius * std::cos(angle);
        double y = radius * std::sin(angle);
        instances_.push_back(Vector3D(x, y, 0.0));
    }
}

bool StampingPattern::apply() noexcept {
    return !instances_.empty();
}

// ===== RolledFeature Implementation =====

bool RolledFeature::executeRoll() noexcept {
    if (roll_radius_ <= 0.0) return false;
    if (rotations_ < 0.0) return false;

    // Validate roll operation
    is_valid_ = true;
    return true;
}

// ===== BendTable Implementation =====

double BendTable::lookupBendAllowance(const std::string& material,
                                      [[maybe_unused]] double thickness,
                                      double angle) const noexcept {
    // Find matching material entry
    auto it = std::find_if(materials_.begin(), materials_.end(),
        [&material](const BendEntry& entry) { return entry.material == material; });

    if (it == materials_.end()) return 0.0;

    // Linear interpolation for angle
    double allowance = it->allowance_90 * (angle / 90.0);
    return allowance;
}

double BendTable::getRecommendedRadius(const std::string& material,
                                       double thickness) const noexcept {
    auto it = std::find_if(materials_.begin(), materials_.end(),
        [&material](const BendEntry& entry) { return entry.material == material; });

    if (it == materials_.end()) return thickness;

    // Recommend radius between min and max
    return (it->min_radius + it->max_radius) / 2.0;
}

std::string BendTable::exportToCSV() const noexcept {
    std::stringstream ss;
    ss << "Material,Thickness (mm),Min Radius,Max Radius,90° Allowance\n";

    for (const auto& entry : materials_) {
        ss << entry.material << ","
           << std::fixed << std::setprecision(2) << entry.thickness << ","
           << entry.min_radius << ","
           << entry.max_radius << ","
           << entry.allowance_90 << "\n";
    }

    return ss.str();
}

// ===== SheetMetalOperations Implementation =====

std::shared_ptr<UnfoldSheet> SheetMetalOperations::createUnfold(const EntityId& solid_id) noexcept {
    auto unfold = std::make_shared<UnfoldSheet>(solid_id);
    unfolds_.push_back(unfold);
    return unfold;
}

std::shared_ptr<TubeNotch> SheetMetalOperations::createNotch(const EntityId& main_tube,
                                                            const EntityId& intersecting) noexcept {
    auto notch = std::make_shared<TubeNotch>(main_tube, intersecting);
    notches_.push_back(notch);
    return notch;
}

std::shared_ptr<StampingPattern> SheetMetalOperations::createPattern(const EntityId& feature) noexcept {
    auto pattern = std::make_shared<StampingPattern>(feature);
    patterns_.push_back(pattern);
    return pattern;
}

std::shared_ptr<RolledFeature> SheetMetalOperations::createRolled(const EntityId& profile,
                                                                  double radius) noexcept {
    auto rolled = std::make_shared<RolledFeature>(profile, radius);
    rolled_.push_back(rolled);
    return rolled;
}

} // namespace BendCore::CadCore
