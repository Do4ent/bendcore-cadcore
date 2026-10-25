#include <bendcore/math/geometry_ops.h>
#include <algorithm>
#include <cmath>

namespace bendcore::math {

// Point3D implementations
double Point3D::distance_to(const Point3D& other) const noexcept {
    double dx = x - other.x;
    double dy = y - other.y;
    double dz = z - other.z;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

double Point3D::length() const noexcept {
    return std::sqrt(x*x + y*y + z*z);
}

// Vector3D implementations
double Vector3D::length() const noexcept {
    return std::sqrt(x*x + y*y + z*z);
}

Vector3D Vector3D::normalized() const {
    double len = length();
    if (len < 1e-10) return Vector3D(0, 0, 0);
    return Vector3D(x/len, y/len, z/len);
}

double Vector3D::dot(const Vector3D& other) const noexcept {
    return x*other.x + y*other.y + z*other.z;
}

Vector3D Vector3D::cross(const Vector3D& other) const noexcept {
    return Vector3D(
        y*other.z - z*other.y,
        z*other.x - x*other.z,
        x*other.y - y*other.x
    );
}

// GeometryOps implementations
bool GeometryOps::point_in_polygon_2d(const Point3D& point, const std::vector<Point3D>& polygon) {
    // Ray casting algorithm
    int count = 0;
    for (size_t i = 0; i < polygon.size(); ++i) {
        const Point3D& p1 = polygon[i];
        const Point3D& p2 = polygon[(i + 1) % polygon.size()];

        if ((p1.y <= point.y && point.y < p2.y) ||
            (p2.y <= point.y && point.y < p1.y)) {
            double xinters = (p2.x - p1.x) * (point.y - p1.y) / (p2.y - p1.y) + p1.x;
            if (point.x < xinters) count++;
        }
    }
    return count % 2 == 1;
}

double GeometryOps::triangle_area(const Point3D& p1, const Point3D& p2, const Point3D& p3) noexcept {
    Vector3D v1(p1, p2);
    Vector3D v2(p1, p3);
    return 0.5 * v1.cross(v2).length();
}

double GeometryOps::polygon_area(const std::vector<Point3D>& polygon) {
    if (polygon.size() < 3) return 0.0;

    double area = 0.0;
    for (size_t i = 1; i < polygon.size() - 1; ++i) {
        area += triangle_area(polygon[0], polygon[i], polygon[i + 1]);
    }
    return area;
}

std::vector<Point3D> GeometryOps::convex_hull_2d(const std::vector<Point3D>& points) {
    // Graham scan algorithm
    if (points.size() < 3) return points;

    std::vector<Point3D> sorted = points;
    std::sort(sorted.begin(), sorted.end(),
        [](const Point3D& a, const Point3D& b) {
            return a.x < b.x || (a.x == b.x && a.y < b.y);
        });

    std::vector<Point3D> hull;

    // Build lower hull
    for (const auto& p : sorted) {
        while (hull.size() >= 2) {
            Vector3D v1(hull[hull.size()-2], hull[hull.size()-1]);
            Vector3D v2(hull[hull.size()-1], p);
            if (v1.cross(v2).z <= 0) hull.pop_back();
            else break;
        }
        hull.push_back(p);
    }

    // Build upper hull
    size_t lower_size = hull.size();
    for (int i = static_cast<int>(sorted.size()) - 2; i >= 0; --i) {
        while (hull.size() > lower_size) {
            Vector3D v1(hull[hull.size()-2], hull[hull.size()-1]);
            Vector3D v2(hull[hull.size()-1], sorted[i]);
            if (v1.cross(v2).z <= 0) hull.pop_back();
            else break;
        }
        hull.push_back(sorted[i]);
    }

    hull.pop_back();  // Remove duplicate point
    return hull;
}

std::vector<Point3D> GeometryOps::convex_hull_3d(const std::vector<Point3D>& points) {
    // Simplified 3D convex hull (projects to 2D)
    return convex_hull_2d(points);
}

std::vector<std::vector<size_t>> GeometryOps::delaunay_triangulation_2d(
    const std::vector<Point3D>& points) {
    // Placeholder - returns simple triangulation
    std::vector<std::vector<size_t>> triangles;
    if (points.size() < 3) return triangles;

    for (size_t i = 1; i < points.size() - 1; ++i) {
        triangles.push_back({0, i, i+1});
    }
    return triangles;
}

std::vector<std::vector<size_t>> GeometryOps::delaunay_triangulation_3d(
    const std::vector<Point3D>& points) {
    return delaunay_triangulation_2d(points);
}

double GeometryOps::point_to_segment_distance(
    const Point3D& point,
    const Point3D& seg_start,
    const Point3D& seg_end) noexcept {

    Vector3D v(seg_start, seg_end);
    Vector3D w(seg_start, point);

    double c1 = w.dot(v);
    if (c1 <= 0) return point.distance_to(seg_start);

    double c2 = v.dot(v);
    if (c1 >= c2) return point.distance_to(seg_end);

    double b = c1 / c2;
    Point3D closest(
        seg_start.x + b * (seg_end.x - seg_start.x),
        seg_start.y + b * (seg_end.y - seg_start.y),
        seg_start.z + b * (seg_end.z - seg_start.z)
    );
    return point.distance_to(closest);
}

Point3D GeometryOps::closest_point_on_segment(
    const Point3D& point,
    const Point3D& seg_start,
    const Point3D& seg_end) noexcept {

    Vector3D v(seg_start, seg_end);
    Vector3D w(seg_start, point);

    double c1 = w.dot(v);
    if (c1 <= 0) return seg_start;

    double c2 = v.dot(v);
    if (c1 >= c2) return seg_end;

    double b = c1 / c2;
    return Point3D(
        seg_start.x + b * (seg_end.x - seg_start.x),
        seg_start.y + b * (seg_end.y - seg_start.y),
        seg_start.z + b * (seg_end.z - seg_start.z)
    );
}

bool GeometryOps::segments_intersect_3d(
    const Point3D& p1, const Point3D& p2,
    const Point3D& p3, const Point3D& p4) {
    Point3D intersection;
    return segments_intersection_3d(p1, p2, p3, p4, intersection);
}

bool GeometryOps::segments_intersection_3d(
    const Point3D&, const Point3D&,
    const Point3D&, const Point3D&,
    Point3D&) {
    // Placeholder implementation
    return false;
}

std::vector<Point3D> GeometryOps::polygon_offset(
    const std::vector<Point3D>& polygon,
    double) {
    // Simplified offset - returns original polygon
    return polygon;
}

std::vector<Point3D> GeometryOps::polygon_union(
    const std::vector<Point3D>& poly1,
    const std::vector<Point3D>& poly2) {
    std::vector<Point3D> result = poly1;
    result.insert(result.end(), poly2.begin(), poly2.end());
    return result;
}

std::vector<Point3D> GeometryOps::polygon_intersection(
    const std::vector<Point3D>& poly1,
    const std::vector<Point3D>&) {
    // Placeholder implementation
    return poly1;
}

std::vector<Point3D> GeometryOps::polygon_difference(
    const std::vector<Point3D>& poly1,
    const std::vector<Point3D>&) {
    return poly1;
}

bool GeometryOps::is_polygon_simple(const std::vector<Point3D>& polygon) {
    // Check for self-intersections
    for (size_t i = 0; i < polygon.size(); ++i) {
        for (size_t j = i + 2; j < polygon.size(); ++j) {
            if (j == (polygon.size() - 1) && i == 0) continue;
            // Would need actual intersection check
        }
    }
    return true;
}

Point3D GeometryOps::polygon_centroid(const std::vector<Point3D>& polygon) noexcept {
    Point3D centroid(0, 0, 0);
    for (const auto& p : polygon) {
        centroid.x += p.x;
        centroid.y += p.y;
        centroid.z += p.z;
    }
    centroid.x /= polygon.size();
    centroid.y /= polygon.size();
    centroid.z /= polygon.size();
    return centroid;
}

bool GeometryOps::is_counterclockwise(const std::vector<Point3D>& polygon) noexcept {
    double area = 0.0;
    for (size_t i = 0; i < polygon.size(); ++i) {
        const Point3D& p1 = polygon[i];
        const Point3D& p2 = polygon[(i + 1) % polygon.size()];
        area += (p2.x - p1.x) * (p2.y + p1.y);
    }
    return area < 0;
}

std::vector<std::vector<size_t>> GeometryOps::triangulate_polygon(
    const std::vector<Point3D>& polygon) {
    std::vector<std::vector<size_t>> triangles;
    if (polygon.size() < 3) return triangles;

    for (size_t i = 1; i < polygon.size() - 1; ++i) {
        triangles.push_back({0, i, i+1});
    }
    return triangles;
}

} // namespace bendcore::math
