#ifndef DYNAMIC_OUTPUT_INTERVAL_STREAM_H
#define DYNAMIC_OUTPUT_INTERVAL_STREAM_H

#define MAX_INTERVALS 10
#define MAX_STREAMS 10
#define TIME_LEN 32

// Represents a single time range and associated output interval
typedef struct {
    char start[TIME_LEN];
    char end[TIME_LEN];   // Empty string means no end time
    char interval[TIME_LEN];
} IntervalRange;

// Represents a stream from the XML
typedef struct {
    char name[64];
    char filename_template[128];
    char type[32];
    char static_output_interval[TIME_LEN]; // Used if no dynamic intervals
    IntervalRange dynamic_intervals[MAX_INTERVALS];
    int num_intervals;
} Stream;

/**
 * @brief Parses the XML string and fills the streams array.
 *
 * @param streams      Preallocated array of Stream structs
 * @param xml_string   Null-terminated XML string containing <streams> root
 * @return int         Number of streams parsed
 */
int parse_streams(Stream *streams, const char *xml_string);

/**
 * @brief Returns the active output interval for a given stream and time.
 *
 * @param s             Pointer to the Stream to query
 * @param current_time  Null-terminated time string (e.g., "0006_00:00:00")
 * @return const char*  The interval string or NULL if not found
 */
const char *get_output_interval(const Stream *s, const char *current_time);

#endif //DYNAMIC_OUTPUT_INTERVAL_STREAM_H
