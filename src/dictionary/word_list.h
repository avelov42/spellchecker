/** @defgroup word_list Module word_list
* Another implementation of word_list*/

/** @file
    Interface of word list.
    <p>
    Due to back compability, there is still possibility to create a word_list on stack and then
    initialise. In newer uses, word_list_new should be used instead.
    @ingroup word_list
    @author Jakub Pawlewicz <pan@mimuw.edu.pl>
    @author Piotr Rybicki <pr360957@students.mimuw.edu.pl>
    @copyright Uniwerstet Warszawski
 */

#ifndef __WORD_LIST_H__
#define __WORD_LIST_H__
#include <wchar.h>

//definicje RETURN
#define WORD_LIST_ERROR (-1) ///<Return value
#define WORD_LIST_ADDED (1) ///<Return value

/**
 * Structure representing single node with word in directed list.
 */
struct word_node
{
    wchar_t* word; ///<Pointer to stored word.
    struct word_node* next;///<Pointer to next node in list.
};

/**
  Dicrected list for storing words.
  One should use functions operating on structure
  because implementation may change in time.
  */
typedef struct word_list
{
    size_t word_count; ///<Number of words in list.
    struct word_node* first; ///<Pointer to first word node.
    struct word_node* last; ///<Pointer to last word node, for fast adding.
} Word_List;

/**
  Inicjuje istniejącą listę słów na pustą.
  @param[in,out] *list Lista słów.
  */
/**
 * @brief word_list_init Initializes existing list.
 * @param list List to init.
 * If performed on existing list, lack will occur.
 */
void word_list_init(struct word_list* list);

/**
 * @brief word_list_new Creates and initialises a new word_list.
 * @return Pointer to new, ready-to-use word list structure.
 */
Word_List* word_list_new(void);

/**
  Zwalnia pamięć zajętą przez dodawanie słów, nie zwalnia samej pamięci.
  @param[in,out] *list Lista słów.
  */

/**
 * @brief word_list_done Deallocates memory used by words (and nodes), but not the list itself.
 * @param list List to be 'cleared'.
 * For total deallocation use word_list_free.
 */
void word_list_done(struct word_list* list);

/**
 * @brief word_list_free Fully deallocates any memory used by list.
 * @param list List to be freed.
 */
void word_list_free(Word_List* list);


/**
 * @brief word_list_add Adds word to list.
 * @param list List.
 * @param word Word.
 * @return WORD_LIST_ADDED
 */
int word_list_add(struct word_list* list, const wchar_t* word);

/**
  Zwraca liczę słów w liście.
  @param[in] list Lista słów.
  @return Liczba słów w liście.
  */

/**
 * @brief word_list_size Returns number of words in list.
 * @param list List.
 * @return Word list size.
 */
size_t word_list_size(const struct word_list* list);


/**
 * @brief word_list_get Returns sorted array of copied words contained in given word_list.
 * @param list List to copy from.
 * @return Array of wide strings, independent of words in list.
 */
wchar_t** word_list_get(const struct word_list* list);

#endif /* __WORD_LIST_H__ */
