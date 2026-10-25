#include <gtest/gtest.h>
#include "bendcore/project/project_format.hpp"

using namespace BendCore::CadCore;

TEST(ProjectFormatTest, CreateProject) {
    ProjectFormat pf;
    ProjectMetadata meta;
    meta.name = "MyProject";
    meta.version = "1.0";

    EXPECT_TRUE(pf.createProject(meta));
    EXPECT_EQ(pf.getMetadata().name, "MyProject");
}

TEST(ProjectFormatTest, SetMetadata) {
    ProjectFormat pf;
    ProjectMetadata meta;
    meta.name = "Project";
    meta.author = "John Doe";

    pf.setMetadata(meta);
    EXPECT_EQ(pf.getMetadata().author, "John Doe");
}

TEST(ProjectFormatTest, AddResource) {
    ProjectFormat pf;
    pf.addResource("geom1", "/path/to/geometry.dat");

    EXPECT_EQ(pf.getResource("geom1"), "/path/to/geometry.dat");
}

TEST(ProjectFormatTest, Validate) {
    ProjectFormat pf;
    ProjectMetadata meta;
    meta.name = "Project";
    meta.version = "1.0";
    pf.setMetadata(meta);

    EXPECT_TRUE(pf.validate());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
