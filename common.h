#ifndef _COMMON_H
#define _COMMON_H

#include <stddef.h>

enum stat
{
    OK = 0,

    ERR_ALLOC,
    ERR_NULL_POINTER,
    ERR_INVALID_ACCESS_OF_INDEX,
    ERR_INDEX_LARGER_THAN_CAPACITY,
    ERR_REACHED_MAX_CAPACITY,
    ERR_NULL_FUNCTION_POINTER,

    ITER_END,
};

struct allocator
{
    void *(*mem_alloc_f)(size_t);
    void *(*mem_calloc_f)(size_t, size_t);
    void *(*mem_realloc_f)(void *, size_t);
    void (*mem_free_f)(void *);
};

typedef void *any_t;

const char *common_stat_str(enum stat stat);

#endif
