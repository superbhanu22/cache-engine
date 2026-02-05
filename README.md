# In-Memory LRU Cache Engine

A thread-safe in-memory cache implementation in C++ with LRU (Least Recently Used) eviction policy and custom memory management.

## Features

- **LRU Eviction**: Automatically removes the least recently used items when capacity is exceeded
- **Thread Safety**: Uses mutexes to ensure safe concurrent access from multiple threads
- **Custom Memory Management**: Implements a simple object pool for cache nodes to reduce memory allocation overhead
- **C++17 Standard**: Utilizes modern C++ features for efficiency

## Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler (GCC, Clang, MSVC)
- POSIX threads support (usually available on Unix-like systems)

## Building the Project

1. Clone or navigate to the project directory:
   ```
   cd /path/to/cache-engine
   ```

2. Create a build directory:
   ```
   mkdir build
   cd build
   ```

3. Generate build files with CMake:
   ```
   cmake ..
   ```

4. Build the executable:
   ```
   make
   ```

## Running the Code

After building, run the executable from the build directory:

```
./cache
```

This will execute the demo program that demonstrates:
- Basic cache operations (put/get)
- LRU eviction behavior
- Thread-safe concurrent access with multiple threads

## Running Tests

The project includes comprehensive tests that verify functionality, LRU behavior, and thread safety. Tests use external data files instead of hardcoded values.

Build and run individual tests:

```
# Basic functionality tests
./test_basic

# LRU eviction tests
./test_lru

# Thread safety tests
./test_thread_safety
```

All tests load data from `test/test_data.txt` and `test/lru_test_data.txt` files for non-hardcoded test scenarios.

## Project Structure

```
cache-engine/
├── CMakeLists.txt          # CMake build configuration
├── README.md              # This file
├── src/
│   ├── cache.hpp          # Cache class header
│   ├── cache.cpp          # Cache implementation
│   └── main.cpp           # Demo program
└── test/
    ├── test_basic.cpp     # Basic functionality tests
    ├── test_lru.cpp       # LRU eviction tests
    ├── test_thread_safety.cpp  # Thread safety tests
    ├── test_data.txt      # Test data for basic/thread tests
    └── lru_test_data.txt  # Test data for LRU tests
```

## Implementation Details

### Data Structures
- **Hash Map**: `std::unordered_map` for O(1) key lookups
- **Doubly Linked List**: Custom implementation for O(1) LRU ordering operations
- **Object Pool**: Vector-based pool for reusing cache nodes to minimize allocations

### Thread Safety
- All public methods are protected with `std::mutex`
- Uses `std::lock_guard` for RAII-style locking

### Memory Management
- Custom allocator that maintains a pool of recycled nodes
- Reduces heap allocations by reusing evicted nodes
- Automatic cleanup in destructor

## Usage Example

```cpp
#include "cache.hpp"

// Create cache with capacity of 100 items
LRUCache cache(100);

// Store key-value pairs
cache.put("user:123", "{\"name\":\"John\"}");
cache.put("config:app", "{\"version\":\"1.0\"}");

// Retrieve values
std::string userData = cache.get("user:123"); // Returns the JSON string
std::string missing = cache.get("nonexistent"); // Returns empty string
```

## Notes

- The cache uses `std::string` for both keys and values
- Get operations return an empty string if the key is not found
- All operations are thread-safe and can be called concurrently
- The implementation is not copyable or movable to prevent accidental sharing
