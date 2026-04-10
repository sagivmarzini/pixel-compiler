#ifndef COMPILER_PROJECT_PXL_ERROR_H
#define COMPILER_PROJECT_PXL_ERROR_H

#include <stdnoreturn.h>

// Crashes execution with the specified error message
noreturn void pxl_runtime_error(const char* message);

#endif //COMPILER_PROJECT_PXL_ERROR_H
