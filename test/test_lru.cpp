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

    std::getline(file, line);

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

    LRUCache cache(3);

    std::vector<std::string> keys = {"key1", "key2", "key3", "key4", "key5", "key6", "key7"};
    std::vector<std::string> values = {"value1", "value2", "value3", "value4", "value5", "value6", "value7"};

    for (size_t i = 0; i < 6; ++i) {
        cache.put(keys[i], values[i]);
    }

    cache.put("key7", "value7");

    std::string evicted = cache.get("key5");
    assert(evicted.empty() && "key5 should be evicted as LRU");

    assert(cache.get("key6") == "value6" && "key6 should still be in cache");
    assert(cache.get("key7") == "value7" && "key7 should be in cache");

    cache.get("key6");

    cache.put("key8", "value8");

    assert(cache.size() == 3 && "Cache should maintain capacity of 3");

    std::cout << "LRU eviction test passed!" << std::endl;
}

void testLRUAccessPattern() {
    std::cout << "Running LRU access pattern test..." << std::endl;

    LRUCache cache(3);

    cache.put("key1", "val1");
    cache.put("key2", "val2");
    cache.put("key3", "val3");

    cache.get("key1");

    cache.put("key4", "val4");

    assert(cache.get("key2").empty() && "key2 should be evicted");

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
