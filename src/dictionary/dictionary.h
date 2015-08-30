/** @defgroup dictionary Module dictionary
    Dictionary-library
  */
/** @file 
    Interface of dictionary library.
   
    @ingroup dictionary
    @author Jakub Pawlewicz <pan@mimuw.edu.pl>
    @author Piotr Rybicki
    @copyright Uniwersytet Warszawski
    @date 2015-05-10, 2015-08
 */

#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__


#include <stdbool.h>
#include <stdio.h>
#include <wchar.h>
#include "word_list.h"
#include "trie.h"

/**
  Struct containing dictionary.
  */
typedef struct dictionary
{
    Node* trie_root; ///<Root of prefix tree.
    Array_Set* alphabet; ///<Set of letters of which consists all words in trie, used in hints.
} Dictionary;

#define DICTIONARY_INSERT_MODIFIED 1 ///<Return value
#define DICTIONARY_INSERT_NOT_MODIFIED 0 ///<Return value
#define DICTIONARY_WORD_DELETED 1 ///<Return value
#define DICTIONARY_WORD_NOT_DELETED 0///<Return value
#define DICTIONARY_WORD_FOUND 1 ///<Return value
#define DICTIONARY_WORD_NOT_FOUND 0 ///<Return value
#define DICTIONARY_SAVE_SUCCESS 0 ///<Return value

/**
 * @brief dictionary_new Creation and initialization of a dictionary.
 * @return An empty, correct dictionary structure.
 */
Dictionary* dictionary_new(void);

/**
 * @brief dictionary_done Destruction of the dictionary.
 * @param dict Dictionary to deallocate
 */
void dictionary_done(struct dictionary *dict);

/**
 * @brief dictionary_insert Inserts given word to dictionary.
 * @param dict Dictionary to insert word in.
 * @param word Word to be inserted
 * @return 0 if word was in dictionary, 1 otherwise.
 */
int dictionary_insert(struct dictionary *dict, const wchar_t* word);

/**
  Usuwa podane słowo ze słownika, jeśli istnieje.
  @param[in,out] dict Słownik.
  @param[in] word Słowo, które należy usunąć ze słownika.
  @return 1 jeśli udało się usunąć, zero jeśli nie.
  */

/**
 * @brief dictionary_delete Removes word from dictionary, if it exists.
 * @param dict Dictionary
 * @param word Word to be deleted
 * @return 0 if word was not present in dict, 1 otherwise.
 */
int dictionary_delete(struct dictionary *dict, const wchar_t* word);

/**
 * @brief dictionary_find Tests existence of given word in dictionary.
 * @param dict Dicionary
 * @param word Searchee
 * @return  True if dict contains word.
 */
bool dictionary_find(const struct dictionary *dict, const wchar_t* word);


/**
 * @brief dictionary_save Saves the dictionary.
 * @param dict Dictionary to save.
 * @param stream Stream where dictionary will be saved.
 * @return 0 if succeeded, <0 otherwise.
 */
int dictionary_save(const struct dictionary *dict, FILE* stream);

/**
 * @brief dictionary_load Creates and loads dictionary from a stream.
 * @param stream Stream to load from.
 * @return A dictionary loaded from stream.
 */
Dictionary* dictionary_load(FILE* stream);

/**
 * @brief dictionary_hints Generates a list of hints for given word according to dict content.
 * @param dict Dictionary upon which hints will be generated.
 * @param word Word to give hints of.
 * @param list Container for generated hints.
 */
void dictionary_hints(const struct dictionary *dict, const wchar_t* word,
                      struct word_list *list);


/**
 * @brief dictionary_lang_list Returns list of available dictionaries.
 * @param list Pointer to pointer which points onto begining of the list.
 * @param list_len Integer pointer to store list's length.
 * @return <0 if failed, 0 otherwise.
 *
 * Returns names of languages, for which dictionaries are available.
 * F.E.: pl_PL, en_US (locale names).
 * Representation of the languages list is similiar to chars strings lists [argz in glibc].
 * Pointer 'list' points onto begining of the buffer in which strings representing languages are stored continously, separated by one \0 sign.
 * If list is non-empty, then also the whole list is ended with \0.
 */
int dictionary_lang_list(char **list, size_t *list_len);


/**
  Inicjuje i wczytuje słownik dla zadanego języka.
  Słownik ten należy zniszczyć za pomocą dictionary_done().
  @param[in] lang Nazwa języka, patrz dictionary_lang_list().
  @return Słownik dla danego języka lub NULL, jeśli operacja się nie powiedzie.
  */

/**
 * @brief dictionary_load_lang Loads dictionary for given language.
 * @param lang Name of language, see dictionary_lang_list().
 * @return Pointer to a dictionary for given language or null, if operation fail.
 * The loaded dictionary should be disposed by dictionary_done()
 */
struct dictionary * dictionary_load_lang(const char *lang);

/**
 * @brief dictionary_save_lang Saves dictionary for given language.
 * @param dict Dictionary
 * @param lang Name of the dictionary, @see dictionary_lang_list().
 * @return <0 if operation fails, 0 otherwise.
 */
int dictionary_save_lang(const Dictionary *dict, const char *lang);
    
#endif /* __DICTIONARY_H__ */
