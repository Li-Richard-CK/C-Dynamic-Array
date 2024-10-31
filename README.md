# C-Dynamic-Array
A simple and fast dynamic array implementation in C.
Here is a simple explaination of how this dynamic array implmentation works:
There is a struct called 'darray_t' which contains all the information about the dynamic array such as the length, capacity, buffer(this is the actual data).

The data is a array of 'any_t' type which is defined as a 'void *' to make the dynamic array generic.

In the 'darray_t' struct there are also a allocator struct, whcih contains the memory allocation functions such as free, alloc, realloc... These memory allocation functions is used to make use of custom allocators that users might provide for better memory management. There are also comparison function pointers to sort and compare an element to another element in the dynamic array.

Error handling is kinda of simple. Almost every single function that is related to the dynamic array returns an enumerated of status which could be OK, and other ERR related error codes. These status codes are defined in 'common.h' header, there is also a function that translates the error code into a more user friendly error string.

To ensure memory safety, nearly everything is dynamically allocated instead of stack allocated since the issue of 'life-time' matters. This is an idea that I learnt from when learning Rust. A simple and basic explaination of this is if the memory is alocated locally in the stack, when we try to use the variable outside of the scope. Undefined behavior might occure. This will slow down run-time but memory safety is key when writing in C.

An important thing to remind is to free the memory after using everything. Memory leaks might occure if it is not properly released back to the system.

# Dynamic-Array-Iterator
**Is not implmented yet in this particular version!**
A struct that is used to iterate through the dynamic array. It stores the array pointer, index, element pointer...

Functions that is used to iterate through the array returns ITERATOR_END status code when it hits the end of the iterator.

New features are coming soon.
