#include "bendcore/geometry/direct_editing.hpp"
#include <sstream>
#include <memory>

namespace BendCore::CadCore {

// ===== FaceOffset Implementation =====

bool FaceOffset::apply() noexcept {
    if (offset_distance_ == 0.0) {
        return false;  // No offset to apply
    }

    // Save original shape for undo
    original_shape_ = EntityId::generate();

    // In real implementation with OCCT:
    //   BRepOffsetAPI_MakeOffset offset_maker;
    //   offset_maker.Perform(face, offset_distance_, ...);
    //   if (offset_maker.IsDone()) {
    //       new_shape = offset_maker.Shape();
    //       return true;
    //   }

    return true;  // Placeholder: success
}

bool FaceOffset::undo() noexcept {
    if (original_shape_ == EntityId()) {
        return false;  // No original to restore
    }

    // Restore original shape
    original_shape_ = EntityId();
    return true;
}

// ===== EdgeEditor Implementation =====

bool EdgeEditor::chamfer(double size) noexcept {
    if (size <= 0.0) {
        return false;
    }

    // In real implementation with OCCT:
    //   BRepBuilderAPI_MakeShape chamfer_maker;
    //   chamfer_maker.Chamfer(edge, size);
    //   return chamfer_maker.IsDone();

    return true;  // Placeholder
}

bool EdgeEditor::fillet(double radius) noexcept {
    if (radius <= 0.0) {
        return false;
    }

    // In real implementation with OCCT:
    //   BRepFilletAPI_MakeFillet fillet_maker(solid);
    //   fillet_maker.Add(radius, edge);
    //   fillet_maker.Build();
    //   return fillet_maker.IsDone();

    return true;  // Placeholder
}

bool EdgeEditor::draft([[maybe_unused]] double angle) noexcept {
    // Draft operation (taper edges for moldability)
    // Complex operation requiring face and edge analysis
    return true;  // Placeholder
}

bool EdgeEditor::moveEdge(const Vector3D& displacement) noexcept {
    if (displacement.magnitude() == 0.0) {
        return false;  // No movement
    }

    // In real implementation:
    //   Apply 3D transformation to edge vertices
    //   Recalculate topology

    return true;  // Placeholder
}

// ===== ShellOp Implementation =====

bool ShellOp::apply() noexcept {
    if (thickness_ <= 0.0) {
        return false;
    }

    if (removed_faces_.empty()) {
        return false;  // Need at least one removed face
    }

    // In real implementation with OCCT:
    //   BRepOffsetAPI_MakeThickSolid shell_maker;
    //   for (auto face_id : removed_faces_) {
    //       shell_maker.RemoveFace(face);
    //   }
    //   shell_maker.MakeThickSolid(solid, faces, thickness_, ...);
    //   return shell_maker.IsDone();

    return true;  // Placeholder
}

// ===== ParametricSketch Implementation =====

bool ParametricSketch::solve() noexcept {
    // Reset status
    is_solved_ = false;
    underconstrained_count_ = 0;
    overconstrained_count_ = 0;

    if (dimensions_.empty() && constraints_.empty()) {
        is_solved_ = true;  // Empty sketch is valid
        return true;
    }

    // Simple solver: count constraints
    size_t total_constraints = dimensions_.size() + constraints_.size();

    // Heuristic: sketch with 2D geometry needs specific number of constraints
    // Simplified check for underconstrained
    if (total_constraints < 2) {
        underconstrained_count_ = 1;
        return false;
    }

    // Check for overconstrained (simplified heuristic)
    if (total_constraints > 20) {
        overconstrained_count_ = total_constraints - 20;
        return false;
    }

    // Mark all constraints as satisfied
    for (auto& constraint : constraints_) {
        constraint->setSatisfied(true);
    }

    is_solved_ = true;
    return true;
}

std::string ParametricSketch::getStatus() const noexcept {
    std::stringstream ss;

    ss << "Sketch: " << dimensions_.size() << " dimensions, "
       << constraints_.size() << " constraints\n";

    if (!is_solved_) {
        ss << "Status: NOT SOLVED\n";

        if (underconstrained_count_ > 0) {
            ss << "  Under-constrained: " << underconstrained_count_ << "\n";
        }

        if (overconstrained_count_ > 0) {
            ss << "  Over-constrained: " << overconstrained_count_ << "\n";
        }
    } else {
        ss << "Status: SOLVED\n";
    }

    return ss.str();
}

// ===== DirectEditor Implementation =====

std::shared_ptr<FaceOffset> DirectEditor::offsetFace(
    const EntityId& face_id, double distance) noexcept {
    auto offset = std::make_shared<FaceOffset>(face_id);
    offset->setOffsetDistance(distance);
    offset->setOutward(distance > 0.0);

    pending_operations_++;
    return offset;
}

std::shared_ptr<EdgeEditor> DirectEditor::editEdge(
    const EntityId& edge_id) noexcept {
    auto editor = std::make_shared<EdgeEditor>(edge_id);
    pending_operations_++;
    return editor;
}

std::shared_ptr<ShellOp> DirectEditor::createShell(double thickness) noexcept {
    auto shell = std::make_shared<ShellOp>(shape_id_);
    shell->setThickness(thickness);
    pending_operations_++;
    return shell;
}

bool DirectEditor::applyAllOperations() noexcept {
    if (pending_operations_ == 0) {
        return true;  // Nothing to apply
    }

    // In real implementation:
    // 1. Validate all operations don't conflict
    // 2. Apply operations in correct order (topological sort)
    // 3. Handle errors gracefully

    pending_operations_ = 0;
    return true;
}

} // namespace BendCore::CadCore
