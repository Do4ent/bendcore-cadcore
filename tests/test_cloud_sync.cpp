#include <gtest/gtest.h>
#include "bendcore/cloud/cloud_sync_manager.hpp"
using namespace BendCore::CadCore;
TEST(CloudTest, Create) { CloudSyncManager sync(EntityId::generate()); EXPECT_FALSE(sync.isSynced()); }
TEST(CloudTest, Sync) { CloudSyncManager sync(EntityId::generate()); sync.syncToCloud(); EXPECT_TRUE(sync.isSynced()); }
int main(int argc, char** argv) { ::testing::InitGoogleTest(&argc, argv); return RUN_ALL_TESTS(); }
