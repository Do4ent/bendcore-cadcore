#include <gtest/gtest.h>
#include "bendcore/simulation/stress_analyzer.hpp"
using namespace BendCore::CadCore;
TEST(StressTest, Create) { StressAnalyzer analyzer(EntityId::generate()); EXPECT_TRUE(analyzer.runSimulation()); }
TEST(StressTest, MaxStress) { StressAnalyzer analyzer(EntityId::generate()); analyzer.runSimulation(); EXPECT_GT(analyzer.getMaxStress(), 0.0); }
int main(int argc, char** argv) { ::testing::InitGoogleTest(&argc, argv); return RUN_ALL_TESTS(); }
