cmake_minimum_required(VERSION 3.8)
project(api_server VERSION 0.1.0)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Boost REQUIRED)

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})

add_library(api_server_lib
            src/filesystem/monitor.cpp
            src/server/server.cpp
            src/server/router.cpp)
target_link_libraries(api_server_lib nlohmann_json::nlohmann_json fmt::fmt)

add_executable(api_server src/main.cpp)
target_link_libraries(api_server api_server_lib)

add_subdirectory(test)

# Create deb pkg

install(TARGETS api_server
    RUNTIME DESTINATION "/usr/bin"
    LIBRARY DESTINATION "/opt/api_server"
    DESTINATION "/opt/api_server"
)

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "grhmdev@users.noreply.github.com")
INCLUDE(CPack)