#include "../src/cache.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>

std::vector<std::string> loadAccessPattern(const std::string& filename) {
    std::vector<std::string> pattern;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open LRU test data file: " << filename << std::endl;
        return pattern;
    }

    // Skip header line
    std::getline(file, line);

    // Read the pattern line
    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string key;
        while (std::getline(ss, key, ',')) {
            if (!key.empty()) {
                pattern.push_back(key);
            }
        }
    }

    return pattern;
}

void testLRUEviction(const std::vector<std::string>& accessPattern) {
    std::cout << "Running LRU eviction test..." << std::endl;

    LRUCache cache(3); // Capacity of 3

    // Access pattern: key1, key2, key3, key4, key5, key6 (repeated), key7
    // After capacity exceeded, key4 should be evicted (LRU)

    std::vector<std::string> keys = {"key1", "key2", "key3", "key4", "key5", "key6", "key7"};
    std::vector<std::string> values = {"value1", "value2", "value3", "value4", "value5", "value6", "value7"};

    // Put first 6 keys (will evict key4 when putting key7)
    for (size_t i = 0; i < 6; ++i) {
        cache.put(keys[i], values[i]);
    }

    // At this point, cache should have key5, key6, and one more recent
    // Put key7, which should evict key5 (LRU)
    cache.put("key7", "value7");

    // Check that key5 is evicted (should return empty)
    std::string evicted = cache.get("key5");
    assert(evicted.empty() && "key5 should be evicted as LRU");

    // Check that other keys are still present
    assert(cache.get("key6") == "value6" && "key6 should still be in cache");
    assert(cache.get("key7") == "value7" && "key7 should be in cache");

    // Test LRU ordering by accessing key6, making it most recent
    cache.get("key6");

    // Now put another key, should evict the current LRU (which would be key4 or whoever)
    cache.put("key8", "value8");

    // Verify cache state
    assert(cache.size() == 3 && "Cache should maintain capacity of 3");

    std::cout << "LRU eviction test passed!" << std::endl;
}

void testLRUAccessPattern() {
    std::cout << "Running LRU access pattern test..." << std::endl;

    LRUCache cache(3);

    // Simulate access pattern that tests LRU behavior
    // Put key1, key2, key3
    cache.put("key1", "val1");
    cache.put("key2", "val2");
    cache.put("key3", "val3");

    // Access key1 (makes it most recent)
    cache.get("key1");

    // Put key4 (should evict key2 as LRU)
    cache.put("key4", "val4");

    // key2 should be evicted
    assert(cache.get("key2").empty() && "key2 should be evicted");

    // Other keys should be present
    assert(cache.get("key1") == "val1" && "key1 should be present");
    assert(cache.get("key3") == "val3" && "key3 should be present");
    assert(cache.get("key4") == "val4" && "key4 should be present");

    std::cout << "LRU access pattern test passed!" << std::endl;
}

int main() {
    std::cout << "Loading LRU test data..." << std::endl;
    auto accessPattern = loadAccessPattern("test/lru_test_data.txt");

    if (accessPattern.empty()) {
        std::cerr << "No LRU test data loaded. Exiting." << std::endl;
        return 1;
    }

    std::cout << "Loaded access pattern with " << accessPattern.size() << " operations." << std::endl;

    try {
        testLRUEviction(accessPattern);
        testLRUAccessPattern();

        std::cout << "All LRU tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "LRU test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
