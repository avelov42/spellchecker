///SORTOWANIE W LISCIE POWINNO SIE ODBYWAC PO WCSCOLL
#include <stdbool.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include "../dictionary/dictionary.h"
#include "../dictionary/word_list.h"
#include "wctype.h"


struct dictionary* dict;
int line = 1;
int column = 1;
int wordLine;
int wordColumn;

#define SINGLE_WORD_MAX_LENGTH 1024

/**
 * @brief Wczytuje słowo lub kawałek śmieci.
 * @param[out] dest - tablica, w której owo wczytane coś jest składowane
 * @param[out] isWord - true, jeżeli składowane coś zostało rozpoznane jako słowo
 * @return False gdy napotkano EOF
 */
bool readIt(wchar_t* dest, bool* isWord)
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
            line++;
        if(it == 0)
            currentState = iswalpha(in); //sprawdzam, czy jest to słowo
        if((iswalpha(in) != 0) != currentState) //zmienił się stan, kończymy
        {
            ungetwc(in, stdin);
            column--;
            if(in == L'\n');
                line--;
            dest[it] = L'\0';
            *isWord = currentState;
            return true; //alles gut
        }
        else //ten sam stan, czytamy sobie dalej
            dest[it++] = in;

    }
}

int main(int argc, char** argv)
{
    argc = 3;
    argv[1] = malloc(64);
    argv[1] = "dupa2";
    //argv[1] = malloc(64);
    //argv[1] = "-v";
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
                    wprintf(L"%d,%d", wordLine, wordColumn);
                    for(int i = 0; i < hlen; i++)
                        fwprintf(stderr, L"%ls ", hintsTab[i]);
                }

            }
        }
        wprintf(L"%ls", word);
    }
    wprintf(L"%ls", word);

}
