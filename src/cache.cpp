#include "cache.hpp"

LRUCache::LRUCache(int capacity)
    : capacity_(capacity), head_(nullptr), tail_(nullptr) {}

LRUCache::~LRUCache() {
    std::lock_guard<std::mutex> lock(mutex_);
    CacheNode* current = head_;
    while (current) {
        CacheNode* next = current->next;
        delete current;
        current = next;
    }
    for (auto node : pool_) {
        delete node;
    }
}

CacheNode* LRUCache::allocateNode() {
    if (pool_.empty()) {
        return new CacheNode();
    }
    CacheNode* node = pool_.back();
    pool_.pop_back();
    node->key.clear();
    node->value.clear();
    node->prev = nullptr;
    node->next = nullptr;
    return node;
}

void LRUCache::deallocateNode(CacheNode* node) {
    node->key.clear();
    node->value.clear();
    node->prev = nullptr;
    node->next = nullptr;
    pool_.push_back(node);
}

void LRUCache::moveToFront(CacheNode* node) {
    if (node == head_) return;

    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    if (node == tail_) tail_ = node->prev;

    node->prev = nullptr;
    node->next = head_;
    if (head_) head_->prev = node;
    head_ = node;
    if (!tail_) tail_ = node;
}

void LRUCache::addToFront(CacheNode* node) {
    node->prev = nullptr;
    node->next = head_;
    if (head_) head_->prev = node;
    head_ = node;
    if (!tail_) tail_ = node;
}

void LRUCache::evict() {
    if (!tail_) return;

    CacheNode* node = tail_;
    map_.erase(node->key);

    tail_ = node->prev;
    if (tail_) tail_->next = nullptr;
    else head_ = nullptr;

    deallocateNode(node);
}

std::string LRUCache::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = map_.find(key);
    if (it == map_.end()) return "";

    CacheNode* node = it->second;
    moveToFront(node);
    return node->value;
}

void LRUCache::put(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = map_.find(key);

    if (it != map_.end()) {
        CacheNode* node = it->second;
        node->value = value;
        moveToFront(node);
        return;
    }

    if ((int)map_.size() >= capacity_) {
        evict();
    }

    CacheNode* node = allocateNode();
    node->key = key;
    node->value = value;
    addToFront(node);
    map_[key] = node;
}

int LRUCache::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return (int)map_.size();
}
