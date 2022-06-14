#include "shared_memory.h"


#define MEM_SIZE 1000


typedef void* shared_memory;

static shared_memory shared_memory_array[10]={NULL};

static int shared_mem_idx=0;

void *shm_open(int id){
    //Si no hay memoria ya mallocada en esa posicion
    if(shared_memory_array[id]!=NULL){
        void* toRet=malloc(MEM_SIZE);
        if(toRet!=NULL){
            shared_memory_array[id]=toRet;
            return shared_memory_array[id];    
        } 
        
        return NULL;
    }else{
        return shared_memory_array[id];
    }
   
}  





