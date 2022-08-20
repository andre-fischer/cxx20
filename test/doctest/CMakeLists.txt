find_package(doctest REQUIRED)

add_executable(
    doctest-main
        doctest-main.cxx
        concepts/printF-Test.cxx
        coroutines/Generator-Test.cxx
        $<TARGET_OBJECTS:cxx20lib>
)
target_compile_features(
    doctest-main
        PRIVATE
            cxx_std_20
)
target_include_directories(
    doctest-main
        PRIVATE
            .
            ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(
    doctest-main
        PRIVATE
            doctest::doctest
)
set_source_files_properties(
        coroutines/Generator-Test.cxx PROPERTIES COMPILE_FLAGS -fcoroutines
)
