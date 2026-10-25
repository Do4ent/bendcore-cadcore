#include <gtest/gtest.h>
#include "bendcore/topology/brep.hpp"

using namespace BendCore::CadCore;

TEST(BRepTest, CreateVertex) {
    BRepManager brep;
    Vector3D point(1.0, 2.0, 3.0);
    EntityId vid = brep.createVertex(point);

    const auto* v = brep.getVertex(vid);
    EXPECT_NE(v, nullptr);
    EXPECT_TRUE(v->point == point);
}

TEST(BRepTest, CreateEdge) {
    BRepManager brep;
    EntityId v1 = brep.createVertex(Vector3D(0, 0, 0));
    EntityId v2 = brep.createVertex(Vector3D(3.0, 4.0, 0));
    EntityId eid = brep.createEdge(v1, v2);

    const auto* e = brep.getEdge(eid);
    EXPECT_NE(e, nullptr);
    EXPECT_DOUBLE_EQ(e->length, 5.0);
}

TEST(BRepTest, CreateFace) {
    BRepManager brep;
    EntityId v1 = brep.createVertex(Vector3D(0, 0, 0));
    EntityId v2 = brep.createVertex(Vector3D(1.0, 0, 0));
    EntityId e1 = brep.createEdge(v1, v2);

    std::vector<EntityId> edges = {e1};
    EntityId fid = brep.createFace(edges);

    const auto* f = brep.getFace(fid);
    EXPECT_NE(f, nullptr);
}

TEST(BRepTest, CreateShell) {
    BRepManager brep;
    EntityId v1 = brep.createVertex(Vector3D(0, 0, 0));
    EntityId v2 = brep.createVertex(Vector3D(1.0, 0, 0));
    EntityId e1 = brep.createEdge(v1, v2);
    EntityId f1 = brep.createFace({e1});
    EntityId sid = brep.createShell({f1});

    const auto* s = brep.getShell(sid);
    EXPECT_NE(s, nullptr);
}

TEST(BRepTest, CreateSolid) {
    BRepManager brep;
    EntityId v1 = brep.createVertex(Vector3D(0, 0, 0));
    EntityId v2 = brep.createVertex(Vector3D(1.0, 0, 0));
    EntityId e1 = brep.createEdge(v1, v2);
    EntityId f1 = brep.createFace({e1});
    EntityId s1 = brep.createShell({f1});
    EntityId sid = brep.createSolid({s1});

    const auto* solid = brep.getSolid(sid);
    EXPECT_NE(solid, nullptr);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
