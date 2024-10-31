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
returns 'ERR_NULL_POINTER' if 'arr' points to nothing
returns 'OK' if everything runs smoothly

time complexity: O(1)
space complexity: O(1)
*/
enum stat darray_set_allocator(struct darray *arr, struct allocator allocator)
{
    if (!arr)
        return ERR_NULL_POINTER;

    if (arr->buffer != NULL)
    {
        // @TODO create a copy of the array and free the original one and allocate the new memory
        // using the new allocator
    }

    arr->allocator = allocator;

    return OK;
}

/*
darray_insert(struct darray *arr, size_t index, any_t data)

insert data to 'index' of the buffer in the dynamic array
if the dynamic array is not used before it means that memory is not allocated for it yet
then memory will be allocated with 'mem_alloc_f' of the capacity provided in the darray struct
returns 'ERR_NULL_POINTER' if 'arr' points to nothing
returns 'ERR_INDEX_LARGER_THAN_CAPACITY' if 'index' is out of scope
returns 'ERR_ALLOC' when allocation of memory failed
returns 'OK' if everything runs smoothly

time complexity: O(n)
space complexity: O(arr->capacity)
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
        arr->buffer = arr->allocator.mem_alloc_f(sizeof(any_t) * arr->capacity);
        if (!arr->buffer)
            return ERR_ALLOC;
    }

    memcpy(&(arr->buffer[index]), &data, sizeof(any_t));

    return OK;
}

/*
darray_at(struct darray *arr, size_t index, any_t *dst)

retreives the data of 'index' from the 'buffer' and copy it into 'dst'
returns 'ERR_NULL_POINTER' if 'arr' points to nothing
returns 'ERR_INDEX_LARGER_THAN_CAPACITY' if 'index' is out of scope
returns 'OK' if everything runs smoothly

time complexity: O(n)
space complexity: O(1)
*/
enum stat darray_at(struct darray *arr, size_t index, any_t *dst)
{
    if (!arr)
        return ERR_NULL_POINTER;
    if (index >= arr->capacity)
        return ERR_INDEX_LARGER_THAN_CAPACITY;

    memcpy(dst, &(arr->buffer[index]), sizeof(any_t));

    return OK;
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
