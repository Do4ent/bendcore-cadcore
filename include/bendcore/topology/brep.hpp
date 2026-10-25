#pragma once

#include "../document/entity_id.hpp"
#include "../geometry/vector.hpp"
#include <vector>
#include <set>
#include <map>
#include <memory>

namespace BendCore::CadCore {

/**
 * @brief Vertex in B-Rep topology
 */
struct Vertex {
    EntityId id;
    Vector3D point;
    uint64_t revision = 0;
};

/**
 * @brief Edge in B-Rep topology
 */
struct Edge {
    EntityId id;
    EntityId startVertexId;
    EntityId endVertexId;
    double length = 0.0;
    uint64_t revision = 0;
};

/**
 * @brief Face in B-Rep topology
 */
struct Face {
    EntityId id;
    std::vector<EntityId> edgeIds;
    Vector3D normal;
    double area = 0.0;
    uint64_t revision = 0;
};

/**
 * @brief Shell (collection of faces)
 */
struct Shell {
    EntityId id;
    std::vector<EntityId> faceIds;
    double volume = 0.0;
    uint64_t revision = 0;
};

/**
 * @brief Solid in B-Rep representation
 */
struct Solid {
    EntityId id;
    std::vector<EntityId> shellIds;
    double volume = 0.0;
    Vector3D centroid;
    uint64_t revision = 0;
};

/**
 * @brief B-Rep manager (Boundary Representation)
 */
class BRepManager {
public:
    BRepManager() noexcept = default;

    /**
     * @brief Create vertex
     */
    EntityId createVertex(const Vector3D& point) noexcept;

    /**
     * @brief Create edge
     */
    EntityId createEdge(const EntityId& startVertex, const EntityId& endVertex) noexcept;

    /**
     * @brief Create face
     */
    EntityId createFace(const std::vector<EntityId>& edges) noexcept;

    /**
     * @brief Create shell
     */
    EntityId createShell(const std::vector<EntityId>& faces) noexcept;

    /**
     * @brief Create solid
     */
    EntityId createSolid(const std::vector<EntityId>& shells) noexcept;

    /**
     * @brief Get vertex
     */
    const Vertex* getVertex(const EntityId& id) const noexcept;

    /**
     * @brief Get edge
     */
    const Edge* getEdge(const EntityId& id) const noexcept;

    /**
     * @brief Get face
     */
    const Face* getFace(const EntityId& id) const noexcept;

    /**
     * @brief Get shell
     */
    const Shell* getShell(const EntityId& id) const noexcept;

    /**
     * @brief Get solid
     */
    const Solid* getSolid(const EntityId& id) const noexcept;

    /**
     * @brief Validate topology
     */
    bool validateTopology(const EntityId& solidId) const noexcept;

    /**
     * @brief Get adjacent faces
     */
    std::set<EntityId> getAdjacentFaces(const EntityId& faceId) const noexcept;

private:
    std::map<EntityId, Vertex> vertices_;
    std::map<EntityId, Edge> edges_;
    std::map<EntityId, Face> faces_;
    std::map<EntityId, Shell> shells_;
    std::map<EntityId, Solid> solids_;
};

} // namespace BendCore::CadCore
