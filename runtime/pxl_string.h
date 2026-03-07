#ifndef COMPILER_PROJECT_PXL_STRING_H
#define COMPILER_PROJECT_PXL_STRING_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    size_t size;
    char data[];
} pxl_string;

pxl_string* pxl_create_string(const char* data, size_t size);

void pxl_destroy_string(pxl_string* str);

pxl_string* pxl_concat_string(const pxl_string* a, const pxl_string* b);

void pxl_copy(pxl_string** dest, const pxl_string* src);

char pxl_char_at(const pxl_string* str, size_t index);

bool pxl_string_equals(const pxl_string* a, const pxl_string* b);

bool pxl_string_not_equals(const pxl_string* a, const pxl_string* b);

bool pxl_string_greater(const pxl_string* a, const pxl_string* b);

bool pxl_string_smaller(const pxl_string* a, const pxl_string* b);

bool pxl_string_greater_equals(const pxl_string* a, const pxl_string* b);

bool pxl_string_smaller_equals(const pxl_string* a, const pxl_string* b);

#endif //COMPILER_PROJECT_PXL_STRING_H
