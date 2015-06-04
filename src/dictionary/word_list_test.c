#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <wchar.h>
#include <cmocka.h>

/*
#define static
struct word_node* new_node(const wchar_t* word);
void delete_word_node(struct word_node* node);
*/

#include "word_list.h"

const wchar_t* test   = L"Test string";
const wchar_t* first  = L"First string";
const wchar_t* second = L"Second string";
const wchar_t* third  = L"Third string";

//trywialny test z wewnetrzna inicjalizacja
static void word_list_init_test(void** state)
{
    struct word_list l;
    word_list_init(&l);
    assert_int_equal(word_list_size(&l), 0);
    assert_null(l.first);
    assert_null(l.last);
    word_list_done(&l);
}

//trywialny test z wewnetrzna inicjalizacja
static void word_list_add_test(void** state)
{
    struct word_list l;
    word_list_init(&l);
    word_list_add(&l, test);
    assert_int_equal(word_list_size(&l), 1);
    assert_non_null(l.first);
    assert_non_null(l.last);
    assert_true(wcscmp(test, word_list_get(&l)[0]) == 0);
    word_list_done(&l);
}

//sprawdza poprawnosc nowo utworzonego node-a
//trywialny test, nie potrzebuje setup'a
static void word_new_node_test(void** state)
{
    wchar_t* wempty = L"";
    wchar_t* wone_letter = L"ź";
    wchar_t* wsentence = L"Zażółć gęślą jaźń";

    struct word_node* empty = new_node(wempty);
    struct word_node* one_letter = new_node(wone_letter);
    struct word_node* sentence = new_node(wsentence);

    assert_null(empty->next);
    assert_null(one_letter->next);
    assert_null(sentence->next);

    assert_non_null(empty->word);
    assert_non_null(one_letter->word);
    assert_non_null(sentence->word);

    assert_int_equal(wcscmp(empty->word, wempty), 0);
    assert_int_equal(wcscmp(one_letter->word, wone_letter), 0);
    assert_int_equal(wcscmp(sentence->word, wsentence), 0);

    delete_word_node(empty);
    delete_word_node(one_letter);
    delete_word_node(sentence);
    return;

}

static int word_list_setup_nonempty(void **state)
{
    struct word_list *l = malloc(sizeof(struct word_list));
    if (!l)
        return -1;
    word_list_init(l);
    word_list_add(l, first);
    word_list_add(l, second);
    word_list_add(l, third);
    *state = l;
    return 0;
}
static int word_list_setup_empty(void** state)
{
    struct word_list *l = malloc(sizeof(struct word_list));
    if(!l)
        return -1;
    word_list_init(l);
    *state = l;
    return 0;
}


static int word_list_teardown(void **state)
{
    struct word_list* l = *state;
    word_list_done(l);
    free(l);
    return 0;
}

static void word_list_get_test(void** state)
{
    struct word_list *l = *state;
    assert_true(wcscmp(first, word_list_get(l)[0]) == 0);
    assert_true(wcscmp(second, word_list_get(l)[1]) == 0);
    assert_true(wcscmp(third, word_list_get(l)[2]) == 0);
}

static void word_list_repeat_test(void** state)
{
    struct word_list *l = *state;
    word_list_add(l, third);
    assert_int_equal(word_list_size(l), 4);
    assert_true(wcscmp(third, word_list_get(l)[3]) == 0);
}


int main(void)
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(word_list_init_test),
        cmocka_unit_test(word_list_add_test),
        cmocka_unit_test(word_new_node_test),
        cmocka_unit_test_setup_teardown(word_list_get_test, word_list_setup_nonempty, word_list_teardown),
        cmocka_unit_test_setup_teardown(word_list_repeat_test, word_list_setup_nonempty, word_list_teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

