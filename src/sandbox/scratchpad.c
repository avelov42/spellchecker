#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include "../dictionary/dictionary.h"
/**
 * Zadania:
 * Wszystkie wcześniejsze <done>
 * Napisanie dokumentacji do array_test. <done>
 * Napisanie testów trie <done>
 * Napisanie funkcji IO do trie <done>
 * Napisanie dokumentacji do trie. <done> * 
 * Napisanie IO testów do trie <done>
 * Napisanie dokumentacji do trie testy <done>
 * OPISAC DZIALANIE TESTOW W TRIE <done>
 * Naprawić bug'a w testach array_set <done>
 * Napisanie IO w dictionary <done>>
 * Napisanie (przekopiowanie) prostych hints do dictionary <done>
 * Napisanie (przekopiowanie) word_list <done>
 * Napisanie testów i dokumentacji do dictionary <done>
 * Napisanie (przekopiowanie) dict-checka. To działa, więc ostrożnie ze zmienianiem <done
 * Dokumentacj do powyższego <done>
 *
 *
 * Napisanie (przekopiowanie) testów do word_list <done>
 * docs do powyzszych testow <done>
 *
 * testy do multidict <done, walkower>
 *
 * Napisanie interfejsu multidict. <done>
 * POPRAWIĆ TESTY DO IO Z TRIE_DEBUG_FUNCTIONS <done>
 *
 * Napisanie rzeczy z GTK.
 * Testy do dictionary
 *
 * <Total overview>
 * Przejrzenie kodu z pierwszego zadania, branch z optymalizacjiami.
 * Wysłanie tegóż zadania.
 * I ogarnięcie makefile-a
 *
 * POPRAWIĆ CHIŃSKIE ZNACZKI W GTKEDITORZE
 *
 * OGARNAC DICT_LOAD
 *
 * README SERIOUSLY
 *
 */

void replace_comma(wchar_t* arr)
{
    for(int i = 0; arr[i] != 0; i++)
        if(arr[i] == L',')
        {
            arr[i] = L'\0';
            return;
        }
}


#define WORD_LEN 64
void load_lang(char* lang, bool with_commas)
{
    printf("Loading %s\n", lang);
    FILE* file = fopen(lang, "r");
    if(file == NULL)
    {
        printf("error..\n");
        return;
    }
    wchar_t word[WORD_LEN];
    Dictionary* dict = dictionary_new();
    while(true)
    {
        memset(word, 0, sizeof(wchar_t) * WORD_LEN);
        fwscanf(file, L"%ls", word);

        if(with_commas) replace_comma(word);
        if(wcslen(word) == 0) //last word
            break;
        dictionary_insert(dict, word);
    }
    fclose(file);
    dictionary_save_lang(dict, lang);
    dictionary_done(dict);
}



int main(int argc, char** argv)
{

    setlocale(LC_ALL, "pl_PL.UTF-8");

    load_lang("pl_PL", true);
    load_lang("eng_UK", false);
    load_lang("eng_US", false);
/*
    const char* lang = "pl_SJP";
    wprintf(L"Saving..\n");
    dictionary_save_lang(dict, lang);
    wprintf(L"Deleting..\n");
    dictionary_done(dict);
    wprintf(L"Loading..\n");
    dict = dictionary_load_lang(lang);
*
    wprintf(L"Finding..\n");
    file = fopen("odm.txt", "r");
    while(true)
    {
        memset(word, 0, sizeof(wchar_t) * WORD_LEN);
        fwscanf(file, L"%ls", word);
        replace_comma(word);
        if(wcscmp(word, L"fghjkl") == 0) //last word
            break;
        if(dictionary_find(dict, word) == DICTIONARY_WORD_NOT_FOUND)
            wprintf(L"NOT FOUND: %ls\n", word);
    }
    */
    //fclose(file);
    return 0;
}



