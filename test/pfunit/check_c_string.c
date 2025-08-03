#include "check_c_string.h"
#include <stdio.h>
#include <string.h>

int check_c_string(const char *cstring) {
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
        return 2;
    }

    return 0; // Success
}

void reverse_c_string(char *cstring) {
    if (cstring == NULL) {
        return; // Handle null pointer
    }

    size_t len = strlen(cstring);
    for (size_t i = 0; i < len / 2; i++) {
        char temp = cstring[i];
        cstring[i] = cstring[len - i - 1];
        cstring[len - i - 1] = temp;
    }
}
