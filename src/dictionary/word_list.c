/**@defgroup word_list Lista słów
* Lista słów bez ograniczeń.*/

/** @file
  Implementacja listy słów.

  @ingroup word_list
  @author Piotr Rybicki <pr360957@students.mimuw.edu.pl>
  @copyright Uniwerstet Warszawski
  @date 2015-05-10
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "word_list.h"

#define PRINT_ERRORS
#ifdef PRINT_ERRORS
static void _error(int line, const char* func)
{
    fprintf(stderr, "Error at line %d in function %s in module word_list\n", line, func);
}
#define error()    {_error(__LINE__, __func__);return WORD_LIST_ERROR;}
#define mem_err()  {_error(__LINE__, __func__);return NULL;}
#else
#define error() return WORD_LIST_ERROR
#define mem_err() return NULL
#endif // PRINT_ERRORS

/* FUNKCJE POMOCNICZE */

/**
 * @brief Tworzy węzeł, kopiuje do niego word.
 * @param[in] *word Wskaźnik na słowo do skopiowania do węzła.
 * @return Zwraca wskaźnik na nowy węzeł.
 * <p>
 * <strong> Ustawia pole next na NULL </strong>
 */
static struct word_node* new_node(const wchar_t* word)
{
    assert(word != NULL);
    int wlen = wcslen(word) + 1; //1 na \0

    struct word_node* ret = malloc(sizeof(struct word_node));
    if(ret == NULL) mem_err();

    ret->next = NULL;
    ret->word = malloc(sizeof(wchar_t) * wlen);
    if(ret->word == NULL) mem_err();

    memcpy(ret->word, word, sizeof(wchar_t) * wlen);

    assert(ret->word[wlen-1] == 0); //ostatni znak jest zerem
    return ret;
}

/**
 * @brief Dealokuje pojedynczy węzeł listy.
 * @param node - węzeł do dealokacji
 */
static void delete_word_node(struct word_node* node)
{
    assert(node != NULL);
    if(node->next != NULL)
        delete_word_node(node->next);
    free(node->word);
    free(node);
}

/** @brief Enkapsulacja funkcji porównującej słowa przy sortowaniu
 * @return -1 gdy *p1 < *p2, 0 gdy *p1 == *p2, 1 wpp
 */
static int wcscomparator(const void* p1, const void* p2)
{
    return wcscoll(*(const wchar_t**) p1, *(const wchar_t**) p2);
}

void word_list_init(struct word_list *list)
{
    assert(list != NULL);
    list->word_count = 0;
    list->first = NULL;
    list->last = NULL;
}

void word_list_done(struct word_list *list)
{
    assert(list != NULL);
    if(list->first != NULL)
        delete_word_node(list->first);
    list->first = NULL;
    list->last = NULL;
}

int word_list_add(struct word_list *list, const wchar_t *word)
{
    assert((list->first != NULL && list->last != NULL) || (list->first == NULL && list->last == NULL));
    assert(word != NULL);
    if(list->first == NULL)
    {

        list->first = new_node(word);
        if(list->first == NULL)
            error();

        list->last = list->first;
        list->word_count++;
        assert(word_list_size(list) == 1);
    }
    else //list->first != NULL
    {
        list->last->next = new_node(word);
        if(list->last->next == NULL)
            error();
        list->last = list->last->next;
        list->word_count++;
    }
    return WORD_LIST_ADDED;
}


wchar_t** word_list_get(const struct word_list* list)
{
    if(word_list_size(list) == 0)
        return NULL;

    wchar_t** ret = malloc(sizeof(wchar_t*) * word_list_size(list));
    if(ret == NULL) mem_err();
    struct word_node* current = list->first;
    int i = 0;
    while(current != NULL)
    {
        int wlen = wcslen(current->word) +1;
        ret[i] = malloc(sizeof(wchar_t) * wlen);
        if(ret[i] == NULL) mem_err();
        memcpy(ret[i], current->word, sizeof(wchar_t) * wlen);
        current = current->next;
        i++;
    }
    assert(i == word_list_size(list));
    qsort((void*) ret, i, sizeof(wchar_t*), wcscomparator);
    return ret;
}

size_t word_list_size(const struct word_list* list)
{
    return list->word_count;
}
