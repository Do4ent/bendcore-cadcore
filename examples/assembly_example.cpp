/**
 * Assembly Example
 *
 * This example demonstrates how to create assemblies with multiple
 * components and constraints in BendCore CAD.
 */

#include <iostream>
#include <vector>
#include "bendcore/cad_document.hpp"
#include "bendcore/assembly.hpp"
#include "bendcore/constraint.hpp"

using namespace BendCore;

int main() {
    try {
        // Create a new CAD document
        CadDocument doc;
        std::cout << "Creating assembly example..." << std::endl;

        // Create first component (Base plate)
        auto base_id = doc.CreateBody("BasePlate");
        auto& base = doc.GetBody(base_id);
        // ... add geometry to base ...
        std::cout << "Created base plate component" << std::endl;

        // Create second component (Support)
        auto support_id = doc.CreateBody("Support");
        auto& support = doc.GetBody(support_id);
        // ... add geometry to support ...
        std::cout << "Created support component" << std::endl;

        // Create assembly
        auto assembly_id = doc.CreateAssembly("MainAssembly");
        auto& assembly = doc.GetAssembly(assembly_id);

        // Add components to assembly
        auto base_instance_id = assembly.AddComponent(base_id, "Base_Inst1");
        auto support_instance_id = assembly.AddComponent(support_id, "Support_Inst1");
        std::cout << "Added components to assembly" << std::endl;

        // Add constraint: Coincident (align bases)
        ConstraintParams coincident;
        coincident.type = ConstraintType::Coincident;
        coincident.entity1_id = base_instance_id;
        coincident.entity2_id = support_instance_id;
        assembly.AddConstraint(coincident);
        std::cout << "Added Coincident constraint" << std::endl;

        // Add constraint: Distance
        ConstraintParams distance;
        distance.type = ConstraintType::Distance;
        distance.entity1_id = base_instance_id;
        distance.entity2_id = support_instance_id;
        distance.value = 50.0;  // 50mm distance
        assembly.AddConstraint(distance);
        std::cout << "Added Distance constraint (50mm)" << std::endl;

        // Solve assembly constraints
        assembly.SolveConstraints();
        std::cout << "Solved assembly constraints" << std::endl;

        // Check for collisions
        auto collisions = assembly.DetectCollisions();
        std::cout << "Collision detection: " << collisions.size() << " collisions found" << std::endl;

        // Export assembly to STEP
        std::string output_path = "/tmp/bendcore_assembly.step";
        doc.ExportSTEP(assembly_id, output_path);
        std::cout << "Exported assembly to: " << output_path << std::endl;

        // Print assembly statistics
        std::cout << "\nAssembly Statistics:" << std::endl;
        std::cout << "  Components: " << assembly.components.size() << std::endl;
        std::cout << "  Constraints: " << assembly.constraints.size() << std::endl;
        std::cout << "  Mass: " << assembly.mass << " kg" << std::endl;

        std::cout << "\nAssembly example completed successfully!" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
