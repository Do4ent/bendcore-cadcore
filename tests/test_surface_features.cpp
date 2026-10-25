#include <gtest/gtest.h>
#include "bendcore/geometry/surface_features.hpp"
#include "bendcore/document/entity_id.hpp"

using namespace BendCore::CadCore;

class SurfaceProfileTest : public ::testing::Test {
protected:
    EntityId profile_id = EntityId::generate();
};

class LoftSurfaceTest : public ::testing::Test {
protected:
    EntityId loft_id = EntityId::generate();
    LoftSurface loft{loft_id};
};

class SweepSurfaceTest : public ::testing::Test {
protected:
    EntityId sweep_id = EntityId::generate();
    EntityId profile_id = EntityId::generate();
    std::shared_ptr<SurfaceProfile> profile =
        std::make_shared<SurfaceProfile>(profile_id, SurfaceProfile::ProfileType::CLOSED_LOOP);
};

// ===== SurfaceProfile Tests =====

TEST_F(SurfaceProfileTest, CreateProfile) {
    SurfaceProfile profile(profile_id, SurfaceProfile::ProfileType::CLOSED_LOOP);

    EXPECT_EQ(profile.getId(), profile_id);
    EXPECT_EQ(profile.getType(), SurfaceProfile::ProfileType::CLOSED_LOOP);
}

TEST_F(SurfaceProfileTest, AddControlPoints) {
    SurfaceProfile profile(profile_id, SurfaceProfile::ProfileType::OPEN_CURVE);

    profile.addControlPoint(Vector3D(0.0, 0.0, 0.0));
    profile.addControlPoint(Vector3D(1.0, 0.0, 0.0));
    profile.addControlPoint(Vector3D(1.0, 1.0, 0.0));

    EXPECT_EQ(profile.getControlPoints().size(), 3);
}

TEST_F(SurfaceProfileTest, BoundingBox) {
    SurfaceProfile profile(profile_id, SurfaceProfile::ProfileType::POINT_CLOUD);

    profile.addControlPoint(Vector3D(1.0, 2.0, 3.0));
    profile.addControlPoint(Vector3D(4.0, 5.0, 6.0));
    profile.addControlPoint(Vector3D(0.0, 1.0, 2.0));

    auto min = profile.getBoundingBoxMin();
    auto max = profile.getBoundingBoxMax();

    EXPECT_DOUBLE_EQ(min.x, 0.0);
    EXPECT_DOUBLE_EQ(min.y, 1.0);
    EXPECT_DOUBLE_EQ(min.z, 2.0);
    EXPECT_DOUBLE_EQ(max.x, 4.0);
    EXPECT_DOUBLE_EQ(max.y, 5.0);
    EXPECT_DOUBLE_EQ(max.z, 6.0);
}

TEST_F(SurfaceProfileTest, ProfileLength) {
    SurfaceProfile profile(profile_id, SurfaceProfile::ProfileType::OPEN_CURVE);

    // Create line segments: (0,0,0) -> (1,0,0) -> (1,1,0)
    profile.addControlPoint(Vector3D(0.0, 0.0, 0.0));
    profile.addControlPoint(Vector3D(1.0, 0.0, 0.0));
    profile.addControlPoint(Vector3D(1.0, 1.0, 0.0));

    double length = profile.getLength();
    EXPECT_NEAR(length, 2.0, 1e-6);  // 1.0 + 1.0
}

TEST_F(SurfaceProfileTest, ProfileValidation) {
    // Closed loop needs at least 3 points
    SurfaceProfile closed_loop(profile_id, SurfaceProfile::ProfileType::CLOSED_LOOP);
    EXPECT_FALSE(closed_loop.isValid());

    closed_loop.addControlPoint(Vector3D(0.0, 0.0, 0.0));
    closed_loop.addControlPoint(Vector3D(1.0, 0.0, 0.0));
    EXPECT_FALSE(closed_loop.isValid());

    closed_loop.addControlPoint(Vector3D(0.0, 1.0, 0.0));
    EXPECT_TRUE(closed_loop.isValid());

    // Open curve needs at least 2 points
    SurfaceProfile open_curve(EntityId::generate(), SurfaceProfile::ProfileType::OPEN_CURVE);
    EXPECT_FALSE(open_curve.isValid());

    open_curve.addControlPoint(Vector3D(0.0, 0.0, 0.0));
    EXPECT_FALSE(open_curve.isValid());

    open_curve.addControlPoint(Vector3D(1.0, 0.0, 0.0));
    EXPECT_TRUE(open_curve.isValid());
}

// ===== LoftSurface Tests =====

TEST_F(LoftSurfaceTest, CreateLoft) {
    EXPECT_EQ(loft.getProfiles().size(), 0);
}

TEST_F(LoftSurfaceTest, AddProfiles) {
    auto profile1 = std::make_shared<SurfaceProfile>(
        EntityId::generate(), SurfaceProfile::ProfileType::CLOSED_LOOP);
    auto profile2 = std::make_shared<SurfaceProfile>(
        EntityId::generate(), SurfaceProfile::ProfileType::CLOSED_LOOP);

    // Add some points to make them valid
    profile1->addControlPoint(Vector3D(0.0, 0.0, 0.0));
    profile1->addControlPoint(Vector3D(1.0, 0.0, 0.0));
    profile1->addControlPoint(Vector3D(0.0, 1.0, 0.0));

    profile2->addControlPoint(Vector3D(0.0, 0.0, 1.0));
    profile2->addControlPoint(Vector3D(1.0, 0.0, 1.0));
    profile2->addControlPoint(Vector3D(0.0, 1.0, 1.0));

    loft.addProfile(profile1);
    loft.addProfile(profile2);

    EXPECT_EQ(loft.getProfiles().size(), 2);
}

TEST_F(LoftSurfaceTest, LoftValidation) {
    // Not enough profiles
    EXPECT_FALSE(loft.validate());

    auto profile1 = std::make_shared<SurfaceProfile>(
        EntityId::generate(), SurfaceProfile::ProfileType::CLOSED_LOOP);
    auto profile2 = std::make_shared<SurfaceProfile>(
        EntityId::generate(), SurfaceProfile::ProfileType::CLOSED_LOOP);

    // Add points
    for (auto p : {profile1, profile2}) {
        p->addControlPoint(Vector3D(0.0, 0.0, 0.0));
        p->addControlPoint(Vector3D(1.0, 0.0, 0.0));
        p->addControlPoint(Vector3D(0.0, 1.0, 0.0));
    }

    loft.addProfile(profile1);
    loft.addProfile(profile2);

    // Now should be valid
    EXPECT_TRUE(loft.validate());
}

TEST_F(LoftSurfaceTest, LoftParameters) {
    loft.setCloseSurface(true);
    loft.setMatchTangency(true);
    loft.setMaxDegree(6);

    EXPECT_TRUE(loft.getCloseSurface());
    EXPECT_TRUE(loft.getMatchTangency());
    EXPECT_EQ(loft.getMaxDegree(), 6);
}

TEST_F(LoftSurfaceTest, GenerateLoft) {
    auto profile1 = std::make_shared<SurfaceProfile>(
        EntityId::generate(), SurfaceProfile::ProfileType::CLOSED_LOOP);
    auto profile2 = std::make_shared<SurfaceProfile>(
        EntityId::generate(), SurfaceProfile::ProfileType::CLOSED_LOOP);

    for (auto p : {profile1, profile2}) {
        p->addControlPoint(Vector3D(0.0, 0.0, 0.0));
        p->addControlPoint(Vector3D(1.0, 0.0, 0.0));
        p->addControlPoint(Vector3D(0.0, 1.0, 0.0));
    }

    loft.addProfile(profile1);
    loft.addProfile(profile2);

    EntityId result = loft.generate();
    EXPECT_NE(result, EntityId());  // Should generate valid ID
}

// ===== SweepSurface Tests =====

TEST_F(SweepSurfaceTest, CreateSweep) {
    SweepSurface sweep(sweep_id, profile);

    EXPECT_EQ(sweep.getProfile(), profile);
    EXPECT_EQ(sweep.getPath(), nullptr);
}

TEST_F(SweepSurfaceTest, SetPath) {
    auto path = std::make_shared<SurfaceProfile>(
        EntityId::generate(), SurfaceProfile::ProfileType::OPEN_CURVE);

    path->addControlPoint(Vector3D(0.0, 0.0, 0.0));
    path->addControlPoint(Vector3D(0.0, 0.0, 1.0));

    SweepSurface sweep(sweep_id, profile);
    sweep.setPath(path);

    EXPECT_EQ(sweep.getPath(), path);
}

TEST_F(SweepSurfaceTest, SweepValidation) {
    // Profile with no points is invalid
    SweepSurface sweep(sweep_id, profile);
    EXPECT_FALSE(sweep.validate());  // No path set

    // Create path
    auto path = std::make_shared<SurfaceProfile>(
        EntityId::generate(), SurfaceProfile::ProfileType::OPEN_CURVE);
    path->addControlPoint(Vector3D(0.0, 0.0, 0.0));
    path->addControlPoint(Vector3D(0.0, 0.0, 1.0));
    sweep.setPath(path);

    EXPECT_FALSE(sweep.validate());  // Profile still has no points

    // Add points to profile
    profile->addControlPoint(Vector3D(1.0, 0.0, 0.0));
    profile->addControlPoint(Vector3D(0.0, 1.0, 0.0));
    profile->addControlPoint(Vector3D(0.0, 0.0, 1.0));

    EXPECT_TRUE(sweep.validate());
}

TEST_F(SweepSurfaceTest, SweepParameters) {
    SweepSurface sweep(sweep_id, profile);

    sweep.setKeepContact(false);
    sweep.setNormalToPath(false);
    sweep.setScaleProfile(2.0);
    sweep.setTwistAngle(M_PI / 4.0);

    EXPECT_FALSE(sweep.getKeepContact());
    EXPECT_FALSE(sweep.getNormalToPath());
    EXPECT_DOUBLE_EQ(sweep.getScaleProfile(), 2.0);
    EXPECT_NEAR(sweep.getTwistAngle(), M_PI / 4.0, 1e-6);
}

TEST_F(SweepSurfaceTest, GenerateSweep) {
    auto path = std::make_shared<SurfaceProfile>(
        EntityId::generate(), SurfaceProfile::ProfileType::OPEN_CURVE);
    path->addControlPoint(Vector3D(0.0, 0.0, 0.0));
    path->addControlPoint(Vector3D(0.0, 0.0, 1.0));

    profile->addControlPoint(Vector3D(1.0, 0.0, 0.0));
    profile->addControlPoint(Vector3D(0.0, 1.0, 0.0));
    profile->addControlPoint(Vector3D(-1.0, 0.0, 0.0));

    SweepSurface sweep(sweep_id, profile);
    sweep.setPath(path);

    EntityId result = sweep.generate();
    EXPECT_NE(result, EntityId());  // Should generate valid ID
}

// ===== SurfaceAnalyzer Tests =====

TEST(SurfaceAnalyzerTest, AnalyzeSurface) {
    EntityId surface_id = EntityId::generate();

    auto metrics = SurfaceAnalyzer::analyzeSurface(surface_id);

    EXPECT_GE(metrics.min_triangle_quality, 0.0);
    EXPECT_LE(metrics.min_triangle_quality, 1.0);
}

TEST(SurfaceAnalyzerTest, ValidateSurface) {
    EntityId surface_id = EntityId::generate();

    bool valid = SurfaceAnalyzer::validateSurface(surface_id);

    EXPECT_TRUE(valid);  // Placeholder always returns true
}

TEST(SurfaceAnalyzerTest, RepairSurface) {
    EntityId surface_id = EntityId::generate();

    bool repaired = SurfaceAnalyzer::repairSurface(surface_id);

    EXPECT_TRUE(repaired);  // Placeholder always returns true
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
