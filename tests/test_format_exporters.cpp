#include <gtest/gtest.h>
#include "bendcore/format_exporters.h"
#include <fstream>
#include <filesystem>

using namespace bendcore::io;
namespace fs = std::filesystem;

TEST(FormatExporterTest, CreateGeometryData) {
    GeometryData geom;
    geom.name = "TestPipe";
    geom.vertices = {0, 0, 0, 1, 0, 0, 0, 1, 0};
    geom.faces = {0, 1, 2};
    geom.materials = {"steel"};

    EXPECT_EQ(geom.name, "TestPipe");
    EXPECT_EQ(geom.vertices.size(), 9);
    EXPECT_EQ(geom.faces.size(), 3);
}

TEST(FormatExporterTest, ExportOptions) {
    ExportOptions opts;
    opts.outputPath = "/tmp/test.stl";
    opts.tolerance = 0.01;
    opts.qualityLevel = 2;

    EXPECT_EQ(opts.tolerance, 0.01);
    EXPECT_EQ(opts.qualityLevel, 2);
}

TEST(STLExportTest, ASCIIExport) {
    GeometryData geom;
    geom.name = "TestPipe";
    geom.vertices = {0, 0, 0, 10, 0, 0, 5, 10, 0};
    geom.faces = {0, 1, 2};

    ExportOptions opts;
    opts.outputPath = "/tmp/bendcore_test.stl";
    opts.includeNormals = false;

    bool result = STL_Exporter::exportToSTL({geom}, opts, false);
    EXPECT_TRUE(result);

    if (fs::exists("/tmp/bendcore_test.stl")) {
        std::ifstream file("/tmp/bendcore_test.stl");
        std::string line;
        std::getline(file, line);
        EXPECT_TRUE(line.find("solid") != std::string::npos);
        fs::remove("/tmp/bendcore_test.stl");
    }
}

TEST(OBJExportTest, BasicExport) {
    GeometryData geom;
    geom.name = "TestPipe";
    geom.vertices = {0, 0, 0, 10, 0, 0, 5, 10, 0, 5, 5, 10};
    geom.faces = {0, 1, 2, 1, 2, 3};
    geom.normals = {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1};

    ExportOptions opts;
    opts.outputPath = "/tmp/bendcore_test.obj";

    bool result = OBJ_Exporter::exportToOBJ({geom}, opts);
    EXPECT_TRUE(result);

    if (fs::exists("/tmp/bendcore_test.obj")) {
        std::ifstream file("/tmp/bendcore_test.obj");
        std::string line;
        std::getline(file, line);
        EXPECT_TRUE(line.find("#") != std::string::npos ||
                   line.find("v") != std::string::npos);
        fs::remove("/tmp/bendcore_test.obj");
    }
}

TEST(JSONExportTest, BasicExport) {
    GeometryData geom;
    geom.name = "TestPipe";
    geom.vertices = {0, 0, 0, 10, 0, 0, 5, 10, 0};
    geom.faces = {0, 1, 2};

    ExportOptions opts;
    opts.outputPath = "/tmp/bendcore_test.json";

    bool result = JSON_Exporter::exportToJSON({geom}, opts);
    EXPECT_TRUE(result);

    if (fs::exists("/tmp/bendcore_test.json")) {
        std::ifstream file("/tmp/bendcore_test.json");
        std::string line;
        std::getline(file, line);
        EXPECT_TRUE(line.find("{") != std::string::npos);
        fs::remove("/tmp/bendcore_test.json");
    }
}

TEST(DXFExportTest, BasicExport) {
    GeometryData geom;
    geom.name = "TestPipe";
    geom.vertices = {0, 0, 0, 10, 0, 0, 5, 10, 0};
    geom.faces = {0, 1, 2};

    ExportOptions opts;
    opts.outputPath = "/tmp/bendcore_test.dxf";

    bool result = DXF_Exporter::exportToDXF({geom}, opts);
    EXPECT_TRUE(result);

    if (fs::exists("/tmp/bendcore_test.dxf")) {
        fs::remove("/tmp/bendcore_test.dxf");
    }
}

TEST(UniversalExporterTest, ExportMultipleFormats) {
    GeometryData geom;
    geom.name = "TestPipe";
    geom.vertices = {0, 0, 0, 10, 0, 0, 5, 10, 0};
    geom.faces = {0, 1, 2};

    ExportOptions opts;

    opts.outputPath = "/tmp/test.obj";
    EXPECT_TRUE(UniversalExporter::exportGeometry({geom}, opts, UniversalExporter::Format::OBJ));
    if (fs::exists("/tmp/test.obj")) fs::remove("/tmp/test.obj");

    opts.outputPath = "/tmp/test.json";
    EXPECT_TRUE(UniversalExporter::exportGeometry({geom}, opts, UniversalExporter::Format::JSON));
    if (fs::exists("/tmp/test.json")) fs::remove("/tmp/test.json");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
