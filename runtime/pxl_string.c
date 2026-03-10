#include "pxl_string.h"

#include "pxl_utilities.h"
#include <string.h>
#include <stdint.h> // for SIZE_MAX

pxl_string* pxl_create_string(const char* data, int size) {
    // Avoid int overflow for the size of the new string
    if (size > SIZE_MAX - sizeof(pxl_string) - 1)
        pxl_runtime_error("String size overflow");

    pxl_string* string = malloc(sizeof(pxl_string) + size + 1);
    if (!string) pxl_runtime_error("Failed to allocate string");

    string->size = size;

    if (data) {
        memcpy(string->data, data, size);
    }

    string->data[size] = '\0';
    return string;
}

void pxl_destroy_string(pxl_string* str) {
    if (!str) return;
    free(str);
}

pxl_string* pxl_concat_string(const pxl_string* a, const pxl_string* b) {
    if (!a || !b)
        pxl_runtime_error("Attempt to concatenate a null string");
    if (SIZE_MAX - a->size < b->size)
        pxl_runtime_error("String concatenation size overflow");


    pxl_string* string = pxl_create_string(NULL, a->size + b->size);
    if (!string) pxl_runtime_error("Failed to allocate string concatenation");

    memcpy(string->data, a->data, a->size);
    memcpy(string->data + a->size, b->data, b->size);

    return string;
}

void pxl_copy(pxl_string** dest, const pxl_string* src) {
    if (!dest)
        pxl_runtime_error("Attempt to copy into a null destination pointer");
    if (!src)
        pxl_runtime_error("Attempt to copy a null string");

    if (*dest == src)
        return;

    pxl_string* new_str = pxl_create_string(src->data, src->size);
    if (!new_str)
        pxl_runtime_error("Failed to allocate string copy");

    pxl_destroy_string(*dest);
    *dest = new_str;
}

char pxl_char_at(const pxl_string* str, const size_t index) {
    if (!str) {
        pxl_runtime_error("Null string in pxl_char_at");
    }
    if (index >= str->size) {
        pxl_runtime_error("Index out of bounds in pxl_char_at");
    }

    return str->data[index];
}

const char* pxl_get_string_data(const pxl_string* str) {
    if (!str) {
        pxl_runtime_error("Null string in pxl_string_data");
    }
    return str->data;
}

static int pxl_string_compare(const pxl_string* a, const pxl_string* b) {
    if (a == b) return 0;
    if (!a || !b) {
        pxl_runtime_error("Null string in comparison");
    }

    size_t min = (a->size < b->size) ? a->size : b->size;

    int cmp = memcmp(a->data, b->data, min);

    if (cmp != 0) return cmp;

    if (a->size < b->size) return -1;
    if (a->size > b->size) return 1;
    return 0;
}

bool pxl_string_equals(const pxl_string* a, const pxl_string* b) {
    if (!a || !b) pxl_runtime_error("Null string in comparison");
    if (a == b) return true;
    if (a->size != b->size) return false;
    return memcmp(a->data, b->data, a->size) == 0;
}

bool pxl_string_not_equals(const pxl_string* a, const pxl_string* b) {
    return !pxl_string_equals(a, b);
}

bool pxl_string_greater(const pxl_string* a, const pxl_string* b) {
    return pxl_string_compare(a, b) > 0;
}

bool pxl_string_smaller(const pxl_string* a, const pxl_string* b) {
    return pxl_string_compare(a, b) < 0;
}

bool pxl_string_greater_equals(const pxl_string* a, const pxl_string* b) {
    return pxl_string_compare(a, b) >= 0;
}

bool pxl_string_smaller_equals(const pxl_string* a, const pxl_string* b) {
    return pxl_string_compare(a, b) <= 0;
}
