cmake_minimum_required(VERSION 3.16)

project(probe VERSION 0.2.4 LANGUAGES C CXX)

# options
option(PROBE_EXAMPLES "Add probe examples to the build." OFF)

# compiler options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Default build type: Release." FORCE)
endif()

if(MSVC)
    message(STATUS "Windows SDK version is ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")

    add_compile_options(/W3 /utf-8 /DUNICODE /D_UNICODE /DNOMINMAX /Zc:preprocessor  /Zc:__cplusplus)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# depdencies
if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    if(NOT X11_Xrandr_FOUND)
        message(FATAL_ERROR "Xrandr is required")
    endif()
endif()

# version header file
configure_file(${PROJECT_SOURCE_DIR}/version.h.in ${PROJECT_BINARY_DIR}/version.h @ONLY)

#
file(GLOB_RECURSE PROBE_HEADERS include/probe/*.h)
file(GLOB_RECURSE PROBE_SOURCES src/*.cpp)

add_library(${PROJECT_NAME} ${PROBE_SOURCES} ${PROBE_HEADERS})
add_library(probe::probe ALIAS probe)

target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        $<$<PLATFORM_ID:Windows>:dwmapi>
        $<$<PLATFORM_ID:Windows>:strmiids>
        $<$<PLATFORM_ID:Windows>:ntdll>
        $<$<PLATFORM_ID:Windows>:Shcore>
        $<$<PLATFORM_ID:Windows>:DXGI>
        $<$<PLATFORM_ID:Windows>:D3D11>
        $<$<PLATFORM_ID:Windows>:SetupAPI>
        $<$<PLATFORM_ID:Linux>:pthread>
        $<$<PLATFORM_ID:Linux>:X11::Xrandr>
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        
        PUBLIC_HEADER "${PROBE_HEADERS}"

        RUNTIME_OUTPUT_DIRECTORY "bin/$<0:>"
        LIBRARY_OUTPUT_DIRECTORY "bin/$<0:>"
        ARCHIVE_OUTPUT_DIRECTORY "lib/$<0:>"

        DEBUG_POSTFIX "d"
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC PROBE_SHARED PRIVATE PROBE_BUILDING)
endif()

if(PROBE_EXAMPLES)
    foreach(example cpu;graphics;system;disk;util)
        add_executable(probe_${example} examples/${example}.cpp)
        target_link_libraries(probe_${example} PUBLIC probe::probe)

        set_target_properties(probe_${example}
            PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "bin/$<0:>"
                LIBRARY_OUTPUT_DIRECTORY "bin/$<0:>"
                ARCHIVE_OUTPUT_DIRECTORY "lib/$<0:>"
        )
    endforeach()

    # 
endif()