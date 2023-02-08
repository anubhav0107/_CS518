// List all group members' names: Anubhav Garikapadu(AG2112), Vijay Swaminathan(VS797)
// iLab machine tested on: -ilab1.cs.rutgers.edu

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifndef _UMALLOC_H
#define _UMALLOC_H

#define malloc( x ) umalloc(( x ), __FILE__, __LINE__)
#define free( x ) ufree(( x ), __FILE__, __LINE__)

#define MAX_SIZE 10485760


typedef struct meta_data {
    unsigned long seg_Size;
    char is_Free;
} meta_Node;


void* umalloc(size_t, char*, int);
void ufree(void*, char*, int);

#endif 