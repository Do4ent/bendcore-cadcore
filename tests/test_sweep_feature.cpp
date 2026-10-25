#include <gtest/gtest.h>
#include "bendcore/features/sweep.hpp"
#include "bendcore/document/entity_id.hpp"

using namespace BendCore::CadCore;

class SweepFeatureTest : public ::testing::Test {
protected:
    EntityId test_id = EntityId::generate();
    SweepFeature feature{test_id, "Test Sweep"};
};

// Test: Create sweep feature
TEST_F(SweepFeatureTest, CreateSweepFeature) {
    EXPECT_EQ(feature.getId(), test_id);
    EXPECT_EQ(feature.getName(), "Test Sweep");
}

// Test: Set configuration
TEST_F(SweepFeatureTest, SetConfiguration) {
    SweepFeature::Config config;
    config.profile_sketch = EntityId::generate();
    config.path_entity = EntityId::generate();
    config.scale_start = 1.0;
    config.scale_end = 0.5;
    config.twist_angle = 45.0;

    feature.setConfig(config);

    const auto& retrieved = feature.getConfig();
    EXPECT_EQ(retrieved.scale_start, 1.0);
    EXPECT_EQ(retrieved.scale_end, 0.5);
    EXPECT_EQ(retrieved.twist_angle, 45.0);
}

// Test: Set scale
TEST_F(SweepFeatureTest, SetScale) {
    feature.setScale(1.0, 2.0);

    const auto& config = feature.getConfig();
    EXPECT_EQ(config.scale_start, 1.0);
    EXPECT_EQ(config.scale_end, 2.0);
}

// Test: Set twist
TEST_F(SweepFeatureTest, SetTwist) {
    feature.setTwist(180.0);

    const auto& config = feature.getConfig();
    EXPECT_EQ(config.twist_angle, 180.0);
}

// Test: Invalid sweep (missing profile)
TEST_F(SweepFeatureTest, InvalidSweepMissingProfile) {
    SweepFeature::Config config;
    config.path_entity = EntityId::generate();

    feature.setConfig(config);
    EXPECT_FALSE(feature.validate());
    EXPECT_FALSE(feature.hasValidProfile());
}

// Test: Invalid sweep (missing path)
TEST_F(SweepFeatureTest, InvalidSweepMissingPath) {
    SweepFeature::Config config;
    config.profile_sketch = EntityId::generate();

    feature.setConfig(config);
    EXPECT_FALSE(feature.validate());
    EXPECT_FALSE(feature.hasValidPath());
}

// Test: Valid sweep
TEST_F(SweepFeatureTest, ValidSweep) {
    SweepFeature::Config config;
    config.profile_sketch = EntityId::generate();
    config.path_entity = EntityId::generate();
    config.scale_start = 1.0;
    config.scale_end = 1.0;
    config.twist_angle = 0.0;
    config.normal_direction = {0.0, 0.0, 1.0};
    config.tolerance = 0.01;

    feature.setConfig(config);
    EXPECT_TRUE(feature.validate());
}

// Test: Invalid scale factors
TEST_F(SweepFeatureTest, InvalidScaleFactors) {
    SweepFeature::Config config;
    config.profile_sketch = EntityId::generate();
    config.path_entity = EntityId::generate();
    config.scale_start = -0.5;  // Invalid: negative

    feature.setConfig(config);
    EXPECT_FALSE(feature.validate());
}

// Test: Invalid twist angle
TEST_F(SweepFeatureTest, InvalidTwistAngle) {
    SweepFeature::Config config;
    config.profile_sketch = EntityId::generate();
    config.path_entity = EntityId::generate();
    config.scale_start = 1.0;
    config.scale_end = 1.0;
    config.twist_angle = 450.0;  // Invalid: > 360

    feature.setConfig(config);
    EXPECT_FALSE(feature.validate());
}

// Test: Closed profile sweep
TEST_F(SweepFeatureTest, ClosedProfileSweep) {
    SweepFeature::Config config;
    config.profile_sketch = EntityId::generate();
    config.path_entity = EntityId::generate();
    config.closed_profile = true;

    feature.setConfig(config);
    EXPECT_TRUE(feature.getConfig().closed_profile);
}

// Test: Keep profile normal option
TEST_F(SweepFeatureTest, KeepProfileNormal) {
    SweepFeature::Config config;
    config.profile_sketch = EntityId::generate();
    config.path_entity = EntityId::generate();
    config.keep_profile_normal = false;

    feature.setConfig(config);
    EXPECT_FALSE(feature.getConfig().keep_profile_normal);
}

// Test: Invalid normal direction
TEST_F(SweepFeatureTest, InvalidNormalDirection) {
    SweepFeature::Config config;
    config.profile_sketch = EntityId::generate();
    config.path_entity = EntityId::generate();
    config.scale_start = 1.0;
    config.scale_end = 1.0;
    config.normal_direction = {0.0, 0.0, 0.0};  // Invalid: zero vector

    feature.setConfig(config);
    EXPECT_FALSE(feature.validate());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
