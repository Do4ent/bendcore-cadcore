#include <gtest/gtest.h>
#include "bendcore/features/feature_system.hpp"

using namespace BendCore::CadCore;

TEST(FeatureSystemTest, CreateFeature) {
    FeatureSystem fs;
    EntityId fid = fs.createFeature(FeatureType::PAD, "Pad1");

    const auto* f = fs.getFeature(fid);
    EXPECT_NE(f, nullptr);
    EXPECT_EQ(f->name, "Pad1");
    EXPECT_EQ(f->type, FeatureType::PAD);
}

TEST(FeatureSystemTest, AddConstraint) {
    FeatureSystem fs;
    EntityId fid = fs.createFeature(FeatureType::PAD, "Pad1");

    Constraint c;
    c.id = EntityId::generate();
    c.name = "Height";
    c.value = 10.0;

    EXPECT_TRUE(fs.addConstraint(fid, c));

    const auto* f = fs.getFeature(fid);
    EXPECT_EQ(f->constraints.size(), 1);
}

TEST(FeatureSystemTest, SetParent) {
    FeatureSystem fs;
    EntityId parent = fs.createFeature(FeatureType::SKETCH, "Sketch1");
    EntityId child = fs.createFeature(FeatureType::PAD, "Pad1");

    EXPECT_TRUE(fs.setParent(child, parent));

    const auto* c = fs.getFeature(child);
    EXPECT_EQ(c->parentId, parent);
}

TEST(FeatureSystemTest, SuppressFeature) {
    FeatureSystem fs;
    EntityId fid = fs.createFeature(FeatureType::PAD, "Pad1");

    EXPECT_TRUE(fs.suppressFeature(fid));

    const auto* f = fs.getFeature(fid);
    EXPECT_TRUE(f->isSuppressed);
}

TEST(FeatureSystemTest, ValidateFeatureTree) {
    FeatureSystem fs;
    (void)fs.createFeature(FeatureType::PAD, "Pad1");

    EXPECT_TRUE(fs.validateFeatureTree());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
