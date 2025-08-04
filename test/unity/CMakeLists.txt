
add_executable(test_stream_inquiry test_stream_inquiry.c)
set_target_properties(test_stream_inquiry PROPERTIES
        LINKER_LANGUAGE C
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test/unity
)
target_link_libraries(test_stream_inquiry PRIVATE
        unity::framework MPAS::external::ezxml MPAS::framework
)

add_test(NAME test_stream_inquiry
        COMMAND test_stream_inquiry)

add_executable(test_xml_stream_parser test_xml_stream_parser.c)
set_target_properties(test_xml_stream_parser PROPERTIES
        LINKER_LANGUAGE C
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test/unity
)
target_link_libraries(test_xml_stream_parser PRIVATE
        unity::framework MPAS::external::ezxml MPAS::framework
)

add_test(NAME test_xml_stream_parser
        COMMAND test_xml_stream_parser)

add_executable(test_regex_matching test_regex_matching.c)
set_target_properties(test_regex_matching PROPERTIES
        LINKER_LANGUAGE C
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test/unity
)
target_link_libraries(test_regex_matching PRIVATE
        unity::framework MPAS::framework
)

add_test(NAME test_regex_matching
        COMMAND test_regex_matching)

add_executable(test_variable_stream_interval test_variable_stream_interval.c)
set_target_properties(test_variable_stream_interval PROPERTIES
        LINKER_LANGUAGE C
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test/unity
)
target_link_libraries(test_variable_stream_interval PRIVATE
        unity::framework MPAS::framework
)

add_test(NAME test_variable_stream_interval
        COMMAND test_variable_stream_interval)
