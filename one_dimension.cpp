#include <iostream>
#include <vector>
#include <chrono>
#include <cstring>
#include <algorithm>

const size_t ARRAY_SIZE = 1000000 * 512;

void test_resize_memcpy(const std::vector<double>& source, std::vector<double>& target) {
    target.resize(source.size());
    std::memcpy(target.data(), source.data(), source.size() * sizeof(double));
}

void test_resize_copy(const std::vector<double>& source, std::vector<double>& target) {
    target.resize(source.size());
    std::copy(source.begin(), source.end(), target.begin());
}

void test_reserve_insert(const std::vector<double>& source, std::vector<double>& target) {
    target.clear();
    target.reserve(source.size());
    target.insert(target.end(), source.begin(), source.end());
}

int main() {
    std::vector<double> source(ARRAY_SIZE);
    
    // Initialize source array with some data
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        source[i] = static_cast<double>(i) * 0.001;
    }
    
    std::cout << "Array size: " << ARRAY_SIZE << " elements (" 
              << (ARRAY_SIZE * sizeof(double)) / (1024 * 1024) << " MB)" << std::endl;
    
    const int ITERATIONS = 10;
    
    // Test reserve + insert
    {
        std::vector<double> target;
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < ITERATIONS; ++i) {
            test_reserve_insert(source, target);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "reserve + insert: " << duration.count() / ITERATIONS << " ms (avg)" << std::endl;
    }

    // Test resize + copy
    {
        std::vector<double> target;
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < ITERATIONS; ++i) {
            test_resize_copy(source, target);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "resize + copy: " << duration.count() / ITERATIONS << " ms (avg)" << std::endl;
    }
    
    // Test resize + memcpy
    {
        std::vector<double> target;
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < ITERATIONS; ++i) {
            test_resize_memcpy(source, target);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "resize + memcpy: " << duration.count() / ITERATIONS << " ms (avg)" << std::endl;
    }
    
    return 0;
}