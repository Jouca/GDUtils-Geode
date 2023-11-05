cmake_minimum_required(VERSION 3.3.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(GEODE_COMPILER "clang-cl")
set(GEODE_TRIPLE "i686-unknown-windows-msvc")

set(PROJECT_NAME GDUtils)
project(${PROJECT_NAME} VERSION 1.0.0)

# Use GLOB_RECURSE instead of GLOB
# to recursively add all source files 
# under src/ 
file(GLOB SOURCES
	src/*.cpp
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
if (APPLE)
  target_link_libraries(${PROJECT_NAME} sioclient "-framework OpenGL")
else()
  target_link_libraries(${PROJECT_NAME} sioclient opengl32)
endif()
if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
  message("compiling on linux!!")
  # why it requires advapi32 is beyond me
  target_link_libraries(${PROJECT_NAME} ws2_32 crypt32 wldap32 normaliz advapi32 comdlg32)
elseif(WIN32)
  target_link_libraries(${PROJECT_NAME} ws2_32 Crypt32 Wldap32 Normaliz)
endif()

# Set up dependencies, resources, link Geode
setup_geode_mod(${PROJECT_NAME})
