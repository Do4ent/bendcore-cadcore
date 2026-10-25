#include <gtest/gtest.h>
#include "bendcore/features/loft.hpp"
#include "bendcore/document/entity_id.hpp"

using namespace BendCore::CadCore;

class LoftFeatureTest : public ::testing::Test {
protected:
    EntityId test_id = EntityId::generate();
    LoftFeature feature{test_id, "Test Loft"};
};

// Test: Create loft feature
TEST_F(LoftFeatureTest, CreateLoftFeature) {
    EXPECT_EQ(feature.getId(), test_id);
    EXPECT_EQ(feature.getName(), "Test Loft");
    EXPECT_EQ(feature.getProfileCount(), 0);
}

// Test: Add profiles
TEST_F(LoftFeatureTest, AddProfiles) {
    LoftFeature::Profile p1;
    p1.sketch_id = EntityId::generate();
    p1.blend_parameter = 0.0;
    p1.is_start = true;

    LoftFeature::Profile p2;
    p2.sketch_id = EntityId::generate();
    p2.blend_parameter = 1.0;
    p2.is_end = true;

    feature.addProfile(p1);
    feature.addProfile(p2);

    EXPECT_EQ(feature.getProfileCount(), 2);
}

// Test: Remove profile
TEST_F(LoftFeatureTest, RemoveProfile) {
    LoftFeature::Profile p1;
    p1.sketch_id = EntityId::generate();
    p1.blend_parameter = 0.0;

    feature.addProfile(p1);
    EXPECT_EQ(feature.getProfileCount(), 1);

    bool removed = feature.removeProfile(0);
    EXPECT_TRUE(removed);
    EXPECT_EQ(feature.getProfileCount(), 0);
}

// Test: Invalid loft (less than 2 profiles)
TEST_F(LoftFeatureTest, InvalidLoftTooFewProfiles) {
    LoftFeature::Profile p1;
    p1.sketch_id = EntityId::generate();
    feature.addProfile(p1);

    EXPECT_FALSE(feature.validate());
}

// Test: Valid loft
TEST_F(LoftFeatureTest, ValidLoft) {
    LoftFeature::Profile p1;
    p1.sketch_id = EntityId::generate();
    p1.blend_parameter = 0.0;
    p1.is_start = true;

    LoftFeature::Profile p2;
    p2.sketch_id = EntityId::generate();
    p2.blend_parameter = 1.0;
    p2.is_end = true;

    feature.addProfile(p1);
    feature.addProfile(p2);

    EXPECT_TRUE(feature.validate());
}

// Test: Invalid blend parameter
TEST_F(LoftFeatureTest, InvalidBlendParameter) {
    LoftFeature::Profile p1;
    p1.sketch_id = EntityId::generate();
    p1.blend_parameter = -0.5;  // Invalid: < 0

    LoftFeature::Profile p2;
    p2.sketch_id = EntityId::generate();
    p2.blend_parameter = 1.0;

    feature.addProfile(p1);
    feature.addProfile(p2);

    EXPECT_FALSE(feature.validate());
}

// Test: Closed loft
TEST_F(LoftFeatureTest, ClosedLoft) {
    LoftFeature::Config config;
    config.closed = true;
    config.profiles.push_back(LoftFeature::Profile{EntityId::generate(), 0.0});
    config.profiles.push_back(LoftFeature::Profile{EntityId::generate(), 0.5});
    config.profiles.push_back(LoftFeature::Profile{EntityId::generate(), 1.0});

    feature.setConfig(config);

    EXPECT_TRUE(feature.getConfig().closed);
    EXPECT_EQ(feature.getProfileCount(), 3);
}

// Test: Ruled surface option
TEST_F(LoftFeatureTest, RuledSurface) {
    LoftFeature::Config config;
    config.ruled_surface = true;
    config.smooth_transition = false;
    config.profiles.push_back(LoftFeature::Profile{EntityId::generate(), 0.0});
    config.profiles.push_back(LoftFeature::Profile{EntityId::generate(), 1.0});

    feature.setConfig(config);

    EXPECT_TRUE(feature.getConfig().ruled_surface);
    EXPECT_FALSE(feature.getConfig().smooth_transition);
}

// Test: Can generate surface
TEST_F(LoftFeatureTest, CanGenerateSurface) {
    EXPECT_FALSE(feature.canGenerateSurface());  // 0 profiles

    feature.addProfile(LoftFeature::Profile{EntityId::generate(), 0.0});
    EXPECT_FALSE(feature.canGenerateSurface());  // 1 profile

    feature.addProfile(LoftFeature::Profile{EntityId::generate(), 1.0});
    EXPECT_TRUE(feature.canGenerateSurface());   // 2 profiles
}

// Test: Tolerance validation
TEST_F(LoftFeatureTest, InvalidTolerance) {
    LoftFeature::Config config;
    config.tolerance = -0.01;  // Invalid: negative
    config.profiles.push_back(LoftFeature::Profile{EntityId::generate(), 0.0});
    config.profiles.push_back(LoftFeature::Profile{EntityId::generate(), 1.0});

    feature.setConfig(config);
    EXPECT_FALSE(feature.validate());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
