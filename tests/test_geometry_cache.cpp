#include <gtest/gtest.h>
#include "bendcore/geometry/geometry_cache.hpp"

using namespace BendCore::CadCore;

TEST(GeometryCacheTest, CacheGeometry) {
    GeometryCache cache;
    EntityId id = EntityId::generate();

    CachedGeometry geom;
    geom.entityId = id;
    geom.volume = 100.0;

    cache.cacheGeometry(id, geom);
    EXPECT_TRUE(cache.isCached(id));
}

TEST(GeometryCacheTest, GetGeometry) {
    GeometryCache cache;
    EntityId id = EntityId::generate();

    CachedGeometry geom;
    geom.entityId = id;
    geom.volume = 50.0;
    cache.cacheGeometry(id, geom);

    const auto* cached = cache.getGeometry(id);
    EXPECT_NE(cached, nullptr);
    EXPECT_DOUBLE_EQ(cached->volume, 50.0);
}

TEST(GeometryCacheTest, MarkDirty) {
    GeometryCache cache;
    EntityId id = EntityId::generate();

    CachedGeometry geom;
    geom.entityId = id;
    cache.cacheGeometry(id, geom);

    cache.markDirty(id);

    const auto* cached = cache.getGeometry(id);
    EXPECT_TRUE(cached->isDirty);
}

TEST(GeometryCacheTest, MarkAllDirty) {
    GeometryCache cache;
    EntityId id1 = EntityId::generate();
    EntityId id2 = EntityId::generate();

    CachedGeometry g1, g2;
    g1.entityId = id1;
    g2.entityId = id2;

    cache.cacheGeometry(id1, g1);
    cache.cacheGeometry(id2, g2);

    cache.markAllDirty();

    auto dirty = cache.getDirtyEntities();
    EXPECT_EQ(dirty.size(), 2);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
