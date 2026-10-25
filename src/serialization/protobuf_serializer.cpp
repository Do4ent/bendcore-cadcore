#include <bendcore/serialization/protobuf_serializer.h>
#include <fstream>
#include <sstream>

#ifdef HAVE_PROTOBUF
    #include <google/protobuf/util/json_util.h>
    #include <google/protobuf/message.h>
#endif

namespace bendcore::serialization {

std::vector<uint8_t> ProtobufSerializer::serialize_model(
    const std::string&,  // model_name
    const std::string& json_data) {

    // Simple serialization: convert JSON to binary format
    std::vector<uint8_t> result(json_data.begin(), json_data.end());
    return result;
}

std::string ProtobufSerializer::deserialize_model(const std::vector<uint8_t>& binary_data) {
    // Simple deserialization: convert binary back to string
    std::string result(binary_data.begin(), binary_data.end());
    return result;
}

std::vector<uint8_t> ProtobufSerializer::serialize_compressed(const std::string& data) {
    // Placeholder for zlib compression
    std::vector<uint8_t> result;
    result.reserve(data.size());

    // For now, just copy the data
    for (char c : data) {
        result.push_back(static_cast<uint8_t>(c));
    }

    return result;
}

std::string ProtobufSerializer::deserialize_compressed(const std::vector<uint8_t>& data) {
    // Placeholder for zlib decompression
    std::string result(data.begin(), data.end());
    return result;
}

bool ProtobufSerializer::save_to_file(const std::string& filepath,
                                     const std::vector<uint8_t>& data) {
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) return false;

        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<uint8_t> ProtobufSerializer::load_from_file(const std::string& filepath) {
    std::vector<uint8_t> data;
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) return data;

        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);

        data.resize(size);
        file.read(reinterpret_cast<char*>(data.data()), size);
        file.close();
    } catch (...) {
        return data;
    }
    return data;
}

uint32_t ProtobufSerializer::get_schema_version() {
    return 1;  // Version 1.0
}

bool ProtobufSerializer::is_compatible(uint32_t version1, uint32_t version2) {
    // Check major version compatibility
    uint32_t major1 = version1 >> 16;
    uint32_t major2 = version2 >> 16;
    return major1 == major2;
}

std::vector<uint8_t> ProtobufSerializer::migrate_data(
    const std::vector<uint8_t>& old_data,
    uint32_t from_version,
    uint32_t to_version) {

    if (is_compatible(from_version, to_version)) {
        return old_data;
    }

    // Placeholder migration logic
    return old_data;
}

size_t ProtobufSerializer::get_serialized_size(const std::vector<uint8_t>& data) {
    return data.size();
}

double ProtobufSerializer::estimate_compression_ratio(
    const std::vector<uint8_t>& original,
    const std::vector<uint8_t>& compressed) {

    if (original.empty()) return 1.0;
    return static_cast<double>(compressed.size()) / original.size();
}

bool ProtobufSerializer::validate_binary(const std::vector<uint8_t>& data) {
    // Basic validation - check if not empty and has reasonable structure
    return !data.empty() && data.size() < (1024 * 1024 * 100);  // Max 100MB
}

std::vector<uint8_t> ProtobufSerializer::merge_objects(
    const std::vector<std::vector<uint8_t>>& objects) {

    std::vector<uint8_t> result;

    for (const auto& obj : objects) {
        result.insert(result.end(), obj.begin(), obj.end());
    }

    return result;
}

std::vector<std::vector<uint8_t>> ProtobufSerializer::split_object(
    const std::vector<uint8_t>& data,
    size_t num_parts) {

    std::vector<std::vector<uint8_t>> parts;

    if (num_parts == 0) return parts;

    size_t part_size = (data.size() + num_parts - 1) / num_parts;

    for (size_t i = 0; i < num_parts; ++i) {
        size_t start = i * part_size;
        size_t end = std::min((i + 1) * part_size, data.size());

        if (start < data.size()) {
            parts.push_back(std::vector<uint8_t>(data.begin() + start, data.begin() + end));
        }
    }

    return parts;
}

uint32_t ProtobufSerializer::calculate_checksum(const std::vector<uint8_t>& data) {
    // Simple CRC32-like checksum
    uint32_t checksum = 0;
    for (uint8_t byte : data) {
        checksum = (checksum << 1) ^ byte;
    }
    return checksum;
}

bool ProtobufSerializer::verify_checksum(const std::vector<uint8_t>& data,
                                        uint32_t expected_checksum) {
    return calculate_checksum(data) == expected_checksum;
}

double ProtobufSerializer::get_memory_reduction_vs_json(
    const std::vector<uint8_t>& binary,
    const std::string& json_data) {

    if (json_data.empty()) return 0.0;

    double json_size = json_data.size();
    double binary_size = binary.size();

    return ((json_size - binary_size) / json_size) * 100.0;  // Percentage reduction
}

} // namespace bendcore::serialization
