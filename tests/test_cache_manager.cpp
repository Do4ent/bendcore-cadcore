#include <gtest/gtest.h>
#include "bendcore/optimization/cache_manager.hpp"
using namespace BendCore::CadCore;
TEST(CacheManagerTest, Create) { CacheManager mgr; EXPECT_EQ(mgr.getHitRate(), 0.0); }
TEST(CacheManagerTest, Cache) { CacheManager mgr; EntityId id = EntityId::generate(); EXPECT_TRUE(mgr.cacheGeometry(id)); }
TEST(CacheManagerTest, Retrieve) { CacheManager mgr; EntityId id = EntityId::generate(); mgr.cacheGeometry(id); EXPECT_TRUE(mgr.getCachedGeometry(id)); }
int main(int argc, char** argv) { ::testing::InitGoogleTest(&argc, argv); return RUN_ALL_TESTS(); }
