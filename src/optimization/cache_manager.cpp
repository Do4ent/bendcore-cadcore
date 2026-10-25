#include "bendcore/optimization/cache_manager.hpp"
namespace BendCore::CadCore {
bool CacheManager::cacheGeometry(const EntityId& id) noexcept { cache_[id] = true; return true; }
bool CacheManager::getCachedGeometry(const EntityId& id) noexcept { return cache_.count(id) > 0; }
double CacheManager::getHitRate() const noexcept { return (hits_ + misses_ > 0) ? (double)hits_ / (hits_ + misses_) : 0.0; }
}
