/** @file
  Tests of word list.
  @ingroup word_list
  @author Piotr Rybicki <pr360957@students.mimuw.edu.pl>
  @date 2015-08
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmocka.h>
#include <wchar.h>
#include <wctype.h>
#include "word_list.h"


#define MAX_WORD_LENGTH 42 ///<Max length of random word.

///Generates random word.
static wchar_t* getRandomWord(void)
{
    int len = rand()%(MAX_WORD_LENGTH+1);
    wchar_t* ret = malloc(sizeof(wchar_t) * (len+1));
    ret[len] = 0;
    for(int i = 0; i<len; i++)
        ret[i] = rand()%20+ 64;
    return ret;
}

///Setup
static int setup_empty_list(void** state)
{
    Word_List* list = word_list_new();
    if(list == NULL)
        return -1;
    *state = list;
    return 0;
}

///Teardown
static int teardown_list(void** state)
{
    Word_List* list = *state;
    word_list_free(list);
    return 0;
}

///Creating empty list, removing it
static void test_create_remove_empty_list(void** state)
{
    setup_empty_list(state);
    Word_List* list = *state;
    assert_null(list->first);
    assert_null(list->last);
    assert_int_equal(list->word_count, 0);


    teardown_list(state);
}

///Adds few words, gets sorted array and checks order of sorted elements.
static void test_add_get_delete(void** state)
{
    setup_empty_list(state);
    Word_List* list = *state;

    wchar_t* wempty = L"";
    wchar_t* wone = L"ź";
    wchar_t* wstr = L"Zażółć gęślą jaźń";

    word_list_add(list, wempty);
    assert_true(word_list_size(list) == 1);
    word_list_add(list, wone);
    word_list_add(list, wstr);
    assert_true(word_list_size(list) == 3);

    wchar_t** tab = word_list_get(list);
    assert_true(wcscoll(tab[0], tab[1]) <= 0);
    assert_true(wcscoll(tab[1], tab[2]) <= 0);

    assert_true(wcscmp(tab[1], wstr) == 0);


    free(tab[0]);
    free(tab[1]);
    free(tab[2]);
    free(tab);
    teardown_list(state);

}


#define WORDS_IN_AUTO_TEST 16384 ///<Number of random words used in auto_test
/**
  Generates a huge number of random words, stores them in list,
  takes out as a sorted array, check whether all added words are in the array
  checks order of the words in array, deletes list.
 */
static void auto_test(void** state)
{
    setup_empty_list(state);
    Word_List* list = *state;

    printf("Generating randoms..\n");

    wchar_t* words[WORDS_IN_AUTO_TEST];
    for(int i = 0; i < WORDS_IN_AUTO_TEST; i++)
        words[i] = getRandomWord();

    printf("Adding..\n");

    for(int i = 0; i < WORDS_IN_AUTO_TEST; i++)
        word_list_add(list, words[i]);

    assert_true(word_list_size(list) == WORDS_IN_AUTO_TEST);
    assert_true(wcscoll(list->first->word, words[0]) == 0);
    assert_true(wcscoll(list->last->word, words[WORDS_IN_AUTO_TEST-1]) == 0);
    assert_true(list->last->next == NULL);

    printf("Getting sorted array..\n");

    wchar_t** tab = word_list_get(list);

    printf("Checking order..\n");

    for(int i = 0; i+1 < WORDS_IN_AUTO_TEST; i++)
        assert_true(wcscoll(tab[i], tab[i+1]) <= 0);

    printf("Checking for presence of every word in sorted tab..\n");

    for(int s = 0; s < WORDS_IN_AUTO_TEST; s++)
    {
        for(int w = 0; w < WORDS_IN_AUTO_TEST; w++)
            if(words[w] != NULL && wcscoll(words[w], tab[s]) == 0)
            {
                free(words[w]); //kasuje slowo juz znalezione w pierwotnej tablicy
                words[w] = NULL;
            }
    }

    //now whole words array should be nulled
    for(int i = 0; i < WORDS_IN_AUTO_TEST; i++)
        assert_null(words[i]);

    printf("Deleting list..\n");


    for(int i = 0; i < WORDS_IN_AUTO_TEST; i++)
        free(tab[i]);
    free(tab);


    teardown_list(state);


}

///Main..
int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create_remove_empty_list),
        cmocka_unit_test(test_add_get_delete),
        cmocka_unit_test(auto_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
