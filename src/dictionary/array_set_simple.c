#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "array_set.h"
#include "error_handling.h"


//everytime i write "malloc" i feel extasy

Array_Set* set_new(int (*cmp)(void*, void*), void (*dispose)(void *))
{
    Array_Set* ret = malloc(sizeof(Array_Set));
    if(ret == NULL) report_error(MEMORY);
    memset(ret, 0, sizeof(Array_Set));
    ret->cmp = cmp;
    ret->dispose = dispose;
    ret->array_size = STORE_START_SIZE;
    ret->storage = malloc(sizeof(void*) * ret->array_size);
    if(ret->storage == NULL) report_error(MEMORY);
    memset(ret->storage, 0, sizeof(void*) * ret->array_size);
    return ret;
}

void set_free(Array_Set* set)
{
    for(int i = 0; i < set->element_count; i++)
        set->dispose(set->storage[i]);
    free(set);
}

static int set_find_position(Array_Set *set, void *element)
{
    assert(set != NULL);
    assert(element != NULL);

    int l = 0;
    int r = set->element_count;
    int s;
    while(l < r)
    {
        s = l+r/2;
        assert(set->storage[s] != NULL);
        if(set->cmp(set->storage[s], element) < 0) l = s+1;
        else r = s;
    }
    assert(0 <= l && l <= set->element_count);
    return l;
}

/**
 * @brief compute_array_size Computes proper store array size based on current element_count and array_size.
 * @param set
 * @return Non-negative int greater or equal to current element_count.
 * This function is called by update_storage_size, which is called every modification of storage.
 * @todo elaborate last assertion
*/
static int compute_array_size(Array_Set* set)
{
    assert(set != NULL);
    assert(set->array_size >= 0);
    assert(set->element_count >= 0);
    assert(set->element_count <= set->array_size + 1); // 7-8, 8-8, 9-8
    //assertion above guarantees that 2 add operations cannot be done without calling update_storage_size
    //in other words, element_count can be maximally greater by 1 than array_size
    //which means, that maximally one element may be wished to be added.

    if(set->element_count == set->array_size+1) //full storage, expansion is required
        return set->array_size == 0 ? 1 : set->array_size*2;
    else if(set->element_count*2+1 < set->array_size) //it fits, so let's try to collapse
        return set->array_size/2;
    else //nothing changes
        return set->array_size;

}

bool set_add(Array_Set* set, void* element)
{
    assert(set != NULL);
    assert(element != NULL);

    int pos = set_find_position(set, element);
    if(set->cmp(set->storage[pos], element) == 0) //object is already in set
        return false;
    else
    {
        set->element_count++;
        int new_size = compute_array_size(set);
        if(new_size == set->array_size) //without array_size change
        {
            ///TUTAJ ZACZNIJ CZYTAC UWAZNIE
            int succedings_len = set->element_count - pos + 1;
            void** succedings = malloc(sizeof(void*) * succedings_len);
            if(succedings == NULL) report_error(MEMORY);
            memcpy(succedings, &(set->storage[pos]), sizeof(void*) * succedings_len);
            set->storage[pos] = element;
            memcpy(&(set->storage[pos+1]), succedings, sizeof(void*) * succedings_len);
            free(succedings);
        }
        else //należy zmienić rozmiar
        {
            void** new_storage = malloc(sizeof(void*) * new_size);
            int preceding_len = pos-1;
            //array_size
        }
        void** new_storage = malloc(sizeof(void*) * new_size);
    }
}



void* set_find(Array_Set *set, void *element)
{
    assert(set != NULL);
    assert(element != NULL);

    int pos = set_find_position(set, element);
    if(set->cmp(set->storage[pos], element) == 0)
        return set->storage[pos];
    else
        return NULL;
}

bool set_remove(Array_Set *set, void *element)
{

}

























