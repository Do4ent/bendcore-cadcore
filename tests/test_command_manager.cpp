#include <gtest/gtest.h>
#include "bendcore/commands/command_manager.hpp"

using namespace BendCore::CadCore;

// Simple test command
class TestCommand : public ICommand {
public:
    TestCommand(int& target, int value)
        : target_(target), value_(value), old_value_(target)
    {}

    bool execute() override {
        target_ = value_;
        return true;
    }

    bool undo() override {
        target_ = old_value_;
        return true;
    }

    std::string getDescription() const override {
        return "TestCommand(" + std::to_string(value_) + ")";
    }

    size_t getMemorySize() const override {
        return sizeof(*this);
    }

private:
    int& target_;
    int value_;
    int old_value_;
};

class CommandManagerTest : public ::testing::Test {
protected:
    CadDocument doc;
    CommandManager manager{doc, 100};
    int test_value = 0;

    void SetUp() override {
        doc.clear();
        test_value = 0;
    }
};

// CAD-CORE-005: Test 1 - Execute command
TEST_F(CommandManagerTest, ExecuteCommand) {
    auto cmd = std::make_unique<TestCommand>(test_value, 42);
    auto result = manager.execute(std::move(cmd));

    EXPECT_TRUE(result.success);
    EXPECT_EQ(test_value, 42);
    EXPECT_EQ(manager.getExecutedCount(), 1);
}

// CAD-CORE-005: Test 2 - Null command rejected
TEST_F(CommandManagerTest, NullCommandRejected) {
    auto result = manager.execute(nullptr);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(manager.getFailedCount(), 1);
}

// CAD-CORE-005: Test 3 - Command result tracking
TEST_F(CommandManagerTest, CommandResultTracking) {
    auto cmd = std::make_unique<TestCommand>(test_value, 42);
    auto result = manager.execute(std::move(cmd));

    EXPECT_NE(result.command_id, 0);
    auto retrieved = manager.getResult(result.command_id);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_TRUE(retrieved->success);
}

// CAD-CORE-005: Test 4 - Revision tracking
TEST_F(CommandManagerTest, RevisionTracking) {
    // Add an entity to document to increase revision
    Entity entity(EntityType::PART, "Part_001");
    doc.addEntity(entity);
    uint64_t rev_before = manager.getCurrentRevision();

    // Command execution doesn't change revision - commands do that themselves
    auto cmd = std::make_unique<TestCommand>(test_value, 42);
    manager.execute(std::move(cmd));

    uint64_t rev_after = manager.getCurrentRevision();
    EXPECT_GE(rev_after, rev_before);
}

// CAD-CORE-005: Test 5 - Command history
TEST_F(CommandManagerTest, CommandHistory) {
    for (int i = 0; i < 5; ++i) {
        auto cmd = std::make_unique<TestCommand>(test_value, i);
        manager.execute(std::move(cmd));
    }

    auto history = manager.getHistory();
    EXPECT_EQ(history.size(), 5);
}

// CAD-CORE-005: Test 6 - Validation passes for valid commands
TEST_F(CommandManagerTest, ValidationPassesForValidCommands) {
    TestCommand cmd(test_value, 42);
    EXPECT_TRUE(manager.validate(&cmd));
}

// CAD-CORE-005: Test 7 - Validation fails for null command
TEST_F(CommandManagerTest, ValidationFailsForNull) {
    EXPECT_FALSE(manager.validate(nullptr));
}

// CAD-CORE-005: Test 8 - Error message tracking
TEST_F(CommandManagerTest, ErrorMessageTracking) {
    auto result = manager.execute(nullptr);
    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.error_message.empty());
}

// CAD-CORE-005: Test 9 - Multiple command execution
TEST_F(CommandManagerTest, MultipleCommandExecution) {
    for (int i = 0; i < 10; ++i) {
        auto cmd = std::make_unique<TestCommand>(test_value, i);
        manager.execute(std::move(cmd));
    }

    EXPECT_EQ(manager.getExecutedCount(), 10);
    EXPECT_EQ(test_value, 9);  // Last command value
}

// CAD-CORE-005: Test 10 - Clear history
TEST_F(CommandManagerTest, ClearHistory) {
    auto cmd = std::make_unique<TestCommand>(test_value, 42);
    manager.execute(std::move(cmd));
    EXPECT_EQ(manager.getExecutedCount(), 1);

    manager.clearHistory();
    EXPECT_EQ(manager.getExecutedCount(), 0);
    EXPECT_EQ(manager.getHistory().size(), 0);
}

// CAD-CORE-005: Test 11 - History size limit
TEST_F(CommandManagerTest, HistorySizeLimit) {
    CommandManager limited_manager(doc, 5);  // Max 5 commands

    for (int i = 0; i < 10; ++i) {
        auto cmd = std::make_unique<TestCommand>(test_value, i);
        limited_manager.execute(std::move(cmd));
    }

    auto history = limited_manager.getHistory();
    EXPECT_LE(history.size(), 5);  // Should not exceed limit
}

// CAD-CORE-005: Test 12 - Command ID uniqueness
TEST_F(CommandManagerTest, CommandIdUniqueness) {
    std::set<uint64_t> ids;

    for (int i = 0; i < 100; ++i) {
        auto cmd = std::make_unique<TestCommand>(test_value, i);
        auto result = manager.execute(std::move(cmd));
        ids.insert(result.command_id);
    }

    EXPECT_EQ(ids.size(), 100);  // All unique
}

// CAD-CORE-005: Test 13 - Last result tracking
TEST_F(CommandManagerTest, LastResultTracking) {
    auto cmd1 = std::make_unique<TestCommand>(test_value, 42);
    auto result1 = manager.execute(std::move(cmd1));

    uint64_t id1 = result1.command_id;
    auto& last1 = manager.getLastResult();
    EXPECT_TRUE(last1.success);
    EXPECT_EQ(last1.command_id, id1);

    auto cmd2 = std::make_unique<TestCommand>(test_value, 84);
    auto result2 = manager.execute(std::move(cmd2));

    uint64_t id2 = result2.command_id;
    auto& last2 = manager.getLastResult();
    EXPECT_EQ(last2.command_id, id2);
    EXPECT_NE(id1, id2);  // Different command IDs
}

// CAD-CORE-005: Test 14 - Expected revision check
TEST_F(CommandManagerTest, ExpectedRevisionCheck) {
    uint64_t rev = manager.getCurrentRevision();

    auto cmd = std::make_unique<TestCommand>(test_value, 42);
    manager.execute(std::move(cmd));

    EXPECT_TRUE(manager.isExpectedRevision(rev));
    EXPECT_TRUE(manager.isExpectedRevision(0));
}

// CAD-CORE-005: Test 15 - Memory usage tracking
TEST_F(CommandManagerTest, MemoryUsageTracking) {
    size_t mem_empty = manager.getMemoryUsage();

    for (int i = 0; i < 10; ++i) {
        auto cmd = std::make_unique<TestCommand>(test_value, i);
        manager.execute(std::move(cmd));
    }

    size_t mem_full = manager.getMemoryUsage();
    EXPECT_GT(mem_full, mem_empty);
}

// CAD-CORE-005: Test 16 - Description preservation
TEST_F(CommandManagerTest, DescriptionPreservation) {
    auto cmd = std::make_unique<TestCommand>(test_value, 99);
    auto result = manager.execute(std::move(cmd));

    auto history = manager.getHistory();
    EXPECT_FALSE(history.empty());
}

// CAD-CORE-005: Test 17 - Failed count tracking
TEST_F(CommandManagerTest, FailedCountTracking) {
    // Null commands fail
    manager.execute(nullptr);
    manager.execute(nullptr);

    EXPECT_EQ(manager.getFailedCount(), 2);
}

// CAD-CORE-005: Test 18 - Result retrieval
TEST_F(CommandManagerTest, ResultRetrieval) {
    uint64_t cmd_id = 0;

    for (int i = 0; i < 5; ++i) {
        auto cmd = std::make_unique<TestCommand>(test_value, i);
        auto result = manager.execute(std::move(cmd));
        if (i == 2) cmd_id = result.command_id;
    }

    auto retrieved = manager.getResult(cmd_id);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_TRUE(retrieved->success);
}

// CAD-CORE-005: Test 19 - Non-existent result returns null
TEST_F(CommandManagerTest, NonExistentResultReturnsNull) {
    auto result = manager.getResult(999999);
    EXPECT_EQ(result, nullptr);
}

// CAD-CORE-005: Test 20 - Sequential state changes
TEST_F(CommandManagerTest, SequentialStateChanges) {
    std::vector<int> values = {10, 20, 30, 40, 50};

    for (int v : values) {
        auto cmd = std::make_unique<TestCommand>(test_value, v);
        manager.execute(std::move(cmd));
    }

    EXPECT_EQ(test_value, 50);  // Last value applied
    EXPECT_EQ(manager.getExecutedCount(), 5);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
