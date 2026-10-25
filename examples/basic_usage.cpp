/**
 * Basic Usage Example
 *
 * This example demonstrates how to use BendCore CAD programmatically
 * to create a simple 3D model with parametric features.
 */

#include <iostream>
#include <vector>
#include "bendcore/cad_document.hpp"
#include "bendcore/sketch.hpp"
#include "bendcore/feature.hpp"
#include "bendcore/geometry.hpp"

using namespace BendCore;

int main() {
    try {
        // Create a new CAD document
        CadDocument doc;
        std::cout << "Created new CAD document" << std::endl;

        // Create a sketch on XY plane
        auto sketch_id = doc.CreateSketch("MainSketch");
        auto& sketch = doc.GetSketch(sketch_id);

        // Add a circle to the sketch (radius 10mm)
        auto circle_id = sketch.AddCircle(Point2D(0, 0), 10.0);
        std::cout << "Added circle to sketch" << std::endl;

        // Create a 3D body from sketch
        auto body_id = doc.CreateBody("Body1");

        // Add a Pad feature (extrude the sketch)
        FeatureParams pad_params;
        pad_params.type = FeatureType::Pad;
        pad_params.sketch_id = sketch_id;
        pad_params.length = 50.0;  // 50mm extrusion

        auto pad_id = doc.AddFeature(body_id, pad_params);
        std::cout << "Added Pad feature (50mm extrusion)" << std::endl;

        // Add a Pocket feature (cutting into the solid)
        FeatureParams pocket_params;
        pocket_params.type = FeatureType::Pocket;
        pocket_params.sketch_id = sketch_id;
        pocket_params.depth = 10.0;  // 10mm cutting depth

        auto pocket_id = doc.AddFeature(body_id, pocket_params);
        std::cout << "Added Pocket feature (10mm cutting depth)" << std::endl;

        // Add a Fillet feature (rounding edges)
        FeatureParams fillet_params;
        fillet_params.type = FeatureType::Fillet;
        fillet_params.radius = 2.0;  // 2mm fillet radius

        auto fillet_id = doc.AddFeature(body_id, fillet_params);
        std::cout << "Added Fillet feature (2mm radius)" << std::endl;

        // Export to STEP format
        std::string output_path = "/tmp/bendcore_example.step";
        doc.ExportSTEP(body_id, output_path);
        std::cout << "Exported to: " << output_path << std::endl;

        // Get body statistics
        auto body = doc.GetBody(body_id);
        std::cout << "\nBody Statistics:" << std::endl;
        std::cout << "  Name: " << body.name << std::endl;
        std::cout << "  Features: " << body.features.size() << std::endl;
        std::cout << "  Volume: " << body.volume << " mm³" << std::endl;
        std::cout << "  Surface Area: " << body.surface_area << " mm²" << std::endl;

        std::cout << "\nExample completed successfully!" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

/**
 * Compilation:
 *
 * mkdir build && cd build
 * cmake -DCMAKE_BUILD_TYPE=Release ..
 * cmake --build . --parallel $(nproc)
 * g++ -std=c++20 -I../include ../examples/basic_usage.cpp -L../build -lbendcore -o basic_example
 * ./basic_example
 */
