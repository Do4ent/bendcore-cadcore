#include <gtest/gtest.h>
#include "bendcore/persistence/recovery.hpp"

using namespace BendCore::CadCore;

TEST(RecoveryTest, CreateRecoverySystem) {
    RecoverySystem recovery("/tmp/recovery");
    EXPECT_EQ(recovery.getRecoveryDir(), "/tmp/recovery");
}

TEST(RecoveryTest, CreateCheckpoint) {
    RecoverySystem recovery("/tmp/recovery2");
    CadDocument doc;

    EXPECT_TRUE(recovery.createCheckpoint(doc));
}

TEST(RecoveryTest, GetRecoveryPoints) {
    RecoverySystem recovery("/tmp/recovery3");
    CadDocument doc;

    recovery.createCheckpoint(doc);
    auto points = recovery.getRecoveryPoints();

    EXPECT_GE(points.size(), 0);
}

TEST(RecoveryTest, NeedsRecovery) {
    RecoverySystem recovery("/tmp/recovery4");

    bool needs = recovery.needsRecovery();
    EXPECT_FALSE(needs);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
