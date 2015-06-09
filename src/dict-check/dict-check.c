/** @defgroup dict-check Program dict-check
 * Prosty program sprawdzający poprawność tekstu.
 */

/** @file
 * Jednomodułowy program sprawdzający poprawność tekstu.
 * @ingroup dict-check
 * @author Piotr Rybicki <pr360957@students.mimuw.edu.pl>
 */

#include <stdbool.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include "../dictionary/dictionary.h"
#include "../dictionary/word_list.h"
#include "wctype.h"

#define SINGLE_WORD_MAX_LENGTH 8192

static struct dictionary* dict;
static int line = 1;
static int column = 1;
static int wordLine;
static int wordColumn;

/**
 * @brief Wczytuje słowo lub kawałek śmieci.
 * @param[out] dest - tablica, w której owo wczytane coś jest składowane
 * @param[out] isWord - true, jeżeli składowane coś zostało rozpoznane jako słowo
 * @return False gdy napotkano EOF
 */
static bool readIt(wchar_t* dest, bool* isWord)
{
    int it = 0;
    bool currentState = false;
    wchar_t in;
    while(true)
    {
        in = getwc(stdin);
        column++;
        if(in == EOF)
        {
            dest[it] = L'\0';
            *isWord = currentState;
            return false;
        }
        if(in == L'\n')
        {
            line++;
            column = 1;
        }
        if(it == 0) //it, nie in !
        {
            currentState = iswalpha(in); //sprawdzam, czy jest to słowo
            wordColumn = column-1;
            wordLine = in == L'\n' ? line-1 : line;
        }
        if((iswalpha(in) != 0) != currentState) //zmienił się stan, kończymy
        {
            ungetwc(in, stdin);
            column--;
            if(in == L'\n') //cofamy zmiany, ktore zrobilismy po getwc
                line--;
            dest[it] = L'\0';
            *isWord = currentState;
            return true; //alles gut
        }
        else //ten sam stan, czytamy dalej
            dest[it++] = in;
    }
}

/** @brief Parsuje argumenty, sprawdza poprawność tekstu.
 * <p>
 * Jako argumenty argument pobiera nazwę pliku słownika i ewentualny parametr -v,
 * który powoduje wypisywanie podpowiedzi. Kolejność parametrów jest dowolna, przy czym
 * słownik nie powinien nazywać się "-*" - wówczas UB.
 */
int main(int argc, char** argv)
{
    setlocale(LC_ALL, "pl_PL.UTF-8");
    char* dictName;
    bool hints;
    if(argc < 2)
        return 1;
    else
    {
        if(argv[1][0] == '-' && argv[1][1] == 'v')
        {
            hints = true;
            if(argc != 3)
                return 1;
            dictName = argv[2];
        }
        else
        {
            dictName = argv[1];
            if(argc == 3 && argv[2][0] == '-' && argv[2][1] == 'v') hints = true;
        }



    }
    FILE* file = fopen(dictName, "r");
    dict = dictionary_load(file);
    if(dict == NULL)
    {
        fwprintf(stderr, L"Cannot load dictionary, ending..\n");
        exit(EXIT_FAILURE);
    }
    wchar_t word[SINGLE_WORD_MAX_LENGTH];
    bool isWord;
    while(readIt(word, &isWord))
    {
        if(isWord)
        {
            if(dictionary_find(dict, word) != DICT_WORD_FOUND)
            {
                wprintf(L"#");
                if(hints)
                {
                    struct word_list list;
                    dictionary_hints(dict, word, &list);
                    wchar_t** hintsTab = word_list_get(&list);
                    int hlen = word_list_size(&list);
                    fwprintf(stderr, L"%d,%d %ls: ", wordLine, wordColumn, word);
                    for(int i = 0; i < hlen; i++)
                        fwprintf(stderr, L"%ls ", hintsTab[i]);

                    fwprintf(stderr, L"\n");
                }
            }
        }
        wprintf(L"%ls", word);
    }
    wprintf(L"%ls", word);
    }
