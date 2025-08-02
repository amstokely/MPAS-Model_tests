#ifndef CHECK_C_STRING_H
#define CHECK_C_STRING_H

#include <stdio.h>
#include <string.h>

// Compare the received string to an expected value.
// Return 0 on success, 1 on null termination failure, 2 on mismatch.
int check_c_string(const char *cstring);


#endif //CHECK_C_STRING_H
