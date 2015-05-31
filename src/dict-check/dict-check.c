//null

#include "../dictionary/word_list.h"
#include <assert.h>
#include <wchar.h>
#include <stdlib.h>
int main(int argc, char** argv)
{
    struct word_list lista;
    word_list_init(&lista);

    wchar_t* tab[] = {L"jeden", L"dwa", L"trzy", L"cztery"};

    word_list_add(&lista, tab[0]);
    word_list_add(&lista, tab[1]);
    word_list_add(&lista, tab[2]);
    word_list_add(&lista, tab[3]);
    word_list_add(&lista, L"japko");
    wchar_t** o = word_list_get(&lista);


    wprintf(L"%ls\n%ls\n%ls\n%ls\n", o[0], o[1], o[2], o[3]);
    word_list_done(&lista);
}


