#include <gtest/gtest.h>
#include "bendcore/rendering/visualization_engine.hpp"

using namespace BendCore::CadCore;

TEST(MaterialTest, CreateMaterial) {
    Material mat(Material::MaterialType::STEEL);
    EXPECT_NE(mat.getDiffuseColor().x, 0.0);
}

TEST(MaterialTest, SetDiffuseColor) {
    Material mat;
    Vector3D color(0.8, 0.2, 0.1);
    mat.setDiffuseColor(color);
    EXPECT_EQ(mat.getDiffuseColor().x, 0.8);
}

TEST(ViewModeTest, CreateViewMode) {
    ViewMode mode(ViewMode::DisplayMode::SHADED);
    EXPECT_EQ(mode.getDisplayMode(), ViewMode::DisplayMode::SHADED);
}

TEST(ViewModeTest, ToggleEdges) {
    ViewMode mode;
    EXPECT_FALSE(mode.areEdgesShown());
    mode.setShowEdges(true);
    EXPECT_TRUE(mode.areEdgesShown());
}

TEST(LightingConfigTest, CreateLighting) {
    LightingConfig lighting;
    EXPECT_EQ(lighting.getLightCount(), 0);
}

TEST(LightingConfigTest, AddLights) {
    LightingConfig lighting;
    lighting.addLight(LightingConfig::LightType::DIRECTIONAL, Vector3D(0, 0, 1), Vector3D(1, 1, 1));
    EXPECT_EQ(lighting.getLightCount(), 1);
}

TEST(CameraConfigTest, CreateCamera) {
    CameraConfig camera;
    EXPECT_DOUBLE_EQ(camera.getFOV(), 45.0);
}

TEST(CameraConfigTest, SetPosition) {
    CameraConfig camera;
    Vector3D pos(10, 20, 30);
    camera.setPosition(pos);
    EXPECT_EQ(camera.getPosition().x, 10);
}

TEST(RenderViewportTest, CreateViewport) {
    RenderViewport viewport(1024, 768);
    EXPECT_EQ(viewport.getWidth(), 1024);
    EXPECT_EQ(viewport.getHeight(), 768);
}

TEST(RenderViewportTest, SetViewMode) {
    RenderViewport viewport(1024, 768);
    ViewMode mode(ViewMode::DisplayMode::WIREFRAME);
    viewport.setViewMode(mode);
    EXPECT_EQ(viewport.getViewMode().getDisplayMode(), ViewMode::DisplayMode::WIREFRAME);
}

TEST(RenderViewportTest, RenderFrame) {
    RenderViewport viewport(1024, 768);
    bool result = viewport.renderFrame();
    EXPECT_TRUE(result);
}

TEST(VisualizationEngineTest, CreateEngine) {
    VisualizationEngine engine(EntityId::generate());
    EXPECT_EQ(engine.getMaterialCount(), 0);
}

TEST(VisualizationEngineTest, SetMaterial) {
    VisualizationEngine engine(EntityId::generate());
    EntityId id = EntityId::generate();
    Material mat;
    engine.setMaterial(id, mat);
    EXPECT_EQ(engine.getMaterialCount(), 1);
}

TEST(VisualizationEngineTest, SetVisibility) {
    VisualizationEngine engine(EntityId::generate());
    EntityId id = EntityId::generate();
    engine.setVisibility(id, false);
    EXPECT_FALSE(engine.isVisible(id));
}

TEST(VisualizationEngineTest, RenderScene) {
    VisualizationEngine engine(EntityId::generate());
    bool result = engine.renderScene();
    EXPECT_TRUE(result);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
