// List all group members' names: Anubhav Garikapadu(AG2112), Vijay Swaminathan(VS797)
// iLab machine tested on: -ilab2.cs.rutgers.edu


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Maximum size of the filesystem (in bytes)
#define MAX_FS_SIZE 4 * 1024 * 1024
// Block size (in bytes)
#define BLOCK_SIZE 1024
// Maximum number of files that can be stored
#define MAX_FILES 100

// Structure for the superblock
typedef struct
{
    int size;            // Size of the filesystem (in blocks)
    int free_blocks;     // Number of free blocks
    int used_blocks;     // Number of used blocks
    int last_Written_Id; // Last Written Index in data region
    char formatted[9];   // To check if disk is formatted or not
} superblock_t;

// Structure for an inode
typedef struct inode_t
{
    char name[256];           // Filename
    int blocks;               // Number of blocks used by the file
    int file_Desc;            // File Descriptor
    int last_Written_Node_Id; // Last Written Node Index
    short int nodes[4096];    // Nodes

    struct inode_t *next; // next inode pointer
} inode_t;

// Structure for an open file descriptor
typedef struct fd_t
{
    int mode;       // File access mode
    inode_t *inode; // Pointer to the inode
    int curser;     // Current file curser
} fd_t;

// File modes
#define WO_RDONLY 1
#define WO_WRONLY 2
#define WO_RDWR 3

// File descriptor table
fd_t fd_table[MAX_FILES];
// Pointer to the start of the data region in memory
char *data_region;
// Pointer to the start of the inode region in memory
inode_t *inode_region;
// Pointer to the start of the superblock in memory
superblock_t *superblock_region;
// Pointer to the start of the filesystem in memory
char *filesystem;
char *disk_Name;

void print_String(char *str, int size)
{
    for (int i = 0; i < size; i++)
        printf("%c", *str++);
}

// Mounts the filesystem from the given file
int wo_mount(char *diskname, void *memory_address)
{
    // Open the file for reading
    disk_Name = diskname;
    FILE *fp = fopen(disk_Name, "rb");
    if (fp == NULL)
    {
        // File access error
        return errno;
    }
    // Check the size of the file
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    if (file_size != MAX_FS_SIZE)
    {
        // Invalid filesystem size
        fclose(fp);
        errno = ENOTDIR;
        return errno;
    }
    // Read the entire filesystem into memory
    fseek(fp, 0, SEEK_SET);
    fread(memory_address, MAX_FS_SIZE, 1, fp);
    fclose(fp);
    // Set up pointers to the different regions in memory
    filesystem = (char *)memory_address;
    superblock_region = (superblock_t *)memory_address;
    inode_region = (inode_t *)(memory_address + sizeof(superblock_t) + 1);

    inode_t *temp = inode_region;

    for (int i = 1; i < MAX_FILES; i++)
    {
        temp->next = (inode_t *)(memory_address + sizeof(superblock_t) + 1 + sizeof(inode_t) * i);
        temp = temp->next;
    }
    data_region = (char *)(memory_address + sizeof(superblock_t) + sizeof(inode_t) * MAX_FILES + 1);
    if (strcmp(superblock_region->formatted, "Formatted") != 0)
    {
        superblock_region->free_blocks = ((MAX_FS_SIZE - (sizeof(superblock_t) + sizeof(inode_t) * MAX_FILES))) / BLOCK_SIZE;
        superblock_region->used_blocks = 0;
        superblock_region->size = MAX_FS_SIZE;
        superblock_region->last_Written_Id = -1;
        strcpy(superblock_region->formatted, "Formatted");
    }

    return 0;
}

int wo_unmount(void *memory_address)
{
    // Open the file for writing
    FILE *fp = fopen(disk_Name, "wb");
    if (fp == NULL)
    {
        // File access error
        return errno;
    }
    // Write the entire filesystem from memory to the file
    fwrite(memory_address, MAX_FS_SIZE, 1, fp);
    fclose(fp);
    return 0;
}

int wo_open(char *filename, int flags)
{
    int inode_index = -1;
    inode_t *inode_Ptr = (inode_t *)inode_region;

    int file_Desc = -1;

    while (inode_Ptr->name[0] != '\0')
    {
        if (strcmp(inode_Ptr->name, filename) == 0)
        {
            // File Found
            break;
        }
        file_Desc++;
        inode_Ptr = inode_Ptr->next;
    }
    if (inode_Ptr->name[0] == '\0')
    {
        errno = ENOENT;
        return errno;
    }

    inode_t *inode = (inode_t *)inode_Ptr;
    // Open the file
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (fd_table[i].inode == NULL)
        {
            // Find the first empty slot in the file descriptor table
            fd_t *fd = &fd_table[i];
            fd->mode = flags;
            fd->inode = inode;
            fd->curser = inode->nodes[0];
            return i;
        }
    }
    errno = EMFILE;
    return errno;
}

int wo_create(char *filename, int flags)
{
    int inode_index = -1;
    inode_t *inode_Ptr = (inode_t *)inode_region;

    int file_Desc = -1;

    if (strlen(filename) > 256)
    {
        errno = ENAMETOOLONG;
        return errno;
    }
    while (inode_Ptr->name[0] != '\0')
    {
        if (strcmp(inode_Ptr->name, filename) == 0)
        {
            // File already present in system
            errno = EEXIST;
            return errno;
        }
        file_Desc++;
        inode_Ptr = inode_Ptr->next;
    }
    inode_t *inode = (inode_t *)inode_Ptr;
    strcpy(inode->name, filename);
    inode->blocks = 0;
    for (int i = 0; i < 4096; i++)
    {
        inode->nodes[i] = -1;
    }
    inode->file_Desc = file_Desc;

    // Open the file
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (fd_table[i].inode == NULL)
        {
            // Find the first empty slot in the file descriptor table
            fd_t *fd = &fd_table[i];
            fd->mode = flags;
            fd->inode = inode;
            fd->curser = inode->nodes[0];
            return i;
        }
    }
    errno = EMFILE;
    return errno;
}

int wo_read(int fd, void *buffer, int bytes)
{
    // Check if the file descriptor is valid
    if (fd < 0 || fd >= MAX_FILES || fd_table[fd].inode == NULL)
    {
        errno = EBADF;
        return errno;
    }
    fd_t *file = &fd_table[fd];
    inode_t *inode = file->inode;

    // Check if the file is open in read mode
    if (file->mode == WO_WRONLY)
    {
        // File is not open in read mode
        errno = EACCES;
        return errno;
    }
    // Calculate the number of blocks to read
    int block_count = (bytes + BLOCK_SIZE - 1) / BLOCK_SIZE;
    // Check if the file has enough data
    if (block_count > inode->blocks)
    {
        block_count = inode->blocks;
    }

    // Read the data
    int bytes_read = 0;
    for (int i = 0; i < block_count; i++)
    {
        int block_index = file->curser;

        char *src = data_region + block_index * BLOCK_SIZE;

        memcpy(buffer + bytes_read, src, BLOCK_SIZE);
        bytes_read += BLOCK_SIZE;
        if (inode->nodes[block_index + 1] != -1)
        {
            file->curser = block_index + 1;
        }
    }
    return bytes_read;
}

int wo_write(int fd, void *buffer, int bytes)
{
    // Check if the file descriptor is valid
    if (fd < 0 || fd >= MAX_FILES || fd_table[fd].inode == NULL)
    {
        // Invalid file descriptor
        errno = EBADF;
        return errno;
    }
    fd_t *file = &fd_table[fd];
    inode_t *inode = file->inode;

    // Check if the file is open in write mode
    if (file->mode != WO_WRONLY && file->mode != WO_RDWR)
    {
        // File is not open in write mode
        errno = EROFS;
        return errno;
    }
    // Calculate the number of blocks to write
    int block_count = (bytes + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // Check if there is enough free space
    if (block_count > superblock_region->free_blocks)
    {
        // Not enough free space
        errno = EFBIG;
        return errno;
    }
    // Find the first available block
    inode_t *inode_Ptr = (inode_t *)inode_region;
    int block_index = superblock_region->last_Written_Id;

    block_index++;

    // Write the data
    int bytes_written = 0;

    for (int i = 0; i < block_count; i++)
    {

        if (inode->nodes[0] == -1)
        {
            file->curser = block_index;
            inode->nodes[0] = block_index;
            inode->last_Written_Node_Id = 0;
        }
        else
        {
            file->curser = block_index;
            inode->last_Written_Node_Id++;
            inode->nodes[inode->last_Written_Node_Id] = block_index;
        }

        superblock_region->free_blocks--;
        superblock_region->used_blocks++;
        superblock_region->last_Written_Id = block_index;
        char *dest = data_region + block_index * BLOCK_SIZE;
        memcpy(dest, buffer + bytes_written, BLOCK_SIZE);

        bytes_written += BLOCK_SIZE;

        block_index++;
    }
    // Update the file size and block count
    inode->blocks += block_count;

    return bytes_written;
}

int wo_close(int fd)
{
    // Check if the file descriptor is valid
    if (fd < 0 || fd >= MAX_FILES || fd_table[fd].inode == NULL)
    {
        // Invalid file descriptor
        errno = EBADF;
        return errno;
    }
    // Close the file
    fd_table[fd].inode = NULL;
    return 0;
}
