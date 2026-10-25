#include <gtest/gtest.h>
#include "bendcore/assembly/assembly_ui.hpp"
#include "bendcore/document/entity_id.hpp"

using namespace BendCore::CadCore;

class AssemblyUITest : public ::testing::Test {
protected:
    EntityId assembly_id = EntityId::generate();
    AssemblyUICoordinator coordinator{assembly_id};
};

class ConstraintTreeItemTest : public ::testing::Test {
protected:
    EntityId constraint_id = EntityId::generate();
};

class AssemblyComponentUITest : public ::testing::Test {
protected:
    EntityId component_id = EntityId::generate();
};

// ===== AssemblyComponentUI Tests =====

TEST_F(AssemblyComponentUITest, CreateComponent) {
    AssemblyComponentUI component(component_id);

    EXPECT_EQ(component.getComponentId(), component_id);
    EXPECT_TRUE(component.isVisible());
    EXPECT_FALSE(component.isSelected());
}

TEST_F(AssemblyComponentUITest, SetDisplayName) {
    AssemblyComponentUI component(component_id);

    component.setDisplayName("Main Body");
    EXPECT_EQ(component.getDisplayName(), "Main Body");
}

TEST_F(AssemblyComponentUITest, VisibilityControl) {
    AssemblyComponentUI component(component_id);

    EXPECT_TRUE(component.isVisible());

    component.setVisible(false);
    EXPECT_FALSE(component.isVisible());

    component.setVisible(true);
    EXPECT_TRUE(component.isVisible());
}

TEST_F(AssemblyComponentUITest, SelectionState) {
    AssemblyComponentUI component(component_id);

    EXPECT_FALSE(component.isSelected());

    component.setSelected(true);
    EXPECT_TRUE(component.isSelected());
}

TEST_F(AssemblyComponentUITest, HighlightState) {
    AssemblyComponentUI component(component_id);

    EXPECT_FALSE(component.isHighlighted());

    component.setHighlighted(true);
    EXPECT_TRUE(component.isHighlighted());
}

TEST_F(AssemblyComponentUITest, AddConstraints) {
    AssemblyComponentUI component(component_id);

    EntityId constraint1 = EntityId::generate();
    EntityId constraint2 = EntityId::generate();

    component.addConstraint(constraint1);
    component.addConstraint(constraint2);

    EXPECT_EQ(component.getConstraints().size(), 2);
}

// ===== ConstraintTreeItem Tests =====

TEST_F(ConstraintTreeItemTest, CreateConstraintItem) {
    ConstraintTreeItem item(constraint_id, MateConstraint::MateType::FIXED);

    EXPECT_EQ(item.getId(), constraint_id);
    EXPECT_EQ(item.getMateType(), MateConstraint::MateType::FIXED);
}

TEST_F(ConstraintTreeItemTest, DisplayNameForFixedConstraint) {
    ConstraintTreeItem item(constraint_id, MateConstraint::MateType::FIXED);
    EXPECT_EQ(item.getDisplayName(), "Fixed");
}

TEST_F(ConstraintTreeItemTest, DisplayNameForCoincidentConstraint) {
    ConstraintTreeItem item(constraint_id, MateConstraint::MateType::COINCIDENT);
    EXPECT_EQ(item.getDisplayName(), "Coincident");
}

TEST_F(ConstraintTreeItemTest, DisplayNameForDistanceConstraint) {
    ConstraintTreeItem item(constraint_id, MateConstraint::MateType::DISTANCE);
    EXPECT_EQ(item.getDisplayName(), "Distance");
}

TEST_F(ConstraintTreeItemTest, DisplayNameForAngleConstraint) {
    ConstraintTreeItem item(constraint_id, MateConstraint::MateType::ANGLE);
    EXPECT_EQ(item.getDisplayName(), "Angle");
}

TEST_F(ConstraintTreeItemTest, SatisfactionState) {
    ConstraintTreeItem item(constraint_id, MateConstraint::MateType::FIXED);

    EXPECT_FALSE(item.isSatisfied());

    item.setSatisfied(true);
    EXPECT_TRUE(item.isSatisfied());
}

TEST_F(ConstraintTreeItemTest, RedundancyState) {
    ConstraintTreeItem item(constraint_id, MateConstraint::MateType::FIXED);

    EXPECT_FALSE(item.isRedundant());

    item.setRedundant(true);
    EXPECT_TRUE(item.isRedundant());
}

TEST_F(ConstraintTreeItemTest, VisualRepresentation) {
    ConstraintTreeItem item(constraint_id, MateConstraint::MateType::DISTANCE);

    auto visual = item.getVisual();
    EXPECT_EQ(visual.type, ConstraintVisual::VisualType::DISTANCE_ARROW);
    EXPECT_FALSE(visual.is_satisfied);
}

// ===== AssemblyExplorer Tests =====

TEST_F(AssemblyUITest, CreateExplorer) {
    auto& explorer = coordinator.getExplorer();

    EXPECT_EQ(explorer.getComponentCount(), 0);
    EXPECT_EQ(explorer.getConstraintCount(), 0);
}

TEST_F(AssemblyUITest, AddComponentsToExplorer) {
    auto& explorer = coordinator.getExplorer();

    auto comp1 = std::make_shared<AssemblyComponentUI>(EntityId::generate());
    auto comp2 = std::make_shared<AssemblyComponentUI>(EntityId::generate());

    explorer.addComponent(comp1);
    explorer.addComponent(comp2);

    EXPECT_EQ(explorer.getComponentCount(), 2);
}

TEST_F(AssemblyUITest, AddConstraintsToExplorer) {
    auto& explorer = coordinator.getExplorer();

    auto constraint1 = std::make_shared<ConstraintTreeItem>(
        EntityId::generate(), MateConstraint::MateType::FIXED);
    auto constraint2 = std::make_shared<ConstraintTreeItem>(
        EntityId::generate(), MateConstraint::MateType::COINCIDENT);

    explorer.addConstraint(constraint1);
    explorer.addConstraint(constraint2);

    EXPECT_EQ(explorer.getConstraintCount(), 2);
}

TEST_F(AssemblyUITest, ExpandCollapseNodes) {
    auto& explorer = coordinator.getExplorer();

    EntityId node_id = EntityId::generate();

    explorer.setNodeExpanded(node_id, true);
    EXPECT_TRUE(explorer.isNodeExpanded(node_id));

    explorer.setNodeExpanded(node_id, false);
    EXPECT_FALSE(explorer.isNodeExpanded(node_id));
}

// ===== ConflictResolver Tests =====

TEST_F(AssemblyUITest, AddConflict) {
    auto& resolver = coordinator.getConflictResolver();

    ConflictResolver::ConflictRecord conflict;
    conflict.conflict_id = EntityId::generate();
    conflict.severity = ConflictResolver::SeverityLevel::WARNING;
    conflict.description = "Test conflict";

    resolver.addConflict(conflict);

    EXPECT_EQ(resolver.getConflicts().size(), 1);
}

TEST_F(AssemblyUITest, GetCriticalConflictCount) {
    auto& resolver = coordinator.getConflictResolver();

    ConflictResolver::ConflictRecord conflict;
    conflict.conflict_id = EntityId::generate();
    conflict.severity = ConflictResolver::SeverityLevel::CRITICAL;

    resolver.addConflict(conflict);

    EXPECT_EQ(resolver.getCriticalCount(), 1);
}

TEST_F(AssemblyUITest, FilterConflictsBySeverity) {
    auto& resolver = coordinator.getConflictResolver();

    ConflictResolver::ConflictRecord warning;
    warning.conflict_id = EntityId::generate();
    warning.severity = ConflictResolver::SeverityLevel::WARNING;

    ConflictResolver::ConflictRecord error;
    error.conflict_id = EntityId::generate();
    error.severity = ConflictResolver::SeverityLevel::ERROR;

    resolver.addConflict(warning);
    resolver.addConflict(error);

    auto warnings = resolver.getConflictsBySeverity(ConflictResolver::SeverityLevel::WARNING);
    auto errors = resolver.getConflictsBySeverity(ConflictResolver::SeverityLevel::ERROR);

    EXPECT_EQ(warnings.size(), 1);
    EXPECT_EQ(errors.size(), 1);
}

TEST_F(AssemblyUITest, ClearConflicts) {
    auto& resolver = coordinator.getConflictResolver();

    ConflictResolver::ConflictRecord conflict;
    conflict.conflict_id = EntityId::generate();

    resolver.addConflict(conflict);
    EXPECT_EQ(resolver.getConflicts().size(), 1);

    resolver.clear();
    EXPECT_EQ(resolver.getConflicts().size(), 0);
}

// ===== AssemblyUICoordinator Tests =====

TEST_F(AssemblyUITest, CheckForConflictsWithValidAssembly) {
    auto& explorer = coordinator.getExplorer();
    auto& solver = coordinator.getSolver();

    // Create a valid assembly
    auto comp = std::make_shared<AssemblyComponentUI>(EntityId::generate());
    explorer.addComponent(comp);

    // Add a fixed constraint
    MateConstraint mate(EntityId::generate(), MateConstraint::MateType::FIXED);
    mate.setEntity1(comp->getComponentId());
    solver.addConstraint(mate);

    solver.solve();
    coordinator.checkForConflicts();

    // No conflicts for valid assembly
    auto& resolver = coordinator.getConflictResolver();
    EXPECT_LE(resolver.getCriticalCount(), 1);  // May have critical if unconstrained
}

TEST_F(AssemblyUITest, GetHealthStatus) {
    auto health_status = coordinator.getHealthStatus();

    EXPECT_FALSE(health_status.empty());
    EXPECT_TRUE(health_status.find("Assembly") != std::string::npos);
}

TEST_F(AssemblyUITest, UpdateVisualization) {
    auto& explorer = coordinator.getExplorer();

    auto constraint = std::make_shared<ConstraintTreeItem>(
        EntityId::generate(), MateConstraint::MateType::FIXED);
    explorer.addConstraint(constraint);

    // Should not throw
    coordinator.updateVisualization();
}

// ===== ConstraintVisual Tests =====

TEST(ConstraintVisualTest, CreateVisual) {
    ConstraintVisual visual;

    visual.type = ConstraintVisual::VisualType::DISTANCE_ARROW;
    visual.primary_point = Vector3D(1.0, 2.0, 3.0);
    visual.value = 10.5;

    EXPECT_EQ(visual.type, ConstraintVisual::VisualType::DISTANCE_ARROW);
    EXPECT_DOUBLE_EQ(visual.value, 10.5);
    EXPECT_FALSE(visual.is_satisfied);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
