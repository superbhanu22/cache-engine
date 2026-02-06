#include "../src/cache.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <random>
#include <fstream>
#include <sstream>
#include <cassert>


struct ThreadTestData {
    std::string key;
    std::string value;
};

std::vector<ThreadTestData> loadThreadTestData(const std::string& filename) {
    std::vector<ThreadTestData> data;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open thread test data file: " << filename << std::endl;
        return data;
    }

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string key, value;
        if (std::getline(ss, key, ',') && std::getline(ss, value)) {
            data.push_back({key, value});
        }
    }

    return data;
}

void threadWorker(LRUCache& cache, const std::vector<ThreadTestData>& data,
                  std::atomic<int>& operations_completed, int thread_id) {
    std::random_device rd;
    std::mt19937 gen(rd() + thread_id);
    std::uniform_int_distribution<> dis(0, data.size() - 1);

    for (int i = 0; i < 100; ++i) {
        int index = dis(gen);
        const auto& item = data[index];

        if (i % 3 == 0) {
            cache.put(item.key + "_t" + std::to_string(thread_id), item.value);
        } else {
            cache.get(item.key + "_t" + std::to_string(thread_id));
        }

        operations_completed++;
    }
}

void testConcurrentAccess(const std::vector<ThreadTestData>& testData) {
    std::cout << "Running concurrent access test..." << std::endl;

    LRUCache cache(50); 
    std::atomic<int> operations_completed(0);

    const int num_threads = 10;
    std::vector<std::thread> threads;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(threadWorker, std::ref(cache), std::ref(testData),
                           std::ref(operations_completed), i);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "Completed " << operations_completed.load() << " operations in "
              << duration.count() << " milliseconds" << std::endl;

    assert(cache.size() <= 50 && "Cache size should not exceed capacity");

    for (size_t i = 0; i < 5 && i < testData.size(); ++i) {
        std::string key = testData[i].key + "_t0";
        std::string value = cache.get(key);
    }

    std::cout << "Concurrent access test passed!" << std::endl;
}

void testRaceConditions() {
    std::cout << "Running race condition test..." << std::endl;

    LRUCache cache(10);
    std::atomic<bool> test_passed(true);

    auto rapidWorker = [&](int thread_id) {
        try {
            for (int i = 0; i < 50; ++i) {
                std::string key = "race_key_" + std::to_string(i % 5) + "_t" + std::to_string(thread_id);
                std::string value = "race_value_" + std::to_string(i);

                cache.put(key, value);
                std::string retrieved = cache.get(key);

                if (!retrieved.empty() && retrieved != value) {
                    test_passed = false;
                }
            }
        } catch (...) {
            test_passed = false;
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(rapidWorker, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    assert(test_passed && "Race condition test should not detect inconsistencies");

    std::cout << "Race condition test passed!" << std::endl;
}

int main() {
    std::cout << "Loading thread test data..." << std::endl;
    auto testData = loadThreadTestData("test/test_data.txt");

    if (testData.empty()) {
        std::cerr << "No thread test data loaded. Exiting." << std::endl;
        return 1;
    }

    std::cout << "Loaded " << testData.size() << " test items for threading tests." << std::endl;

    try {
        testConcurrentAccess(testData);
        testRaceConditions();

        std::cout << "All thread safety tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Thread safety test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
