#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ezxml.h"
#include "dynamic_output_interval_stream.h"



int parse_streams(Stream *streams, const char *xml_string) {
    char *writable_xml = strdup(xml_string);  // allocate writable memory
    ezxml_t root = ezxml_parse_str(writable_xml, strlen(writable_xml));
    if (!root) {
        fprintf(stderr, "Failed to parse XML\n");
        return 0;
    }

    int count = 0;
    for (ezxml_t stream_elem = ezxml_child(root, "stream");
         stream_elem && count < MAX_STREAMS;
         stream_elem = stream_elem->next, count++) {

        Stream *s = &streams[count];

        strncpy(s->name, ezxml_attr(stream_elem, "name"), sizeof(s->name));
        strncpy(s->filename_template, ezxml_attr(stream_elem, "filename_template"), sizeof(s->filename_template));
        strncpy(s->type, ezxml_attr(stream_elem, "type"), sizeof(s->type));

        const char *static_interval = ezxml_attr(stream_elem, "output_interval");
        s->static_output_interval[0] = '\0';
        s->num_intervals = 0;

        if (static_interval) {
            strncpy(s->static_output_interval, static_interval, TIME_LEN);
        }

        ezxml_t interval_block = ezxml_child(stream_elem, "output_interval");
        if (interval_block) {
            for (ezxml_t r = ezxml_child(interval_block, "range");
                 r && s->num_intervals < MAX_INTERVALS;
                 r = r->next) {

                IntervalRange *ir = &s->dynamic_intervals[s->num_intervals++];
                const char *start = ezxml_attr(r, "start");
                const char *end = ezxml_attr(r, "end");
                const char *interval = ezxml_attr(r, "interval");

                strncpy(ir->start, start ? start : "", TIME_LEN);
                strncpy(ir->end, end ? end : "", TIME_LEN);
                strncpy(ir->interval, interval ? interval : "", TIME_LEN);
            }
        }
    }

    ezxml_free(root);
    return count;
}

const char *get_output_interval(const Stream *s, const char *current_time) {
    if (s->num_intervals == 0) {
        return s->static_output_interval;
    }

    for (int i = 0; i < s->num_intervals; ++i) {
        const IntervalRange *ir = &s->dynamic_intervals[i];
        if (strcmp(current_time, ir->start) >= 0 &&
            (strlen(ir->end) == 0 || strcmp(current_time, ir->end) < 0)) {
            return ir->interval;
        }
    }

    return NULL;
}
