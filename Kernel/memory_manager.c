#include <stddef.h>
#include <stdint.h>
#define TRUE 1
#define FALSE !TRUE

typedef long Align;

typedef union header{
    struct{
        union header *ptr;
        uint64_t size;
    }data;
    
    Align x;

}header;

//Nodos ocupados
static header *base;
//Nodos libres
static header *free_node = NULL;

// Tamaño total de memoria de heap
uint64_t total_heap_size;

void initialize_memory_manager(char *heap_base, uint64_t heap_size)
{
    if(heap_base==NULL){
        return;
    }

    total_heap_size = (heap_size + sizeof(header)-1)/sizeof(header) + 1;

    free_node = base=(header*)heap_base;

    free_node->data.size = total_heap_size;

    free_node->data.ptr= free_node;
}

void *malloc(uint64_t malloc_bytes){
    if(malloc_bytes == 0){
        return NULL;
    }

    //2 nodos para iterar sobre la lista hasta encontrar un nodo que satisfaga el espacio requerido.

    header *current_node, *prevptr;

    void *result;

    char node_found;

    /* Calculate the number of memory units needed to provide at least nbytes of memory.
     *
     * Suppose that we need n >= 0 bytes and that the memory unit sizes are b > 0
     * bytes.  Then n / b (using integer division) yields one less than the number
     * of units needed to provide n bytes of memory, except in the case that n is
     * a multiple of b; then it provides exactly the number of units needed.  It
     * can be verified that (n - 1) / b provides one less than the number of units
     * needed to provide n bytes of memory for all values of n > 0.  Thus ((n - 1)
     * / b) + 1 provides exactly the number of units needed for n > 0.
     *
     * The extra sizeof(Header) in the numerator is to include the unit of memory
     * needed for the header itself.
     * CAMBIAR ESTE COMMENT
     */

    uint64_t malloc_units = (malloc_bytes + sizeof(header) - 1) / sizeof(header) + 1;

    prevptr= free_node;

    node_found= TRUE;

    for (current_node = prevptr->data.ptr; node_found; current_node->data.ptr)
    {

        // Cuando encuentre un bloque de memoria suficientemente grande como para que
        // la memoria solicitada entre hago un segundo chequeo

        
    // Si la memoria es exacta, asigno al previo el actual

        if(current_node->data.size >= malloc_units){
            if(current_node->data.size == malloc_units){
                prevptr->data.ptr = current_node->data.ptr;

            }else{

                // Si fuera mayor, recorto la memoria las nunits que voy a usar
                // Desplazo el current la nueva cantidad de tamaño que obtuve
                // Le asigno al nodo current como data.size el tamaño solicitado

                current_node->data.size -= malloc_units;
                current_node+= current_node->data.size;
                current_node->data.size = malloc_units;
            }

            free_node = prevptr;

            result= current_node + 1;

            node_found = FALSE;

        }  //si no encuentra bloque , devueve null(final de la lista)
        if(current_node == free_node){
            return NULL;
        }
        prevptr= current_node;
    }
    return result;


}

void free(void *block)
{

    if (block == NULL )
    {
        return;
    }

    
    header *free_block, *current_node;

  
    free_block = (header *)block - 1;

  
    if (free_block < base || free_block >= (base + total_heap_size * sizeof(header)))
    {
        return;
    }

    block = NULL;

   char external = FALSE;

    
    for (current_node = free_node; !(free_block > current_node && free_block < current_node->data.ptr) && !external; current_node = current_node->data.ptr)
    {

        
        if (free_block == current_node || free_block == current_node->data.ptr)
        {
            return;
        }

        // Se asegura que el nodo que estamos intentando insertar no sea el primero ni el último de la lista
        if (current_node >= current_node->data.ptr && (free_block > current_node || free_block < current_node->data.ptr))
        {
            external = TRUE;
            
        }
    }

  
    if (!external && (current_node + current_node->data.size > free_block || free_block + free_block->data.size > current_node->data.ptr))
    {
        return;
    }

    // Caso free_block está a la izquierda de donde se tiene que insertar

    // Si son iguales sus direcciones, uno los dos bloques de memoria adyacentes
    if (free_block + free_block->data.size == current_node->data.ptr)
    {
        free_block->data.size += current_node->data.ptr->data.size;
        free_block->data.ptr = current_node->data.ptr->data.ptr;

        // Sino, reemplazo a lo que apunte free_block por current
    }
    else
    {
        free_block->data.ptr = current_node->data.ptr;
    }

    
    if (current_node + current_node->data.size == free_block)
    {
        current_node->data.size += free_block->data.size;

        current_node->data.ptr = free_block->data.ptr;

       
    }
    else
    {
        current_node->data.ptr = free_block;
    }

  
    free_node = current_node;
}



