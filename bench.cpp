#include <vector>
#include <chrono>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <random>

class MockInvList {
private:
    std::vector<std::vector<uint8_t>> lists;
    std::size_t d;
    
public:
    MockInvList(std::size_t nlist, std::size_t total_vectors, std::size_t d) : d(d) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> float_dist(-1.0f, 1.0f);
        
        // First generate random sizes that sum to total_vectors
        std::vector<std::size_t> sizes(nlist, 0);
        for (std::size_t i = 0; i < total_vectors; ++i) {
            std::uniform_int_distribution<std::size_t> list_dist(0, nlist - 1);
            std::size_t list_idx = list_dist(gen);
            sizes[list_idx]++;
        }
        
        // Debug: check total
        std::size_t total_check = 0;
        for (std::size_t i = 0; i < nlist; ++i) {
            total_check += sizes[i];
        }
        std::cout << "Total vectors check: " << total_check << " (should be " << total_vectors << ")" << std::endl;
        
        lists.resize(nlist);
        for (std::size_t i = 0; i < nlist; ++i) {
            std::size_t list_size = sizes[i];
            lists[i].resize(list_size * d * sizeof(float));
            
            // Fill with random float data
            float* float_data = reinterpret_cast<float*>(lists[i].data());
            for (std::size_t j = 0; j < list_size * d; ++j) {
                float_data[j] = float_dist(gen);
            }
        }
    }
    
    std::size_t list_size(std::size_t list_id) const {
        return lists[list_id].size() / (d * sizeof(float));
    }
    
    const uint8_t* get_codes(std::size_t list_id) const {
        return lists[list_id].empty() ? nullptr : lists[list_id].data();
    }
};

void method1_reserve_insert(const MockInvList* invlists, std::size_t nlist, std::size_t ntotal, std::size_t d) {
    std::vector<float> all_vectors;
    all_vectors.reserve(ntotal * d);
    
    for (std::size_t list_id = 0; list_id < nlist; ++list_id) {
        if (std::size_t list_size = invlists->list_size(list_id); list_size > 0) {
            const uint8_t* codes = invlists->get_codes(list_id);
            if (!codes) {
                continue;
            }
            const auto* vectors = reinterpret_cast<const float*>(codes);
            all_vectors.insert(all_vectors.end(), vectors, vectors + list_size * d);
        }
    }
}

void method2_resize_copy(const MockInvList* invlists, std::size_t nlist, std::size_t ntotal, std::size_t d) {
    std::vector<float> all_vectors;
    all_vectors.resize(ntotal * d);
    
    std::size_t offset = 0;
    for (std::size_t list_id = 0; list_id < nlist; ++list_id) {
        if (std::size_t list_size = invlists->list_size(list_id); list_size > 0) {
            const uint8_t* codes = invlists->get_codes(list_id);
            if (!codes) {
                continue;
            }
            const auto* vectors = reinterpret_cast<const float*>(codes);
            std::copy(vectors, vectors + list_size * d, all_vectors.data() + offset);
            offset += list_size * d;
        }
    }
}

void method3_resize_memcpy(const MockInvList* invlists, std::size_t nlist, std::size_t ntotal, std::size_t d) {
    std::vector<float> all_vectors;
    all_vectors.resize(ntotal * d);
    
    std::size_t offset = 0;
    for (std::size_t list_id = 0; list_id < nlist; ++list_id) {
        if (std::size_t list_size = invlists->list_size(list_id); list_size > 0) {
            const uint8_t* codes = invlists->get_codes(list_id);
            if (!codes) {
                continue;
            }
            const auto* vectors = reinterpret_cast<const float*>(codes);
            std::memcpy(all_vectors.data() + offset, vectors, list_size * d * sizeof(float));
            offset += list_size * d;
        }
    }
}

int main() {
    const std::size_t ntotal = 1000000;
    const std::size_t d = 512;
    const std::size_t nlist = 1000;
    
    std::cout << "Creating mock data..." << std::endl;
    std::cout << "ntotal=" << ntotal << ", nlist=" << nlist << ", d=" << d << std::endl;
    MockInvList invlists(nlist, ntotal, d);
    std::cout << "Mock data created successfully." << std::endl;
    
    const int num_runs = 10;
    
    // Cache warming run
    std::cout << "Cache warming..." << std::endl;
    method1_reserve_insert(&invlists, nlist, ntotal, d);
    method2_resize_copy(&invlists, nlist, ntotal, d);
    method3_resize_memcpy(&invlists, nlist, ntotal, d);
    std::cout << "Cache warming completed." << std::endl << std::endl;

    std::cout << "Method 1: reserve + insert" << std::endl;
    auto total_time1 = std::chrono::microseconds(0);
    for (int i = 0; i < num_runs; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        method1_reserve_insert(&invlists, nlist, ntotal, d);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        total_time1 += duration;
        std::cout << "Run " << i+1 << ": " << duration.count() << " μs" << std::endl;
    }
    std::cout << "Average: " << total_time1.count() / num_runs << " μs" << std::endl << std::endl;
    
    std::cout << "Method 2: resize + std::copy" << std::endl;
    auto total_time2 = std::chrono::microseconds(0);
    for (int i = 0; i < num_runs; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        method2_resize_copy(&invlists, nlist, ntotal, d);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        total_time2 += duration;
        std::cout << "Run " << i+1 << ": " << duration.count() << " μs" << std::endl;
    }
    std::cout << "Average: " << total_time2.count() / num_runs << " μs" << std::endl << std::endl;

    std::cout << "Method 3: resize + memcpy" << std::endl;
    auto total_time3 = std::chrono::microseconds(0);
    for (int i = 0; i < num_runs; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        method3_resize_memcpy(&invlists, nlist, ntotal, d);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        total_time3 += duration;
        std::cout << "Run " << i+1 << ": " << duration.count() << " μs" << std::endl;
    }
    std::cout << "Average: " << total_time3.count() / num_runs << " μs" << std::endl << std::endl;
    
    
    double speedup2 = static_cast<double>(total_time1.count()) / total_time2.count();
    double speedup3 = static_cast<double>(total_time1.count()) / total_time3.count();
    
    std::cout << "Performance comparison:" << std::endl;
    std::cout << "Method 2 speedup vs Method 1: " << speedup2 << "x" << std::endl;
    std::cout << "Method 3 speedup vs Method 1: " << speedup3 << "x" << std::endl;
    
    return 0;
}