# Write-Once Filesystem (WOFS)

## Overview

The Write-Once Filesystem (WOFS) is a custom filesystem implemented entirely within a 4MB file. This filesystem allows for mounting, reading, writing, creating, and managing files while preserving changes across sessions. The filesystem image is fully loaded into memory during operation and written back to the disk file upon unmounting.

## Features

- **Mounting and Unmounting**: The filesystem can be mounted from a disk file and fully loaded into memory. It can also be unmounted, writing all changes back to the disk.
- **File Operations**: Provides basic file operations such as open, create, read, write, and close.
- **Memory Management**: Manages the filesystem entirely in memory for fast access and manipulation.

## Functions

### 1. `int wo_mount(char *diskname, void *memory_address)`
Mounts the filesystem from the specified disk file.

- **Parameters**:
  - `diskname`: The name of the disk file.
  - `memory_address`: The memory address where the filesystem image will be loaded.
- **Returns**: Error number if there is a file access error.

### 2. `int wo_unmount(void *memory_address)`
Unmounts the filesystem and writes the modified image back to the disk file.

- **Parameters**:
  - `memory_address`: The memory address where the filesystem image is stored.
- **Returns**: Error number if there is a file access error.

### 3. `int wo_open(char *filename, int flags)`
Opens a file and finds the first empty slot in the file descriptor table.

- **Parameters**:
  - `filename`: The name of the file to open.
  - `flags`: The flags for opening the file (e.g., read, write).
- **Returns**: File descriptor or error number.

### 4. `int wo_create(char *filename, int flags)`
Creates a new file in the filesystem.

- **Parameters**:
  - `filename`: The name of the file to create.
  - `flags`: The flags for creating the file.
- **Returns**: File descriptor or error number if the file already exists.

### 5. `int wo_read(int fd, void *buffer, int bytes)`
Reads data from an open file.

- **Parameters**:
  - `fd`: The file descriptor of the open file.
  - `buffer`: The buffer where the read data will be stored.
  - `bytes`: The number of bytes to read.
- **Returns**: Number of bytes read or error number.

### 6. `int wo_write(int fd, void *buffer, int bytes)`
Writes data to an open file.

- **Parameters**:
  - `fd`: The file descriptor of the open file.
  - `buffer`: The buffer containing the data to write.
  - `bytes`: The number of bytes to write.
- **Returns**: Number of bytes written or error number if there is not enough free space.

### 7. `int wo_close(int fd)`
Closes an open file.

- **Parameters**:
  - `fd`: The file descriptor of the open file.
- **Returns**: Error number if the file descriptor is invalid.

## Implementation Notes

- All reads and writes are performed in memory, making the filesystem operations fast.
- Changes are preserved by writing the entire 4MB image back to the disk file upon unmounting.
- Basic error handling is implemented for file access, descriptor validation, and space management.

## Error Handling

- Error numbers are returned in case of file access errors, invalid file descriptors, insufficient space, and other operational issues.
- Each function performs necessary checks to ensure the integrity and validity of operations.

## Getting Started

1. **Mount the Filesystem**: Use `wo_mount` to load the filesystem from a disk file into memory.
2. **Perform File Operations**: Use `wo_open`, `wo_create`, `wo_read`, `wo_write`, and `wo_close` to manage files within the mounted filesystem.
3. **Unmount the Filesystem**: Use `wo_unmount` to write changes back to the disk file and clean up.

## Example

```c
// Example usage of WOFS

char diskname[] = "filesystem.img";
void *memory_address = malloc(4 * 1024 * 1024); // Allocate 4MB for the filesystem

if (wo_mount(diskname, memory_address) != 0) {
    // Handle error
}

// Perform file operations
int fd = wo_create("example.txt", 0);
if (fd >= 0) {
    char data[] = "Hello, World!";
    wo_write(fd, data, sizeof(data));
    wo_close(fd);
}

wo_unmount(memory_address);
free(memory_address);
```

## Conclusion

WOFS provides a simple, in-memory, write-once filesystem that ensures fast access and efficient management of file operations within a 4MB file. Use this guide to integrate and utilize WOFS in your projects.
