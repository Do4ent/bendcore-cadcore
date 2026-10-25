#include <gtest/gtest.h>
#include "bendcore/compat/legacy_adapter.hpp"

using namespace BendCore::CadCore;

TEST(LegacyAdapterTest, DetectV1) {
    std::vector<uint8_t> data = {0x01, 0x00, 0x00, 0x00};
    EXPECT_EQ(LegacyAdapter::detectVersion(data), LegacyVersion::V1_0);
}

TEST(LegacyAdapterTest, DetectV1_5) {
    std::vector<uint8_t> data = {0x15, 0x00, 0x00, 0x00};
    EXPECT_EQ(LegacyAdapter::detectVersion(data), LegacyVersion::V1_5);
}

TEST(LegacyAdapterTest, DetectV2) {
    std::vector<uint8_t> data = {0x20, 0x00, 0x00, 0x00};
    EXPECT_EQ(LegacyAdapter::detectVersion(data), LegacyVersion::V2_0);
}

TEST(LegacyAdapterTest, IsSupportedVersion) {
    EXPECT_TRUE(LegacyAdapter::isSupportedVersion(LegacyVersion::V1_0));
    EXPECT_TRUE(LegacyAdapter::isSupportedVersion(LegacyVersion::V1_5));
    EXPECT_TRUE(LegacyAdapter::isSupportedVersion(LegacyVersion::V2_0));
    EXPECT_FALSE(LegacyAdapter::isSupportedVersion(LegacyVersion::UNKNOWN));
}

TEST(LegacyAdapterTest, GetVersionString) {
    EXPECT_EQ(LegacyAdapter::getVersionString(LegacyVersion::V1_0), "1.0");
    EXPECT_EQ(LegacyAdapter::getVersionString(LegacyVersion::V1_5), "1.5");
    EXPECT_EQ(LegacyAdapter::getVersionString(LegacyVersion::V2_0), "2.0");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
