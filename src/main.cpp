#include "cache.hpp"
#include <iostream>
#include <thread>
#include <vector>

void testBasicOperations() {
    LRUCache cache(3);

    std::cout << "Testing basic operations:" << std::endl;

    cache.put("key1", "value1");
    cache.put("key2", "value2");
    cache.put("key3", "value3");

    std::cout << "key1: " << cache.get("key1") << std::endl; 
    std::cout << "key2: " << cache.get("key2") << std::endl; 

    cache.put("key4", "value4"); 

    std::cout << "key3: " << cache.get("key3") << std::endl; 
    std::cout << "key4: " << cache.get("key4") << std::endl; 

    std::cout << "Cache size: " << cache.size() << std::endl;
}

void worker(LRUCache& cache, int id) {
    for (int i = 0; i < 10; ++i) {
        std::string key = "key" + std::to_string(id * 10 + i);
        std::string value = "value" + std::to_string(id * 10 + i);
        cache.put(key, value);
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
    }
}

void testThreadSafety() {
    LRUCache cache(20);

    std::cout << "\nTesting thread safety:" << std::endl;

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(worker, std::ref(cache), i);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final cache size: " << cache.size() << std::endl;

    for (int i = 0; i < 5; ++i) {
        std::string key = "key" + std::to_string(i * 10);
        std::string val = cache.get(key);
        if (!val.empty()) {
            std::cout << key << ": " << val << std::endl;
        }
    }
}

int main() {
    std::cout << "In-Memory LRU Cache Demo" << std::endl;

    testBasicOperations();
    testThreadSafety();

    std::cout << "\nDemo completed." << std::endl;

    return 0;
}
