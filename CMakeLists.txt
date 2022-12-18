cmake_minimum_required (VERSION 3.15)

set(TARGET_NAME azangara-mod)

project(${TARGET_NAME})

set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_GENERATOR_PLATFORM Win32)

set(DEFS
    # Compliation
    -DCXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"

    # Platform
    -DUNICODE
    -D_UNICODE
)

file(GLOB_RECURSE PROJECT_SOURCE_FILES "src/*.h" "src/*.cpp" "src/proxy/TextShaping.def")

if(WIN32 AND (MSVC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    add_definitions(/MT /utf-8)
endif()

add_library(${TARGET_NAME} SHARED "${PROJECT_SOURCE_FILES}")
# add_executable(${TARGET_NAME} "${PROJECT_SOURCE_FILES}")
target_compile_definitions(${TARGET_NAME} PRIVATE ${DEFS} )
include_directories(${TARGET_NAME} PRIVATE "src" "src/thirdparty" "libs")
target_link_libraries(${TARGET_NAME} ${PROJECT_SOURCE_DIR}/libs/minhook.lib)