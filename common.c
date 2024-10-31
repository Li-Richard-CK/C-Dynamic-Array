#include "common.h"

const char *common_stat_str(enum stat stat)
{
    switch (stat)
    {
    case OK:
        return "";

    case ERR_ALLOC:
        return "memory allocation error";
    case ERR_NULL_POINTER:
        return "null pointer is not allowed";
    case ERR_INVALID_ACCESS_OF_INDEX:
        return "invalid access to element of index";
    case ERR_INDEX_LARGER_THAN_CAPACITY:
        return "index can't be larger than the capacity";
    case ERR_REACHED_MAX_CAPACITY:
        return "collection reached max capacity";
    case ERR_NULL_FUNCTION_POINTER:
        return "null function pointer is not allowed";
    case ITER_END:
        return "end of iterator";

    default:
        return "unsupported stat";
    }
}
