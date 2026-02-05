#include "../src/cache.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>

struct TestData {
    std::string key;
    std::string value;
};

std::vector<TestData> loadTestData(const std::string& filename) {
    std::vector<TestData> data;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open test data file: " << filename << std::endl;
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

void testBasicOperations(const std::vector<TestData>& testData) {
    std::cout << "Running basic operations test..." << std::endl;

    LRUCache cache(5); // Small capacity for testing

    // Test put operations
    for (const auto& item : testData) {
        cache.put(item.key, item.value);
    }

    // Test get operations
    for (const auto& item : testData) {
        std::string result = cache.get(item.key);
        if (cache.size() <= 5) {
            assert(result == item.value && "Get should return correct value for existing key");
        }
        // Note: Some keys may be evicted due to LRU, so we don't assert for all
    }

    // Test non-existent key
    std::string missing = cache.get("nonexistent_key");
    assert(missing.empty() && "Get should return empty string for non-existent key");

    // Test update existing key
    if (!testData.empty()) {
        std::string updatedValue = testData[0].value + "_updated";
        cache.put(testData[0].key, updatedValue);
        std::string result = cache.get(testData[0].key);
        assert(result == updatedValue && "Update should work correctly");
    }

    std::cout << "Basic operations test passed!" << std::endl;
}

void testCacheSize(const std::vector<TestData>& testData) {
    std::cout << "Running cache size test..." << std::endl;

    LRUCache cache(3);

    // Initially empty
    assert(cache.size() == 0 && "Cache should start empty");

    // Add items up to capacity
    for (size_t i = 0; i < 3 && i < testData.size(); ++i) {
        cache.put(testData[i].key, testData[i].value);
    }
    assert(cache.size() == 3 && "Cache size should match capacity when not exceeded");

    // Add one more (should evict one)
    if (testData.size() > 3) {
        cache.put(testData[3].key, testData[3].value);
        assert(cache.size() == 3 && "Cache size should remain at capacity after eviction");
    }

    std::cout << "Cache size test passed!" << std::endl;
}

int main() {
    std::cout << "Loading test data..." << std::endl;
    auto testData = loadTestData("test/test_data.txt");

    if (testData.empty()) {
        std::cerr << "No test data loaded. Exiting." << std::endl;
        return 1;
    }

    std::cout << "Loaded " << testData.size() << " test items." << std::endl;

    try {
        testBasicOperations(testData);
        testCacheSize(testData);

        std::cout << "All basic tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
