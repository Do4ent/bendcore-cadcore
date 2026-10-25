#include <gtest/gtest.h>
#include "bendcore/serialization/serializer.hpp"

using namespace BendCore::CadCore;

TEST(SerializerTest, CreateBinarySerializer) {
    Serializer ser(SerializationFormat::BINARY);
    EXPECT_EQ(ser.getFormat(), SerializationFormat::BINARY);
}

TEST(SerializerTest, CreateJsonSerializer) {
    Serializer ser(SerializationFormat::JSON);
    EXPECT_EQ(ser.getFormat(), SerializationFormat::JSON);
}

TEST(SerializerTest, SetFormat) {
    Serializer ser(SerializationFormat::BINARY);
    ser.setFormat(SerializationFormat::JSON);
    EXPECT_EQ(ser.getFormat(), SerializationFormat::JSON);
}

TEST(SerializerTest, SerializeDocument) {
    Serializer ser(SerializationFormat::BINARY);
    CadDocument doc;

    auto data = ser.serialize(doc);
    EXPECT_GT(data.size(), 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
