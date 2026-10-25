#include <bendcore/parallel/parallel_executor.h>
#include <thread>
#include <algorithm>
#include <numeric>
#include <iostream>

#ifdef HAVE_TASKFLOW
    #include <taskflow/taskflow.hpp>
#endif

namespace bendcore::parallel {

ParallelExecutor::ParallelExecutor() {
    #ifdef HAVE_TASKFLOW
    size_t num_workers = std::thread::hardware_concurrency();
    executor_ = std::make_unique<tf::Executor>(num_workers);
    #endif
}

ParallelExecutor::~ParallelExecutor() = default;

void ParallelExecutor::set_num_workers(size_t) {
    #ifdef HAVE_TASKFLOW
    size_t num_workers = std::thread::hardware_concurrency();
    executor_ = std::make_unique<tf::Executor>(num_workers);
    #endif
}

size_t ParallelExecutor::get_num_workers() const {
    #ifdef HAVE_TASKFLOW
    return executor_->num_workers();
    #else
    return std::thread::hardware_concurrency();
    #endif
}

void ParallelExecutor::execute_parallel(const std::vector<Task>&) {
    start_time_ = std::chrono::high_resolution_clock::now();
    end_time_ = std::chrono::high_resolution_clock::now();
}

void ParallelExecutor::execute_for_each(size_t, size_t, TaskWithId) {
    start_time_ = std::chrono::high_resolution_clock::now();
    end_time_ = std::chrono::high_resolution_clock::now();
}

void ParallelExecutor::create_task_graph(const std::function<void(void*)>&) {
    // Placeholder
}

void ParallelExecutor::execute_task_graph() {
    start_time_ = std::chrono::high_resolution_clock::now();
    end_time_ = std::chrono::high_resolution_clock::now();
}

void ParallelExecutor::wait_all() {
    // Placeholder
}

std::vector<TaskMetrics> ParallelExecutor::execute_with_metrics(const std::vector<Task>&) {
    std::vector<TaskMetrics> metrics;
    start_time_ = std::chrono::high_resolution_clock::now();
    end_time_ = std::chrono::high_resolution_clock::now();
    return metrics;
}

std::chrono::milliseconds ParallelExecutor::get_execution_time() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ - start_time_);
}

void ParallelExecutor::print_statistics() const {
    auto duration = get_execution_time();
    std::cout << "Execution time: " << duration.count() << " ms\n";
    std::cout << "Workers: " << get_num_workers() << "\n";
}

// Parallel computation helpers
namespace utils {

size_t calculate_chunk_size(size_t total_items, size_t num_workers) {
    if (num_workers == 0) num_workers = 1;
    size_t chunk = total_items / num_workers;
    return std::max(chunk, size_t(1));
}

size_t get_optimal_threads() {
    return std::thread::hardware_concurrency();
}

double parallel_dot_product(
    const std::vector<double>& a,
    const std::vector<double>& b) {

    if (a.size() != b.size()) {
        throw std::invalid_argument("Vector sizes must match");
    }

    if (a.empty()) return 0.0;

    double result = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    return result;
}

std::vector<std::vector<double>> parallel_matmul(
    const std::vector<std::vector<double>>& A,
    const std::vector<std::vector<double>>& B) {

    if (A.empty() || B.empty() || A[0].size() != B.size()) {
        throw std::invalid_argument("Incompatible matrix dimensions");
    }

    size_t m = A.size();
    size_t n = B[0].size();
    size_t k = B.size();

    std::vector<std::vector<double>> C(m, std::vector<double>(n, 0.0));

    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            for (size_t p = 0; p < k; ++p) {
                C[i][j] += A[i][p] * B[p][j];
            }
        }
    }

    return C;
}

} // namespace utils

} // namespace bendcore::parallel
