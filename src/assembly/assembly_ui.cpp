#include "bendcore/assembly/assembly_ui.hpp"
#include <sstream>

namespace BendCore::CadCore {

// ===== ConstraintTreeItem Implementation =====

std::string ConstraintTreeItem::getDisplayName() const noexcept {
    switch (mate_type_) {
        case MateConstraint::MateType::FIXED:
            return "Fixed";
        case MateConstraint::MateType::COINCIDENT:
            return "Coincident";
        case MateConstraint::MateType::CONCENTRIC:
            return "Concentric";
        case MateConstraint::MateType::PARALLEL:
            return "Parallel";
        case MateConstraint::MateType::PERPENDICULAR:
            return "Perpendicular";
        case MateConstraint::MateType::DISTANCE:
            return "Distance";
        case MateConstraint::MateType::ANGLE:
            return "Angle";
        case MateConstraint::MateType::TANGENT:
            return "Tangent";
        case MateConstraint::MateType::SYMMETRIC:
            return "Symmetric";
        default:
            return "Unknown";
    }
}

ConstraintVisual ConstraintTreeItem::getVisual() const noexcept {
    ConstraintVisual visual;
    visual.type = ConstraintVisual::VisualType::POINT_TO_POINT;
    visual.is_satisfied = is_satisfied_;
    visual.is_redundant = is_redundant_;

    // Map constraint type to visual representation
    switch (mate_type_) {
        case MateConstraint::MateType::FIXED:
        case MateConstraint::MateType::COINCIDENT:
            visual.type = ConstraintVisual::VisualType::POINT_TO_POINT;
            break;
        case MateConstraint::MateType::DISTANCE:
            visual.type = ConstraintVisual::VisualType::DISTANCE_ARROW;
            break;
        case MateConstraint::MateType::ANGLE:
            visual.type = ConstraintVisual::VisualType::ANGLE_ARC;
            break;
        case MateConstraint::MateType::PARALLEL:
        case MateConstraint::MateType::PERPENDICULAR:
            visual.type = ConstraintVisual::VisualType::PLANE_TO_PLANE;
            break;
        case MateConstraint::MateType::TANGENT:
            visual.type = ConstraintVisual::VisualType::TANGENT_SURFACE;
            break;
        case MateConstraint::MateType::CONCENTRIC:
        case MateConstraint::MateType::SYMMETRIC:
            visual.type = ConstraintVisual::VisualType::AXIS_ALIGNED;
            break;
        default:
            break;
    }

    return visual;
}

// ===== AssemblyUICoordinator Implementation =====

void AssemblyUICoordinator::updateVisualization() noexcept {
    // Update visual state for all constraints
    for ([[maybe_unused]] const auto& constraint : explorer_.getConstraints()) {
        // Visual is generated on-demand from constraint state
        // In a real implementation, this would update 3D visualization
    }
}

void AssemblyUICoordinator::checkForConflicts() noexcept {
    conflict_resolver_.clear();

    // Check for over-constraints
    if (solver_.isOverConstrained()) {
        const auto& redundant = solver_.getRedundantConstraints();

        for (const auto& constraint_id : redundant) {
            ConflictResolver::ConflictRecord record;
            record.conflict_id = constraint_id;
            record.severity = ConflictResolver::SeverityLevel::ERROR;
            record.description = "Redundant constraint detected";
            record.involved_constraints.push_back(constraint_id);
            record.suggested_resolution = "Remove or modify this constraint";

            conflict_resolver_.addConflict(record);
        }
    }

    // Check for under-constraints
    if (solver_.isUnderConstrained()) {
        const auto& unsatisfied = solver_.getUnsatisfiedConstraints();

        for (const auto& constraint_id : unsatisfied) {
            ConflictResolver::ConflictRecord record;
            record.conflict_id = constraint_id;
            record.severity = ConflictResolver::SeverityLevel::WARNING;
            record.description = "Constraint not satisfied";
            record.involved_constraints.push_back(constraint_id);
            record.suggested_resolution = "Add additional constraints or adjust values";

            conflict_resolver_.addConflict(record);
        }
    }

    // Check for critical issues (completely unsolvable)
    if (solver_.getConstraints().empty() && explorer_.getComponentCount() > 1) {
        ConflictResolver::ConflictRecord record;
        record.conflict_id = assembly_id_;
        record.severity = ConflictResolver::SeverityLevel::CRITICAL;
        record.description = "Assembly has no constraints - fully unconstrained";
        record.suggested_resolution = "Add at least one fixed component and connect others with constraints";

        conflict_resolver_.addConflict(record);
    }
}

std::string AssemblyUICoordinator::getHealthStatus() const noexcept {
    std::stringstream ss;

    size_t component_count = explorer_.getComponentCount();
    size_t constraint_count = explorer_.getConstraintCount();

    ss << "Assembly: " << component_count << " components, "
       << constraint_count << " constraints\n";

    // Add solver diagnosis
    ss << solver_.diagnose() << "\n";

    // Add conflict summary
    size_t critical = conflict_resolver_.getCriticalCount();
    if (critical > 0) {
        ss << "⚠ CRITICAL ISSUES: " << critical << "\n";
    }

    auto errors = conflict_resolver_.getConflictsBySeverity(
        ConflictResolver::SeverityLevel::ERROR);
    if (!errors.empty()) {
        ss << "⚠ ERRORS: " << errors.size() << "\n";
    }

    auto warnings = conflict_resolver_.getConflictsBySeverity(
        ConflictResolver::SeverityLevel::WARNING);
    if (!warnings.empty()) {
        ss << "ℹ WARNINGS: " << warnings.size() << "\n";
    }

    return ss.str();
}

} // namespace BendCore::CadCore
