#include "../dictionary/word_list.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdbool.h>
#define TRIE_DEBUG_FUNTIONS
#include "../dictionary/trie.h"
#include "../dictionary/dictionary.h"

#define VERBOSE false

#define WORD_MAX_LENGTH 12
#define LIST_NUMBER_OF_AUTO_TESTS 1000

wchar_t* getRandomWord(void)
{
    int len = rand()%(WORD_MAX_LENGTH+1);
    wchar_t* ret = malloc(sizeof(wchar_t) * (len+1));
    ret[len] = 0;
    for(int i = 0; i<len; i++)
        ret[i] = rand()%20+ 64;
    return ret;
}

void list_manual_test(void)
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

    assert(wcscoll(tab[0], tab[1]) <= 0);
    assert(wcscoll(tab[1], tab[2]) <= 0);

    wprintf(L"Get test done?..\n");
    wprintf(L"Deleting..\n");
    word_list_done(&l);
    assert(l.first == l.last && l.first == NULL);
    wprintf(L"Everything to be ok! ***\n\n");
    free(tab[0]);
    free(tab[1]);
    free(tab[2]);
    free(tab);
}

void list_auto_test(void)
{
    wprintf(L"*** STARTED AUTOMATIC TEST *** \n");

    wchar_t* words[LIST_NUMBER_OF_AUTO_TESTS];
    for(int i = 0; i<LIST_NUMBER_OF_AUTO_TESTS; i++)
        words[i] = getRandomWord(); //losujemy LIST_NUMBER_OF_AUTO_TESTS słów

    wprintf(L"%d words generated: \n", LIST_NUMBER_OF_AUTO_TESTS);
    if(VERBOSE)
        for(int i = 0; i<LIST_NUMBER_OF_AUTO_TESTS; i++)
            wprintf(L"%ls (%d)\n", words[i], wcslen(words[i]));


    wprintf(L"Adding to list..\n");
    struct word_list l;
    word_list_init(&l);
    for(int i = 0; i<LIST_NUMBER_OF_AUTO_TESTS; i++)
        word_list_add(&l, words[i]);
    assert(word_list_size(&l) == LIST_NUMBER_OF_AUTO_TESTS);
    assert(wcscoll(l.first->word, words[0]) == 0);
    assert(wcscoll(l.last->word, words[LIST_NUMBER_OF_AUTO_TESTS-1]) == 0);
    assert(l.last->next == NULL);
    wprintf(L"Added correctly!..\n");

    wprintf(L"Getting array..\n");
    wchar_t** tab = word_list_get(&l);
    wprintf(L"Got %d words: \n", LIST_NUMBER_OF_AUTO_TESTS);
    if(VERBOSE)
        for(int i = 0; i<LIST_NUMBER_OF_AUTO_TESTS; i++)
            wprintf(L"[%d] = %ls\n", i, tab[i]);

    wprintf(L"Checking if sorted: \n");

    for(int i = 0; i+1<LIST_NUMBER_OF_AUTO_TESTS; i++)
    {
        assert(wcscoll(tab[i], tab[i+1]) <= 0 );
        if(VERBOSE)wprintf(L"%d<=%d checked\n", i, i+1);
    }
    wprintf(L"Words seems to be sorted!\n");

    if(LIST_NUMBER_OF_AUTO_TESTS <= 40000)
    {
        wprintf(L"Checking presence of every word in sorted tab.. O(n^2), it may take a while..\n");
        for(int s = 0; s < LIST_NUMBER_OF_AUTO_TESTS; s++)
        {
            for(int w = 0; w < LIST_NUMBER_OF_AUTO_TESTS; w++)
                if(words[w] != NULL && wcscoll(words[w], tab[s]) == 0)
                {
                    free(words[w]); //kasuje slowo juz znalezione w pierwotnej tablicy
                    words[w] = NULL;
                }
        }
        //w tym momencie cala tablica words powinna byc wynullowana
        for(int i = 0; i < LIST_NUMBER_OF_AUTO_TESTS; i++)
            assert(words[i] == NULL);
        wprintf(L"OK!\n");
    }
    else
    {
        wprintf(L"Number of tests too large, skipped presence check..\n");
    }


    wprintf(L"Deleting list..\n");
    word_list_done(&l);
    assert(l.first == l.last && l.first == NULL);
    wprintf(L"Deleted!\n");

    wprintf(L"Deleting returned array!\n");
    for(int i = 0; i<LIST_NUMBER_OF_AUTO_TESTS; i++)
        free(tab[i]);
    free(tab);
    wprintf(L"Deleted!\n");

}


int verifyAndPrintfStats(Node* root)
{
    int sum, checksum = 0;
    wprintf(L"Checking trie: ");
    sum = trieVerifyTrie(root, &checksum, true, false);
    wprintf(L"NonEmptyNodes: %d | checksum: %d, trie correct\n", sum, checksum);
    return sum;
}

void manualTrieRevenge(void)
{
    wprintf(L"\n***STARTED MANUAL TRIE REVENGE ***\n");

    Node* root = trieNewNode();
    wprintf(L"\nCreated empty trie..\n");
    assert(verifyAndPrintfStats(root) == 1);

    wprintf(L"\nDeleting empty trie..\n");
    trieDeleteTrie(root);
    root = NULL;
    //w tym momencie nie moge wiecej sprawdzic, ale jezeli cos jest nie tak
    //to wyjdzie to na jaw w valgrindzie
    wprintf(L"Deleted, creating new..\n");
    root = trieNewNode();

    wprintf(L"\nFinding in empty trie..\n");
    assert(trieFindWord(root, L"") == TRIE_WORD_NOT_FOUND);
    assert(trieFindWord(root, L"©") == TRIE_WORD_NOT_FOUND);
    assert(trieFindWord(root, L"Zażółć") == TRIE_WORD_NOT_FOUND);
    assert(trieFindWord(root, L"Zażółć") == TRIE_WORD_NOT_FOUND);
    wprintf(L"Passed..\n");

    wprintf(L"\nDeleting in empty trie..\n");
    assert(trieDeleteWord(root, L"") == TRIE_WORD_NOT_DELETED);
    assert(trieDeleteWord(root, L"C") == TRIE_WORD_NOT_DELETED);
    assert(trieDeleteWord(root, L"Zażółć") == TRIE_WORD_NOT_DELETED);
    assert(trieDeleteWord(root, L"Zażółć") == TRIE_WORD_NOT_DELETED);
    wprintf(L"Passed..\n");

    wprintf(L"\nInsert test ***\n");
    assert(trieInsertWord(root, L"") == TRIE_INSERT_SUCCESS_NOT_MODIFIED); //NIE POWINNO SIE DAC WSTAWIC
    assert(trieInsertWord(root, L"") == TRIE_INSERT_SUCCESS_NOT_MODIFIED); //PUSTEGO SLOWA

    assert(trieInsertWord(root, L"a") == TRIE_INSERT_SUCCESS_MODIFIED);
    assert(trieInsertWord(root, L"a") == TRIE_INSERT_SUCCESS_NOT_MODIFIED);

    assert(trieInsertWord(root, L"aaa") == TRIE_INSERT_SUCCESS_MODIFIED);
    assert(trieInsertWord(root, L"aaa") == TRIE_INSERT_SUCCESS_NOT_MODIFIED);

    assert(trieInsertWord(root, L"aab") == TRIE_INSERT_SUCCESS_MODIFIED);
    assert(trieInsertWord(root, L"aab") == TRIE_INSERT_SUCCESS_NOT_MODIFIED);

    assert(trieInsertWord(root, L"aabaa") == TRIE_INSERT_SUCCESS_MODIFIED);
    assert(trieInsertWord(root, L"aabaa") == TRIE_INSERT_SUCCESS_NOT_MODIFIED);
    wprintf(L"Inserting done ***\n");

    wprintf(L"\nShould be (7) nodes in trie..\n");
    assert(verifyAndPrintfStats(root) == 7);
    wprintf(L"Success, 7 nodes in trie!\n");

    wprintf(L"\nFind test *** \n");
    wprintf(L"Before finding (any modyfications in find?)\n");
    int sum, checksum = 0;
    sum = trieVerifyTrie(root, &checksum, true, false);
    verifyAndPrintfStats(root);

    assert(trieFindWord(root, L"") == TRIE_WORD_NOT_FOUND);
    assert(trieFindWord(root, L"") == TRIE_WORD_NOT_FOUND);

    assert(trieFindWord(root, L"a") == TRIE_WORD_FOUND);
    assert(trieFindWord(root, L"a") == TRIE_WORD_FOUND);

    assert(trieFindWord(root, L"aa") == TRIE_WORD_NOT_FOUND); //jest taki wierzcholek, ale nie ma slowa
    assert(trieFindWord(root, L"aa") == TRIE_WORD_NOT_FOUND);

    assert(trieFindWord(root, L"aaa") == TRIE_WORD_FOUND);
    assert(trieFindWord(root, L"aaa") == TRIE_WORD_FOUND);

    assert(trieFindWord(root, L"AAA") == TRIE_WORD_NOT_FOUND);
    assert(trieFindWord(root, L"Aaa") == TRIE_WORD_NOT_FOUND);
    assert(trieFindWord(root, L"aaA") == TRIE_WORD_NOT_FOUND);

    assert(trieFindWord(root, L"aab") == TRIE_WORD_FOUND);
    assert(trieFindWord(root, L"aac") == TRIE_WORD_NOT_FOUND);

    assert(trieFindWord(root, L"aaaa") == TRIE_WORD_NOT_FOUND);
    assert(trieFindWord(root, L"aaab") == TRIE_WORD_NOT_FOUND);

    assert(trieFindWord(root, L"aabaa") == TRIE_WORD_FOUND);
    assert(trieFindWord(root, L"aabac") == TRIE_WORD_NOT_FOUND);
    //uff
    wprintf(L"After find: \n");
    verifyAndPrintfStats(root);
    int newChecksum = 0;
    assert(trieVerifyTrie(root, &newChecksum, true, false) == sum);
    assert(newChecksum == checksum);
    wprintf(L"Find test done ***\n");
    //drzewo :
    //a*
    //    a
    //        a*
    //        b*
    //            a
    //                a*
    //

    //sum przechowuje aktualna liczbe wierzcholkow
    wprintf(L"\nDelete test ***\n");
    assert(trieDeleteWord(root, L"a") == TRIE_WORD_DELETED); //suma wierzcholkow pozostaje stala
    assert(trieFindWord(root, L"a") == TRIE_WORD_NOT_FOUND);
    assert(trieVerifyTrie(root, &checksum, true, false) == sum);

    assert(trieDeleteWord(root, L"aaa") == TRIE_WORD_DELETED); //kasacja dziecka, ktore ma rodzenstwo
    assert(trieFindWord(root, L"aaa") == TRIE_WORD_NOT_FOUND); //deleteObsoleteNodes nie powinno nic robic
    assert(trieVerifyTrie(root, &checksum, true, false) == sum-1);
    assert(root->childrenArray[0]->childrenArray[0]->childrenArray[0]->childrenCount == 1);

    //SUM oznacza wartosc wyjsciowa czyli 7
    //a
    //    a
    //        b*
    //            a
    //                a*

    assert(trieDeleteWord(root, L"aab") == TRIE_WORD_DELETED);
    assert(trieFindWord(root, L"aab") == TRIE_WORD_NOT_FOUND);
    //a
    //    a
    //        b
    //            a
    //                a*
    //po usunieciu ostatniego wszystko sie powinno zwinac
    assert(trieDeleteWord(root, L"aabaa") == TRIE_WORD_DELETED);
    assert(trieFindWord(root, L"aabaa") == TRIE_WORD_NOT_FOUND);
    assert(trieVerifyTrie(root, &checksum, true, false) == 1); //tylko root
    assert(root->childrenCount == 0);
    wprintf(L"Delete test done ***\n");
    trieDeleteTrie(root);

    //todo: load, save i delete
    //wprintfy, dodac, zapisac, sprawdzic obecnosc, usunac, srpawdzic pustosc

    wprintf(L"\nSave test..\n");
    wprintf(L"Inserting some words..\n");
    root = trieNewNode();
    trieInsertWord(root, L"slow");
    trieInsertWord(root, L"slowo");
    trieInsertWord(root, L"slowacja");
    trieInsertWord(root, L"slimak");
    trieInsertWord(root, L"ipp");
    trieInsertWord(root, L"ippjestfajne");
    trieInsertWord(root, L"alemdlepsze");

    trieDeleteWord(root, L"slow");
    trieDeleteWord(root, L"slimak");

    int sum2, checksum2;
    sum2 = trieVerifyTrie(root, &checksum2, true, false);
    wprintf(L"Before save: \n");
    verifyAndPrintfStats(root);
    wprintf(L"Saving.. \n");

    FILE* file = fopen("file", "w");
    trieSaveToFile(root, file);
    fclose(file);

    wprintf(L"Saved, deleting trie..\n");
    trieDeleteTrie(root);

    wprintf(L"Deleted, loading..\n");

    file = fopen("file", "r");
    root = trieLoadFromFile(file);
    fclose(file);
    wprintf(L"Trie loaded, stats below: \n");
    verifyAndPrintfStats(root);

    int sum3, checksum3 = 0;
    sum3 = trieVerifyTrie(root, &checksum3, true, false);

    assert(sum3 == sum2);
    assert(checksum3 == checksum2);

    wprintf(L"Sum & Checksum correct..\n");
    wprintf(L"Finding added words in loaded trie..\n");
    assert(trieFindWord(root, L"slowo") == TRIE_WORD_FOUND);
    assert(trieFindWord(root, L"slowacja") == TRIE_WORD_FOUND);
    assert(trieFindWord(root, L"ipp") == TRIE_WORD_FOUND);
    assert(trieFindWord(root, L"ippjestfajne") == TRIE_WORD_FOUND);
    assert(trieFindWord(root, L"alemdlepsze") == TRIE_WORD_FOUND);

    assert(trieFindWord(root, L"slow") == TRIE_WORD_NOT_FOUND);
    assert(trieFindWord(root, L"slimak") == TRIE_WORD_NOT_FOUND);
    wprintf(L"All words present!\n");
    //delete

    wprintf(L"\nDeleting trie, say goodbye!\n");
    trieDeleteTrie(root);
    wprintf(L"Deleted..\n");

    wprintf(L"\n*** ALL MANUAL TESTS PASSED ***");
}


#define TRIE_NUMBER_OF_WORDS 100000

wchar_t** getArrayOfRandomWords(int count)
{
    wchar_t** ret = malloc(sizeof(wchar_t*) * count);
    for(int i = 0; i < count; i++)
        ret[i] = getRandomWord();
    return ret;
}

void deleteArrayOfWords(wchar_t** tab, int count)
{
    for(int i = 0; i < count; i++)
        if(tab[i] != NULL)
            free(tab[i]);
    free(tab);
}

///dodaje count slow z tablicy words do drzewa root i sprawdza czy zostaly dodane
///w przypadku pustych slow w tablicy, sprawdza czy rzeczywiscie nie ma ich w drzewie. (nie powinno ich tam być, nawet po "dodaniu")
void testInsert(Node* root, wchar_t** words, int count)
{
    wprintf(L"\n***Insertion test ***\n");
    wprintf(L"Inserting %d words..\n", count);
    wprintf(L"Trie before: \n");
    verifyAndPrintfStats(root);
    for(int i = 0; i < count; i++)
    {
        trieInsertWord(root, words[i]);
        if(wcslen(words[i]) == 0)
            assert(trieFindWord(root, words[i]) == TRIE_WORD_NOT_FOUND);
        else
            assert(trieFindWord(root, words[i]) == TRIE_WORD_FOUND);
    }
    wprintf(L"Inserting done, trie after: \n");
    verifyAndPrintfStats(root);
    wprintf(L"***Insertion test done ***\n");
}

#define PROBABILITY_DENOMINATOR 4
void testDeletePartial(Node* root, wchar_t** words, int count)
{
    wprintf(L"\n***PARTIAL DELETE TEST***\n");
    int it = 0;
    for(int i = 0; i < count; i++)
    {
        if(rand()%PROBABILITY_DENOMINATOR == 0)
        {
            trieDeleteWord(root, words[i]);
            free(words[i]);
            words[i] = NULL;
            it++;
        }
    }
    wprintf(L"Partial test done, deleted %d words\n", it);
    verifyAndPrintfStats(root);
    wprintf(L"***PARTIAL TEST DONE ***\n");
}

void testDeleteAll(Node* root, wchar_t** words, int count)
{
    wprintf(L"\n***DELETE ALL TEST***\n");
    int it = 0;
    for(int i = 0; i < count; i++)
    {
        if(words[i] != NULL)
        {
            trieDeleteWord(root, words[i]);
            free(words[i]);
            words[i] = NULL;
            it++;
        }
    }
    wprintf(L"All test done, deleted %d words\n", it);
    verifyAndPrintfStats(root);
    wprintf(L"***DELETE ALL TEST DONE ***\n");
}


/**
Automatyczne testy drzewa:
wygenerowanie listy słów
insert wszystkich
find wszystkich
delete polowy
find polowy
delete drugiej polowy
pustosc?
usuniecie drzewa

wygenerowanie slow
wstawienie
usuniecie drzewa

wygenerowanie slow
wstawienie ich do drzewa
zapisanie do pliku
wczytanie do pliku
porownanie hash, sumy
zwolnienie drzewa.
*/
void trieAutoTests()
{
    wchar_t** words = getArrayOfRandomWords(TRIE_NUMBER_OF_WORDS);
    Node* root = trieNewNode();

    testInsert(root, words, TRIE_NUMBER_OF_WORDS);
    testDeletePartial(root, words, TRIE_NUMBER_OF_WORDS);
    testDeleteAll(root, words, TRIE_NUMBER_OF_WORDS);


    wprintf(L"Trie should be empty: \n");
    assert(1 == verifyAndPrintfStats(root));
    wprintf(L"Indeed, it is, deleting\n");

    trieDeleteTrie(root);
    deleteArrayOfWords(words, TRIE_NUMBER_OF_WORDS);

    wprintf(L"Deleted..\n");


    wprintf(L"\n***REMOVAL OF FULL TRIE TEST ***\n");
    root = trieNewNode();
    words = getArrayOfRandomWords(TRIE_NUMBER_OF_WORDS);
    testInsert(root, words, TRIE_NUMBER_OF_WORDS);

    deleteArrayOfWords(words, TRIE_NUMBER_OF_WORDS);
    trieDeleteTrie(root);
    root = NULL;

    //tutaj tylko wycieki moge wykrywac, nie sprawdze czy drzewo jest poprawne :P
    wprintf(L"***REMOVAL OF FULL TRIE TEST DONE ***\n");


    //teraz save
    wprintf(L"\n***SAVE&LOAD TEST***\n");
    root = trieNewNode();
    words = getArrayOfRandomWords(TRIE_NUMBER_OF_WORDS);
    testInsert(root, words, TRIE_NUMBER_OF_WORDS);
    FILE* file = fopen("testtrie", "w");
    trieSaveToFile(root, file);
    fclose(file);

    Node* root2;
    file = fopen("testtrie", "r");
    root2 = trieLoadFromFile(file);
    fclose(file);
    assert(root2 != NULL);

    wprintf(L"Trie saved and loaded, checking for identity..\n");
    int checksum = 0, checksum2 = 0;
    int sum, sum2;

    //wprintf(L"TRIE1:\n");
    //triePrint(root);
    //wprintf(L"TRIE2:\n");
    //triePrint(root2);

    sum = trieVerifyTrie(root, &checksum, true, false);
    sum2 = trieVerifyTrie(root2, &checksum2, true, false);
    assert(sum == sum2);
    assert(checksum == checksum2);

    ///FINDY!
    wprintf(L"Checking indentity by random finds on both tries..\n");

    wchar_t* word;
    for(int i = 0; i<TRIE_NUMBER_OF_WORDS; i++)
    {
        word = getRandomWord();
        assert(trieFindWord(root, word) == trieFindWord(root2, word));
        free(word);
    }

    wprintf(L"Original and loaded trie seems to be equal..\n");


    deleteArrayOfWords(words, TRIE_NUMBER_OF_WORDS  );
    trieDeleteTrie(root);
    trieDeleteTrie(root2);

    wprintf(L"\n***ALL TESTS DONE ***\n\n\n");

}



/*
TODO:
-automatyczne testy do trie DONE
-przejrzenie slownika + napisanie alfabetu (czyt. przekopiowanie binsearcha) <done>
-napisanie save i load w dict
-napisanie HINTS
//koniec na dzis moze byc
-przeczytanie multi_dict
-napisanie dict-checka i ogarniecie testow
-valgrind
-dodanie multi-dict
*/
extern void printAlphabet(struct dictionary*);
int main(int argc, char** argv)
{

    setlocale(LC_ALL, "pl_PL.UTF-8");
     list_manual_test();
     list_auto_test();
    manualTrieRevenge();
    // for(int i = 0 ; i<42; i++)
         trieAutoTests();

    //struct dictionary* dict = dictionary_new();

}





