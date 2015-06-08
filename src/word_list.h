/** @file
    Interfejs listy słów.
    <p>
    W pliku jest pewien subtelny problem. Mianowicie dopuszcza się
    tworzenie listy na stosie (w przeciwieństwie do wszystkich innych modułów),
    co sprawia, że nie word_list_done nie może zwalniać listy (bo free wywołane na stosie to UB)

    @ingroup dictionary
    @author Jakub Pawlewicz <pan@mimuw.edu.pl>
    @author Piotr Rybicki <pr360957@students.mimuw.edu.pl>
    @copyright Uniwerstet Warszawski
 */

#ifndef __WORD_LIST_H__
#define __WORD_LIST_H__
#include <wchar.h>

//definicje RETURN
#define WORD_LIST_ERROR (-1)
#define WORD_LIST_ADDED (1)

/**
 Pomocnicza struktura reprezentująca pojedynczy węzeł w liście.
 */
struct word_node
{
    wchar_t* word;
    struct word_node* next;
};

/**
  Jednokierunkowa lista przechowująca słowa.
  Należy używać funkcji operujących na strukturze,
  gdyż jej implementacja może się zmienić.
  */
struct word_list
{
    size_t word_count;
    struct word_node* first;
    struct word_node* last;
};

void word_list_init(struct word_list* list);

void word_list_done(struct word_list* list);

int word_list_add(struct word_list* list, const wchar_t* word);

size_t word_list_size(const struct word_list* list);

wchar_t** word_list_get(const struct word_list* list);

#endif /* __WORD_LIST_H__ */