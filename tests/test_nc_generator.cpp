#include <gtest/gtest.h>
#include "bendcore/manufacturing/nc_generator.hpp"

using namespace BendCore::CadCore;

TEST(NCGeneratorTest, AddAndGenerate) {
    NCGenerator gen(EntityId::generate());
    NCGenerator::NCBlock b;
    b.position = Vector3D(10, 20, 5);
    b.feed_rate = 100;
    gen.addOperation(b);

    EXPECT_EQ(gen.getBlocks().size(), 1);
    std::string code = gen.generateGCode();
    EXPECT_TRUE(code.find("G90") != std::string::npos);
}

TEST(BOMMakerTest, CostCalculation) {
    BOMMaker bom(EntityId::generate());
    BOMMaker::BOMItem item;
    item.quantity = 3;
    item.unit_cost = 100.0;
    bom.addItem(item);
    EXPECT_DOUBLE_EQ(bom.getTotalCost(), 300.0);
}

TEST(DrawingGeneratorTest, SVGGeneration) {
    DrawingGenerator gen(EntityId::generate());
    std::string svg = gen.generateSVG();
    EXPECT_TRUE(svg.find("<svg") != std::string::npos);
}

TEST(ManufacturingReportTest, HTMLReport) {
    ManufacturingReport report(EntityId::generate());
    report.setMetadata("Test", "1.0", "2026-10-25");
    std::string html = report.generateHTML();
    EXPECT_TRUE(html.find("<html>") != std::string::npos);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
