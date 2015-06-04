
#include "../dictionary/word_list.h"
#include <assert.h>

#include <stdlib.h>
#include "../dictionary/word_list.h"
#include <string.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdbool.h>

//poki co jest to plik z testami

static void list_manual_test(void)
{
    wprintf(L"***STARTED MANUAL TEST***\n");
    wprintf(L"Testing list..\n");
    struct word_list l;
    word_list_init(&l);
    assert(l.first == NULL);
    assert(l.last == NULL);
    assert(l.word_count == 0);
    wprintf(L"Init done..\n");

    wprintf(L"Removing empty..\n");
    word_list_done(&l);
    wprintf(L"Removing done..\n");

    wchar_t* wempty = L"";
    wchar_t* wone = L"ź";
    wchar_t* wstr = L"Zażółć gęślą jaźń";

    wprintf(L"Testing adding");
    word_list_init(&l);

    word_list_add(&l, wempty);
    assert(word_list_size(&l) == 1);

    word_list_add(&l, wone);
    word_list_add(&l, wstr);

    assert(word_list_size(&l) == 3);

    wprintf(L"Adding done..\n");
    wprintf(L"Testing get..\n");
    wchar_t** tab = word_list_get(&l);

    assert(wcscmp(tab[0], tab[1]) <= 0);
    assert(wcscmp(tab[1], tab[2]) <= 0);

    wprintf(L"Get test done?..\n");
    wprintf(L"Deleting..\n");
    word_list_done(&l);
    assert(l.first == l.last && l.first == NULL);
    wprintf(L"Everything to be ok! ***\n\n");
}

#define MAX_WORD_LENGTH 120
wchar_t* getRandomWord(void)
{
    int len = rand()%(MAX_WORD_LENGTH+1);
    wchar_t* ret = malloc(sizeof(wchar_t) * (len+1));
    ret[len] = 0;
    for(int i = 0; i<len; i++)
        ret[i] = rand()%90+ 32;
    return ret;
}

#define NUMBER_OF_TESTS 200000
#define VERBOSE false
static void list_auto_test(void)
{
    wprintf(L"*** STARTED AUTOMATIC TEST *** \n");
    wchar_t* words[NUMBER_OF_TESTS];
    for(int i = 0; i<NUMBER_OF_TESTS; i++)
        words[i] = getRandomWord();
    wprintf(L"%d words generated: \n", NUMBER_OF_TESTS);
    if(VERBOSE)
    {
        for(int i = 0; i<NUMBER_OF_TESTS; i++)
            wprintf(L"%ls (%d)\n", words[i], wcslen(words[i]));
    }
    struct word_list l;
    word_list_init(&l);
    wprintf(L"Adding to list..\n");
    for(int i = 0; i<NUMBER_OF_TESTS; i++)
        word_list_add(&l, words[i]);
    assert(word_list_size(&l) == NUMBER_OF_TESTS);

    wprintf(L"Getting array..\n");
    wchar_t** tab = word_list_get(&l);
    wprintf(L"Got words: \n");
    if(VERBOSE)
    {
        for(int i = 0; i<NUMBER_OF_TESTS; i++)
            wprintf(L"[%d] = %ls\n", i, tab[i]);
    }
    wprintf(L"Checking if sorted: \n");

    for(int i = 0; i+1<NUMBER_OF_TESTS; i++)
    {
        assert(wcscmp(tab[i], tab[i+1]) <= 0 );
        if(VERBOSE)wprintf(L"%d<=%d checked\n", i, i+1);
    }


    wprintf(L"Words seems to be sorted, so it's OK!\n");

    wprintf(L"Deleting..\n");
    word_list_done(&l);
    assert(l.first == l.last && l.first == NULL);

}



int main(int argc, char** argv)
{
    setlocale(LC_ALL, "pl_PL.UTF-8");
    list_manual_test();
    list_auto_test();
}





