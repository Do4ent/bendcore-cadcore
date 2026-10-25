#include <gtest/gtest.h>
#include "bendcore/persistence/atomic_save.hpp"
#include <filesystem>

namespace fs = std::filesystem;
using namespace BendCore::CadCore;

TEST(AtomicSaveTest, CreateAtomicSave) {
    std::string tempFile = "/tmp/test_atomic.dat";
    AtomicSave save(tempFile);

    EXPECT_EQ(save.getFilePath(), tempFile);
}

TEST(AtomicSaveTest, Begin) {
    std::string tempFile = "/tmp/test_atomic2.dat";
    AtomicSave save(tempFile);

    EXPECT_TRUE(save.begin());
    EXPECT_TRUE(save.isInProgress());
}

TEST(AtomicSaveTest, Write) {
    std::string tempFile = "/tmp/test_atomic3.dat";
    AtomicSave save(tempFile);

    EXPECT_TRUE(save.begin());

    uint8_t data[] = {1, 2, 3, 4, 5};
    EXPECT_TRUE(save.write(data, sizeof(data)));
}

TEST(AtomicSaveTest, GetTempPath) {
    std::string file = "/tmp/test.dat";
    AtomicSave save(file);

    EXPECT_EQ(save.getTempFilePath(), file + ".tmp");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
