#include <gtest/gtest.h>
#include "bendcore/document/document_event_bus.hpp"

using namespace BendCore::CadCore;

class TestEvent : public IEvent {
public:
    explicit TestEvent(const std::string& msg = "") : msg_(msg) {}
    std::string getType() const override { return "TestEvent"; }
    std::string msg_;
};

TEST(DocumentEventBusTest, Subscribe) {
    DocumentEventBus bus;
    int count = 0;
    auto cb = [&count](const std::shared_ptr<IEvent>&) { count++; };

    uint64_t id = bus.subscribe("TestEvent", cb);
    EXPECT_NE(id, 0);
}

TEST(DocumentEventBusTest, DispatchAndProcess) {
    DocumentEventBus bus;
    int count = 0;
    bus.subscribe("TestEvent", [&count](const std::shared_ptr<IEvent>&) { count++; });

    bus.dispatch(std::make_shared<TestEvent>());
    EXPECT_EQ(bus.getQueueSize(), 1);

    bus.processQueue();
    EXPECT_EQ(count, 1);
    EXPECT_EQ(bus.getQueueSize(), 0);
}

TEST(DocumentEventBusTest, MultipleSubscribers) {
    DocumentEventBus bus;
    int count1 = 0, count2 = 0;

    bus.subscribe("TestEvent", [&count1](const std::shared_ptr<IEvent>&) { count1++; });
    bus.subscribe("TestEvent", [&count2](const std::shared_ptr<IEvent>&) { count2++; });

    bus.dispatch(std::make_shared<TestEvent>());
    bus.processQueue();

    EXPECT_EQ(count1, 1);
    EXPECT_EQ(count2, 1);
}

TEST(DocumentEventBusTest, Unsubscribe) {
    DocumentEventBus bus;
    int count = 0;
    auto cb = [&count](const std::shared_ptr<IEvent>&) { count++; };

    uint64_t id = bus.subscribe("TestEvent", cb);
    bus.unsubscribe(id);

    bus.dispatch(std::make_shared<TestEvent>());
    bus.processQueue();
    EXPECT_EQ(count, 0);
}

TEST(DocumentEventBusTest, ClearQueue) {
    DocumentEventBus bus;
    bus.subscribe("TestEvent", [](const std::shared_ptr<IEvent>&) {});

    bus.dispatch(std::make_shared<TestEvent>());
    bus.dispatch(std::make_shared<TestEvent>());
    EXPECT_EQ(bus.getQueueSize(), 2);

    bus.clearQueue();
    EXPECT_EQ(bus.getQueueSize(), 0);
}

TEST(DocumentEventBusTest, ProcessedCount) {
    DocumentEventBus bus;
    bus.subscribe("TestEvent", [](const std::shared_ptr<IEvent>&) {});

    bus.dispatch(std::make_shared<TestEvent>());
    bus.dispatch(std::make_shared<TestEvent>());

    uint64_t before = bus.getProcessedCount();
    bus.processQueue();
    uint64_t after = bus.getProcessedCount();

    EXPECT_GT(after, before);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
