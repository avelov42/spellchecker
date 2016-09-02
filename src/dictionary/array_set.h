#ifndef ARRAY_SET_H
#define ARRAY_SET_H

/** @defgroup array_set Module Array_Set
 * Array-based set implementation. Used in dictionary as alphabet and in trie as a
 * node's children set.
 */
/** @file
 * Header file of array_set module
 * @ingroup array_set
 * @author Piotr Rybicki <pr360957@mimuw.edu.pl>
 * @date 2015-08
 */

#include "stdbool.h"

/** Macro specifying start size of the set's storage array. Must be positive */
#define SET_STORE_START_SIZE 1

/**
  * Set_Functions is a container for functions operating on elements. Every set
  * contains a pointer to such structure, which provides functions to compare and dispose elements
  */
typedef struct
{
    int (*cmp)(void*, void*); ///<Function used to compare elements. Must provide linear ordering.
    void (*dispose)(void*); ///<Function used to dispose elements
} Set_Functions;

/**
  * Main structure of the set.
  */
typedef struct
{
    int array_size; ///<Current storage array size.
    int element_count; ///<Current number of elements stored in set.
    Set_Functions* fun; ///<Pointer to elements-related functions.
    void** storage; ///<Storage array.
} Array_Set;

/**
 * @brief set_new Creates and initializes a new Array_Set.
 * @param fun Pointer to structure with functions to operate on elements in set, like compare or dispose.
 * @return Pointer to new, ready-to-use set.
 */
Array_Set* set_new(Set_Functions* fun);

/**
 * @brief set_free Deallocates Array_Set.
 * @param set Set to be deallocated.
 */
void set_free(Array_Set* set);

/**
 * @brief set_add Adds element to the set.
 * @param set Set to add to.
 * @param element Must be a pointer to an element which can be compared and disposed by functions given as arguments while initializing set.
 * @return Returns true if element was added to set, false otherwise (f.e. if it was already present in the set).
 * <p><strong>Remember, that Array_Set stores only pointers to data and not data itself. External disposing elements contained in set is a very bad UB.</strong>
*/
bool set_add(Array_Set* set, void* element);

/**
 * @brief set_find Binary-search (using cmp function) in set.
 * @param set Set to search in.
 * @param element Element to be found.
 * @return Pointer to an element in set, which equals to element argument. If set does not such an element, NULL is returned.
 * Elements are equal when given comparison function returns 0.
 */
void* set_find(Array_Set* set, void* element);

/**
 * @brief set_remove Removes (and disposes) element from set which equals to given element.
 * @param set Set to remove from.
 * @param element Element to be removed.
 * @return True if element was removed, false otherwise (if it was not present).
 * Object pointed by element is disposed and should not be accessed.
 * <p>
 * To avoid disposing of element, a fake dispose function may be provided. In such case element will be only removed, but not disposed.
 */
bool set_remove(Array_Set* set, void* element);

/**
 * @brief set_ensure_capacity Changes internal storage array to capacity.
 * @param set Set to change.
 * @param capacity Required capacity. This must be a strictly positive number, greater than current array_size. Otherwise, nothing happens.
 */
void set_ensure_capacity(Array_Set* set, int capacity);


/**
 * @brief set_size Returns set's cardinality.
 * @param set The set.
 * @return Set's cardinality.
 */
int set_size(Array_Set* set);

#ifdef UNIT_TESTING
/**
 * @brief check_set_correctness Checks a number of conditions which must be fulfilled in correct set.
 * @param set The set.
 * @return True, if set is fully correct, false otherwise.
 */
bool set_check_correctness(Array_Set* set);
/**
 * @brief check_set_emptiness Checks whether set is empty.
 * @param set The set.
 * @return Is set empty?
 */
bool set_check_emptiness(Array_Set* set);
#endif // UNIT_TESTING

#endif // ARRAY_SET_H

