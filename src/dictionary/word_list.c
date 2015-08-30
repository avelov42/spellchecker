/** @file
  Implementation of word list.

  @ingroup word_list
  @author Piotr Rybicki <pr360957@students.mimuw.edu.pl>
  @date 2015-08
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "word_list.h"
#include "error_handling.h"

#ifdef WORD_LIST_UNIT_TESTING
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#ifdef malloc
#undef malloc
#endif // malloc
#define malloc(size) _test_malloc(size, __FILE__, __LINE__)

#ifdef calloc
#undef calloc
#endif // calloc
#define calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)

#ifdef free
#undef free
#endif // free
#define free(ptr) _test_free(ptr, __FILE__, __LINE__)

#ifdef assert
#undef assert
#define assert(expression) mock_assert((int)(expression), #expression, __FILE__, __LINE__);
#endif

#ifdef fread
#undef fread
#endif //fread
#define fread testing_fread

#ifdef fwrite
#undef fwrite
#endif //fwrite
#define fwrite testing_fwrite

#endif // WORD_LIST_UNIT_TESTING

/**
 * @brief new_node Creates node, copies gien word into them.
 * @param word Word which is used to init new node.
 * @return Word node with word inside.
 */
static struct word_node* new_node(const wchar_t* word)
{
    assert(word != NULL);
    int wlen = wcslen(word) + 1; //1 na \0

    struct word_node* ret = malloc(sizeof(struct word_node));
    if(ret == NULL) report_error(MEMORY);

    ret->next = NULL;
    ret->word = malloc(sizeof(wchar_t) * wlen);
    if(ret->word == NULL) report_error(MEMORY);

    memcpy(ret->word, word, sizeof(wchar_t) * wlen);

    assert(ret->word[wlen-1] == 0); //ostatni znak jest zerem
    return ret;
}

/**
 * @brief delete_word_node Deallocates single nodes and word within the node.
 * @param node Node to be deleted.
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

/**
 * @brief wcscomparator Comparison function between wchars, used in sorting.
 * @param p1 First letter.
 * @param p2 Second letter.
 * @return -1 when *p1 < *p2, 0 when *p1 == *p2, 1 otherwise.
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

Word_List* word_list_new(void)
{
    Word_List* ret = malloc(sizeof(Word_List));
    if(ret == NULL) report_error(MEMORY);
    word_list_init(ret);
    return ret;
}

void word_list_done(struct word_list *list)
{
    assert(list != NULL);
    if(list->first != NULL)
        delete_word_node(list->first);
    list->first = NULL;
    list->last = NULL;
}

void word_list_free(Word_List* list)
{
    word_list_done(list);
    free(list);
}

int word_list_add(struct word_list *list, const wchar_t *word)
{
    assert((list->first != NULL && list->last != NULL) || (list->first == NULL && list->last == NULL));
    assert(word != NULL);
    if(list->first == NULL)
    {

        list->first = new_node(word);

        list->last = list->first;
        list->word_count++;
        assert(word_list_size(list) == 1);
    }
    else //list->first != NULL
    {
        list->last->next = new_node(word);

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
    if(ret == NULL) report_error(MEMORY);
    struct word_node* current = list->first;
    int i = 0;
    while(current != NULL)
    {
        int wlen = wcslen(current->word) + 1;
        ret[i] = malloc(sizeof(wchar_t) * wlen);
        if(ret[i] == NULL) report_error(MEMORY);
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
