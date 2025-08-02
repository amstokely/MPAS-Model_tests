#include "check_c_string.h"
int check_c_string(const char *cstring)
{
  const char *expected = "foo";
    if (cstring == NULL) {
        return 1; // Null pointer
    }

    // Check if the string is null-terminated within a reasonable limit
    size_t max_len = 1024;
    size_t len = strnlen(cstring, max_len);

    if (len == max_len) {
        // Not null-terminated within expected length
        return 1;
    }

    if (strcmp(cstring, expected) != 0) {
        // Content mismatch
        return 2;
    }

    return 0; // Success
}