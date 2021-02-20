aux_source_directory(. RGLU_SRC)
aux_source_directory(./shape RGLU_SHAPE_SRC)

file(GLOB_RECURSE RGLU_HEADERS ./*.h)
file(GLOB_RECURSE RGLU_SHAPE_HEADERS ./shape/*.h)
source_group("include/rglu" FILES ${RGLU_HEADERS} ${RGLU_SHAPE_HEADERS})

add_library(
    rglu
    ${RGLU_SRC}
    ${RGLU_SHAPE_SRC}
    ${RGLU_HEADERS}
    ${RGLU_SHAPE_HEADERS}
)

target_link_libraries(
    rglu
    glad
    rlib
    cglm
    SOIL2
    assimp
)
