#include "unity.h"
#include "dynamic_output_interval_stream.h"
#include <stdlib.h>
#include <string.h>

static char *test_xml = NULL;
static Stream streams[MAX_STREAMS];
static int num_streams = 0;

void setUp(void) {
    const char *xml_src =
        "  <stream name=\"variable_output\" type=\"output\" filename_template=\"output.nc\">"
        "    <output_interval>"
        "      <range start=\"0000_00:00:00\" end=\"0005_00:00:00\" interval=\"0_01:00:00\"/>"
        "    </output_interval>"
        "  </stream>";

    test_xml = strdup(xml_src);
    num_streams = parse_streams(streams, test_xml);
}

void tearDown(void) {
    free(test_xml);
    test_xml = NULL;
}

void test_check_output_interval_type(void) {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_check_output_interval_type);
    return UNITY_END();
}
