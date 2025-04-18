cmake_minimum_required(VERSION 3.20)
project(crowJourney VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option for shared libs
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Force user to define the target to build
if(NOT DEFINED TARGET_TO_BUILD)
    message(FATAL_ERROR "Please specify -DTARGET_TO_BUILD=<target>. Options: book_api_server, user_app, run_tests")
endif()

# Dependencies
find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

# nlohmann_json fallback
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp
            PATHS /usr/include /usr/local/include ${CMAKE_SOURCE_DIR}/external/json/include)
    if(NOT NLOHMANN_JSON_INCLUDE_DIR)
        message(FATAL_ERROR "Could not find nlohmann/json.hpp")
    endif()
    include_directories(${NLOHMANN_JSON_INCLUDE_DIR})
endif()

# Crow fallback
find_path(CROW_INCLUDE_DIR crow.h
        PATHS /usr/include /usr/local/include ${CMAKE_SOURCE_DIR}/external/crow/include)
if(NOT CROW_INCLUDE_DIR)
    message(FATAL_ERROR "Could not find crow.h")
endif()

# Include directories
include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${CROW_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
)

# Common library
add_library(crowJourney src/library.cpp)
target_include_directories(crowJourney PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(crowJourney ${Boost_LIBRARIES} Threads::Threads)

# Define executable based on TARGET_TO_BUILD
if(TARGET_TO_BUILD STREQUAL "book_api_server")
    add_executable(book_api_server src/main.cpp)
    target_link_libraries(book_api_server crowJourney ${Boost_LIBRARIES} Threads::Threads)
    set(TARGET_BIN book_api_server)

elseif(TARGET_TO_BUILD STREQUAL "user_app")
    add_executable(user_app src/users.cpp)
    target_link_libraries(user_app crowJourney ${Boost_LIBRARIES} Threads::Threads)
    set(TARGET_BIN user_app)

elseif(TARGET_TO_BUILD STREQUAL "run_tests")
    add_executable(run_tests tests/test_api.cpp)
    target_link_libraries(run_tests crowJourney ${Boost_LIBRARIES} Threads::Threads)
    set(TARGET_BIN run_tests)

else()
    message(FATAL_ERROR "Unknown TARGET_TO_BUILD: ${TARGET_TO_BUILD}")
endif()

# Windows specific
if(WIN32)
    target_compile_definitions(${TARGET_BIN} PRIVATE -D_WIN32_WINNT=0x0601)
    target_compile_definitions(crowJourney PRIVATE -D_WIN32_WINNT=0x0601)

    add_compile_options(/wd4996)  # Disable deprecated warnings

    # Set output directory
    set_target_properties(${TARGET_BIN}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# Installation
install(TARGETS ${TARGET_BIN}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
        DESTINATION include/crowJourney
        FILES_MATCHING PATTERN "*.h"
)

# Info
message(STATUS "Target to build: ${TARGET_TO_BUILD}")
