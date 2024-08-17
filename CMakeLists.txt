cmake_minimum_required(VERSION 3.10)

# Project Name
project(Watchdog)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the path to the vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "D:/Program Files/Microsoft Visual Studio/vcpkg-2024.07.12/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")

# Find packages
find_package(nlohmann_json CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

# Source files for the library
set(LIB_SOURCES
    src/ConfigurationLoader.cpp
    src/WatchdogWorkflow.cpp
    src/WindowsAPI.cpp
    src/LinuxAPI.cpp
    src/Logger.cpp
)

# Create a static library for the main application
add_library(WatchdogLib STATIC ${LIB_SOURCES})

# Include directories for the library
target_include_directories(WatchdogLib PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/nlohmann
)

# Link libraries for the static library
target_link_libraries(WatchdogLib PRIVATE nlohmann_json::nlohmann_json)

# Create an executable for the main application
add_executable(Watchdog src/main.cpp)
target_link_libraries(Watchdog PRIVATE WatchdogLib nlohmann_json::nlohmann_json)

# Include directories for the executable
target_include_directories(Watchdog PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Copy configuration file to the build output directory
add_custom_command(TARGET Watchdog POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/config/config.json
        $<TARGET_FILE_DIR:Watchdog>/config.json)

# Source files for the test application
set(TEST_SOURCES
    tests/test_NativeAPI.cpp
)

# Create an executable for the tests
add_executable(test_NativeAPI ${TEST_SOURCES})

# Include directories for the test executable
target_include_directories(test_NativeAPI PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/nlohmann
)

# Link libraries for the test executable
target_link_libraries(test_NativeAPI PRIVATE
    GTest::gtest
    GTest::gtest_main
    nlohmann_json::nlohmann_json
    WatchdogLib  # Link against the library to resolve symbols
)

# Discover tests
include(GoogleTest)
gtest_discover_tests(test_NativeAPI)
