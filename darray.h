#ifndef _DARRAY_H
#define _DARRAY_H

#include "common.h"

typedef struct darray darray_t;

/*
darray_t_size(void)

returns the size of the darray struct
this function exists since darray is a foreward declaration in the header file

time complexity: O(1)
space complexity: O(1)
*/
size_t darray_t_size(void);

/*
darray_init(struct darray *arr)

initialize the dynamic array
set every single thing to default values, including memory allocation functions
this function doesnt allocate any memory
returns the status of the initialization
if the pointer to the array is null then it returns 'ERR_NULL_POINTER'
returns OK if everything runs smoothly

time complexity: O(1)
space complexity: O(1)
*/
enum stat darray_init(struct darray *arr);

/*
darray_init_w_allocator(struct darray *arr, struct allocator allocator)

initialize dynamic array and set the allocator
returns the larger error status code to ensure if there is error occuring in either 1 of the
functions it returns it without causing any undefined behaviour can be either 'OK' or any
error code

time complexity: O(1)
space complexity: O(1)
*/
enum stat darray_init_w_allocator(struct darray *arr, struct allocator allocator);

/*
darray_set_cmp_f(struct darray *arr, int (*cmp_f)(const void *, const void *))

set the comparison function of the array
returns 'ERR_NULL_POINTER' if 'arr' points to nothing
returns 'OK' if everything runs smoothly

time complexity: O(1)
space complexity: O(1)
*/
enum stat darray_set_cmp_f(struct darray *arr, int (*cmp_f)(const void *, const void *));

/*
darray_set_allocator(struct darray *arr, struct allocator allocator)

set the allocator of the array with 'struct allocator' in 'common.h'
returns 'ERR_NULL_POINTER' if 'arr' points to nothing
returns 'OK' if everything runs smoothly

time complexity: O(1)
space complexity: O(1)
*/
enum stat darray_set_allocator(struct darray *arr, struct allocator allocator);

/*
darray_insert(struct darray *arr, size_t index, any_t data)

insert data to 'index' of the buffer in the dynamic array
if the dynamic array is not used before it means that memory is not allocated for it yet
then memory will be allocated with 'mem_alloc_f' of the capacity provided in the darray struct
returns 'ERR_NULL_POINTER' if 'arr' points to nothing
returns 'ERR_INDEX_LARGER_THAN_CAPACITY' if index is out of scope
returns 'ERR_ALLOC' when allocation of memory failed
returns 'OK' if everything runs smoothly

time complexity: O(n)
space complexity: O(arr->capacity)
*/
enum stat darray_insert(struct darray *arr, size_t index, any_t data);

/*
darray_at(struct darray *arr, size_t index, any_t *dst)

retreives the data of 'index' from the 'buffer' and copy it into 'dst'
returns 'ERR_NULL_POINTER' if 'arr' points to nothing
returns 'ERR_INDEX_LARGER_THAN_CAPACITY' if 'index' is out of scope
returns 'OK' if everything runs smoothly

time complexity: O(n)
space complexity: O(1)
*/
enum stat darray_at(struct darray *arr, size_t index, any_t *dst);

/*
darray_free(struct darray *arr)

free the memory if allocated any and set all the values of the dynamic array to 0
returns 'ERR_NULL_POINTER' if 'arr' points to nothing
returns 'ERR_NULL_FUNCTION_POINTER' if 'mem_free_f' function pointer points to nothing
returns 'OK' if everything runs smoothly

time complexity: O(1)
space complexity: O(1)
*/
enum stat darray_free(struct darray *arr);

#endif
