#include "darray.h"
#include "common.h"

#include <stdarg.h>
#include <mm_malloc.h>
#include <string.h>
#include <stdio.h>

#define MAX(x, y) ((x > y) ? x : y)

#define DEFAULT_CAPACITY 8
#define DEFAULT_EXPANSION_FACTOR 2

#define DEFAULT_ALLOC malloc
#define DEFAULT_CALLOC calloc
#define DEFAULT_REALLOC realloc
#define DEFAULT_FREE free

struct darray
{
    size_t len;
    size_t capacity;
    any_t *buffer; /* array of any elements */

    int (*cmp_f)(const void *, const void *);

    /* memory allocation functions */
    struct allocator allocator;
};

/*
darray_t_size(void)

returns the size of the darray struct
this function exists since darray is a foreward declaration in the header file

time complexity: O(1)
space complexity: O(1)
*/
size_t darray_t_size(void)
{
    return sizeof(struct darray);
}

/*
darray_init(struct darray *arr)

initialize the dynamic array
set every single thing to default values, including memory allocation functions
this function doesnt allocate any memory
returns 'ERR_NULL_POINTER' if 'arr' points to nothing
returns 'OK' if everything runs smoothly

time complexity: O(1)
space complexity: O(1)
*/
enum stat darray_init(struct darray *arr)
{
    if (!arr)
        return ERR_NULL_POINTER;

    arr->len = 0;
    arr->capacity = DEFAULT_CAPACITY;
    arr->buffer = NULL;

    arr->cmp_f = NULL;

    /* default memory allocation functions */
    arr->allocator.mem_alloc_f = DEFAULT_ALLOC;
    arr->allocator.mem_calloc_f = DEFAULT_CALLOC;
    arr->allocator.mem_realloc_f = DEFAULT_REALLOC;
    arr->allocator.mem_free_f = DEFAULT_FREE;

    return OK;
}

/*
darray_init_w_allocator(struct darray *arr, struct allocator allocator)

initialize dynamic array and set the allocator
returns the larger error status code to ensure if there is error occuring in either 1 of the
functions it returns it without causing any undefined behaviour can be either 'OK' or any
error code

time complexity: O(1)
space complexity: O(1)
*/
enum stat darray_init_w_allocator(struct darray *arr, struct allocator allocator)
{
    if (!arr)
        return ERR_NULL_POINTER;

    return MAX(darray_init(arr), darray_set_allocator(arr, allocator));
}

/*
darray_set_cmp_f(struct darray *arr, int (*cmp_f)(const void *, const void *))

set the comparison function of the array
returns 'ERR_NULL_POINTER' if 'arr' points to nothing
returns 'OK' if everything runs smoothly

time complexity: O(1)
space complexity: O(1)
*/
enum stat darray_set_cmp_f(struct darray *arr, int (*cmp_f)(const void *, const void *))
{
    if (!arr)
        return ERR_NULL_POINTER;

    arr->cmp_f = cmp_f;
    return OK;
}

/*
darray_set_allocator(struct darray *arr, struct allocator allocator)

set the allocator of the array with 'struct allocator' in 'common.h'
this function also reallocates the memory with the new allocator and free the old ones if
the 'buffer' is not new
returns 'ERR_NULL_POINTER' if 'arr' points to nothing
returns 'ERR_NULL_FUNCTION_POINTER' if 'mem_alloc_f' points to nothing
returns 'ERR_ALLOC' if memory allocation failed
returns 'OK' if everything runs smoothly

time complexity: O(arr->capacity)
space complexity: O(arr->capacity)
*/
static enum stat _mv_data(struct darray *arr, struct allocator allocator)
{
    if (!allocator.mem_alloc_f || !allocator.mem_free_f)
        return ERR_NULL_FUNCTION_POINTER;

    any_t *buffer_cpy = (any_t *)allocator.mem_alloc_f(sizeof(any_t) * arr->capacity);
    if (!buffer_cpy)
        return ERR_ALLOC;

    memcpy(buffer_cpy, arr->buffer, sizeof(any_t) * arr->capacity);

    arr->allocator.mem_free_f(arr->buffer);

    arr->buffer = allocator.mem_alloc_f(sizeof(any_t) * arr->capacity);
    if (!arr->buffer)
    {
        allocator.mem_free_f(arr->buffer);
        return ERR_ALLOC;
    }

    memcpy(arr->buffer, buffer_cpy, sizeof(any_t) * arr->capacity);

    allocator.mem_free_f(buffer_cpy);

    return OK;
}

enum stat darray_set_allocator(struct darray *arr, struct allocator allocator)
{
    if (!arr)
        return ERR_NULL_POINTER;

    if (arr->buffer != NULL)
    {
        enum stat stat = _mv_data(arr, allocator);

        if (stat != OK)
            return stat;
    }

    arr->allocator = allocator;

    return OK;
}

/*
darray_resize(struct darray *arr, size_t new_capacity)

resize the 'buffer' in the dynamic array
reallocates the memory to the new capacity and copies the original data into the new buffer
returns 'ERR_NULL_POINTER' if 'arr' points to nothing
returns 'ERR_NULL_FUNCTION_POINTER' if 'mem_realloc_f' points to nothing
returns 'ERR_ALLOC' when allocation of memory failed
returns 'OK' if everything runs smoothly

time complexity: O(n + k)
space complexity: O(n)
*/
enum stat darray_resize(struct darray *arr, size_t new_capacity)
{
    if (!arr)
        return ERR_NULL_POINTER;
    if (!arr->allocator.mem_realloc_f)
        return ERR_NULL_FUNCTION_POINTER;

    any_t *new_buffer = arr->allocator.mem_realloc_f(arr->buffer, new_capacity * sizeof(any_t));
    if (!new_buffer)
        return ERR_ALLOC;

    /* copy the original data into new buffer */
    memcpy(new_buffer, arr->buffer,
           ((arr->capacity < new_capacity) ? arr->capacity : new_capacity) * sizeof(any_t));

    arr->buffer = new_buffer;
    arr->capacity = new_capacity;

    return OK;
}

/*
darray_insert(struct darray *arr, size_t index, any_t data)

insert data to 'index' of the buffer in the dynamic array
if the dynamic array is not used before it means that memory is not allocated for it yet
then memory will be allocated with 'mem_alloc_f' of the capacity provided in the darray struct
this functions doesn't automatically resize the dynamic array
returns 'ERR_NULL_POINTER' if 'arr' points to nothing
returns 'ERR_INDEX_LARGER_THAN_CAPACITY' if 'index' is out of scope
returns 'ERR_NULL_FUNCTION_POINTER' if 'mem_alloc_f' points to nothing
returns 'ERR_ALLOC' when allocation of memory failed
returns 'OK' if everything runs smoothly

time complexity: O(n)
space complexity: O(1)
*/
enum stat darray_insert(struct darray *arr, size_t index, any_t data)
{
    if (!arr)
        return ERR_NULL_POINTER;
    if (index >= arr->capacity)
        return ERR_INDEX_LARGER_THAN_CAPACITY;

    /* allocate memory for darray buffer before inserting */
    /*
    if buffer is empty it means that it haven't been used yet
    so we allocate the memory for it with the default capacity
    */
    if (arr->buffer == NULL)
    {
        if (!arr->allocator.mem_alloc_f)
            return ERR_NULL_FUNCTION_POINTER;

        arr->buffer = arr->allocator.mem_alloc_f(sizeof(any_t) * arr->capacity);
        if (!arr->buffer)
            return ERR_ALLOC;
    }

    memcpy(&(arr->buffer[index]), &data, sizeof(any_t));

    if (index >= arr->len)
        arr->len = index + 1;

    return OK;
}

/*
darray_append(struct darray *arr, any_t data)

appends 'data' to the end of buffer of 'arr'
this function automatically resizes the buffer if no space is left
returns whatever 'darray_resize' or 'darray_insert' returns

time complexity: O(n)
space complexity: O(1)
*/
enum stat darray_append(struct darray *arr, any_t data)
{
    if (!arr)
        return ERR_NULL_POINTER;

    if (arr->len >= arr->capacity)
    {
        enum stat stat = darray_resize(arr, arr->capacity + DEFAULT_EXPANSION_FACTOR);

        /* return if any error codes are returned from 'darray_resize' */
        if (stat != OK)
            return stat;
    }

    return darray_insert(arr, arr->len, data);
}

/*
darray_at_dst(struct darray *arr, size_t index, any_t *dst)

retreives the data of 'index' from the 'buffer' and copy it into 'dst'
returns 'ERR_NULL_POINTER' if 'arr' points to nothing
returns 'ERR_INDEX_LARGER_THAN_CAPACITY' if 'index' is out of scope
returns 'OK' if everything runs smoothly

time complexity: O(1)
space complexity: O(1)
*/
enum stat darray_at_dst(struct darray *arr, size_t index, any_t *dst)
{
    if (!arr)
        return ERR_NULL_POINTER;
    if (index >= arr->capacity)
        return ERR_INDEX_LARGER_THAN_CAPACITY;

    *dst = (arr->buffer[index]);

    return OK;
}

/*
darray_at_ret(struct darray *arr, size_t index)

calls 'darray_at_dst' to get the data of 'index'
returns 'NULL' if anything is returns except 'OK' from 'darray_at_dst'
returns the data if everything runs smoothly

time complexity: O(1)
space complexity: O(1)
*/
any_t darray_at_ret(struct darray *arr, size_t index)
{
    any_t ret;
    if (darray_at_dst(arr, index, &ret) != OK)
        return NULL;

    return ret;
}

/*
darray_free(struct darray *arr)

free the memory if allocated any and set all the values of the dynamic array to 0
returns 'ERR_NULL_POINTER' if 'arr' points to nothing
returns 'ERR_NULL_FUNCTION_POINTER' if 'mem_free_f' function pointer points to nothing
returns 'OK' if everything runs smoothly

time complexity: O(1)
space complexity: O(1)
*/
enum stat darray_free(struct darray *arr)
{
    if (!arr)
        return ERR_NULL_POINTER;
    if (arr->allocator.mem_free_f == NULL)
        return ERR_NULL_FUNCTION_POINTER;

    arr->allocator.mem_free_f(arr->buffer);

    arr->len = 0;
    arr->capacity = 0;

    arr->cmp_f = NULL;

    arr->allocator.mem_alloc_f = NULL;
    arr->allocator.mem_calloc_f = NULL;
    arr->allocator.mem_realloc_f = NULL;
    arr->allocator.mem_free_f = NULL;

    return OK;
}
