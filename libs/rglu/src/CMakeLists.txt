aux_source_directory(. RGLU_SRC)

file(GLOB_RECURSE RGLU_HEADERS ./*.h)
source_group("include/rglu" FILES ${RGLU_HEADERS})

add_library(
    rglu
    ${RGLU_SRC}
    ${RGLU_HEADERS}
)

target_link_libraries(
    rglu
    glad
    rlib
)