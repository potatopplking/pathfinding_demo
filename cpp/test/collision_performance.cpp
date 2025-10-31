#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <random>
#include <algorithm>
#include <set>

#include "positional_container.hpp"

/**
 * @file collision_performance.cpp
 * @brief Performance tests for collision detection systems
 * 
 * This file contains performance benchmarks and tests for various
 * collision detection algorithms and optimizations.
 */

/**
 * @brief Helper class to measure and print execution time
 */
class PerformanceTimer {
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<double, std::milli>;
    
    PerformanceTimer(const std::string& name) : name_(name) {
        start_ = Clock::now();
    }
    
    ~PerformanceTimer() {
        auto end = Clock::now();
        Duration duration = end - start_;
        std::cout << std::fixed << std::setprecision(3)
                  << "[PERF] " << name_ << ": " 
                  << duration.count() << " ms" << std::endl;
    }
    
    double elapsed_ms() const {
        auto end = Clock::now();
        Duration duration = end - start_;
        return duration.count();
    }
    
private:
    std::string name_;
    TimePoint start_;
};

/**
 * @brief Run a function multiple times and measure average execution time
 * @param name Name of the test for output
 * @param iterations Number of iterations to run
 * @param func Function to benchmark
 */
template<typename Func>
void benchmark_function(const std::string& name, int iterations, Func func) {
    auto start = PerformanceTimer::Clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        func();
    }
    
    auto end = PerformanceTimer::Clock::now();
    PerformanceTimer::Duration total_duration = end - start;
    double avg_duration = total_duration.count() / iterations;
    
    std::cout << std::fixed << std::setprecision(6)
              << "[BENCHMARK] " << name << ":\n"
              << "  Total: " << total_duration.count() << " ms\n"
              << "  Iterations: " << iterations << "\n"
              << "  Average: " << avg_duration << " ms\n"
              << "  Throughput: " << (iterations / (total_duration.count() / 1000.0))
              << " ops/sec" << std::endl;
}

/**
 * @brief Simple dummy class that conforms to HasPosition concept
 * Used for testing PositionalContainer without heavy dependencies
 */
class Dummy {
public:
    Dummy() : m_Position{0.0f, 0.0f}, m_Id(next_id++) {}
    Dummy(float x, float y) : m_Position{x, y}, m_Id(next_id++) {}
    Dummy(WorldPos pos) : m_Position(pos), m_Id(next_id++) {}
    
    WorldPos GetPosition() const { return m_Position; }
    void SetPosition(WorldPos pos) { m_Position = pos; }
    int GetId() const { return m_Id; }
    
private:
    WorldPos m_Position;
    int m_Id;
    static int next_id;
};

int Dummy::next_id = 0;

/**
 * @brief Helper function to generate random float in range [min, max]
 */
float random_float(std::mt19937& gen, float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

/**
 * @brief Compare two sets of weak_ptrs by comparing the IDs of the objects they point to
 */
bool compare_results(const std::vector<std::weak_ptr<Dummy>>& a, 
                    const std::vector<std::weak_ptr<Dummy>>& b) {
    std::set<int> ids_a, ids_b;
    
    for (const auto& weak : a) {
        if (auto shared = weak.lock()) {
            ids_a.insert(shared->GetId());
        }
    }
    
    for (const auto& weak : b) {
        if (auto shared = weak.lock()) {
            ids_b.insert(shared->GetId());
        }
    }
    
    return ids_a == ids_b;
}

TEST(CollisionPerformance, CompareContainers) {
    std::cout << "\n=== Collision Performance Comparison ===\n" << std::endl;
    
    // Configuration
    const int NUM_OBJECTS = 1000;
    const int NUM_LOOKUPS = 100;
    const float WORLD_SIZE = 1000.0f;
    const float LOOKUP_RADIUS = 50.0f;
    const size_t CHUNKS = 20;
    
    // Random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Create containers
    PositionalContainer<Dummy> pos_cont{WorldSize{WORLD_SIZE, WORLD_SIZE}, CHUNKS};
    SimpleContainer<Dummy> simp_cont;
    
    // Create and add dummy objects with random positions
    std::vector<std::shared_ptr<Dummy>> objects;
    objects.reserve(NUM_OBJECTS);
    
    std::cout << "Creating " << NUM_OBJECTS << " objects with random positions..." << std::endl;
    for (int i = 0; i < NUM_OBJECTS; ++i) {
        float x = random_float(gen, 10.0f, WORLD_SIZE - 10.0f);
        float y = random_float(gen, 10.0f, WORLD_SIZE - 10.0f);
        auto obj = std::make_shared<Dummy>(x, y);
        objects.push_back(obj);
        pos_cont.Add(obj);
        simp_cont.Add(obj);
    }
    std::cout << "Objects created and added to containers." << std::endl;
    
    // Generate random lookup positions
    std::vector<WorldPos> lookup_positions;
    lookup_positions.reserve(NUM_LOOKUPS);
    for (int i = 0; i < NUM_LOOKUPS; ++i) {
        float x = random_float(gen, 0.0f, WORLD_SIZE);
        float y = random_float(gen, 0.0f, WORLD_SIZE);
        lookup_positions.push_back(WorldPos{x, y});
    }
    
    // Benchmark SimpleContainer
    double simple_total_time = 0.0;
    std::vector<std::vector<std::weak_ptr<Dummy>>> simple_results;
    simple_results.reserve(NUM_LOOKUPS);
    
    std::cout << "\nBenchmarking SimpleContainer with " << NUM_LOOKUPS << " lookups..." << std::endl;
    for (const auto& pos : lookup_positions) {
        auto start = PerformanceTimer::Clock::now();
        auto result = simp_cont.Get(pos, LOOKUP_RADIUS);
        auto end = PerformanceTimer::Clock::now();
        
        PerformanceTimer::Duration duration = end - start;
        simple_total_time += duration.count();
        simple_results.push_back(result);
    }
    
    double simple_avg_time = simple_total_time / NUM_LOOKUPS;
    std::cout << std::fixed << std::setprecision(6)
              << "[BENCHMARK] SimpleContainer:\n"
              << "  Total time: " << simple_total_time << " ms\n"
              << "  Average time per lookup: " << simple_avg_time << " ms\n"
              << "  Throughput: " << (NUM_LOOKUPS / (simple_total_time / 1000.0)) 
              << " lookups/sec" << std::endl;
    
    // Benchmark PositionalContainer
    double positional_total_time = 0.0;
    std::vector<std::vector<std::weak_ptr<Dummy>>> positional_results;
    positional_results.reserve(NUM_LOOKUPS);
    
    std::cout << "\nBenchmarking PositionalContainer with " << NUM_LOOKUPS << " lookups..." << std::endl;
    for (const auto& pos : lookup_positions) {
        auto start = PerformanceTimer::Clock::now();
        auto result = pos_cont.Get(pos, LOOKUP_RADIUS);
        auto end = PerformanceTimer::Clock::now();
        
        PerformanceTimer::Duration duration = end - start;
        positional_total_time += duration.count();
        positional_results.push_back(result);
    }
    
    double positional_avg_time = positional_total_time / NUM_LOOKUPS;
    std::cout << std::fixed << std::setprecision(6)
              << "[BENCHMARK] PositionalContainer:\n"
              << "  Total time: " << positional_total_time << " ms\n"
              << "  Average time per lookup: " << positional_avg_time << " ms\n"
              << "  Throughput: " << (NUM_LOOKUPS / (positional_total_time / 1000.0)) 
              << " lookups/sec" << std::endl;
    
    // Verify results match
    std::cout << "\nVerifying results correctness..." << std::endl;
    int mismatches = 0;
    for (size_t i = 0; i < NUM_LOOKUPS; ++i) {
        if (!compare_results(simple_results[i], positional_results[i])) {
            mismatches++;
            std::cout << "Mismatch at lookup " << i 
                     << " (pos: " << lookup_positions[i] << ")" << std::endl;
        }
    }
    
    if (mismatches == 0) {
        std::cout << "✓ All " << NUM_LOOKUPS << " lookups produced identical results!" << std::endl;
    } else {
        std::cout << "✗ Found " << mismatches << " mismatches out of " 
                 << NUM_LOOKUPS << " lookups" << std::endl;
    }
    
    // Performance comparison
    std::cout << "\n=== Performance Summary ===\n";
    double speedup = simple_avg_time / positional_avg_time;
    std::cout << std::fixed << std::setprecision(2)
              << "PositionalContainer is " << speedup << "x "
              << (speedup > 1.0 ? "faster" : "slower") 
              << " than SimpleContainer" << std::endl;
    
    std::cout << "\n======================================\n" << std::endl;
    
    // Assertions
    EXPECT_EQ(mismatches, 0) << "Results should match between containers";
    EXPECT_GT(speedup, 1.0) << "PositionalContainer should be faster than SimpleContainer";
}
