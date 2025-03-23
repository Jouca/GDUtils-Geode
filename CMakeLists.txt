cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
include(cmake/CPM.cmake)

if(NOT DEFINED GEODE_TARGET_PLATFORM)
    set(GEODE_TARGET_PLATFORM "Win64")
    set(GEODE_GD_VERSION 2.2074)
    set(GEODE_COMP_GD_VERSION 22074)
endif()

if (${GEODE_TARGET_PLATFORM} STREQUAL "Win64")
    set(GEODE_COMPILER "clang-cl")
    set(GEODE_CCACHE_VARIANT "sccache")
    #set(GEODE_TRIPLE "x86_64-unknown-windows-msvc")
    if(WIN32) # force it!
      set(CMAKE_SYSTEM_PROCESSOR x86_64)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
    endif()
endif()

set(PROJECT_NAME GDUtils)
project(${PROJECT_NAME} VERSION 1.0.0)

# Use GLOB_RECURSE instead of GLOB
# to recursively add all source files 
# under src/ 
file(GLOB SOURCES
    src/*.cpp
    src/MoreLeaderboards/*.cpp
    src/Notifications/*.cpp
    src/Secret/*.cpp
    src/Settings/*.cpp
    src/Utils/*.cpp
    src/Styles/*.cpp
    include/*.cpp
)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES})

set(PAHO_BUILD_SHARED OFF CACHE INTERNAL "")
set(PAHO_BUILD_STATIC ON CACHE INTERNAL "")
set(PAHO_ENABLE_TESTING OFF CACHE INTERNAL "")
set(PAHO_BUILD_TESTS OFF CACHE INTERNAL "")
set(PAHO_WITH_MQTT_C ON CACHE INTERNAL "")
set(PAHO_WITH_SSL OFF CACHE INTERNAL "")

FetchContent_Declare(
    paho-mqttpp3-static
    GIT_REPOSITORY https://github.com/eclipse-paho/paho.mqtt.cpp.git
    GIT_TAG 71f78aafc3246c05d624098b8438129f5e0a70e5
)

FetchContent_MakeAvailable(paho-mqttpp3-static)

target_include_directories(${PROJECT_NAME} PRIVATE
  libs/filesystem
  libs/pl_mpeg/
  libs/glew/
  ${paho-mqttpp3-static_SOURCE_DIR}/src
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

if (APPLE) # AND NOT (${GEODE_TARGET_PLATFORM} STREQUAL "iOS" OR IOS)
  target_link_libraries(${PROJECT_NAME} paho-mqttpp3-static "-framework OpenGL")
else()
  target_link_libraries(${PROJECT_NAME} paho-mqttpp3-static)
endif()
if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" AND WIN32)
  message("compiling on linux!!")
  # why it requires advapi32 is beyond me
  target_link_libraries(${PROJECT_NAME} opengl32 ws2_32 crypt32 wldap32 normaliz advapi32)
elseif(${GEODE_TARGET_PLATFORM} STREQUAL "Win32")
  target_link_libraries(${PROJECT_NAME} opengl32 ws2_32 Crypt32 Wldap32 Normaliz)
endif()
if (WIN32)
  # Disable annoying warnings
  target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Set up dependencies, resources, link Geode
setup_geode_mod(${PROJECT_NAME})
