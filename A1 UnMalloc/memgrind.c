// List all group members' names: Anubhav Garikapadu(AG2112), Vijay Swaminathan(VS797)
// iLab machine tested on: -ilab1.cs.rutgers.edu

#include "umalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX 999999

int main(int argc, const char *argv[])
{
    //consistency
    printf("\n 1. Checking consistency \n");
    int *ptr;
    ptr = (int*)malloc(10);
    printf("pointer 1 address %d \n",ptr);
    free(ptr);
    ptr = (int*)malloc(10);
    printf("pointer 2 address %d \n",ptr);
    free(ptr);
    
    printf("\n 2. Finding Maximum Value \n");
    long int *max;
    long int i=1;
    long int result=0;
    while (1){   
        //printf("\n inside 1st loop\n");
        max = (long int*)malloc(i);
        if(max!=NULL){
            free(max);
            result=i;
            i=i*2;
            //printf("i*2 %ld \n",i);
        }
        else if(max==NULL){
            i=i/2;
            max = (long int*)malloc(i);
            //printf("i/2 %ld \n",i);           
            break;
        }
        
    }
    i=i/2;
    while(1){
        //printf("\n inside 2nd loop\n");
        long int *temp=(long int*)malloc(i);
        if(temp!=NULL){
            result=result+i;
            free(max);
            free(temp);
            break;
        }
        i=i/2;
        //printf("%ld \n",i);
    }
    
    printf("\n Max value:  %ld\n",result);
   
    printf("\n 3. Basic Coalescence \n");
    int *h;
    int *q;
    int *full;
    h = (int*)malloc(result/2);
    printf("\n H Mem Loc - %d", h);
    q = (int*)malloc(result/4);
    printf("\n Q Mem Loc - %d", q);
    free(h);
    free(q);
    full = (int*)malloc(result);
    printf("\n Max Allocated Memory Location - %d", full);
    free(full);
   
   //4 saturation
    printf("\n 4. Saturation \n");
    int *new;
    int c=0;
    int *previous;
    int *block_To_Free;
    int *ptr1[MAX];
    while(c<9216){
        new = (int*)malloc(1024);
        ptr1[c]=new;
        c++;
    }
    while(1)
    {
        previous=new;
        new = (int*)malloc(1);
        if(new==NULL || new == 0){
            break;
        }
         
        ptr1[c]=new;
        c++;
        
    }
    printf("\n Saturation Complete \n");
   //5 time overhead
    printf("\n 5. Time Overhead \n");

    int time_elapsed;
    
    free(previous);
    

    clock_t begin = clock();
    
    full = (int*)malloc(1);
    
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("\n Memory Location of last Allocation: %d ", full);
    printf("\n Max time overhead: %f \n",time_spent);

    //6 intermediate coalescence
    printf("\n 6. Intermediate Coalescence \n");
    
    free(full);
    c -=2;
    while(c > -1){
        free(ptr1[c]);
        c--;
    }
    full = (int*)malloc(result);
    printf("\n Final Full Memory Location - %d \n", full);
    free(full);
   
    return 0;
}

