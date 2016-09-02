/** @file
 * Source file of array_set module
 * @ingroup array_set
 * @author Piotr Rybicki <pr360957@students.mimuw.edu.pl>
 * @date 2015-08
 */
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "limits.h"
#include "array_set.h"
#include "error_handling.h"

#ifdef ARRAY_SET_UNIT_TESTING
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#ifdef malloc
#undef malloc
#endif // malloc
#define malloc(size) _test_malloc(size, __FILE__, __LINE__)

#ifdef calloc
#undef calloc
#endif // calloc
#define calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)

#ifdef free
#undef free
#endif // free
#define free(ptr) _test_free(ptr, __FILE__, __LINE__)
#endif //ARRAY_SET_UNIT_TESTING

Array_Set* set_new(Set_Functions* fun)
{
    Array_Set* ret = malloc(sizeof(Array_Set));
    if(ret == NULL) report_error(MEMORY);
    memset(ret, 0, sizeof(Array_Set));
    ret->fun = fun;
    ret->array_size = SET_STORE_START_SIZE;
    ret->storage = calloc(ret->array_size, sizeof(void*));
    if(ret->storage == NULL) report_error(MEMORY);
    return ret;
}

void set_free(Array_Set* set)
{
    for(int i = 0; i < set->element_count; i++)
        set->fun->dispose(set->storage[i]);
    //don't free fun, because it may be shared with other sets
    free(set->storage);
    free(set);
}

/**
 * @brief set_find_position Binary search for element
 * @param set Set to search in.
 * @param element Element to be found.
 * @return Position of the element in set or its proposed position in case it is not in the set.
 */
static int set_find_position(Array_Set* set, void* element)
{
    assert(set != NULL);
    assert(element != NULL);

    int l, r, s;
    l = 0;
    r = set->element_count;
    while(l < r)
    {
        s = (l+r)/2;
        if(set->fun->cmp(set->storage[s], element) < 0) l = s+1;
        else r = s;
    }
    assert(0 <= l && l <= set->element_count);
    return l;
}

/**
 * @brief compute_array_size Computes proper store array size based on current array_size and given increment.
 * @param set Set of which array_size will be computed.
 * @param increment Increment of elements_count. Must be in inteval [-1;1].
 * @return Non-negative size that gurantees element_count+increment may be stored optimally in storage.
*/
static int compute_array_size(Array_Set* set, int increment)
{
    assert(set != NULL);
    assert(set->array_size >= 0);
    assert(set->element_count+increment >= 0);
    assert(set->element_count <= set->array_size); // 7-8, 8-8, 9-8
    //assertion above guarantees that 2 add operations cannot be done without calling update_storage_size
    //in other words, element_count can be maximally greater by 1 than array_size
    //which means, that maximally one element may be wished to be added.
    assert(increment == -1 || increment == 1);

    if(set->element_count+increment == set->array_size+1) //full storage, expansion is required
        return set->array_size == 0 ? 1 : set->array_size*2;
    else if((set->element_count+increment)*2+1 < set->array_size) //it fits, so let's try to collapse
        return set->array_size/2;
    else //nothing changes
        return set->array_size;
}


/**
 * @brief element_available Helper function to determine whether position in storage is valid.
 * @param set Set.
 * @param i Position to check.
 * @return True if on i-th position is an element.
 */
static bool element_available(Array_Set* set, int i)
{
    assert(set != NULL);
    return 0 <= i && i < set->element_count;
}

bool set_add(Array_Set* set, void* element)
{
    assert(set != NULL);
    assert(element != NULL);

    int pos = set_find_position(set, element);
    if(element_available(set, pos) && set->fun->cmp(set->storage[pos], element) == 0) //object is already in set
        return false;
    else //in case of out of bounds or found pos is not equal to element
    {
        //this case means that element is not in set
        int new_size = compute_array_size(set, 1);
        if(new_size == set->array_size)
        {
            int successors_length = set->element_count - pos;
            memmove(&(set->storage[pos+1]), &(set->storage[pos]), sizeof(void*) * successors_length);
            set->storage[pos] = element;
            set->element_count++;
        }
        else //storage should be expanded
        {
            int predecessors_len = pos;
            int successors_len = set->element_count - pos;
            void** new_storage = calloc(new_size, sizeof(void*));
            if(new_storage == NULL) report_error(MEMORY);
            memcpy(new_storage, set->storage, sizeof(void*) * predecessors_len);
            new_storage[pos] = element;
            memcpy(&(new_storage[pos+1]), &(set->storage[pos]), sizeof(void*) * successors_len);
            free(set->storage);
            set->element_count++;
            set->array_size = new_size;
            set->storage = new_storage;
        }
        return true;
    }
}

void* set_find(Array_Set* set, void* element)
{
    assert(set != NULL);
    assert(element != NULL);

    int pos = set_find_position(set, element);
    if(element_available(set, pos) && set->fun->cmp(set->storage[pos], element) == 0)
        return set->storage[pos];
    else
        return NULL;
}

bool set_remove(Array_Set* set, void* element)
{
    assert(set != NULL);
    assert(element != NULL);

    int pos = set_find_position(set, element);
    if(!element_available(set, pos) || set->fun->cmp(set->storage[pos], element) != 0)
        return false; //no such element
    else //element found
    {
        int new_size = compute_array_size(set, -1);
        if(new_size == set->array_size) //no change of size
        {
            //1234, [1] == 2, 4-1-1
            //12345678, [3] == 4, 8-3-1 == 4
            int successors_length = set->element_count - pos -1;
            set->fun->dispose(set->storage[pos]);
            memmove(&(set->storage[pos]), &(set->storage[pos+1]), sizeof(void*) * successors_length);
            set->element_count--;
            set->storage[set->element_count] = NULL;

            ///ensure to null area after elements
        }
        else //shrink of storage may be performed
        {
            int predecessors_length = pos;
            int successors_length = set->element_count - pos -1;
            void** new_storage = calloc(new_size, sizeof(void*));
            if(new_storage == NULL) report_error(MEMORY);

            set->fun->dispose(set->storage[pos]);
            memcpy(new_storage, set->storage, sizeof(void*) * predecessors_length);
            memcpy(&new_storage[pos], &(set->storage[pos+1]), sizeof(void*) * successors_length);
            free(set->storage);
            set->storage = new_storage;
            set->array_size = new_size;
            set->element_count--;
            set->storage[set->element_count] = NULL;
        }
        return true;
    }
}

int set_size(Array_Set *set)
{
    return set->element_count;
}

void set_ensure_capacity(Array_Set* set, int capacity)
{
    if(capacity <= SET_STORE_START_SIZE)
        return;
    if(capacity <= set->array_size)
        return;

    void** new_storage = calloc(capacity, sizeof(void*));
    if(new_storage == NULL) report_error(MEMORY);

    memcpy(new_storage, set->storage, set->array_size);
    free(set->storage);
    set->storage = new_storage;
    set->array_size = capacity;
    return;

}

#ifdef UNIT_TESTING
bool set_check_correctness(Array_Set* set)
{
    if(set == NULL) return false;
    if(!(0 <= set->array_size && set->array_size <= INT_MAX)) return false;
    if(!(0 <= set->element_count && set->element_count <= set->array_size)) return false;
    if(set->fun->cmp == NULL) return false;
    if(set->fun->dispose == NULL) return false;
    if(set->storage == NULL) return false;

    for(int i = 0; i < set-> element_count; i++) //presence of elements
        if(set->storage[i] == NULL) return false;

    for(int i = set->element_count; i < set->array_size; i++) //nullity of free space in storage
        if(set->storage[i] != NULL) return false;


    for(int i = 1; i < set->element_count; i++) //valid ordering of elements
        if(!set->fun->cmp(set->storage[i-1], set->storage[i]) == -1 ) return false;

    return true;
}
bool set_check_emptiness(Array_Set* set)
{
    if(set == NULL) return false;
    if(set->array_size != 1 && set->array_size != SET_STORE_START_SIZE) return false;
    if(set->element_count != 0) return false;
    if(set->storage[0] != NULL) return false;
    return true;
}
#endif // UNIT_TESTING
