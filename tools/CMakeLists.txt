cmake_minimum_required(VERSION 3.16)

project(pciids VERSION 0.0.2 LANGUAGES C CXX)

# compiler options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Default build type: Release." FORCE)
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/pciids")
    message(FATAL_ERROR  "pciids is required. please run:\n\tgit submodule init\n\tgit submodule update")
endif()

add_executable(${PROJECT_NAME} pciids.cpp)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "$<0:>"
)

target_compile_options(${PROJECT_NAME} 
    PRIVATE 
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /utf-8 /DUNICODE /D_UNICODE /DNOMINMAX /Zc:preprocessor /Zc:__cplusplus /wd5054>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Wno-deprecated-enum-enum-conversion>
)