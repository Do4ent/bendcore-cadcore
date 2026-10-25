#include <gtest/gtest.h>
#include "bendcore/api/rest_api_handler.hpp"
using namespace BendCore::CadCore;
TEST(APITest, Create) { RESTAPIHandler handler; EXPECT_EQ(handler.getRequestCount(), 0); }
TEST(APITest, Request) { RESTAPIHandler handler; handler.handleRequest("/api/status"); EXPECT_EQ(handler.getRequestCount(), 1); }
int main(int argc, char** argv) { ::testing::InitGoogleTest(&argc, argv); return RUN_ALL_TESTS(); }
