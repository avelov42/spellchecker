/** @file
    Tests of trie implementation.
    @ingroup trie
    @author Piotr Rybicki
    @date 2015-08
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
#include <limits.h>
#include <string.h>
#include "trie.h"

#define STRING_SIZE 6 ///<Size of string array
wchar_t* string[] = {L"", L"©", L"zażółć", L"gĘśLą", L"JAŹŃ", L"ΩŒĘ®™¥↑↔ÓÞĄŚÐÆŊ•ƏŁżźć„”ńµ≤≥"};
///<Few strings used to test trie

#define FILL_SIZE 9 ///<Size of fill array.
wchar_t* fill[] = {L"ą", L"ąąb", L"ąąbąą", L"ąąbć", L"ę", L"b", L"ć", L"d", L"dąb"};
///<Strings used to more advanced trie tests.


///Empty trie setup.
static int setup_trie_empty(void** state)
{
    Node* root = trie_new_node();
    if(root == NULL)
        return -1;
    *state = root;
    assert_true(trie_verify(root, true));
    return 0;
}

///Setup based on string array.
static int setup_trie_with_strings(void** state)
{
    Node* root = trie_new_node();
    if(root == NULL)
        return -1;
    for(int i = 1; i < STRING_SIZE; i++)
        trie_insert_word(root, string[i]);
    *state = root;
    return 0;
}

///Setup based on fill array.
static int setup_trie_full_structure(void** state)
{
    Node* root = trie_new_node();
    if(root == NULL)
        return -1;
    for(int i = 0; i < 8; i++)
        trie_insert_word(root, fill[i]);
    *state = root;
    return 0;
}

///Freeing trie.
static int teardown_trie(void** state)
{
    trie_free_node(*state);
    *state = NULL;
    return 0;
}

///Deleting empty trie, before any insertions.
static void test_delete_empty_trie(void** state)
{
    setup_trie_empty(state);
    Node* root = *state;
    trie_verify(root, true);
    teardown_trie(state);
    assert_null(*state);
    return;
}

///Trying to delete non-existing trie.
static void test_delete_null_trie(void** state)
{
    Node* root = NULL;
    expect_assert_failure(trie_free_node(root));
    return;
}

///Searching in empty trie.
static void test_search_empty_trie(void** state)
{
    setup_trie_empty(state);
    Node* root = *state;
    expect_assert_failure(trie_find_word(root, string[0]));
    for(int i = 1; i < STRING_SIZE; i++)
        assert_false(trie_find_word(root, string[i]));
    assert_true(trie_verify(root, true));
    teardown_trie(state);
    return;
}

///Deleting in empty trie.
static void test_delete_word_empty_trie(void** state)
{
    setup_trie_empty(state);
    Node* root = *state;
    expect_assert_failure(trie_delete_word(root, string[0]));
    for(int i = 1; i < STRING_SIZE; i++)
        assert_false(trie_delete_word(root, string[i]));
    assert_true(trie_verify(root, true));
    teardown_trie(state);
    return;
}

///Inserting empty word, inserting word twice.
static void test_simple_insert(void** state)
{
    setup_trie_empty(state);
    Node* root = *state;
    expect_assert_failure(trie_insert_word(root, string[0]));
    for(int i = 1; i < STRING_SIZE; i++)
    {
        assert_true(trie_insert_word(root, string[i]));
        assert_false(trie_insert_word(root, string[i]));
    }
    assert_true(trie_verify(root, true));
    teardown_trie(state);
    return;
}

///Finds strings, deletes them, deletes again expecting failure.
static void test_find_delete_strings(void** state)
{
    setup_trie_with_strings(state);
    Node* root = *state;
    for(int i = 1; i < STRING_SIZE; i++)
    {
        assert_true(trie_find_word(root, string[i]));
        assert_true(trie_verify(root, true));
        assert_true(trie_delete_word(root, string[i]));
        assert_true(trie_verify(root, true));
        assert_false(trie_delete_word(root, string[i]));
    }
    assert_true(trie_verify(root, true));
    teardown_trie(state);
    return;
}

///Test of insertion of full structure and trie correcntess after that.
static void test_insert_full_structure(void** state)
{
    setup_trie_empty(state);
    Node* root = *state;

    for(int i = 0; i < FILL_SIZE; i++)
        assert_true(trie_insert_word(root, fill[i]));
    assert_true(trie_verify(root, true));
    teardown_trie(state);
    return;
}

///Loads a structure described in fill array and checks whatever is possible.
static void test_trie_structure_basic(void** state)
{
    setup_trie_full_structure(state);
    Node* root = *state;

    Node *a, *b, *c, *d, *e;
    b = root->children->storage[0];
    d = root->children->storage[1];
    a = root->children->storage[2];
    c = root->children->storage[3];
    e = root->children->storage[4];


    assert_int_equal(root->children->element_count, 5);
    assert_true(a->is_word);
    assert_true(b->is_word);
    assert_true(c->is_word);
    assert_true(d->is_word);
    assert_true(e->is_word);

    assert_true(a->value == L'ą');
    assert_true(b->value == L'b');
    assert_true(c->value == L'ć');
    assert_true(d->value == L'd');
    assert_true(e->value == L'ę');
    //hardcoded, change in fill will result in fail

    assert_int_equal(a->children->element_count, 1);
    assert_int_equal(b->children->element_count, 0);
    assert_int_equal(c->children->element_count, 0);
    assert_int_equal(d->children->element_count, 0);
    assert_int_equal(e->children->element_count, 0);

    assert_ptr_equal(a->parent, root);
    assert_ptr_equal(b->parent, root);
    assert_ptr_equal(c->parent, root);
    assert_ptr_equal(d->parent, root);
    assert_ptr_equal(e->parent, root);

    Node* aa = a->children->storage[0];

    assert_false(aa->is_word);
    assert_true(aa->value == L'ą');
    assert_int_equal(aa->children->element_count, 1);
    assert_ptr_equal(aa->parent, a);

    Node* aab = aa->children->storage[0];

    assert_true(aab->is_word);
    assert_true(aab->value == L'b');
    assert_int_equal(aab->children->element_count, 2);
    assert_ptr_equal(aab->parent, aa);

    Node* aaba = aab->children->storage[0];
    Node *aabc = aab->children->storage[1];

    assert_false(aaba->is_word);
    assert_true(aaba->value == L'ą');
    assert_int_equal(aaba->children->element_count, 1);
    assert_ptr_equal(aaba->parent, aab);

    assert_true(aabc->is_word);
    assert_true(aabc->value == L'ć');
    assert_int_equal(aabc->children->element_count, 0);
    assert_ptr_equal(aabc->parent, aab);

    Node* aabaa = aaba->children->storage[0];

    assert_true(aabaa->is_word);
    assert_true(aabaa->value == L'ą');
    assert_int_equal(aabaa->children->element_count, 0);
    assert_ptr_equal(aabaa->parent, aaba);

    teardown_trie(state);
    return;
}

///Tests whether children set is collapsing correctly.
static void test_trie_structure_horizontal_collapse(void** state)
{
    setup_trie_full_structure(state);
    Node* root = *state;

    assert_true(trie_delete_word(root, L"b"));
    assert_true(trie_delete_word(root, L"ć"));
    assert_true(trie_delete_word(root, L"d"));
    assert_true(trie_delete_word(root, L"ę"));
    assert_true(root->children->array_size < 5);
    assert_int_equal(root->children->element_count, 1);

    assert_true(trie_verify(root, true));

    teardown_trie(state);
    return;
}

///Tests if long lane of vertices is removed when last vertex is no more a word.
static void test_trie_structure_vertical_collapse(void** state)
{
    setup_trie_full_structure(state);
    Node* root = *state;

    Node* a = root->children->storage[2];
    Node* aa = a->children->storage[0];
    Node* aab = aa->children->storage[0];
    //Node* aaba = aab->children->storage[0];
    //Node* aabc = aab->children->storage[1];
    //Node* aabaa = aaba->children->storage[0];

    assert_true(trie_delete_word(root, L"ąąbć"));
    assert_true(trie_verify(root, true));
    assert_int_equal(aab->children->element_count, 1);
    assert_false(trie_find_word(root, L"ąąbć"));

    assert_true(trie_delete_word(root, L"ąąbąą"));
    assert_true(trie_verify(root, true));
    assert_int_equal(aab->children->element_count, 0);
    assert_false(trie_find_word(root, L"ąąbąą"));

    assert_true(trie_delete_word(root, L"ą"));
    assert_true(trie_verify(root, true));
    assert_false(a->is_word);
    assert_int_equal(a->children->element_count, 1);
    assert_false(trie_find_word(root, L"ą"));

    assert_false(trie_find_word(root, L"ąą"));

    assert_true(trie_delete_word(root, L"ąąb"));
    assert_true(trie_verify(root, true));
    Node* first = root->children->storage[0];
    assert_false(first->value == *L"ą"); //this branch should be fully removed;

    a = NULL;
    aa = NULL;
    aab = NULL;
    //aaba = NULL;
    //aabc = NULL; //in case of leaks, here are losts.

    teardown_trie(state);
    return;
}

//IO TESTS

//IO buffer size in bytes (chars)
extern char* get_io_buffer(void);
extern void reset_io_buffer(void);
#define io_buffer (get_io_buffer()) ///<Shortening macro

/*
 * Notice, that every test must be followed by clearing io_buffer.
 * */

//Just one single node, saving, then restoring it.
/* Due to change of implementation, it's impossible to save single node to file.
static void test_save_read_single_node(void** state)
{
    reset_io_buffer();
    Node* node = trie_new_node();
    wchar_t sign = L'a';
    node->is_word = true;
    node->value = sign;

    trie_save_to_file(node, (FILE*) 42);
    //first 4 bytes are value, then there is a number, which stores both is_word
    //and number of children, is_word in oldest byte
    Node* read = trie_load_from_file((FILE*) 42);

    assert_true(read->is_word);
    assert_true(read->value == sign);
    assert_null(read->parent);
    assert_int_equal(read->children->element_count, 0);

    trie_free_node(node);
    trie_free_node(read);

}
*/

///Saving empty trie
static void test_save_load_empty_trie(void** state)
{
    reset_io_buffer();
    Node* node = trie_new_node();
    trie_save_to_file(node, (FILE*) 42);
    Node* read = trie_load_from_file((FILE*) 42);

    assert_non_null(read);

    assert_true(read->value == node->value);
    assert_true(read->is_word == node->is_word);
    assert_true(read->parent == node->parent);
    assert_true(read->children->element_count == node->children->element_count);

    trie_free_node(read);
    trie_free_node(node);
}

///Saving trie with two words: "a", "b".
static void test_save_read_three_nodes(void** state)
{
    reset_io_buffer();
    Node* root = trie_new_node();
    wchar_t* a = L"a";
    wchar_t* b = L"b";
    trie_insert_word(root, a);
    trie_insert_word(root, b);

    assert_non_null(root->children->storage[0]);
    assert_non_null(root->children->storage[1]);

    trie_save_to_file(root, (FILE*) 42);

    Node* read_root = trie_load_from_file((FILE*) 42);
    assert_non_null(read_root);
    Node* read_a = read_root->children->storage[0];
    Node* read_b = read_root->children->storage[1];

    assert_non_null(read_a);
    assert_non_null(read_b);

    assert_true(read_root->value == 0);
    assert_false(read_root->is_word);
    assert_null(read_root->parent);
    assert_int_equal(read_root->children->element_count, 2);

    assert_true(read_a->value == a[0]);
    assert_true(read_b->value == b[0]);
    assert_true(read_a->is_word);
    assert_true(read_b->is_word);
    assert_ptr_equal(read_a->parent, read_b->parent);
    assert_ptr_equal(read_b->parent, read_root);

    trie_free_node(root);
    trie_free_node(read_root);
}

///Saving trie structure described in fill table.
///Involves non-ascii characters.
static void test_save_load_full_structure(void** state)
{
    reset_io_buffer();
    setup_trie_full_structure(state);
    Node* root = *state;

    trie_save_to_file(root, (FILE*) 42);


    Node* read_root = trie_load_from_file((FILE*) 42);

    Node *a, *b, *c, *d, *e;
    b = read_root->children->storage[0];
    d = read_root->children->storage[1];
    a = read_root->children->storage[2];
    c = read_root->children->storage[3];
    e = read_root->children->storage[4];

    assert_int_equal(read_root->children->element_count, 5);
    assert_true(a->is_word);
    assert_true(b->is_word);
    assert_true(c->is_word);
    assert_true(d->is_word);
    assert_true(e->is_word);

    assert_true(a->value == L'ą');
    assert_true(b->value == L'b');
    assert_true(c->value == L'ć');
    assert_true(d->value == L'd');
    assert_true(e->value == L'ę');
    //hardcoded, change in fill will result in fail

    assert_int_equal(a->children->element_count, 1);
    assert_int_equal(b->children->element_count, 0);
    assert_int_equal(c->children->element_count, 0);
    assert_int_equal(d->children->element_count, 0);
    assert_int_equal(e->children->element_count, 0);

    assert_ptr_equal(a->parent, read_root);
    assert_ptr_equal(b->parent, read_root);
    assert_ptr_equal(c->parent, read_root);
    assert_ptr_equal(d->parent, read_root);
    assert_ptr_equal(e->parent, read_root);

    Node* aa = a->children->storage[0];

    assert_false(aa->is_word);
    assert_true(aa->value == L'ą');
    assert_int_equal(aa->children->element_count, 1);
    assert_ptr_equal(aa->parent, a);

    Node* aab = aa->children->storage[0];

    assert_true(aab->is_word);
    assert_true(aab->value == L'b');
    assert_int_equal(aab->children->element_count, 2);
    assert_ptr_equal(aab->parent, aa);

    Node* aaba = aab->children->storage[0];
    Node *aabc = aab->children->storage[1];

    assert_false(aaba->is_word);
    assert_true(aaba->value == L'ą');
    assert_int_equal(aaba->children->element_count, 1);
    assert_ptr_equal(aaba->parent, aab);

    assert_true(aabc->is_word);
    assert_true(aabc->value == L'ć');
    assert_int_equal(aabc->children->element_count, 0);
    assert_ptr_equal(aabc->parent, aab);

    Node* aabaa = aaba->children->storage[0];

    assert_true(aabaa->is_word);
    assert_true(aabaa->value == L'ą');
    assert_int_equal(aabaa->children->element_count, 0);
    assert_ptr_equal(aabaa->parent, aaba);

    trie_free_node(read_root);
    teardown_trie(state);
}

///Just to document this function.
int main(int argc, char** argv)
{
    const struct CMUnitTest trie_manual_tests[] =
    {
        cmocka_unit_test(test_delete_empty_trie),
        cmocka_unit_test(test_delete_null_trie),
        cmocka_unit_test(test_search_empty_trie),
        cmocka_unit_test(test_delete_word_empty_trie),
        cmocka_unit_test(test_simple_insert),
        cmocka_unit_test(test_find_delete_strings),
        cmocka_unit_test(test_insert_full_structure),
        cmocka_unit_test(test_trie_structure_basic),
        cmocka_unit_test(test_trie_structure_horizontal_collapse),
        cmocka_unit_test(test_trie_structure_vertical_collapse)
    };
    cmocka_run_group_tests_name("Trie logic manual tests", trie_manual_tests, NULL, NULL);

    const struct CMUnitTest trie_io_tests[] =
    {
        //cmocka_unit_test(test_save_read_single_node),
        cmocka_unit_test(test_save_load_empty_trie),
        cmocka_unit_test(test_save_read_three_nodes),
        cmocka_unit_test(test_save_load_full_structure)
    };
    cmocka_run_group_tests_name("Trie io tests", trie_io_tests, NULL, NULL);

}
