cmake_minimum_required(VERSION 3.14)
project(s2t_wenet VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(WITH_TORCH "Build with PyTorch support" ON)
option(BUILD_TESTING "Build tests" OFF)
option(MOCK_IMPLEMENTATION "Use mock implementation (without actual WeNet dependencies)" OFF)

# Find dependencies
if(WITH_TORCH AND NOT MOCK_IMPLEMENTATION)
    find_package(Torch QUIET)
    if(NOT Torch_FOUND)
        message(STATUS "Torch not found, using mock implementation")
        set(MOCK_IMPLEMENTATION ON)
    endif()
endif()

# WeNet paths
set(WENET_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../wenet_build/wenet" CACHE PATH "WeNet root directory")
if(EXISTS "${WENET_ROOT}/runtime/core/api/wenet_api.h")
    set(WENET_INCLUDE_DIRS 
        "${WENET_ROOT}/runtime/core"
        "${WENET_ROOT}/runtime/core/api"
        "${WENET_ROOT}/runtime/core/decoder"
        "${WENET_ROOT}/runtime/core/frontend"
    )
    message(STATUS "Using WeNet includes from: ${WENET_ROOT}")
else()
    message(STATUS "WeNet headers not found at ${WENET_ROOT}, using mock implementation")
    set(MOCK_IMPLEMENTATION ON)
endif()

# Create include directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/impl/include/wenet)

# Copy WeNet includes to our include directory if available and not using mock
if(NOT MOCK_IMPLEMENTATION AND EXISTS "${WENET_ROOT}/runtime/core/api/wenet_api.h")
    file(COPY ${WENET_INCLUDE_DIRS} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/impl/include/wenet)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/impl/include
)

if(NOT MOCK_IMPLEMENTATION)
    include_directories(${WENET_INCLUDE_DIRS})
endif()

# Find additional libraries if not using mock
if(NOT MOCK_IMPLEMENTATION)
    find_library(WENET_LIB wenet PATHS ${WENET_ROOT}/runtime/build/lib)
    find_library(GLOG_LIB glog)
    find_library(GFLAGS_LIB gflags)

    # If still not found, use mock implementation
    if(NOT WENET_LIB)
        message(STATUS "WeNet library not found, using mock implementation")
        set(MOCK_IMPLEMENTATION ON)
    endif()
endif()

# Source files
set(SOURCES
    impl/include/WenetSTTImpl.cpp
)

# Add mock implementation if needed
if(MOCK_IMPLEMENTATION)
    message(STATUS "Building with mock implementation of WeNet")
    add_definitions(-DMOCK_WENET_IMPLEMENTATION)
    set(SOURCES ${SOURCES} impl/include/MockWenetImpl.cpp)
endif()

# Build the library
add_library(wenetcpp ${SOURCES})

# Link with dependencies if not using mock
if(NOT MOCK_IMPLEMENTATION)
    if(Torch_FOUND)
        target_link_libraries(wenetcpp PRIVATE ${TORCH_LIBRARIES})
    endif()
    
    if(WENET_LIB)
        target_link_libraries(wenetcpp PRIVATE ${WENET_LIB})
    endif()
    
    if(GLOG_LIB)
        target_link_libraries(wenetcpp PRIVATE ${GLOG_LIB})
    endif()
    
    if(GFLAGS_LIB)
        target_link_libraries(wenetcpp PRIVATE ${GFLAGS_LIB})
    endif()
endif()

# Installation
install(TARGETS wenetcpp
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/impl/include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")

# Testing
if(BUILD_TESTING)
    # Find testing dependencies
    find_package(GTest QUIET)
    
    if(NOT GTest_FOUND)
        message(STATUS "GTest not found, using simple test approach")
    endif()
    
    # Test executable
    add_executable(wenet_tests test/WenetTest.cpp)
    
    # Link test executable with our library
    target_link_libraries(wenet_tests PRIVATE wenetcpp)
    
    if(NOT MOCK_IMPLEMENTATION)
        if(GLOG_LIB)
            target_link_libraries(wenet_tests PRIVATE ${GLOG_LIB})
        endif()
        
        if(GFLAGS_LIB)
            target_link_libraries(wenet_tests PRIVATE ${GFLAGS_LIB})
        endif()
    endif()
    
    if(GTest_FOUND)
        target_link_libraries(wenet_tests PRIVATE GTest::GTest GTest::Main)
    endif()
    
    # Install test executable
    install(TARGETS wenet_tests
            RUNTIME DESTINATION bin)
endif()