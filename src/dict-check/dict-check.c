/** @defgroup dict-check Program dict-check
 * Simple program checking correctness of text based on given dictionary as an argument.
 */

/** @file
 * Single-module program that checks correctness of the text.
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

#define SINGLE_WORD_MAX_LENGTH 8192 ///<Size of the buffer for single word.

static struct dictionary* dict; ///<Global pointer to store dict.
static int line = 1; ///<Variable used by read_it to store current line number.
static int column = 1; ///<Variable used by read_it to store current column number.
static int word_line; ///<Variable used to store recently read word line number.
static int word_column; ///<Variable used to store recently read word column number.

/**
 * @brief read_it Reads word or piece of something.
 * @param dest Pointer to array, where read stuff is stored.
 * @param is_word true, if stored element was recognized as a word.
 * @return
 */
static bool read_it(wchar_t* dest, bool* is_word)
{
    int it = 0;
    bool current_state = false;
    wchar_t in;
    while(true)
    {
        in = getwc(stdin);
        column++;
        if(in == EOF)
        {
            dest[it] = L'\0';
            *is_word = current_state;
            return false;
        }
        if(in == L'\n')
        {
            line++;
            column = 1;
        }
        if(it == 0) //it, not in!
        {
            current_state = iswalpha(in); //checking whether is it a word
            word_column = column-1;
            word_line = in == L'\n' ? line-1 : line;
        }
        if((iswalpha(in) != 0) != current_state) //state changed, ending..
        {
            ungetwc(in, stdin);
            column--;
            if(in == L'\n') //backing changes made by after getwc
                line--;
            dest[it] = L'\0';
            *is_word = current_state;
            return true; //alles gut
        }
        else //same state, reading farther
            dest[it++] = in;
    }
}

/**
 * @brief main Parses arguments, checks text correctess.
 * @param argc Argument count. If less than 2, program terminates.
 * @param argv Arguments array. Possible arguments in detailed description.
 * @return Zero.
 * Arguments:<br>
 * -v - Switch on showing hints.
 * -[path_to_file] - Indicates path to dictionary file. This argument is mandatory.
 */
int main(int argc, char** argv)
{
    setlocale(LC_ALL, "pl_PL.UTF-8");
    char* dict_name;
    bool hints;
    if(argc < 2)
    {
        wprintf(L"What about dictionary..? Ending\n");
        return 1;
    }

    else
    {
        if(argv[1][0] == '-' && argv[1][1] == 'v')
        {
            hints = true;
            if(argc != 3)
                return 1;
            dict_name = argv[2];
        }
        else
        {
            dict_name = argv[1];
            if(argc == 3 && argv[2][0] == '-' && argv[2][1] == 'v') hints = true;
        }

    }
    FILE* file = fopen(dict_name, "r");
    dict = dictionary_load(file);
    if(dict == NULL)
    {
        fwprintf(stderr, L"Cannot load dictionary, ending..\n");
        exit(EXIT_FAILURE);
    }
    wchar_t word[SINGLE_WORD_MAX_LENGTH];
    bool is_word;
    while(read_it(word, &is_word))
    {
        if(is_word)
        {
            if(dictionary_find(dict, word) != DICTIONARY_WORD_FOUND)
            {
                wprintf(L"#");
                if(hints)
                {
                    struct word_list list;
                    dictionary_hints(dict, word, &list);
                    wchar_t** hints_tab = word_list_get(&list);
                    int hlen = word_list_size(&list);
                    fwprintf(stderr, L"%d,%d %ls: ", word_line, word_column, word);
                    for(int i = 0; i < hlen; i++)
                        fwprintf(stderr, L"%ls ", hints_tab[i]);

                    fwprintf(stderr, L"\n");
                }
            }
        }
        wprintf(L"%ls", word);
    }
    wprintf(L"%ls", word);
}
