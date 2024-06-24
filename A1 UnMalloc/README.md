# User Defined Malloc Library (UDMalloc)

## Overview

The User Defined Malloc Library (UDMalloc) is a custom memory allocation library providing basic functionalities for dynamic memory management. It includes functions for allocating (`umalloc`) and freeing (`ufree`) memory, along with additional error checking and metadata management.

## Main Functions

### 1. `void *umalloc(size_t req_Size, char *file, int line)`
Allocates a block of memory of the requested size.

- **Parameters**:
  - `req_Size`: The size of the memory block to be allocated.
  - `file`: The source file name where the allocation request is made.
  - `line`: The line number in the source file where the allocation request is made.
- **Functionality**:
  - Checks whether the requested memory is available, including the additional metadata size.
  - Scans the entire memory to find a free segment large enough to accommodate the requested block.
  - Splits the memory into used and unused segments.

### 2. `void ufree(void *addr_To_Free, char *file, int line)`
Frees a previously allocated block of memory.

- **Parameters**:
  - `addr_To_Free`: The address of the memory block to be freed.
  - `file`: The source file name where the free request is made.
  - `line`: The line number in the source file where the free request is made.
- **Functionality**:
  - Checks if the address to be freed is within the valid memory range.
  - Checks if the memory block has been allocated by `umalloc`.
  - Checks if the memory block is already free.
  - Frees the block located at the provided pointer address, including accounting for the metadata.
  - Returns an error if the block is already freed.

## Implementation Notes

- The library maintains metadata for each allocated memory block to facilitate management and error checking.
- Functions include detailed error checking to ensure valid operations and to catch potential issues early.
- The source file name and line number parameters help in debugging by providing context about where the memory operations are performed.

## Example

```c
#include "udmalloc.h"

int main() {
    char *buffer;

    // Allocate memory
    buffer = (char *) umalloc(100, __FILE__, __LINE__);
    if (buffer == NULL) {
        // Handle allocation error
    }

    // Use the memory

    // Free the memory
    ufree(buffer, __FILE__, __LINE__);

    return 0;
}
```

## Conclusion

UDMalloc provides a robust solution for dynamic memory management with added error checking and metadata handling. Use the `umalloc` and `ufree` functions for safe and efficient memory operations in your applications.
