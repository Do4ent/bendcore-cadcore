#include <gtest/gtest.h>
#include "bendcore/parameters/parameter_store.hpp"

using namespace BendCore::CadCore;

TEST(ParameterStoreTest, SetAndGet) {
    ParameterStore store;
    store.set("length", 100.0);
    EXPECT_EQ(std::get<double>(store.get("length")), 100.0);
}

TEST(ParameterStoreTest, Has) {
    ParameterStore store;
    store.set("width", 50.0);
    EXPECT_TRUE(store.has("width"));
    EXPECT_FALSE(store.has("height"));
}

TEST(ParameterStoreTest, GetAll) {
    ParameterStore store;
    store.set("a", 1.0);
    store.set("b", 2.0);
    auto all = store.getAll();
    EXPECT_EQ(all.size(), 2);
}

TEST(ParameterStoreTest, Units) {
    EXPECT_DOUBLE_EQ(Units::toMM(1.0, "cm"), 10.0);
    EXPECT_DOUBLE_EQ(Units::toMM(1.0, "m"), 1000.0);
    EXPECT_DOUBLE_EQ(Units::fromMM(25.4, "in"), 1.0);
}

TEST(ParameterStoreTest, UnitsValid) {
    EXPECT_TRUE(Units::isValid("mm"));
    EXPECT_TRUE(Units::isValid("cm"));
    EXPECT_FALSE(Units::isValid("xyz"));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
