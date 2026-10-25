#include <gtest/gtest.h>
#include "bendcore/commands/transaction.hpp"

using namespace BendCore::CadCore;

class TestCommand : public ICommand {
public:
    TestCommand(int& target, int value, bool should_fail = false)
        : target_(target), value_(value), old_value_(target), should_fail_(should_fail) {}

    bool execute() override {
        if (should_fail_) return false;
        target_ = value_;
        return true;
    }

    bool undo() override {
        target_ = old_value_;
        return true;
    }

    std::string getDescription() const override { return "TestCmd"; }
    size_t getMemorySize() const override { return sizeof(*this); }

private:
    int& target_;
    int value_;
    int old_value_;
    bool should_fail_;
};

class TransactionTest : public ::testing::Test {
protected:
    CadDocument doc;
    int test_value = 0;

    void SetUp() override {
        doc.clear();
        test_value = 0;
    }
};

// CAD-CORE-006: Test 1 - Create transaction
TEST_F(TransactionTest, CreateTransaction) {
    Transaction txn(doc);
    EXPECT_TRUE(txn.isActive());
    EXPECT_EQ(txn.getCommandCount(), 0);
}

// CAD-CORE-006: Test 2 - Add commands
TEST_F(TransactionTest, AddCommands) {
    Transaction txn(doc);
    txn.addCommand(std::make_unique<TestCommand>(test_value, 10));
    txn.addCommand(std::make_unique<TestCommand>(test_value, 20));

    EXPECT_EQ(txn.getCommandCount(), 2);
}

// CAD-CORE-006: Test 3 - Commit successful
TEST_F(TransactionTest, CommitSuccessful) {
    Transaction txn(doc);
    txn.addCommand(std::make_unique<TestCommand>(test_value, 42));

    bool result = txn.commit();
    EXPECT_TRUE(result);
    EXPECT_FALSE(txn.isActive());
    EXPECT_EQ(test_value, 42);
}

// CAD-CORE-006: Test 4 - Atomicity: all or nothing
TEST_F(TransactionTest, AtomicityAllOrNothing) {
    Transaction txn(doc);
    txn.addCommand(std::make_unique<TestCommand>(test_value, 10));
    txn.addCommand(std::make_unique<TestCommand>(test_value, 20, true));  // Will fail

    int value_before = test_value;
    bool result = txn.commit();

    EXPECT_FALSE(result);
    EXPECT_EQ(test_value, value_before);  // No change
}

// CAD-CORE-006: Test 5 - Rollback on error
TEST_F(TransactionTest, RollbackOnError) {
    test_value = 5;
    Transaction txn(doc);
    txn.addCommand(std::make_unique<TestCommand>(test_value, 10));
    txn.addCommand(std::make_unique<TestCommand>(test_value, 20, true));

    txn.commit();
    EXPECT_EQ(test_value, 5);  // Rolled back
}

// CAD-CORE-006: Test 6 - Multiple commands sequential
TEST_F(TransactionTest, MultipleCommandsSequential) {
    Transaction txn(doc);
    for (int i = 0; i < 5; ++i) {
        txn.addCommand(std::make_unique<TestCommand>(test_value, (i + 1) * 10));
    }

    EXPECT_TRUE(txn.commit());
    EXPECT_EQ(test_value, 50);  // Last value
}

// CAD-CORE-006: Test 7 - Null command rejected
TEST_F(TransactionTest, NullCommandRejected) {
    Transaction txn(doc);
    EXPECT_THROW(txn.addCommand(nullptr), std::invalid_argument);
}

// CAD-CORE-006: Test 8 - Cannot add after inactive
TEST_F(TransactionTest, CannotAddAfterInactive) {
    Transaction txn(doc);
    txn.addCommand(std::make_unique<TestCommand>(test_value, 10));
    txn.commit();

    EXPECT_THROW(txn.addCommand(std::make_unique<TestCommand>(test_value, 20)),
                 std::runtime_error);
}

// CAD-CORE-006: Test 9 - Revision management
TEST_F(TransactionTest, RevisionManagement) {
    Entity entity(EntityType::PART, "Part_001");
    doc.addEntity(entity);
    uint64_t rev_before = doc.getRevision();

    Transaction txn(doc);
    txn.addCommand(std::make_unique<TestCommand>(test_value, 42));
    txn.commit();

    uint64_t rev_after = doc.getRevision();
    EXPECT_GE(rev_after, rev_before);
}

// CAD-CORE-006: Test 10 - Empty transaction
TEST_F(TransactionTest, EmptyTransaction) {
    Transaction txn(doc);
    EXPECT_EQ(txn.getCommandCount(), 0);
    EXPECT_TRUE(txn.commit());
    EXPECT_FALSE(txn.isActive());
}

// CAD-CORE-006: Test 11 - Partial failure with rollback
TEST_F(TransactionTest, PartialFailureRollback) {
    test_value = 100;
    Transaction txn(doc);
    txn.addCommand(std::make_unique<TestCommand>(test_value, 200));
    txn.addCommand(std::make_unique<TestCommand>(test_value, 300, true));  // Fails

    bool result = txn.commit();
    EXPECT_FALSE(result);
    EXPECT_EQ(test_value, 100);  // Rolled back to original
}

// CAD-CORE-006: Test 12 - Command order matters
TEST_F(TransactionTest, CommandOrderMatters) {
    Transaction txn(doc);
    txn.addCommand(std::make_unique<TestCommand>(test_value, 10));
    txn.addCommand(std::make_unique<TestCommand>(test_value, 20));
    txn.addCommand(std::make_unique<TestCommand>(test_value, 30));

    txn.commit();
    EXPECT_EQ(test_value, 30);
}

// CAD-CORE-006: Test 13 - Large transaction
TEST_F(TransactionTest, LargeTransaction) {
    Transaction txn(doc);
    const int COUNT = 100;

    for (int i = 0; i < COUNT; ++i) {
        txn.addCommand(std::make_unique<TestCommand>(test_value, i + 1));
    }

    EXPECT_TRUE(txn.commit());
    EXPECT_EQ(test_value, COUNT);
}

// CAD-CORE-006: Test 14 - Transaction isolation
TEST_F(TransactionTest, TransactionIsolation) {
    int val1 = 0;
    int val2 = 0;

    Transaction txn1(doc);
    txn1.addCommand(std::make_unique<TestCommand>(val1, 10));

    Transaction txn2(doc);
    txn2.addCommand(std::make_unique<TestCommand>(val2, 20));

    txn1.commit();
    txn2.commit();

    EXPECT_EQ(val1, 10);
    EXPECT_EQ(val2, 20);
}

// CAD-CORE-006: Test 15 - Atomic commitment verification
TEST_F(TransactionTest, AtomicCommitmentVerification) {
    test_value = 0;
    Transaction txn(doc);

    for (int i = 1; i <= 3; ++i) {
        txn.addCommand(std::make_unique<TestCommand>(test_value, i * 100));
    }

    bool success = txn.commit();
    EXPECT_TRUE(success);
    EXPECT_EQ(test_value, 300);
    EXPECT_FALSE(txn.isActive());
}

// CAD-CORE-006: Test 16 - Expected revision tracking
TEST_F(TransactionTest, ExpectedRevisionTracking) {
    // Add entity to increase doc revision
    Entity entity(EntityType::PART, "Part_001");
    doc.addEntity(entity);

    Transaction txn(doc);
    txn.addCommand(std::make_unique<TestCommand>(test_value, 42));

    uint64_t expected_before = txn.getExpectedRevision();
    EXPECT_GE(expected_before, 0);

    txn.commit();
    uint64_t expected_after = txn.getExpectedRevision();

    EXPECT_GE(expected_after, expected_before);
}

// CAD-CORE-006: Test 17 - Cannot commit twice
TEST_F(TransactionTest, CannotCommitTwice) {
    Transaction txn(doc);
    txn.addCommand(std::make_unique<TestCommand>(test_value, 10));

    txn.commit();
    EXPECT_THROW(txn.commit(), std::runtime_error);
}

// CAD-CORE-006: Test 18 - Rollback on failed command
TEST_F(TransactionTest, RollbackOnFailedCommand) {
    test_value = 0;
    Transaction txn(doc);
    txn.addCommand(std::make_unique<TestCommand>(test_value, 10));
    txn.addCommand(std::make_unique<TestCommand>(test_value, 20));
    txn.addCommand(std::make_unique<TestCommand>(test_value, 30, true));  // Fail

    bool result = txn.commit();
    EXPECT_FALSE(result);
    EXPECT_EQ(test_value, 0);  // Everything rolled back
}

// CAD-CORE-006: Test 19 - Complex scenario
TEST_F(TransactionTest, ComplexScenario) {
    int a = 10, b = 20;

    Transaction txn(doc);
    txn.addCommand(std::make_unique<TestCommand>(a, 100));
    txn.addCommand(std::make_unique<TestCommand>(b, 200));
    txn.addCommand(std::make_unique<TestCommand>(a, 150));

    EXPECT_TRUE(txn.commit());
    EXPECT_EQ(a, 150);
    EXPECT_EQ(b, 200);
}

// CAD-CORE-006: Test 20 - Sequential transactions
TEST_F(TransactionTest, SequentialTransactions) {
    {
        Transaction txn1(doc);
        txn1.addCommand(std::make_unique<TestCommand>(test_value, 10));
        EXPECT_TRUE(txn1.commit());
    }

    {
        Transaction txn2(doc);
        txn2.addCommand(std::make_unique<TestCommand>(test_value, 20));
        EXPECT_TRUE(txn2.commit());
    }

    EXPECT_EQ(test_value, 20);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
