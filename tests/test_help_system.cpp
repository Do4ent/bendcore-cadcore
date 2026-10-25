#include <gtest/gtest.h>
#include "bendcore/documentation/help_system.hpp"
using namespace BendCore::CadCore;
TEST(HelpTest, Create) { HelpSystem help; EXPECT_EQ(help.getTopicCount(), 0); }
TEST(HelpTest, AddTopic) { HelpSystem help; help.addTopic("Getting Started"); EXPECT_EQ(help.getTopicCount(), 1); }
int main(int argc, char** argv) { ::testing::InitGoogleTest(&argc, argv); return RUN_ALL_TESTS(); }
