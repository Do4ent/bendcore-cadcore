#include "bendcore/assembly/mate_solver.hpp"
#include <cmath>
#include <algorithm>

namespace BendCore::CadCore {

// MateConstraint implementation
MateConstraint::MateConstraint(const EntityId& id, MateType type)
    : id_(id), type_(type) {}

bool MateConstraint::validate() const noexcept {
    // Both entities must be specified
    if (entity1_id_ == EntityId() || entity2_id_ == EntityId()) {
        return false;
    }

    // Value-based constraints must have a value
    if ((type_ == MateType::DISTANCE || type_ == MateType::ANGLE) &&
        !value_.has_value()) {
        return false;
    }

    return true;
}

// MateSolver implementation

bool MateSolver::removeConstraint(const EntityId& constraint_id) noexcept {
    auto it = std::find_if(constraints_.begin(), constraints_.end(),
        [&constraint_id](const MateConstraint& c) {
            return c.getId() == constraint_id;
        });

    if (it != constraints_.end()) {
        constraints_.erase(it);
        return true;
    }
    return false;
}

bool MateSolver::solve() noexcept {
    // Clear previous results
    unsatisfied_constraints_.clear();
    redundant_constraints_.clear();

    // Initialize positions if empty
    if (component_positions_.empty()) {
        initializePositions();
    }

    // Attempt to solve each constraint
    bool all_satisfied = true;
    for (auto& constraint : constraints_) {
        if (!solveConstraint(constraint)) {
            unsatisfied_constraints_.push_back(constraint.getId());
            all_satisfied = false;
        }
    }

    // Check for redundancy
    checkRedundancy();

    return all_satisfied && redundant_constraints_.empty();
}

Vector3D MateSolver::getComponentPosition(const EntityId& component_id) const noexcept {
    auto it = component_positions_.find(component_id);
    if (it != component_positions_.end()) {
        return it->second;
    }
    return Vector3D(0.0, 0.0, 0.0);
}

Vector3D MateSolver::getComponentOrientation(const EntityId& component_id) const noexcept {
    auto it = component_orientations_.find(component_id);
    if (it != component_orientations_.end()) {
        return it->second;
    }
    return Vector3D(0.0, 0.0, 0.0);  // Identity (no rotation)
}

std::string MateSolver::diagnose() const noexcept {
    std::string result;

    if (unsatisfied_constraints_.empty() && redundant_constraints_.empty()) {
        result = "Assembly is fully constrained. All " +
                std::to_string(constraints_.size()) + " constraints satisfied.";
    } else {
        result = "Assembly constraint issues detected:\n";

        if (!unsatisfied_constraints_.empty()) {
            result += "  Under-constrained: " +
                     std::to_string(unsatisfied_constraints_.size()) +
                     " constraint(s) not satisfied\n";
        }

        if (!redundant_constraints_.empty()) {
            result += "  Over-constrained: " +
                     std::to_string(redundant_constraints_.size()) +
                     " redundant constraint(s)\n";
        }
    }

    return result;
}

bool MateSolver::solveConstraint(MateConstraint& constraint) noexcept {
    if (!constraint.validate()) {
        return false;
    }

    const auto& entity1 = constraint.getEntity1();
    const auto& entity2 = constraint.getEntity2();

    switch (constraint.getType()) {
        case MateConstraint::MateType::FIXED: {
            // Fixed: component stays at origin
            component_positions_[entity1] = Vector3D(0.0, 0.0, 0.0);
            component_orientations_[entity1] = Vector3D(0.0, 0.0, 0.0);
            constraint.setSatisfied(true);
            return true;
        }

        case MateConstraint::MateType::COINCIDENT: {
            // Coincident: position of entity2 matches entity1
            auto pos1 = getComponentPosition(entity1);
            component_positions_[entity2] = pos1;
            constraint.setSatisfied(true);
            return true;
        }

        case MateConstraint::MateType::CONCENTRIC: {
            // Concentric: axes aligned (same position, may differ orientation)
            auto pos1 = getComponentPosition(entity1);
            component_positions_[entity2] = pos1;
            constraint.setSatisfied(true);
            return true;
        }

        case MateConstraint::MateType::PARALLEL: {
            // Parallel: orientations aligned
            auto orient1 = getComponentOrientation(entity1);
            component_orientations_[entity2] = orient1;
            constraint.setSatisfied(true);
            return true;
        }

        case MateConstraint::MateType::PERPENDICULAR: {
            // Perpendicular: dot product = 0
            auto orient1 = getComponentOrientation(entity1);
            // Rotate by 90 degrees (add pi/2 to one axis)
            Vector3D perp(orient1.x + M_PI / 2.0, orient1.y, orient1.z);
            component_orientations_[entity2] = perp;
            constraint.setSatisfied(true);
            return true;
        }

        case MateConstraint::MateType::DISTANCE: {
            // Distance: maintain distance between entities
            if (!constraint.getValue().has_value()) {
                return false;
            }
            auto distance = constraint.getValue().value();
            auto pos1 = getComponentPosition(entity1);

            // Position entity2 at specified distance along X axis
            Vector3D pos2(pos1.x + distance, pos1.y, pos1.z);
            component_positions_[entity2] = pos2;
            constraint.setSatisfied(true);
            return true;
        }

        case MateConstraint::MateType::ANGLE: {
            // Angle: maintain angle between entities
            if (!constraint.getValue().has_value()) {
                return false;
            }
            auto angle = constraint.getValue().value();
            auto orient1 = getComponentOrientation(entity1);

            // Add angle to Z rotation
            Vector3D orient2(orient1.x, orient1.y, orient1.z + angle);
            component_orientations_[entity2] = orient2;
            constraint.setSatisfied(true);
            return true;
        }

        case MateConstraint::MateType::TANGENT: {
            // Tangent: surfaces touch
            auto pos1 = getComponentPosition(entity1);
            component_positions_[entity2] = pos1;
            constraint.setSatisfied(true);
            return true;
        }

        case MateConstraint::MateType::SYMMETRIC: {
            // Symmetric: mirrored across plane
            auto pos1 = getComponentPosition(entity1);
            Vector3D pos2(-pos1.x, pos1.y, pos1.z);  // Mirror across YZ plane
            component_positions_[entity2] = pos2;
            constraint.setSatisfied(true);
            return true;
        }

        default:
            return false;
    }
}

void MateSolver::checkRedundancy() noexcept {
    // Simplified redundancy check: if more than N constraints for M components,
    // likely over-constrained (a 3D rigid body has 6 DOF: 3 position + 3 rotation)

    // Count unique components
    std::vector<EntityId> components;
    for (const auto& constraint : constraints_) {
        if (std::find(components.begin(), components.end(), constraint.getEntity1())
            == components.end()) {
            components.push_back(constraint.getEntity1());
        }
        if (std::find(components.begin(), components.end(), constraint.getEntity2())
            == components.end()) {
            components.push_back(constraint.getEntity2());
        }
    }

    // DOF = components.size() * 6 (position + rotation for each)
    // Over-constrained if constraints > DOF
    size_t dof = components.size() * 6;
    if (constraints_.size() > dof) {
        // Likely over-constrained (simplified heuristic)
        redundant_constraints_.push_back(constraints_.back().getId());
    }
}

void MateSolver::initializePositions() noexcept {
    // Initialize all components at origin with no rotation
    for (const auto& constraint : constraints_) {
        if (component_positions_.find(constraint.getEntity1()) == component_positions_.end()) {
            component_positions_[constraint.getEntity1()] = Vector3D(0.0, 0.0, 0.0);
            component_orientations_[constraint.getEntity1()] = Vector3D(0.0, 0.0, 0.0);
        }
        if (component_positions_.find(constraint.getEntity2()) == component_positions_.end()) {
            component_positions_[constraint.getEntity2()] = Vector3D(0.0, 0.0, 0.0);
            component_orientations_[constraint.getEntity2()] = Vector3D(0.0, 0.0, 0.0);
        }
    }
}

} // namespace BendCore::CadCore
