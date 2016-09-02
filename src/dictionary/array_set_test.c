/** @file
 * Tests file of array_set
 * @ingroup array_set
 * @author Piotr Rybicki <pr360957@mimuw.edu.pl>
 * @date 2015-08
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <wchar.h>
#include <cmocka.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "array_set.h"

#define TEST_INT_1 4 ///<Integer used in tests.
#define TEST_INT_2 2 ///<Integer used in tests.
#define TEST_INT_3 3 ///<Integer used in tests.
#define TEST_INT_UNUSED 42 ///<Integer never present in trie while testing.

#ifdef malloc
#undef malloc
#endif // malloc
#define malloc(size) _test_malloc(size, __FILE__, __LINE__) ///<Macro substituting memory management function.

#ifdef calloc
#undef calloc
#endif // calloc
#define calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__) ///<Macro substituting memory management function.

#ifdef free
#undef free
#endif // free
#define free(ptr) _test_free(ptr, __FILE__, __LINE__) ///<Macro substituting memory management function.

///CMOCKA FUNCTION
extern void* _test_calloc(const size_t number_of_elements, const size_t size,
                   const char* file, const int line);
///CMOCKA FUNCTION
extern void _test_free(void* const ptr, const char* file, const int line);
///CMOCKA FUNCTION
extern void* _test_malloc(size_t size, const char* ffile, const int line);


///Environment destruction
static int teardown_set(void** state)
{
    set_free(*state);
    *state = NULL;
    return 0;
}

///Comparator for int-set.
static int cmp_int(void* a, void* b)
{
    if(*(int*) a < *(int*) b)
        return -1;
    if(*(int*) a > *(int*) b)
        return 1;
    return 0;
}

///Fake dispose fun not disposing. Disposing is tested with nodes.
static void no_dispose(void* a)
{
    return;
}

///Package of functions to operate on int elements in set.
static Set_Functions int_set_functions = {.cmp = cmp_int, .dispose = no_dispose};

///Creation of empty int set.
static int setup_empty_int_set(void** state)
{
    Array_Set* set = set_new(&int_set_functions);
    if(set == NULL)
        return -1;
    *state = set;
    return 0;
}

///Adding one int, searching for it, removing it.
static void test_int_set_add_find_remove_one(void** state)
{
    if(setup_empty_int_set(state) != 0)
        fail_msg("Error while setup");

    Array_Set* set = *state;
    assert_non_null(set);

    assert_true(set_check_emptiness(set));
    assert_true(set_check_correctness(set));

    int* number = malloc(sizeof(int)); //if it does not work, fuck everything
    int* unused = malloc(sizeof(int));
    *number = TEST_INT_1;
    *unused = TEST_INT_UNUSED;

    assert_true(set_add(set, number)); //adding first int

    assert_true(set_check_correctness(set));
    assert_false(set_check_emptiness(set));
    assert_int_equal(set->element_count, 1); //there must be 1 element
    assert_true(cmp_int(set_find(set, number), number) == 0); //should be in set
    assert_null(set_find(set, unused)); //should not be in set


    assert_false(set_add(set, number)); //trying to add again, must fail

    assert_false(set_remove(set, unused)); //not in set, must fail
    assert_true(set_remove(set, number)); //remove of existing, must succeed

    assert_true(set_check_correctness(set));
    assert_true(set_check_emptiness(set));
    assert_null(set_find(set, number)); //removed element is not in set
    assert_null(set_find(set, unused));

    assert_false(set_remove(set, number)); //cannot remove again

    free(number);
    free(unused);

    teardown_set(state);
}

/**
* @brief test_int_set_add_find_remove_two
* @param state
* Adding two elements, searching them<br>
* Removing first, searching for both<br>
* Adding first, removing second, searching for both<br>
* Removing even second, searching for both<br>
* Tests corner cases for binary-search<br>
*/
static void test_int_set_add_find_remove_two(void** state)
{
    if(setup_empty_int_set(state) != 0)
        fail_msg("Error while setup");

    Array_Set* set = *state;
    assert_true(set_check_correctness(set));
    assert_true(set_check_emptiness(set));
    int *n1, *n2, *nu; //last - 'not used'
    n1 = malloc(sizeof(int));
    n2 = malloc(sizeof(int));
    nu = malloc(sizeof(int));
    *n1 = TEST_INT_1;
    *n2 = TEST_INT_2;
    *nu = TEST_INT_UNUSED;

    assert_true(set_add(set, n1)); //adding two inequal elements
    assert_true(set_add(set, n2));

    assert_false(set_add(set, n1)); //twice, should fail
    assert_false(set_add(set, n2));

    assert_true(set_check_correctness(set));
    assert_false(set_check_emptiness(set));
    assert_int_equal(set->element_count, 2);
    assert_true(cmp_int(set_find(set, n1), n1) == 0); //finding them
    assert_true(cmp_int(set_find(set, n2), n2) == 0);
    assert_null(set_find(set, nu));

    assert_true(set_remove(set, n1)); //removing first

    assert_true(set_check_correctness(set));
    assert_false(set_check_emptiness(set));
    assert_int_equal(set->element_count, 1); //still ok?
    assert_null(set_find(set, n1));
    assert_true(cmp_int(set_find(set, n2), n2) == 0);
    assert_null(set_find(set, nu));

    assert_true(set_add(set, n1)); //adding back n1, deleting n2
    assert_true(set_remove(set, n2));

    assert_true(set_check_correctness(set));
    assert_false(set_check_emptiness(set));
    assert_int_equal(set->element_count, 1);
    assert_true(cmp_int(set_find(set, n1), n1) == 0);
    assert_null(set_find(set, n2)); //set should contain only n1
    assert_null(set_find(set, nu));

    assert_true(set_remove(set, n1)); //deleting even n1, empty set

    assert_true(set_check_correctness(set));
    assert_true(set_check_emptiness(set));
    assert_int_equal(set->element_count, 0);
    assert_null(set_find(set, n1));
    assert_null(set_find(set, n2));
    assert_null(set_find(set, nu));

    assert_false(set_remove(set, n1));
    assert_false(set_remove(set, n2));

    free(n1);
    free(n2);
    free(nu);

    teardown_set(state);
}

///Same as add_find_remove_two, but tests corner cases using three items.
static void test_int_set_add_find_remove_three(void **state)
{
    if(setup_empty_int_set(state) != 0)
        fail_msg("Error while setup");

    Array_Set* set = *state;
    assert_true(set_check_correctness(set));
    assert_true(set_check_emptiness(set));
    int *n1, *n2, *n3, *nu; //last - 'not used'
    n1 = malloc(sizeof(int));
    n2 = malloc(sizeof(int));
    n3 = malloc(sizeof(int));
    nu = malloc(sizeof(int));
    *n1 = TEST_INT_1;
    *n2 = TEST_INT_2;
    *n3 = TEST_INT_3;
    *nu = TEST_INT_UNUSED;

    assert_true(set_add(set, n1));
    assert_true(set_add(set, n2));
    assert_true(set_add(set, n3));

    assert_true(set_check_correctness(set));
    assert_false(set_check_emptiness(set));
    assert_int_equal(set->element_count, 3);
    assert_true(cmp_int(set_find(set, n1), n1) == 0);
    assert_true(cmp_int(set_find(set, n2), n2) == 0);
    assert_true(cmp_int(set_find(set, n3), n3) == 0);
    assert_null(set_find(set, nu));

    //let's test if all all elements are correctly removed

    assert_true(set_remove(set, n1)); //removed first

    assert_true(set_check_correctness(set));
    assert_false(set_check_emptiness(set));
    assert_int_equal(set->element_count, 2);
    assert_null(set_find(set, n1));
    assert_true(cmp_int(set_find(set, n2), n2) == 0);
    assert_true(cmp_int(set_find(set, n3), n3) == 0);
    assert_null(set_find(set, nu));

    assert_true(set_add(set, n1)); //now removed second
    assert_true(set_remove(set, n2));

    assert_true(set_check_correctness(set));
    assert_false(set_check_emptiness(set));
    assert_int_equal(set->element_count, 2);
    assert_true(cmp_int(set_find(set, n1), n1) == 0);
    assert_null(set_find(set, n2));
    assert_true(cmp_int(set_find(set, n3), n3) == 0);
    assert_null(set_find(set, nu));

    assert_true(set_add(set, n2)); //removed third
    assert_true(set_remove(set, n3));

    assert_true(set_check_correctness(set));
    assert_false(set_check_emptiness(set));
    assert_int_equal(set->element_count, 2);
    assert_true(cmp_int(set_find(set, n1), n1) == 0);
    assert_true(cmp_int(set_find(set, n2), n2) == 0);
    assert_null(set_find(set, n3));
    assert_null(set_find(set, nu));

    assert_false(set_remove(set, n3));
    assert_true(set_remove(set, n2));
    assert_true(set_remove(set, n1));

    assert_true(set_check_correctness(set));
    assert_true(set_check_emptiness(set));

    free(n1);
    free(n2);
    free(n3);
    free(nu);

    teardown_set(state);
}

///Checks if array arr of length len contains int == k.
static bool array_contains(int** arr, int len, int k)
{
    for(int i = 0; i < len; i++)
        if(*arr[i] == k)
            return true;
    return false;
}

///Returns an array of unique integers
static int** new_unique_random_int_array(int n)
{
    int** arr = malloc(sizeof(int*) * n);
    int rand_int;
    for(int i = 0; i < n; i++)
    {
        arr[i] = malloc(sizeof(int));
        do
        {
            rand_int = rand(); //may last forever, but i hope it wont
        }
        while(array_contains(arr, i, rand_int));
        *arr[i] = rand_int;

    }
    return arr;
}

///Returns an array of not unique integer.
static int** new_random_int_array(int n)
{
    int** arr = malloc(sizeof(int*) * n);
    for(int i = 0; i < n; i++)
    {
        arr[i] = malloc(sizeof(int));
        *arr[i] = rand();
    }
    return arr;
}

///Freeing int array.
static void free_array(int** arr, int n)
{
    for(int i = 0; i < n; i++)
        free(arr[i]);
    free(arr);
}

/// Automatic tests involving operations on given number of elements.
/// Generates N unique randoms, adds them to set.
/// Adds them all to set.
/// Tries to add half of them again.
/// Finds all added numbers.
/// Removes half of randoms.
/// Removes again the same half of randoms.
/// Finds all removed randoms.
/// Finds all not removed randoms
/// Removes all.
/// Tests set for emptiness.
/// After every action correctness test and (non)emptiness test is performed.
static void test_int_set_add_find_remove_N_random(void** state, int n)
{
    if(n*10 > RAND_MAX) //to avoid freaking long randomizing
        skip();

    setup_empty_int_set(state);
    Array_Set* set = *state;
    int** number = new_unique_random_int_array(n);

    for(int i = 0; i < n; i++) //adding all randoms
        assert_true(set_add(set, number[i]));

    assert_true(set_check_correctness(set));
    assert_false(set_check_emptiness(set));
    assert_int_equal(set->element_count, n);

    for(int i = 0; i < n/2; i++) //adding half of them again
        assert_false(set_add(set, number[i]));

    assert_true(set_check_correctness(set));
    assert_false(set_check_emptiness(set));
    assert_int_equal(set->element_count, n);

    for(int i = 0; i < n; i++) //finding all randoms
        assert_true(cmp_int(set_find(set, number[i]), number[i]) == 0);

    for(int i = 0; i < n/2; i++)
        assert_true(set_remove(set, number[i])); //removing half of randoms

    assert_true(set_check_correctness(set));
    assert_false(set_check_emptiness(set));

    for(int i = 0; i < n/2; i++)
        assert_false(set_remove(set, number[i])); //removing removed randoms

    for(int i = 0; i < n/2; i++)
        assert_null(set_find(set, number[i])); //finding all removed numbers

    for(int i = n/2; i < n; i++)
        assert_true(cmp_int(set_find(set, number[i]), number[i]) == 0); //finding not removed numbers


    for(int i = n/2; i < n; i++)
        assert_true(set_remove(set, number[i])); //removing rest of randoms

    assert_true(set_check_correctness(set));
    assert_true(set_check_emptiness(set));

    free_array(number, n);
    teardown_set(state);
}

///As name says..
static void test_int_set_add_find_remove_1024_unique_random(void** state)
{
    test_int_set_add_find_remove_N_random(state, 1024);
}
///As name says..
static void test_int_set_add_find_remove_2048_unique_random(void** state)
{
    test_int_set_add_find_remove_N_random(state, 2048);
}
///As name says..
static void test_int_set_add_find_remove_4096_unique_random(void** state)
{
    test_int_set_add_find_remove_N_random(state, 4096);
}
///As name says..
static void test_int_set_add_find_remove_8192_unique_random(void** state)
{
    test_int_set_add_find_remove_N_random(state, 8192);
}

#define EXTENSIVE_NUMBER 81818 ///<Defines maximum number of elements added to set when auto test is performed.
/// Tests set for plenty of elements. Its actually small number due to expensive add and remove implementation.
static void test_int_set_add_remove_extensive(void** state)
{
    setup_empty_int_set(state);
    Array_Set* set = *state;
    int** array = new_random_int_array(EXTENSIVE_NUMBER);

    for(int i = 0; i < EXTENSIVE_NUMBER; i++)
       set_add(set, array[i]);

    assert_true(set_check_correctness(set));

    for(int i = 0; i < EXTENSIVE_NUMBER/2; i++)
        set_remove(set, array[i]);

    assert_true(set_check_correctness(set));

    for(int i = EXTENSIVE_NUMBER/2; i < EXTENSIVE_NUMBER; i++)
        set_remove(set, array[i]);

    assert_true(set_check_emptiness(set));
    assert_true(set_check_correctness(set));

    free_array(array, EXTENSIVE_NUMBER);
    teardown_set(state);
}

/* ** ** ** ** **** *** ***** ** ** ** ** */
/* ** ** ** ** NODE SET TESTS ** ** ** ** */
/* ** ** ** ** **** *** ***** ** ** ** ** */

///Simple definition of node used in dicionary.
typedef struct Node
{
    wchar_t value; ///<Sign represented by node.
    bool is_word; ///<Bool determining whether node represents a full word.

    struct Node* parent; ///<Pointer to parent node, useful when deleting node.
    Array_Set* children; ///<Pointer to Array_Set, used to store child-nodes.
} Node;

///Disposing node function.
static void dispose_node(void* ptr)
{
    if(ptr == NULL)
        return;
    Node* node = ptr;
    set_free(node->children);
    free(node);
}

///Compares by value in node.
static int cmp_node(void* a, void* b)
{
    Node *x, *y;
    x = a;
    y = b;
    if(x->value < y->value) return -1;
    if(x->value > y->value) return 1;
    return 0;
}
///Package of functions to be provided for set of nodes.
static Set_Functions node_set_functions = {.cmp = cmp_node, .dispose = dispose_node}
                                          ;
///Creating new node
static Node* new_node(void)
{
    Node* ret = malloc(sizeof(Node));
    if(ret == NULL)
        fail();
    memset(ret, 0, sizeof(Node));
    ret->children = set_new(&node_set_functions);
    return ret;
}
///Creating empty nodes set.
static int setup_empty_node_set(void** state)
{
    Array_Set* set = set_new(&node_set_functions);
    if(set == NULL)
        return -1;
    *state = set;
    return 0;
}


///Simple manual test to verify disposing.
static void test_node_set_add_find_remove_two(void** state)
{
    if(setup_empty_node_set(state) != 0)
        fail_msg("Error while setup");

    Array_Set* set = *state;
    assert_non_null(set);

    assert_true(set_check_correctness(set));
    assert_true(set_check_emptiness(set));

    Node* a = new_node();
    Node* b = new_node();
    a->value = L'ż';
    b->value = L'ó';

    assert_true(set_add(set, a));
    assert_true(set_add(set, b)); //adding two
    assert_false(set_add(set, a));
    assert_false(set_add(set, b));

    assert_true(set_check_correctness(set));
    assert_false(set_check_emptiness(set));

    assert_true(cmp_node(set_find(set, a), a) == 0); //finding
    assert_true(cmp_node(set_find(set, a), a) == 0);

    assert_true(set_remove(set, a));
    assert_true(set_check_correctness(set)); //removing
    assert_true(set_remove(set, b));
    assert_true(set_check_correctness(set));
    assert_true(set_check_emptiness(set));

    //a, b should be already disposed
    a = b = NULL;
    //if, not, leak above

    teardown_set(state);
}

///Building some structure similiar to real trie.
static void test_node_set_tree_structure(void** state)
{
    Node* root = new_node();
    Node* l = new_node();
    Node* r = new_node();
    Node* rl = new_node();
    Node* rr = new_node();

    root->value = L'q';
    l->value = L'l';
    r->value = L'r';
    rl->value = L'l';
    rr->value = L'r';

    assert_true(set_add(r->children, rl));
    assert_true(set_add(r->children, rr));
    assert_true(set_add(root->children, r));
    assert_true(set_add(root->children, l));

    assert_true(set_check_correctness(root->children));
    assert_true(set_check_correctness(l->children));
    assert_true(set_check_emptiness(l->children));
    assert_true(set_check_correctness(r->children));
    assert_false(set_check_emptiness(r->children));
    assert_true(set_check_correctness(rl->children));
    assert_true(set_check_correctness(rr->children));
    assert_true(set_check_emptiness(rl->children));
    assert_true(set_check_emptiness(rr->children));

    assert_true(set_remove(r->children, rl));
    assert_true(set_remove(r->children, rr));
    assert_false(set_remove(r->children, rl));
    dispose_node(root);

    root = l = r = rl = rr = NULL;
    //any leaks?
}

/**
 * @brief main
 * @return 0
 *
 * Tests consists mainly of int-set tests, which examine manually corner cases
 * for one, two, and three elements. Then goes automatic tests for < 10 000 elements.
 * And finally an huge test using ~100k elements.
 *
 * Then, some minor tests are performed with Node as an element of the set.
 */
int main(void)
{
    srand(time(NULL));
    const struct CMUnitTest int_set_manual[] =
    {
        //manual tests covering corner cases on int's
        cmocka_unit_test(test_int_set_add_find_remove_one),
        cmocka_unit_test(test_int_set_add_find_remove_two),
        cmocka_unit_test(test_int_set_add_find_remove_three),
    };
    cmocka_run_group_tests_name("Integer set manual tests", int_set_manual, NULL, NULL);

    const struct CMUnitTest int_set_automatic[] =
    {
        cmocka_unit_test(test_int_set_add_find_remove_1024_unique_random),
        cmocka_unit_test(test_int_set_add_find_remove_2048_unique_random),
        cmocka_unit_test(test_int_set_add_find_remove_4096_unique_random),
        cmocka_unit_test(test_int_set_add_find_remove_8192_unique_random),
        cmocka_unit_test(test_int_set_add_remove_extensive)
    };
    cmocka_run_group_tests_name("Integer set automatic tests", int_set_automatic, NULL, NULL);

    // Node tests below

    const struct CMUnitTest node_set_manual[] =
    {
        cmocka_unit_test(test_node_set_add_find_remove_two),
        cmocka_unit_test(test_node_set_tree_structure)
    };

    cmocka_run_group_tests_name("Node set manual tests", node_set_manual, NULL, NULL);

    return 0;
}
