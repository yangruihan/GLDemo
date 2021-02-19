cmake_minimum_required(VERSION 3.1)

set(PROJ_NAME GLDemo)

project(${PROJ_NAME} VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

option(DEBUG "Debug Version" on)

if(WIN32 AND MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG /MTd")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG /MTd")
elseif(APPLE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
else()
    if(DEBUG)
        add_definitions(-DDEBUG)
    endif(DEBUG)
endif()

set(PROJ_SOURCE_ROOT ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/libs/cglm/include)
include_directories(${PROJECT_SOURCE_DIR}/libs/glad/include)
include_directories(${PROJECT_SOURCE_DIR}/libs/glfw/include)
include_directories(${PROJECT_SOURCE_DIR}/libs/rlib/include)
include_directories(${PROJECT_SOURCE_DIR}/libs/rglu/include)

add_subdirectory(libs/glad)
add_subdirectory(libs/glfw)
add_subdirectory(libs/rlib)
add_subdirectory(libs/cglm)
add_subdirectory(libs/rglu)
add_subdirectory(projects)