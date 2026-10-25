#include "bendcore/analysis/inspection_tools.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace BendCore::CadCore {

// ===== CurvatureAnalysis Implementation =====

double CurvatureAnalysis::getMinCurvature() const noexcept {
    if (samples_.empty()) return 0.0;

    return std::min_element(samples_.begin(), samples_.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; })->second;
}

double CurvatureAnalysis::getMaxCurvature() const noexcept {
    if (samples_.empty()) return 0.0;

    return std::max_element(samples_.begin(), samples_.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; })->second;
}

double CurvatureAnalysis::getAverageCurvature() const noexcept {
    if (samples_.empty()) return 0.0;

    double sum = std::accumulate(samples_.begin(), samples_.end(), 0.0,
        [](double acc, const auto& sample) { return acc + sample.second; });

    return sum / samples_.size();
}

std::string CurvatureAnalysis::analyzeContinuity() const noexcept {
    if (samples_.size() < 2) return "C0";

    // Check for continuity based on curvature variations
    double prev_curvature = samples_[0].second;
    double max_diff = 0.0;

    for (size_t i = 1; i < samples_.size(); ++i) {
        double diff = std::abs(samples_[i].second - prev_curvature);
        max_diff = std::max(max_diff, diff);
        prev_curvature = samples_[i].second;
    }

    // Heuristic: classify continuity level
    if (max_diff < 0.01) return "C2";  // Smooth curvature
    if (max_diff < 0.1) return "C1";   // Continuous tangent
    return "C0";                        // Positional continuity
}

// ===== SurfaceQuality Implementation =====

bool SurfaceQuality::evaluateQuality() noexcept {
    // Simulate quality evaluation
    quality_score_ = 75;  // Default score

    // Adjust based on defect count
    quality_score_ -= defect_locations_.size() * 5;
    quality_score_ = std::max(0, quality_score_);

    return true;
}

std::string SurfaceQuality::getAssessment() const noexcept {
    if (quality_score_ >= 90) return "Excellent";
    if (quality_score_ >= 80) return "Good";
    if (quality_score_ >= 70) return "Acceptable";
    if (quality_score_ >= 60) return "Fair";
    return "Poor";
}

// ===== ToleranceChecker Implementation =====

bool ToleranceChecker::checkTolerance(double measured_value) noexcept {
    violations_.clear();

    for (const auto& tolerance : tolerances_) {
        if (measured_value < tolerance.lower_limit ||
            measured_value > tolerance.upper_limit) {

            std::string violation = tolerance.description +
                ": measured=" + std::to_string(measured_value);
            violations_.push_back(violation);
        }
    }

    return violations_.empty();
}

// ===== GeometryValidator Implementation =====

bool GeometryValidator::validate() noexcept {
    issues_.clear();
    self_intersecting_ = false;
    has_gaps_overlaps_ = false;

    // Simulate validation checks
    // In real implementation, check actual geometry

    // Example: Add some validation issues
    ValidationIssue issue1;
    issue1.description = "Surface smoothness below threshold";
    issue1.entity_id = assembly_id_;
    issue1.severity = 2;
    issue1.suggestion = "Check surface continuity";

    // Only add if we simulate finding issues (for now, no issues)

    return issues_.empty();
}

bool GeometryValidator::autoRepair() noexcept {
    repair_log_.clear();

    // Attempt repairs
    if (self_intersecting_) {
        repair_log_.push_back("Fixed self-intersecting surface");
        self_intersecting_ = false;
    }

    if (has_gaps_overlaps_) {
        repair_log_.push_back("Closed gaps and resolved overlaps");
        has_gaps_overlaps_ = false;
    }

    return repair_log_.empty();
}

// ===== InspectionTools Implementation =====

std::shared_ptr<DistanceMeasurement> InspectionTools::measureDistance(
    const EntityId& entity1, const EntityId& entity2) noexcept {

    auto measurement = std::make_shared<DistanceMeasurement>(entity1, entity2);
    distances_.push_back(measurement);
    return measurement;
}

std::shared_ptr<AngleMeasurement> InspectionTools::measureAngle(
    const EntityId& entity1, const EntityId& entity2) noexcept {

    auto measurement = std::make_shared<AngleMeasurement>(entity1, entity2);
    angles_.push_back(measurement);
    return measurement;
}

std::shared_ptr<CurvatureAnalysis> InspectionTools::analyzeCurvature(
    const EntityId& geometry_id) noexcept {

    auto analysis = std::make_shared<CurvatureAnalysis>(geometry_id);
    curvatures_.push_back(analysis);
    return analysis;
}

std::shared_ptr<SurfaceQuality> InspectionTools::assessSurfaceQuality(
    const EntityId& surface_id) noexcept {

    auto quality = std::make_shared<SurfaceQuality>(surface_id);
    surfaces_.push_back(quality);
    return quality;
}

std::shared_ptr<ToleranceChecker> InspectionTools::createToleranceChecker(
    const EntityId& feature_id) noexcept {

    auto checker = std::make_shared<ToleranceChecker>(feature_id);
    tolerances_.push_back(checker);
    return checker;
}

} // namespace BendCore::CadCore
