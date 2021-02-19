aux_source_directory(./SOIL2 SOIL2_SRC)

file(GLOB_RECURSE SOIL2_HEADERS ./SOIL2/*.h)
source_group("include/SOIL2" FILES ${SOIL2_HEADERS})

add_library(
    SOIL2
    ${SOIL2_SRC}
    ${SOIL2_HEADERS}
)

