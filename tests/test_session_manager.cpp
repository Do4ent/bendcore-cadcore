#include <gtest/gtest.h>
#include "bendcore/collaboration/session_manager.hpp"
using namespace BendCore::CadCore;
TEST(SessionTest, Create) { SessionManager sess(EntityId::generate()); EXPECT_TRUE(sess.startSession()); }
TEST(SessionTest, AddUser) { SessionManager sess(EntityId::generate()); sess.addUser("user1"); EXPECT_EQ(sess.getUserCount(), 1); }
int main(int argc, char** argv) { ::testing::InitGoogleTest(&argc, argv); return RUN_ALL_TESTS(); }
