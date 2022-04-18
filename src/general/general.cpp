#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "general.h"

//===================================================================

int _clearstdin(FOR_LOGS(LOG_PARAMS)) {

    general_log_report();

    while (getchar() != '\n') {;}

    return 0;
}

//===================================================================

FILE* _open_file(const char* filename, const char* mode FOR_LOGS(, LOG_PARAMS)) {

    general_log_report();

    if (filename == NULL) {

        error_report(INV_FILE_NAME);
        return NULL;
    }

    if (mode == NULL) {

        error_report(INV_FOPEN_MODE);
        return NULL;
    }

    FILE* file_ptr = fopen(filename, mode);
    if (file_ptr == NULL) {

        error_report(FOPEN_ERROR);
        return NULL;
    }

    return file_ptr;
}

//===================================================================

int _close_file(FILE* file_ptr FOR_LOGS(, LOG_PARAMS)) {

    general_log_report();

    if (file_ptr == NULL) {

        error_report(INV_FILE_PTR);
        return -1;
    }

    int is_ok = fclose(file_ptr);
    if (is_ok == EOF) {

        error_report(FCLOSE_ERR);
        return -1;
    }

    return 0;
}

//===================================================================

int _close_file(FILE* file_prt);

//===================================================================

int my_swap(void* first_, void* second_, size_t size) {

	assert(first_  != NULL);
	assert(second_ != NULL);

	char* first_ptr  = (char*)first_;
	char* second_ptr = (char*)second_;

	while (size >= sizeof(int64_t)) {

		int64_t temp = *(int64_t*)first_ptr;
		*(int64_t*)first_ptr  = *(int64_t*)second_ptr;
		*(int64_t*)second_ptr = temp;

		first_ptr += sizeof(int64_t);
		second_ptr += sizeof(int64_t);

		size -= sizeof(int64_t);
	}

	while (size >= sizeof(int32_t)) {

		int32_t temp = *(int32_t*)first_ptr;
		*(int32_t*)first_ptr = *(int32_t*)second_ptr;
		*(int32_t*)second_ptr = temp;

		first_ptr += sizeof(int32_t);
		second_ptr += sizeof(int32_t);

		size -= sizeof(int32_t);
	}

	while (size >= sizeof(int16_t)) {

		int16_t temp = *(int16_t*)first_ptr;
		*(int16_t*)first_ptr  = *(int16_t*)second_ptr;
		*(int16_t*)second_ptr = temp;

		first_ptr += sizeof(int16_t);
		second_ptr += sizeof(int16_t);

		size -= sizeof(int16_t);
	}

	while (size >= sizeof(int8_t)) {

		int8_t temp = *(int8_t*)first_ptr;
		*(int8_t*)first_ptr  = *(int8_t*)second_ptr;
		*(int8_t*)second_ptr = temp;

		first_ptr += sizeof(int8_t);
		second_ptr += sizeof(int8_t);

		size -= sizeof(int8_t);
	}

	return 0;
}

//===================================================================

int _clear_memory(void* base, size_t num, size_t size FOR_LOGS(, LOG_PARAMS)) {

    general_log_report();

    if (base == NULL || num < 1 || size < 1) {

        error_report(CLEAR_MEM_CHECK_ERR);
        return 0;
    }

    long unsigned memset_length = num * size;
    memset(base, 0, memset_length);

    return is_memory_clear(base, num, size);
}

//===================================================================

int _is_memory_clear(void* base, size_t num, size_t size FOR_LOGS(, LOG_PARAMS)) {

    general_log_report();

    if (base == NULL || num < 1 || size < 1) {

        error_report(CLEAR_MEM_CHECK_ERR);
        return 0;
    }

    char* base_char = (char*)base;
    size_t char_number = num * size;

    for (size_t char_ct = 0; char_ct < char_number; char_ct++) {

        if (base_char[char_ct] != 0) return 0; 
    }

    return 1;
}

//===================================================================

void* _my_recalloc(void* ptr, size_t number, size_t prev_number, size_t size_of_elem
                                                              FOR_LOGS(, LOG_PARAMS)) {
    general_log_report();

    if (ptr == NULL) {

        error_report(RECALLOC_INV_POINTER);
        return NULL;
    }

    size_t new_size = number * size_of_elem;
    void* new_ptr = realloc(ptr, new_size);

    if (new_ptr == NULL) {

        error_report(CANNOT_ALLOCATE_MEM);
        return NULL;
    }

    if (number > prev_number) {

        size_t offset = number - prev_number;
        char* base = (char*)new_ptr + size_of_elem * prev_number;

        int value = clear_memory(base, offset, size_of_elem);
        if (value != 1) 
            return NULL;
    }

    return new_ptr;
}

//===================================================================

void ___System_Sleep(float _Duration) {

    int64_t start = clock();
    while ((clock() - start) < int64_t(1000000 * _Duration));
}

//===================================================================

int64_t get_hash_(void* void_base, long unsigned int len FOR_LOGS(, LOG_PARAMS)) {

    general_log_report();

    char* base = (char*)void_base;

    assert(base);

    const unsigned int m = 0x5bd1e995;
    const unsigned int seed = 0;
    const int r = 24;

    unsigned int h = seed ^ (unsigned int)len;

    const unsigned char* data = (const unsigned char*)base;
    unsigned int k = 0;

    while (len >= 4) {

        k = data[0];
        k |= (unsigned)(data[1] << 8);
        k |= (unsigned)(data[2] << 16);
        k |= (unsigned)(data[3] << 24);

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;

    }

    switch (len) {
        case 3:
            h ^= (unsigned)(data[2] << 16);
            [[fallthrough]];
        case 2:
            h ^= (unsigned)(data[1] << 8);
            [[fallthrough]];
        case 1:
            h ^= data[0];
            h *= m;
            [[fallthrough]];
        default:;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

//===================================================================

int double_is_equal(double first, double second) {

    return (fabs(first - second) < Precise);
}
