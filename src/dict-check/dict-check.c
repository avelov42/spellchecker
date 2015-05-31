//null

#include "../dictionary/word_list.h"
#include <assert.h>
#include <wchar.h>
#include <stdlib.h>
#include "../dictionary/word_list.h"
#include <string.h>
static wchar_t* insertLetter(wchar_t* word, wchar_t letter, int position)
{
    int wordlen = wcslen(word);
    wchar_t* ret = malloc(wordlen + 1); //1 na wstawiona litere, drugi na \0
    memcpy(ret, word, sizeof(wchar_t) * position);
    ret[position] = letter;
    memcpy(ret, &word[position+1], sizeof(wchar_t) * (wordlen-position));
    return ret;
}

int main(int argc, char** argv)
{

    wchar_t* word = L"lkj";
    wchar_t* alphabet = L"abc";
    struct word_list w;
    word_list_init(&w);

    int len = wcslen(word);
    int alphlen = wcslen(alphabet);
    for(int pos = 0; pos <= len; pos++)
    {
        for(int let = 0; let < alphlen; let++)
            word_list_add(&w, insertLetter(word, alphabet[let], pos));
    }
    wchar_t** tab = word_list_get(&w);
    for(int i = 0; i < word_list_size(&w); i++)
        wprintf(L"%ls\n", tab[i]);
}


