cmake_minimum_required(VERSION 3.10)
project(cache-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add main executable
add_executable(cache src/main.cpp src/cache.cpp)

# Include directories
target_include_directories(cache PRIVATE src)

# Find and link threads library for mutex support
find_package(Threads REQUIRED)
target_link_libraries(cache Threads::Threads)

# Add test executables
add_executable(test_basic test/test_basic.cpp src/cache.cpp)
add_executable(test_lru test/test_lru.cpp src/cache.cpp)
add_executable(test_thread_safety test/test_thread_safety.cpp src/cache.cpp)

# Include directories for tests
target_include_directories(test_basic PRIVATE src)
target_include_directories(test_lru PRIVATE src)
target_include_directories(test_thread_safety PRIVATE src)

# Link threads for all test executables
target_link_libraries(test_basic Threads::Threads)
target_link_libraries(test_lru Threads::Threads)
target_link_libraries(test_thread_safety Threads::Threads)
