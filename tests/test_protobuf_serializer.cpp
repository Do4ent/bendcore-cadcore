#include <gtest/gtest.h>
#include <bendcore/serialization/protobuf_serializer.h>
#include <fstream>
#include <sstream>

using namespace bendcore::serialization;

class ProtobufSerializerTest : public ::testing::Test {
protected:
    std::string test_file = "/tmp/test_data.pb";

    void TearDown() override {
        std::remove(test_file.c_str());
    }
};

TEST_F(ProtobufSerializerTest, Serialize) {
    std::string json_data = "{\"name\": \"test\", \"value\": 42}";

    auto binary = ProtobufSerializer::serialize_model("test_model", json_data);

    EXPECT_GT(binary.size(), 0);
    EXPECT_LE(binary.size(), json_data.size() + 10);  // Should not grow much
}

TEST_F(ProtobufSerializerTest, SerializeDeserializeRoundTrip) {
    std::string original = "{\"test\": \"data\"}";

    auto binary = ProtobufSerializer::serialize_model("model", original);
    std::string deserialized = ProtobufSerializer::deserialize_model(binary);

    EXPECT_EQ(deserialized, original);
}

TEST_F(ProtobufSerializerTest, SerializeCompressed) {
    std::string data = "test data";

    auto compressed = ProtobufSerializer::serialize_compressed(data);

    EXPECT_GT(compressed.size(), 0);
}

TEST_F(ProtobufSerializerTest, SerializeDeserializeCompressed) {
    std::string original = "test data";

    auto compressed = ProtobufSerializer::serialize_compressed(original);
    std::string decompressed = ProtobufSerializer::deserialize_compressed(compressed);

    EXPECT_EQ(decompressed, original);
}

TEST_F(ProtobufSerializerTest, SaveToFile) {
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};

    bool success = ProtobufSerializer::save_to_file(test_file, data);

    EXPECT_TRUE(success);
    EXPECT_TRUE(std::ifstream(test_file).good());
}

TEST_F(ProtobufSerializerTest, LoadFromFile) {
    std::vector<uint8_t> original = {1, 2, 3, 4, 5};

    ProtobufSerializer::save_to_file(test_file, original);
    auto loaded = ProtobufSerializer::load_from_file(test_file);

    EXPECT_EQ(loaded, original);
}

TEST_F(ProtobufSerializerTest, LoadFromNonexistentFile) {
    auto data = ProtobufSerializer::load_from_file("/nonexistent/file.pb");

    EXPECT_TRUE(data.empty());
}

TEST_F(ProtobufSerializerTest, GetSchemaVersion) {
    uint32_t version = ProtobufSerializer::get_schema_version();

    EXPECT_GE(version, 1);
}

TEST_F(ProtobufSerializerTest, IsCompatible) {
    uint32_t v1 = 0x00010000;  // 1.0
    uint32_t v2 = 0x00010001;  // 1.1
    uint32_t v3 = 0x00020000;  // 2.0

    EXPECT_TRUE(ProtobufSerializer::is_compatible(v1, v2));
    EXPECT_FALSE(ProtobufSerializer::is_compatible(v1, v3));
}

TEST_F(ProtobufSerializerTest, GetSerializedSize) {
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};

    size_t size = ProtobufSerializer::get_serialized_size(data);

    EXPECT_EQ(size, 5);
}

TEST_F(ProtobufSerializerTest, ValidateBinary) {
    std::vector<uint8_t> valid_data = {1, 2, 3};
    std::vector<uint8_t> empty_data;

    EXPECT_TRUE(ProtobufSerializer::validate_binary(valid_data));
    EXPECT_FALSE(ProtobufSerializer::validate_binary(empty_data));
}

TEST_F(ProtobufSerializerTest, MergeObjects) {
    std::vector<std::vector<uint8_t>> objects = {
        {1, 2, 3},
        {4, 5},
        {6, 7, 8, 9}
    };

    auto merged = ProtobufSerializer::merge_objects(objects);

    EXPECT_EQ(merged.size(), 9);
    EXPECT_EQ(merged[0], 1);
    EXPECT_EQ(merged[3], 4);
    EXPECT_EQ(merged[5], 6);
}

TEST_F(ProtobufSerializerTest, SplitObject) {
    std::vector<uint8_t> data = {1, 2, 3, 4, 5, 6, 7, 8};

    auto parts = ProtobufSerializer::split_object(data, 3);

    EXPECT_LE(parts.size(), 3);
    EXPECT_GT(parts.size(), 0);

    // Verify all parts together equal original
    std::vector<uint8_t> reconstructed;
    for (const auto& part : parts) {
        reconstructed.insert(reconstructed.end(), part.begin(), part.end());
    }
    EXPECT_EQ(reconstructed, data);
}

TEST_F(ProtobufSerializerTest, SplitObjectIntoOne) {
    std::vector<uint8_t> data = {1, 2, 3};

    auto parts = ProtobufSerializer::split_object(data, 1);

    EXPECT_EQ(parts.size(), 1);
    EXPECT_EQ(parts[0], data);
}

TEST_F(ProtobufSerializerTest, CalculateChecksum) {
    std::vector<uint8_t> data1 = {1, 2, 3};
    std::vector<uint8_t> data2 = {1, 2, 3};
    std::vector<uint8_t> data3 = {1, 2, 4};

    uint32_t cs1 = ProtobufSerializer::calculate_checksum(data1);
    uint32_t cs2 = ProtobufSerializer::calculate_checksum(data2);
    uint32_t cs3 = ProtobufSerializer::calculate_checksum(data3);

    EXPECT_EQ(cs1, cs2);
    EXPECT_NE(cs1, cs3);
}

TEST_F(ProtobufSerializerTest, VerifyChecksum) {
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    uint32_t checksum = ProtobufSerializer::calculate_checksum(data);

    EXPECT_TRUE(ProtobufSerializer::verify_checksum(data, checksum));
    EXPECT_FALSE(ProtobufSerializer::verify_checksum(data, checksum + 1));
}

TEST_F(ProtobufSerializerTest, EstimateCompressionRatio) {
    std::vector<uint8_t> original = {1, 2, 3, 4, 5};
    std::vector<uint8_t> compressed = {1, 2, 3};  // Artificially small

    double ratio = ProtobufSerializer::estimate_compression_ratio(original, compressed);

    EXPECT_NEAR(ratio, 0.6, 0.01);
}

TEST_F(ProtobufSerializerTest, GetMemoryReductionVsJson) {
    std::string json = "{\"name\": \"test\", \"value\": 42, \"description\": \"long text here\"}";
    std::vector<uint8_t> binary = {1, 2, 3};  // Much smaller

    double reduction = ProtobufSerializer::get_memory_reduction_vs_json(binary, json);

    EXPECT_GT(reduction, 0.0);  // Should show reduction
    EXPECT_LT(reduction, 100.0);
}

TEST_F(ProtobufSerializerTest, MigrateData) {
    std::vector<uint8_t> old_data = {1, 2, 3};
    uint32_t from_v = 0x00010000;
    uint32_t to_v = 0x00010001;

    auto migrated = ProtobufSerializer::migrate_data(old_data, from_v, to_v);

    // Compatible versions should return same data
    EXPECT_EQ(migrated, old_data);
}
