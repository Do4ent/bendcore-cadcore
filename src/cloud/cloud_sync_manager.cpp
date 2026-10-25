#include "bendcore/cloud/cloud_sync_manager.hpp"
namespace BendCore::CadCore {
bool CloudSyncManager::syncToCloud() noexcept { synced_ = true; return true; }
}
