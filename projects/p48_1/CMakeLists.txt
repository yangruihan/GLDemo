set(PROJ_NAME p48_1)

project(${PROJ_NAME})

set(SOURCE_PATH ${PROJECT_SOURCE_DIR})
configure_file(
    "${PROJECT_SOURCE_DIR}/../framework/include/fconfig.h.in"
    "${PROJECT_SOURCE_DIR}/include/fconfig.h"
)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_FILES)
file(GLOB FRAMEWORK_SRC_FILES "${PROJECT_SOURCE_DIR}/../framework/src/*.c")
file(GLOB HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")
file(GLOB FRAMEWORK_HEADERS "${PROJECT_SOURCE_DIR}/../framework/include/*.h")

source_group("Headers" FILES ${HEADERS})
source_group("Headers" FILES ${FRAMEWORK_HEADERS})

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/../framework/include)

if (WIN32)
    set(EXECUTABLE_OUTPUT_PATH "${PROJ_SOURCE_ROOT}/bin/${PROJ_NAME}")
else (WIN32)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(EXECUTABLE_OUTPUT_PATH "${PROJ_SOURCE_ROOT}/bin/Debug")
    else (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(EXECUTABLE_OUTPUT_PATH "${PROJ_SOURCE_ROOT}/bin/Release")
    endif (CMAKE_BUILD_TYPE MATCHES "Debug")
endif (WIN32)

add_executable(
    ${PROJ_NAME}
    ${SRC_FILES}
    ${FRAMEWORK_SRC_FILES}
    ${HEADERS}
    ${FRAMEWORK_HEADERS}
)

target_link_libraries(
    ${PROJ_NAME}
    glad
    glfw
    rlib
    cglm
    rglu
)
