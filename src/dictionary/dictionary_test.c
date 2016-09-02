/** @file
    Tests of dictionary library.
    @ingroup dictionary
    @author Piotr Rybicki
    @date 2015-08
 */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <wchar.h>
#include <stdio.h>
#include <locale.h>
#include "dictionary.h"


#define TEST_EMPTY_BEGIN setup_empty_dict(state);Dictionary* dict = *state ///<Macro shortening init on the beginning of every test.
#define TEST_END teardown_dict(state) ///<Macro used at the end of every test.

///Setup empty dictionary.
static int setup_empty_dict(void** state)
{
    Dictionary* dict = dictionary_new();
    if(dict == NULL)
        return -1; //dead code probably
    *state = dict;
    return 0;
}

///Teardown
static int teardown_dict(void** state)
{
    Dictionary* dict = *state;
    dictionary_done(dict);
    return 0;
}

///Tests if everything is created. Correctness of trie and alphabet is ensured by other unit-tests.
static void test_empty_dict_correctness(void** state)
{
    TEST_EMPTY_BEGIN;

    assert_non_null(dict->alphabet);
    assert_int_equal(dict->alphabet->element_count, 0);
    assert_non_null(dict->trie_root);

    TEST_END;
}

///Testing if adding empty word is impossible.
static void test_empty_insert(void** state)
{
    TEST_EMPTY_BEGIN;

    wchar_t* empty = L"";

    expect_assert_failure(dictionary_insert(dict, empty));

    TEST_END;
}


///Test of insertion, find, delete with only ascii characters.
static void test_ascii_insert_find_delete(void** state)
{
    TEST_EMPTY_BEGIN;

    wchar_t* first = L"jeden";
    wchar_t* second = L"dwa";
    wchar_t* third = L"trzy";
    wchar_t* symbols = L"!@#$%^&*()";

    assert_true(dictionary_insert(dict, first) == DICTIONARY_INSERT_MODIFIED);
    assert_int_equal(dict->alphabet->element_count, 4);
    assert_true(set_find(dict->alphabet, first)); //j
    assert_true(set_find(dict->alphabet, first+1)); //e
    assert_true(set_find(dict->alphabet, first+2)); //d
    assert_true(set_find(dict->alphabet, first+4)); //n
    assert_true(dictionary_insert(dict,first) == DICTIONARY_INSERT_NOT_MODIFIED);
    assert_int_equal(dict->alphabet->element_count, 4); //number of letters should not change

    assert_true(dictionary_find(dict, first) == DICTIONARY_WORD_FOUND);

    assert_true(dictionary_insert(dict, second));
    assert_true(dictionary_insert(dict, third));
    assert_true(dictionary_insert(dict, symbols));

    assert_true(dictionary_find(dict, second) == DICTIONARY_WORD_FOUND);
    assert_true(dictionary_find(dict, third) == DICTIONARY_WORD_FOUND);
    assert_true(dictionary_find(dict, symbols) == DICTIONARY_WORD_FOUND);

    assert_true(set_find(dict->alphabet, symbols + 3)); //random symbol

    assert_true(dictionary_delete(dict, first));
    assert_true(dictionary_delete(dict, second));
    assert_true(dictionary_delete(dict, third));
    assert_true(dictionary_delete(dict, symbols));

    assert_true(dictionary_find(dict, first) == DICTIONARY_WORD_NOT_FOUND);
    assert_true(dictionary_find(dict, second) == DICTIONARY_WORD_NOT_FOUND);
    assert_true(dictionary_find(dict, third) == DICTIONARY_WORD_NOT_FOUND);
    assert_true(dictionary_find(dict, symbols) == DICTIONARY_WORD_NOT_FOUND);

    TEST_END;
}

///Test with inserting, finding, deleting strings containing non-ascii characters.
static void test_unicode_insert_find_delete(void** state)
{
    TEST_EMPTY_BEGIN;

    wchar_t* first = L"zażółć";
    wchar_t* second = L"gęślą";
    wchar_t* third = L"jaźń";
    wchar_t* symbols = L"πœę©ß←↓→óþł…ə’ŋæðśążźć„”ńµ";

    assert_true(dictionary_insert(dict, first) == DICTIONARY_INSERT_MODIFIED);
    assert_int_equal(dict->alphabet->element_count, 6);
    assert_true(set_find(dict->alphabet, first)); //j
    assert_true(set_find(dict->alphabet, first+1)); //e
    assert_true(set_find(dict->alphabet, first+2)); //d
    assert_true(set_find(dict->alphabet, first+4)); //n
    assert_true(dictionary_insert(dict,first) == DICTIONARY_INSERT_NOT_MODIFIED);
    assert_int_equal(dict->alphabet->element_count, 6); //number of letters should not change

    assert_true(dictionary_find(dict, first) == DICTIONARY_WORD_FOUND);

    assert_true(dictionary_insert(dict, second));
    assert_true(dictionary_insert(dict, third));
    assert_true(dictionary_insert(dict, symbols));

    assert_true(dictionary_find(dict, second) == DICTIONARY_WORD_FOUND);
    assert_true(dictionary_find(dict, third) == DICTIONARY_WORD_FOUND);
    assert_true(dictionary_find(dict, symbols) == DICTIONARY_WORD_FOUND);

    assert_true(set_find(dict->alphabet, symbols + 3)); //random symbol

    assert_true(dictionary_delete(dict, first));
    assert_true(dictionary_delete(dict, second));
    assert_true(dictionary_delete(dict, third));
    assert_true(dictionary_delete(dict, symbols));

    assert_true(dictionary_find(dict, first) == DICTIONARY_WORD_NOT_FOUND);
    assert_true(dictionary_find(dict, second) == DICTIONARY_WORD_NOT_FOUND);
    assert_true(dictionary_find(dict, third) == DICTIONARY_WORD_NOT_FOUND);
    assert_true(dictionary_find(dict, symbols) == DICTIONARY_WORD_NOT_FOUND);

    TEST_END;
}

extern wchar_t* new_low_wstring(const wchar_t* word); ///<Function in dictionary.c used to convert string to low string.

///Test of new_low_wstring function.
static void test_new_lower_string(void** state)
{
    wchar_t* word = L"miMóÓwiaki";
    wchar_t* low = L"mimóówiaki";
    wchar_t* lowered = new_low_wstring(word);

    assert_true(wcscmp(low, lowered) == 0);


    free(lowered);
    return;
}

///Inserts mix-cased words, searches for these words written differently
static void test_lower_case_ness(void**state)
{
    TEST_EMPTY_BEGIN;
    wchar_t* first = L"miMóÓW";
    wchar_t* second = L"MimÓów";
    wchar_t* low = L"mimóów";
    wchar_t* up = L"MIMÓÓW";

    assert_true(dictionary_insert(dict, first) == DICTIONARY_INSERT_MODIFIED);
    assert_true(dictionary_find(dict, first) == DICTIONARY_WORD_FOUND);


    assert_true(dictionary_insert(dict, second) == DICTIONARY_INSERT_NOT_MODIFIED);
    assert_true(dictionary_insert(dict, low) == DICTIONARY_INSERT_NOT_MODIFIED);
    assert_true(dictionary_insert(dict, up) == DICTIONARY_INSERT_NOT_MODIFIED);

    assert_true(dictionary_find(dict, second) == DICTIONARY_WORD_FOUND);
    assert_true(dictionary_find(dict, low) == DICTIONARY_WORD_FOUND);
    assert_true(dictionary_find(dict, up) == DICTIONARY_WORD_FOUND);

    assert_true(dictionary_delete(dict, up) == DICTIONARY_WORD_DELETED);

    assert_true(dictionary_find(dict, first) == DICTIONARY_WORD_NOT_FOUND);
    assert_true(dictionary_find(dict, second) == DICTIONARY_WORD_NOT_FOUND);
    assert_true(dictionary_find(dict, low) == DICTIONARY_WORD_NOT_FOUND);
    assert_true(dictionary_find(dict, up) == DICTIONARY_WORD_NOT_FOUND);

    TEST_END;

}

///Searches for given word in given array.
static bool find_word_in_array(wchar_t** array, wchar_t* word, int array_len)
{
    for(int i = 0; i < array_len; i++)
        if(wcscmp(array[i], word) == 0)
            return true;
    return false;
}

///Tests hints by adding letter.
static void test_hints_add(void** state)
{
    TEST_EMPTY_BEGIN;

    wchar_t* alphabet = L"gąś";
    wchar_t* hintee = L"ą";
    wchar_t* hints[] = {L"gą", L"ąą", L"śą", L"ąg", L"ąś", L"ą"};
    int hints_len = sizeof(hints)/sizeof(wchar_t*);

    assert_true(dictionary_insert(dict, alphabet));
    for(int i = 0; i < hints_len; i++)
        assert_true(dictionary_insert(dict, hints[i]));

    Word_List* hlist = word_list_new();
    dictionary_hints(dict, hintee, hlist);
    wchar_t** ret_hints = word_list_get(hlist);
    int ret_hints_len = word_list_size(hlist);
    assert_int_equal(hints_len, ret_hints_len);
    for(int i = 0; i < hints_len; i++)
        assert_true(find_word_in_array(hints, ret_hints[i], hints_len));

    for(int i = 0; i < hints_len; i++)
        free(ret_hints[i]);
    free(ret_hints);
    word_list_free(hlist);
    TEST_END;
}

///Tests hints by replacing letter.
static void test_hints_replace(void** state)
{
    TEST_EMPTY_BEGIN;
    wchar_t* alphabet = L"abc";
    wchar_t* hintee = L"qp";
    wchar_t* hints[] = {L"ap", L"bp", L"cp", L"qa", L"qb", L"qc", L"qp"};
    int hints_len = sizeof(hints)/sizeof(wchar_t*);

    assert_true(dictionary_insert(dict, alphabet));
    for(int i = 0; i < hints_len; i++)
        assert_true(dictionary_insert(dict, hints[i]));

    Word_List* hlist = word_list_new();
    dictionary_hints(dict, hintee, hlist);
    wchar_t** ret_hints = word_list_get(hlist);
    int ret_hints_len = word_list_size(hlist);
    assert_int_equal(hints_len, ret_hints_len);
    for(int i = 0; i < hints_len; i++)
        assert_true(find_word_in_array(hints, ret_hints[i], hints_len));

    for(int i = 0; i < hints_len; i++)
        free(ret_hints[i]);
    free(ret_hints);
    word_list_free(hlist);
    TEST_END;
}

///Tests hints by deleting one letter.
static void test_hints_delete(void** state)
{
    TEST_EMPTY_BEGIN;
    wchar_t* hintee = L"abcd";
    wchar_t* hints[] = {L"bcd", L"acd", L"abd", L"abc"};
    int hints_len = sizeof(hints)/sizeof(wchar_t*);


    for(int i = 0; i < hints_len; i++)
        assert_true(dictionary_insert(dict, hints[i]));

    Word_List* hlist = word_list_new();
    dictionary_hints(dict, hintee, hlist);
    wchar_t** ret_hints = word_list_get(hlist);
    int ret_hints_len = word_list_size(hlist);
    assert_int_equal(hints_len, ret_hints_len);
    for(int i = 0; i < hints_len; i++)
        assert_true(find_word_in_array(hints, ret_hints[i], hints_len));

    for(int i = 0; i < hints_len; i++)
        free(ret_hints[i]);
    free(ret_hints);
    word_list_free(hlist);
    TEST_END;
}

//IO TESTS
extern char* get_io_buffer(void);
extern void reset_io_buffer(void);
#define io_buffer (get_io_buffer()) ///<Shortening macro

///Simple test checking mainly correctness of the written alphabet.
static void test_io_dictionary(void** state)
{
    reset_io_buffer();
    TEST_EMPTY_BEGIN;
    wchar_t* alphabet = L"abc";
    wchar_t* hintee = L"qp";
    wchar_t* hints[] = {L"ap", L"bp", L"cp", L"qa", L"qb", L"qc", L"qp"};
    int hints_len = sizeof(hints)/sizeof(wchar_t*);

    assert_true(dictionary_insert(dict, alphabet));
    for(int i = 0; i < hints_len; i++)
        assert_true(dictionary_insert(dict, hints[i]));

    dictionary_save(dict, (FILE*) 42);
    dictionary_done(dict);
    dict = dictionary_load((FILE*) 42);

    Word_List* hlist = word_list_new();
    dictionary_hints(dict, hintee, hlist);
    wchar_t** ret_hints = word_list_get(hlist);
    int ret_hints_len = word_list_size(hlist);
    assert_int_equal(hints_len, ret_hints_len);
    for(int i = 0; i < hints_len; i++)
        assert_true(find_word_in_array(hints, ret_hints[i], hints_len));

    for(int i = 0; i < hints_len; i++)
        free(ret_hints[i]);
    free(ret_hints);
    word_list_free(hlist);

    *state = dict;
    TEST_END;


}

///Main of tests.
int main(int argc, char** argv)
{
    setlocale(LC_ALL, "pl_PL.UTF-8");
    const struct CMUnitTest multi_tests[] =
    {
        cmocka_unit_test(test_empty_dict_correctness),
        cmocka_unit_test(test_empty_insert),
        cmocka_unit_test(test_ascii_insert_find_delete),
        cmocka_unit_test(test_unicode_insert_find_delete),
        cmocka_unit_test(test_new_lower_string),
        cmocka_unit_test(test_lower_case_ness),

        //cmocka_unit_test(test_hints_replace),
        cmocka_unit_test(test_hints_add),
        cmocka_unit_test(test_hints_replace),
        cmocka_unit_test(test_hints_delete),
        cmocka_unit_test(test_io_dictionary)
    };
    cmocka_run_group_tests_name("Dict-tests", multi_tests, NULL, NULL);
}
