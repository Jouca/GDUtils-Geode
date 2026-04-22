cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    add_definitions(-DGLES_SILENCE_DEPRECATION)
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
include(cmake/CPM.cmake)

set(PROJECT_NAME GDUtils)
project(${PROJECT_NAME} VERSION 1.0.0)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES})

set(ENABLE_SSL_SUPPORT OFF)
set(BUILD_EXAMPLES OFF)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTING OFF)
set(BUILD_TOOLS OFF)
set(BUILD_TOOLS_DOCS OFF)
#set(INSTALL_STATIC_LIBS OFF)
CPMAddPackage("gh:alanxz/rabbitmq-c#035eaa8")

if (WIN32)
  target_compile_options(rabbitmq-static PRIVATE -Wno-incompatible-pointer-types)
endif()

set(BUILD_TESTING OFF)
set(BUILD_BENCH OFF)
set(BUILD_SHARED_LIBS OFF)

#CPMAddPackage("gh:libuv/libuv#c1a9f01")
#CPMAddPackage("gh:CopernicaMarketingSoftware/AMQP-CPP#ca49382")
CPMAddPackage("gh:camila314/uibuilder#main") # i love it

target_include_directories(${PROJECT_NAME} PRIVATE
  libs/filesystem
  libs/pl_mpeg/
  libs/glew/
  ${rabbitmq-c_SOURCE_DIR}/include
  src/
  #${amqpcpp_SOURCE_DIR}/src
  #${amqpcpp_SOURCE_DIR}/include
  #${libuv_SOURCE_DIR}/include
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

if (APPLE AND NOT (${CMAKE_SYSTEM_NAME} STREQUAL "iOS" OR IOS))
    target_link_libraries(${PROJECT_NAME} UIBuilder rabbitmq::rabbitmq-static "-framework OpenGL")
    #target_link_libraries(${PROJECT_NAME} uv_a amqpcpp "-framework OpenGL")
else()
    target_link_libraries(${PROJECT_NAME} UIBuilder rabbitmq::rabbitmq-static)
    #target_link_libraries(${PROJECT_NAME} uv_a amqpcpp)
endif()
if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" AND WIN32)
  message("compiling on linux!!")
  # why it requires advapi32 is beyond me
  target_link_libraries(${PROJECT_NAME} opengl32 ws2_32 crypt32 wldap32 normaliz advapi32)
elseif(${GEODE_TARGET_PLATFORM} STREQUAL "Win32")
  target_link_libraries(${PROJECT_NAME} opengl32 ws2_32 Crypt32 Wldap32 Normaliz)
endif()
if (WIN32)
  add_compile_definitions(WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0601)
  # Disable annoying warnings
  target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Set up dependencies, resources, link Geode
setup_geode_mod(${PROJECT_NAME})
