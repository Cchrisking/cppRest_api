cmake_minimum_required(VERSION 3.20)
project(crowJourney VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option for shared libs
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

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
# Define executable based on TARGET_TO_BUILD
if(TARGET_TO_BUILD STREQUAL "book_server")
    message(WARNING "Targeting book_server")
    add_executable(book_api_server src/library.cpp
            src/models/Book.cpp
            src/models/Book.h
            src/controllers/LibraryController.cpp
            src/controllers/LibraryController.h
            src/utils/utils.h)
    target_link_libraries(book_api_server ${Boost_LIBRARIES} Threads::Threads)
    set(TARGET_BIN book_api_server)

elseif(TARGET_TO_BUILD STREQUAL "user_app")
    message(WARNING "Targeting user app")
    add_executable(user_app src/users.cpp
            src/models/User.cpp
            src/models/User.h
            src/controllers/UserController.cpp
            src/controllers/UserController.h
            src/utils/utils.h)
    target_link_libraries(user_app ${Boost_LIBRARIES} Threads::Threads)
    set(TARGET_BIN user_app)

elseif(TARGET_TO_BUILD STREQUAL "run_tests")
    message(WARNING "Targeting test app")
    add_executable(run_tests tests/test_api.cpp
            src/utils/utils.h)
    target_link_libraries(run_tests ${Boost_LIBRARIES} Threads::Threads)
    set(TARGET_BIN run_tests)
else()
    message(WARNING "Targeting main app no target specified")
    add_executable(rest_app src/main.cpp
            src/utils/utils.h)
    target_link_libraries(rest_app ${Boost_LIBRARIES} Threads::Threads)
    set(TARGET_BIN rest_app)
endif()

# Windows specific
if(WIN32)
    target_compile_definitions(${TARGET_BIN} PRIVATE -D_WIN32_WINNT=0x0601)
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

# Info
message(STATUS "Target to build: ${TARGET_TO_BUILD}")