#include <gtest/gtest.h>
#include "bendcore/features/revolve.hpp"
#include "bendcore/document/entity_id.hpp"

using namespace BendCore::CadCore;

class RevolveFeatureTest : public ::testing::Test {
protected:
    EntityId test_id = EntityId::generate();
    RevolveFeature feature{test_id, "Test Revolve"};
};

// Test: Create revolve feature
TEST_F(RevolveFeatureTest, CreateRevolveFeature) {
    EXPECT_EQ(feature.getId(), test_id);
    EXPECT_EQ(feature.getName(), "Test Revolve");
}

// Test: Default configuration
TEST_F(RevolveFeatureTest, DefaultConfiguration) {
    const auto& config = feature.getConfig();
    EXPECT_EQ(config.axis_type, RevolveFeature::AxisType::Z);
    EXPECT_EQ(config.angle, 360.0);
    EXPECT_FALSE(config.bidirectional);
    EXPECT_FALSE(config.thin_wall);
}

// Test: Set Z axis
TEST_F(RevolveFeatureTest, SetZAxis) {
    feature.setAxis(RevolveFeature::AxisType::Z);

    const auto& config = feature.getConfig();
    EXPECT_EQ(config.axis_type, RevolveFeature::AxisType::Z);
    EXPECT_EQ(config.axis_direction.x, 0.0);
    EXPECT_EQ(config.axis_direction.y, 0.0);
    EXPECT_EQ(config.axis_direction.z, 1.0);
}

// Test: Set X axis
TEST_F(RevolveFeatureTest, SetXAxis) {
    feature.setAxis(RevolveFeature::AxisType::X);

    const auto& config = feature.getConfig();
    EXPECT_EQ(config.axis_type, RevolveFeature::AxisType::X);
    EXPECT_EQ(config.axis_direction.x, 1.0);
}

// Test: Set Y axis
TEST_F(RevolveFeatureTest, SetYAxis) {
    feature.setAxis(RevolveFeature::AxisType::Y);

    const auto& config = feature.getConfig();
    EXPECT_EQ(config.axis_type, RevolveFeature::AxisType::Y);
    EXPECT_EQ(config.axis_direction.y, 1.0);
}

// Test: Set custom axis
TEST_F(RevolveFeatureTest, SetCustomAxis) {
    Vector3D origin{1.0, 2.0, 3.0};
    Vector3D direction{1.0, 1.0, 1.0};

    feature.setCustomAxis(origin, direction);

    const auto& config = feature.getConfig();
    EXPECT_EQ(config.axis_type, RevolveFeature::AxisType::CUSTOM);
    EXPECT_EQ(config.axis_origin.x, 1.0);
    EXPECT_EQ(config.axis_origin.y, 2.0);
    EXPECT_EQ(config.axis_origin.z, 3.0);
}

// Test: Set angle
TEST_F(RevolveFeatureTest, SetAngle) {
    feature.setAngle(180.0);

    const auto& config = feature.getConfig();
    EXPECT_EQ(config.angle, 180.0);
}

// Test: Is full revolution
TEST_F(RevolveFeatureTest, IsFullRevolution) {
    feature.setAngle(360.0);
    EXPECT_TRUE(feature.isFullRevolution());

    feature.setAngle(180.0);
    EXPECT_FALSE(feature.isFullRevolution());
}

// Test: Set thin wall
TEST_F(RevolveFeatureTest, SetThinWall) {
    feature.setThinWall(2.0);

    const auto& config = feature.getConfig();
    EXPECT_TRUE(config.thin_wall);
    EXPECT_EQ(config.thickness, 2.0);
}

// Test: Invalid revolve (missing profile)
TEST_F(RevolveFeatureTest, InvalidRevolveMissingProfile) {
    RevolveFeature::Config config;
    config.angle = 360.0;

    feature.setConfig(config);
    EXPECT_FALSE(feature.validate());
    EXPECT_FALSE(feature.hasValidProfile());
}

// Test: Valid revolve (full 360)
TEST_F(RevolveFeatureTest, ValidRevolveFull360) {
    RevolveFeature::Config config;
    config.profile_sketch = EntityId::generate();
    config.axis_direction = {0.0, 0.0, 1.0};
    config.angle = 360.0;

    feature.setConfig(config);
    EXPECT_TRUE(feature.validate());
    EXPECT_TRUE(feature.hasValidProfile());
}

// Test: Valid revolve (partial angle)
TEST_F(RevolveFeatureTest, ValidRevolvePartialAngle) {
    RevolveFeature::Config config;
    config.profile_sketch = EntityId::generate();
    config.axis_direction = {0.0, 0.0, 1.0};
    config.angle = 180.0;

    feature.setConfig(config);
    EXPECT_TRUE(feature.validate());
}

// Test: Invalid angle (0 degrees)
TEST_F(RevolveFeatureTest, InvalidAngleZero) {
    RevolveFeature::Config config;
    config.profile_sketch = EntityId::generate();
    config.angle = 0.0;

    feature.setConfig(config);
    EXPECT_FALSE(feature.validate());
}

// Test: Invalid angle (> 360)
TEST_F(RevolveFeatureTest, InvalidAngleGreaterThan360) {
    RevolveFeature::Config config;
    config.profile_sketch = EntityId::generate();
    config.angle = 450.0;

    feature.setConfig(config);
    EXPECT_FALSE(feature.validate());
}

// Test: Bidirectional revolve
TEST_F(RevolveFeatureTest, BidirectionalRevolve) {
    RevolveFeature::Config config;
    config.profile_sketch = EntityId::generate();
    config.angle = 90.0;
    config.bidirectional = true;

    feature.setConfig(config);
    EXPECT_TRUE(feature.getConfig().bidirectional);
}

// Test: Invalid bidirectional (angle > 180)
TEST_F(RevolveFeatureTest, InvalidBidirectionalAngle) {
    RevolveFeature::Config config;
    config.profile_sketch = EntityId::generate();
    config.angle = 270.0;
    config.bidirectional = true;

    feature.setConfig(config);
    EXPECT_FALSE(feature.validate());
}

// Test: Invalid thin wall (negative thickness)
TEST_F(RevolveFeatureTest, InvalidThinWallThickness) {
    RevolveFeature::Config config;
    config.profile_sketch = EntityId::generate();
    config.thin_wall = true;
    config.thickness = -1.0;

    feature.setConfig(config);
    EXPECT_FALSE(feature.validate());
}

// Test: Invalid axis direction (zero vector)
TEST_F(RevolveFeatureTest, InvalidAxisDirection) {
    RevolveFeature::Config config;
    config.profile_sketch = EntityId::generate();
    config.axis_direction = {0.0, 0.0, 0.0};

    feature.setConfig(config);
    EXPECT_FALSE(feature.validate());
}

// Test: Angle wrapping (negative angle)
TEST_F(RevolveFeatureTest, AngleWrappingNegative) {
    feature.setAngle(-90.0);
    // Angle should wrap to positive
    EXPECT_GT(feature.getConfig().angle, 0.0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
