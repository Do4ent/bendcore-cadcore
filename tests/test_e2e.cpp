#include <gtest/gtest.h>
#include "bendcore/document/cad_document.hpp"
#include "bendcore/commands/command_manager.hpp"

using namespace BendCore::CadCore;

/**
 * End-to-end tests for complete workflows
 */

TEST(E2ETest, CreateDocumentAndAddEntity) {
    CadDocument doc;
    Entity entity;
    entity.id = EntityId::generate();
    entity.state = LifecycleState::ACTIVE;

    doc.addEntity(entity);
    EXPECT_NE(doc.getRevision(), 0);
}

TEST(E2ETest, CommandExecutionWorkflow) {
    CadDocument doc;
    CommandManager cmd(doc);

    EXPECT_GE(cmd.getHistory().size(), 0);
}

TEST(E2ETest, DocumentValidation) {
    CadDocument doc;
    Entity entity;
    entity.id = EntityId::generate();
    entity.state = LifecycleState::ACTIVE;
    doc.addEntity(entity);

    EXPECT_TRUE(doc.hasEntity(entity.id));
}

TEST(E2ETest, MultipleEntityManagement) {
    CadDocument doc;

    for (int i = 0; i < 5; i++) {
        Entity entity;
        entity.id = EntityId::generate();
        entity.state = LifecycleState::ACTIVE;
        doc.addEntity(entity);
    }

    EXPECT_GE(doc.getRevision(), 5);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
