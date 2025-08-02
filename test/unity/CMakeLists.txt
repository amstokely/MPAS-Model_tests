add_library(dynamic_output_interval_stream SHARED dynamic_output_interval_stream.c)
set_target_properties(dynamic_output_interval_stream PROPERTIES
        LINKER_LANGUAGE C
)
target_link_libraries(dynamic_output_interval_stream PUBLIC
        MPAS::external::ezxml
)


add_executable(test_dynamic_output_interval_stream test_dynamic_output_interval_stream.c)
set_target_properties(test_dynamic_output_interval_stream PROPERTIES
        LINKER_LANGUAGE C
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test/unity
)
target_link_libraries(test_dynamic_output_interval_stream PRIVATE
        dynamic_output_interval_stream unity::framework
)

add_test(NAME test_dynamic_output_interval_stream
         COMMAND test_dynamic_output_interval_stream)