set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test/pfunit)

add_library(mpas_test_utils SHARED
        mpas_test_utils_mod.f90
)
add_library(check_c_string SHARED
        check_c_string.c
)
set_target_properties(check_c_string PROPERTIES
        LINKER_LANGUAGE C
)
target_include_directories(check_c_string PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if (MPAS_USE_PIO)
    add_pfunit_ctest(test_mpas_pio_put_att_mod
            TEST_SOURCES test_mpas_pio_put_att_mod.pf
            LINK_LIBRARIES MPAS::framework mpas_test_utils pfunit
    )

endif ()

add_pfunit_ctest(test_mpas_stream_list_mod
        TEST_SOURCES test_mpas_stream_list_mod.pf
        LINK_LIBRARIES MPAS::framework mpas_test_utils pfunit
)
add_pfunit_ctest(test_mpas_c_interfacing_mod
        TEST_SOURCES test_mpas_c_interfacing_mod.pf
        LINK_LIBRARIES MPAS::framework mpas_test_utils pfunit check_c_string
)

add_pfunit_ctest(test_mpas_basic_mod
        TEST_SOURCES test_mpas_basic_mod.pf
        LINK_LIBRARIES MPAS::core::core_atmosphere MPAS::external::smiol mpas_test_utils pfunit
        MAX_PES 4
)

