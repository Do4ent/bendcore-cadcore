#include <bendcore/io/model_importer.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstring>
#include <limits>
#include <algorithm>
#include <cmath>

// Try to include Assimp if available
#ifdef HAVE_ASSIMP
    #include <assimp/Importer.hpp>
    #include <assimp/scene.h>
    #include <assimp/postprocess.h>
#endif

namespace bendcore::io {

size_t Model3D::total_vertices() const {
    size_t total = 0;
    for (const auto& mesh : meshes) {
        total += mesh.vertex_count();
    }
    return total;
}

size_t Model3D::total_faces() const {
    size_t total = 0;
    for (const auto& mesh : meshes) {
        total += mesh.face_count();
    }
    return total;
}

std::unique_ptr<Model3D> ModelImporter::load_model(const std::string& filepath) {
    return load_model_with_options(filepath, true, true, true);
}

std::unique_ptr<Model3D> ModelImporter::load_model_with_options(
    const std::string& filepath,
    bool,  // triangulate
    bool,  // remove_duplicates
    bool) {  // generate_normals

    auto model = std::make_unique<Model3D>();
    model->filename = filepath;

    // Extract file extension for format
    size_t dot_pos = filepath.find_last_of(".");
    if (dot_pos != std::string::npos) {
        model->format = filepath.substr(dot_pos + 1);
    }

    #ifdef HAVE_ASSIMP
    Assimp::Importer importer;

    unsigned int post_process_flags = aiProcess_Triangulate | aiProcess_RemoveDuplicates | aiProcess_GenNormals;

    const aiScene* scene = importer.ReadFile(filepath, post_process_flags);

    if (!scene || !scene->HasMeshes()) {
        throw std::runtime_error("Failed to load model: " + filepath);
    }

    // Load meshes
    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        const aiMesh* ai_mesh = scene->mMeshes[i];
        Mesh mesh;
        mesh.name = ai_mesh->mName.C_Str();

        // Load vertices
        for (unsigned int j = 0; j < ai_mesh->mNumVertices; ++j) {
            const aiVector3D& v = ai_mesh->mVertices[j];
            mesh.vertices.push_back(Mesh::Vertex(v.x, v.y, v.z));
        }

        // Load normals
        if (ai_mesh->HasNormals()) {
            for (unsigned int j = 0; j < ai_mesh->mNumVertices; ++j) {
                const aiVector3D& n = ai_mesh->mNormals[j];
                mesh.normals.push_back(Mesh::Normal{n.x, n.y, n.z});
            }
        }

        // Load faces
        for (unsigned int j = 0; j < ai_mesh->mNumFaces; ++j) {
            const aiFace& face = ai_mesh->mFaces[j];
            if (face.mNumIndices == 3) {  // Triangles only
                mesh.faces.push_back(Mesh::Face(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
            }
        }

        model->meshes.push_back(std::move(mesh));
    }
    #else
    throw std::runtime_error("Assimp library not available - model loading disabled.");
    #endif

    return model;
}

std::vector<std::string> ModelImporter::supported_formats() {
    return {
        "fbx", "dae", "x3d", "obj", "gltf", "glb", "stl",
        "3ds", "ase", "ply", "dxf", "lwo", "lws", "lxo",
        "ac", "ms3d", "cob", "scn", "mesh", "mdl", "md5",
        "nff", "raw", "smd", "vta", "terragen", "ogre"
    };
}

bool ModelImporter::is_format_supported(const std::string& format) {
    auto supported = supported_formats();
    std::string lower_format = format;
    std::transform(lower_format.begin(), lower_format.end(),
                  lower_format.begin(), ::tolower);

    return std::find(supported.begin(), supported.end(), lower_format) != supported.end();
}

bool ModelImporter::validate_model_file(const std::string&) {
    #ifdef HAVE_ASSIMP
    try {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate);
        return scene && scene->HasMeshes();
    } catch (...) {
        return false;
    }
    #else
    return false;
    #endif
}

std::string ModelImporter::get_model_info(const std::string&) {
    #ifdef HAVE_ASSIMP
    try {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate);

        if (!scene) return "Invalid model";

        std::stringstream ss;
        ss << "Meshes: " << scene->mNumMeshes << "\n";
        ss << "Vertices: " << (scene->HasMeshes() ? scene->mMeshes[0]->mNumVertices : 0) << "\n";
        ss << "Faces: " << (scene->HasMeshes() ? scene->mMeshes[0]->mNumFaces : 0) << "\n";

        return ss.str();
    } catch (...) {
        return "Error reading model";
    }
    #else
    return "Assimp not available";
    #endif
}

bool ModelImporter::convert_model(
    const std::string&,  // input_file
    const std::string&) {  // output_file
    return false;  // Not supported without Assimp
}

void ModelImporter::scale_model(Model3D& model, float scale_factor) {
    for (auto& mesh : model.meshes) {
        for (auto& vertex : mesh.vertices) {
            vertex.x *= scale_factor;
            vertex.y *= scale_factor;
            vertex.z *= scale_factor;
        }
    }
}

void ModelImporter::translate_model(Model3D& model, float dx, float dy, float dz) {
    for (auto& mesh : model.meshes) {
        for (auto& vertex : mesh.vertices) {
            vertex.x += dx;
            vertex.y += dy;
            vertex.z += dz;
        }
    }
}

void ModelImporter::get_bounds(
    const Model3D& model,
    float& min_x, float& max_x,
    float& min_y, float& max_y,
    float& min_z, float& max_z) {

    min_x = min_y = min_z = std::numeric_limits<float>::max();
    max_x = max_y = max_z = std::numeric_limits<float>::lowest();

    for (const auto& mesh : model.meshes) {
        for (const auto& vertex : mesh.vertices) {
            min_x = std::min(min_x, vertex.x);
            max_x = std::max(max_x, vertex.x);
            min_y = std::min(min_y, vertex.y);
            max_y = std::max(max_y, vertex.y);
            min_z = std::min(min_z, vertex.z);
            max_z = std::max(max_z, vertex.z);
        }
    }
}

std::vector<float> ModelImporter::get_vertex_data(const Model3D& model) {
    std::vector<float> data;
    for (const auto& mesh : model.meshes) {
        for (const auto& vertex : mesh.vertices) {
            data.push_back(vertex.x);
            data.push_back(vertex.y);
            data.push_back(vertex.z);
        }
    }
    return data;
}

std::vector<unsigned int> ModelImporter::get_face_indices(const Model3D& model) {
    std::vector<unsigned int> indices;
    unsigned int vertex_offset = 0;

    for (const auto& mesh : model.meshes) {
        for (const auto& face : mesh.faces) {
            indices.push_back(face.v1 + vertex_offset);
            indices.push_back(face.v2 + vertex_offset);
            indices.push_back(face.v3 + vertex_offset);
        }
        vertex_offset += mesh.vertex_count();
    }
    return indices;
}

void ModelImporter::calculate_statistics(const Model3D& model,
    float& avg_face_area,
    float& min_face_area,
    float& max_face_area) {

    avg_face_area = 0.0f;
    min_face_area = std::numeric_limits<float>::max();
    max_face_area = 0.0f;

    size_t face_count = 0;

    for (const auto& mesh : model.meshes) {
        for (const auto& face : mesh.faces) {
            if (face.v1 < mesh.vertices.size() &&
                face.v2 < mesh.vertices.size() &&
                face.v3 < mesh.vertices.size()) {

                const auto& v1 = mesh.vertices[face.v1];
                const auto& v2 = mesh.vertices[face.v2];
                const auto& v3 = mesh.vertices[face.v3];

                float a = v1.x - v2.x, b = v1.y - v2.y, c = v1.z - v2.z;
                float d = v1.x - v3.x, e = v1.y - v3.y, f = v1.z - v3.z;

                float area = 0.5f * std::sqrt(
                    (b*f - c*e)*(b*f - c*e) +
                    (c*d - a*f)*(c*d - a*f) +
                    (a*e - b*d)*(a*e - b*d)
                );

                avg_face_area += area;
                min_face_area = std::min(min_face_area, area);
                max_face_area = std::max(max_face_area, area);
                face_count++;
            }
        }
    }

    if (face_count > 0) {
        avg_face_area /= face_count;
    }
}

void ModelImporter::print_model_structure(const Model3D& model) {
    std::cout << "Model: " << model.filename << "\n";
    std::cout << "Format: " << model.format << "\n";
    std::cout << "Meshes: " << model.mesh_count() << "\n";
    std::cout << "Total Vertices: " << model.total_vertices() << "\n";
    std::cout << "Total Faces: " << model.total_faces() << "\n";

    for (size_t i = 0; i < model.meshes.size(); ++i) {
        const auto& mesh = model.meshes[i];
        std::cout << "  Mesh " << i << " (" << mesh.name << "): "
                  << mesh.vertex_count() << " vertices, "
                  << mesh.face_count() << " faces\n";
    }
}

} // namespace bendcore::io
