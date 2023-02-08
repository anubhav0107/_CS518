// List all group members' names: Anubhav Garikapadu(AG2112), Vijay Swaminathan(VS797)
// iLab machine tested on: -ilab1.cs.rutgers.edu

#include "umalloc.h"

static char MEM[MAX_SIZE];
#define MAX 999999
char init = '0';
char * get_Next_Seg(char *curr) {
    /* get the pointer of the last index of the memory */
    char *max_MEM = &MEM[MAX_SIZE];
    char *next = curr + ((meta_Node *)curr)->seg_Size + sizeof(meta_Node) + 1;
    /* If next is outside the memory return NULL.*/
    if (next >= max_MEM) {
        return NULL;
    } else {
        return next;
    }
}

void* umalloc(size_t req_Size, char* file, int line){
    int free_Mem_Flag = 0;
    size_t total_Free_Size = 0;
    if (req_Size > (size_t)MAX_SIZE-sizeof(meta_Node)-1) {
        printf("%s:%d Error:: Requested Size beyond memory limit.\n", file, line);
        return NULL;
    }
    if(req_Size == 0){
        printf("%s:%d Error:: Requested Size makes no sense. Please Allocate more than 0 Bytes.\n", file, line);
        return NULL;
    }
    if (init == '0') {
        /* Initialize the memory. Set is free as 1 to indicate free memory.*/
        ((meta_Node *)&MEM[0])->is_Free = '1';
        ((meta_Node *)&MEM[0])->seg_Size = (size_t)MAX_SIZE - (sizeof(meta_Node)) - 1;
        init = '1';
    }

    char *open_Seg;
    char *curr = &MEM[0];
    size_t tot_Req_Size = sizeof(meta_Node) + req_Size + 1;
    /* scan entire memory until we reach a memory segment that is free and big enough to accomodate requestd memory. */
    while (curr < &MEM[MAX_SIZE] && curr != NULL) {
    
        if (((meta_Node *)curr)->is_Free == '1') {
            free_Mem_Flag = 1;
            total_Free_Size += ((meta_Node *)curr)->seg_Size;
            if(((meta_Node *)curr)->seg_Size >= tot_Req_Size) {
                open_Seg = curr;
                break;
            }else{
        	    open_Seg = NULL;
            }
        }
        curr = get_Next_Seg(curr);

    }

	if(total_Free_Size == 0){
        printf("%s:%d Error:: There is no free memory left.\n", file, line);
        return NULL;    
    }
    if (total_Free_Size >= tot_Req_Size && open_Seg == NULL) {
        printf("%s:%d Error:: There is enough free memory, but there is no block large enough for the allocation. \n", file, line);
        return NULL;
    } else if(free_Mem_Flag == 1 && open_Seg == NULL){
        printf("%s:%d Error:: Memory is not full but there is not enough free memory for the allocation. \n", file, line);
        return NULL;
    } else {
        /*Split the memory into Used and Unused memory segment.*/

        size_t free_Seg_Size = ((meta_Node *)open_Seg)->seg_Size;
        size_t new_Seg_Size = free_Seg_Size - tot_Req_Size;
    
        /* we need to find the next free segment that starts adjacent to the current segment */
        char *next_Free_Seg = curr + tot_Req_Size;

        /* Current segment is now in use. Marked Is Free flag as 0 to indicate not free. */
        ((meta_Node *)open_Seg)->is_Free = '0';
        ((meta_Node *)open_Seg)->seg_Size = req_Size;

        /* set segment size & is free flag for next segment*/
        ((meta_Node *)next_Free_Seg)->seg_Size = new_Seg_Size;
        ((meta_Node *)next_Free_Seg)->is_Free = '1';

        /*Return pointer to current addressable memory location where data can be stored*/
        return (void *)(curr + sizeof(meta_Node) + 1);
    }
}

void ufree(void *addr_To_Free, char *file, int line) {

    if (addr_To_Free == NULL) {
        printf("%s:%d Error:: Specified address is invalid. Please use a correct pointer. \n", file, line);
        return;
    }
    
    /* checking if memory is inside the MEM range */
    if (MEM + sizeof(meta_Node) + 1 > (char*)addr_To_Free || (char*)addr_To_Free > &MEM[MAX_SIZE] - sizeof(meta_Node) - 1) {
        printf("%s:%d Error:: Specified address %d is invalid (not within memory range).\n", file, line, addr_To_Free);
        return;
    }
    
    char *curr_Pos = &MEM[0];
    char *meta_Addr_To_Free = (char *)((char *)addr_To_Free - sizeof(meta_Node) - 1);
    
    /* checking if memory has ever been allocated by malloc. */
    if (((meta_Node *)meta_Addr_To_Free)->is_Free != '1' && ((meta_Node *)meta_Addr_To_Free)->is_Free != '0') {
        printf("%s:%d Error:: Specified address %d was not allocated by malloc().\n", file, line, addr_To_Free);
        return;
    }
    /* checking if memory is already free */
    if (((meta_Node *)meta_Addr_To_Free)->is_Free == '1') {
        printf("%s:%d Error:: Specified address %d is already free.\n", file, line, meta_Addr_To_Free);
        return;
    }
    /* scan entire memory to find memory address to free */
    while (curr_Pos < &MEM[MAX_SIZE]) {
        if (curr_Pos == meta_Addr_To_Free) {
            if (((meta_Node *)curr_Pos)->is_Free == '1') {
                printf("%s:%d Error:: Specified address is already free.\n", file, line);
                return;
            }
            ((meta_Node *)curr_Pos)->is_Free = '1';

            char *curr_Seg = &MEM[0];
            char *next_Seg = NULL;
    
            /* scan entire memory until we reach the end to search for segments of free memory to meld them into one free block */

            while (curr_Seg < &MEM[MAX_SIZE] && curr_Seg != NULL) {
                if (((meta_Node*)curr_Seg)->is_Free == '1'){
                    next_Seg = get_Next_Seg(curr_Seg);
                     /* reached the end of the memory */
                    if (next_Seg >= &MEM[MAX_SIZE] || next_Seg == NULL) {
                        return;
                    }

                    if (((meta_Node*)next_Seg)->is_Free == '1') {
                        /* add the size of the next_Seg and the size of the metadata */
                        size_t next_Seg_Size = sizeof(meta_Node) + ((meta_Node*)next_Seg)->seg_Size + 1;
                        ((meta_Node *)curr_Seg)->seg_Size += next_Seg_Size;
                        next_Seg = get_Next_Seg(next_Seg);
                    } else {
                        curr_Seg = get_Next_Seg(next_Seg);
                    }
                } else {
            
                    curr_Seg = get_Next_Seg(curr_Seg);
                }
            }
            return;
        }
        curr_Pos = get_Next_Seg(curr_Pos);
    }
    printf("%s:%d Error:: No memory has been allocated yet.\n", file, line);
    return;
}