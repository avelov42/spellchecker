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

/**
  Inicjuje istniejącą listę słów na pustą.
  <strong>
  W przypadku użycia na liście już istniejącej usuwa jej zawartość.
  </strong>
  @param[in,out] *list Lista słów.
  */
void word_list_init(struct word_list* list);

/**
  Zwalnia pamięć zajętą przez dodawanie słów, nie zwalnia samej pamięci.
  @param[in,out] *list Lista słów.
  */
void word_list_done(struct word_list* list);

/**
  Dodaje słowo do listy.
  @param[in,out] list Lista słów.
  @param[in] word Dodawane słowo.
  @return #WORD_LIST_ADDED lub #WORD_LIST_ERROR
  */
int word_list_add(struct word_list* list, const wchar_t* word);

/**
  Zwraca liczę słów w liście.
  @param[in] list Lista słów.
  @return Liczba słów w liście.
  */
static inline
size_t word_list_size(const struct word_list* list)
{
    return list->word_count;
}


/**
  Zwraca tablicę słów w liście.
  @param[in] list Lista słów.
  @return Tablica słów.
  */
const wchar_t* const* word_list_get(const struct word_list* list);

#endif /* __WORD_LIST_H__ */
