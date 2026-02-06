#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

struct CacheNode {
    std::string key;
    std::string value;
    CacheNode* prev = nullptr;
    CacheNode* next = nullptr;
};

class LRUCache {
public:
    explicit LRUCache(int capacity);
    ~LRUCache();

    std::string get(const std::string& key);
    void put(const std::string& key, const std::string& value);
    int size() const;

private:
    int capacity_;
    std::unordered_map<std::string, CacheNode*> map_;
    CacheNode* head_;
    CacheNode* tail_;
    std::vector<CacheNode*> pool_;

    mutable std::mutex mutex_;

    CacheNode* allocateNode();
    void deallocateNode(CacheNode* node);
    void moveToFront(CacheNode* node);
    void addToFront(CacheNode* node);
    void evict();
};
