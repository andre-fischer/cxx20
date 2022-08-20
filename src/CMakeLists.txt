message("this is src")

add_library(
    cxx20lib
        OBJECT
            concepts/printF.hxx
            coroutines/Generator.cxx
            util/Assert.cxx
)
set_target_properties(
    cxx20lib
        PROPERTIES LINKER_LANGUAGE CXX
)
target_include_directories(
    cxx20lib
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src
)
set_source_files_properties(
        coroutines/Generator.cxx PROPERTIES COMPILE_FLAGS -fcoroutines
)
