cmake_minimum_required(VERSION 3.5.0)

set(GEODE_GD_VERSION 2.206)
set(GEODE_COMP_GD_VERSION 22060)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT DEFINED GEODE_TARGET_PLATFORM)
    set(GEODE_TARGET_PLATFORM "Win32")
endif()
if (${GEODE_TARGET_PLATFORM} STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

if (WIN32)
    set(GEODE_COMPILER "clang-cl")
    set(GEODE_CCACHE_VARIANT "sccache")
    set(GEODE_TRIPLE "i686-unknown-windows-msvc")
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
)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
  libs/socket.io-client-cpp
  libs/filesystem
  libs/pl_mpeg/
  libs/glew/
  libs/socket.io-client-cpp/src
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

add_subdirectory(libs/socket.io-client-cpp)
if (APPLE) # AND NOT (${GEODE_TARGET_PLATFORM} STREQUAL "iOS" OR IOS)
  target_link_libraries(${PROJECT_NAME} sioclient "-framework OpenGL")
  #${CMAKE_CURRENT_SOURCE_DIR}/libs/link-libs/discord-rpc/libdiscord-rpc.a
else()
  if (WIN32)
  target_link_libraries(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/libs/socket.io-client-cpp/sioclient.lib)
  else()
  target_link_libraries(${PROJECT_NAME} sioclient)
  endif()
  #${CMAKE_CURRENT_SOURCE_DIR}/libs/link-libs/discord-rpc/discord-rpc.lib
endif()
if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" AND WIN32)
  message("compiling on linux!!")
  # why it requires advapi32 is beyond me
  target_link_libraries(${PROJECT_NAME} opengl32 ws2_32 crypt32 wldap32 normaliz advapi32 comdlg32)
elseif(WIN32)
  target_link_libraries(${PROJECT_NAME} opengl32 ws2_32 Crypt32 Wldap32 Normaliz)
endif()

# Set up dependencies, resources, link Geode
setup_geode_mod(${PROJECT_NAME})
