#include <gtest/gtest.h>
#include "bendcore/commands/history_stack.hpp"

using namespace BendCore::CadCore;

class TestCmd : public ICommand {
public:
    TestCmd(int& t, int v) : target_(t), val_(v), old_(t) {}
    bool execute() override { target_ = val_; return true; }
    bool undo() override { target_ = old_; return true; }
    std::string getDescription() const override { return "Cmd"; }
    size_t getMemorySize() const override { return 32; }
private:
    int& target_; int val_; int old_;
};

class HistoryStackTest : public ::testing::Test {
protected:
    HistoryStack stack{100, 1000000};
    int val = 0;
};

TEST_F(HistoryStackTest, PushAndUndo) {
    auto cmd = std::make_unique<TestCmd>(val, 42);
    stack.push(std::move(cmd));
    EXPECT_EQ(stack.size(), 1);
    EXPECT_TRUE(stack.canUndo());
}

TEST_F(HistoryStackTest, UndoRedo) {
    auto cmd = std::make_unique<TestCmd>(val, 42);
    stack.push(std::move(cmd));
    EXPECT_TRUE(stack.undo());
    EXPECT_TRUE(stack.canRedo());
    EXPECT_TRUE(stack.redo());
}

TEST_F(HistoryStackTest, MultipleUndoRedo) {
    for (int i = 1; i <= 5; ++i) {
        auto cmd = std::make_unique<TestCmd>(val, i * 10);
        stack.push(std::move(cmd));
    }
    EXPECT_EQ(stack.size(), 5);

    for (int i = 0; i < 3; ++i) stack.undo();
    EXPECT_EQ(stack.getUndoCount(), 2);
    EXPECT_EQ(stack.getRedoCount(), 3);
}

TEST_F(HistoryStackTest, ClearHistory) {
    stack.push(std::make_unique<TestCmd>(val, 42));
    stack.clear();
    EXPECT_EQ(stack.size(), 0);
    EXPECT_FALSE(stack.canUndo());
}

TEST_F(HistoryStackTest, CanUndoRedo) {
    EXPECT_FALSE(stack.canUndo());
    stack.push(std::make_unique<TestCmd>(val, 42));
    EXPECT_TRUE(stack.canUndo());
    stack.undo();
    EXPECT_TRUE(stack.canRedo());
}

TEST_F(HistoryStackTest, BranchPoint) {
    stack.push(std::make_unique<TestCmd>(val, 10));
    stack.push(std::make_unique<TestCmd>(val, 20));
    stack.createBranchPoint();
    EXPECT_EQ(stack.getRedoCount(), 0);
}

TEST_F(HistoryStackTest, MemoryUsage) {
    for (int i = 0; i < 10; ++i) {
        stack.push(std::make_unique<TestCmd>(val, i));
    }
    EXPECT_GT(stack.getMemoryUsage(), 0);
}

TEST_F(HistoryStackTest, History) {
    stack.push(std::make_unique<TestCmd>(val, 42));
    auto h = stack.getHistory();
    EXPECT_EQ(h.size(), 1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
