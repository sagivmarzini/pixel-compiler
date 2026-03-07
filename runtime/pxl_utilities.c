#include "pxl_utilities.h"

#include <stdio.h>
#include <stdlib.h>

noreturn void pxl_runtime_error(const char* message) {
    fprintf(stderr, "Runtime error: %s\n", message);
    abort();
}
