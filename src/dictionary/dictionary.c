/** @file
  Implementacja słownika oparta na drzewie trie

  @ingroup dictionary
  @author Jakub Pawlewicz <pan@mimuw.edu.pl>
  @author Piotr Rybicki <pr360957@students.mimuw.edu.pl>
  @copyright Uniwerstet Warszawski
  @todo Poprawić save, load.
 */

#include "dictionary.h"
#include "trie.h"
#include "../conf.h"
#include "signCounter.h"
#include <stdio.h>
#include <stdlib.h>

//niniejsze makrodefinicje sa wymuszone przez dictionary.
#define DICT_WORD_INSERTED 1
#define DICT_WORD_NOT_INSERTED 0

#define DICT_WORD_DELETED 1
#define DICT_WORD_NOT_DELETED 0

#define DICT_WORD_FOUND 1
#define DICT_WORD_NOT_FOUND 0

#define DICT_SAVE_SUCCESS 0
#define DICT_SAVE_ERROR (-1)

//a te sa uzywane przeze mnie:
#define DICT_ERROR (-1)
#define DICT_PRINT_ERRORS

struct dictionary
{
    Node* trie_root;
    SignCounter* alphabet;
};

static void _error(int line, const char* func)
{
    fprintf(stderr, "Error at line %d (module trie) in function %s()\n", line, func);
}

#ifdef DICT_PRINT_ERRORS
#define error() {_error(__LINE__, __func__); return DICT_ERROR;}
#define memError() {_error(__LINE__, __func__);return NULL;}
#else
#define error()
#define memError()
#endif // DICT_PRINT_ERRORS


/**@}*/
/** @name Elementy interfejsu
  @{
 */
struct dictionary* dictionary_new()
{
    struct dictionary* dict = malloc(sizeof(struct dictionary));
    if(dict == NULL)
        memError();

    dict->trie_root = trieNewNode();
    if(dict->trie_root == NULL)
    {
        free(dict);
        memError();
    }

    dict->alphabet = newSignCounter();
    if(dict->alphabet == NULL)
    {
        trieDeleteTrie(dict->trie_root); //po uzyciu tej funkcji nie ma potrzeby zwalniac trie_root'a
        free(dict);
        memError();
    }
    return dict;
}

void dictionary_done(struct dictionary *dict)
{
    trieDeleteTrie(dict->trie_root);
    deleteSignCounter(dict->alphabet);
    free(dict);
}

int dictionary_insert(struct dictionary *dict, const wchar_t *word)
{
    int len = wcslen(word);
    for(int i = 0; i < wcslen; i++)
        incrementSign(dict->alphabet, word[i]);
    if(trieInsertWord(dict->trie_root, word) == TRIE_INSERT_SUCCESS_MODIFIED)
        return DICT_WORD_INSERTED;
    else
        return DICT_WORD_NOT_INSERTED;
}

int dictionary_delete(struct dictionary *dict, const wchar_t *word)
{
    int len = wcslen(word);
    for(int i = 0; i < wcslen; i++)
        decrementSign(dict->alphabet, word[i]);

    if(trieDeleteWord(dict->trie_root, word) == TRIE_WORD_DELETED)
        return DICT_WORD_DELETED;
    else
        return DICT_WORD_NOT_DELETED;
}

bool dictionary_find(const struct dictionary *dict, const wchar_t* word)
{
    if(trieFindWord(dict->trie_root, word) == TRIE_WORD_FOUND)
        return DICT_WORD_FOUND;
    else
        return DICT_WORD_NOT_FOUND;
}

int dictionary_save(const struct dictionary *dict, FILE* stream)
{
    if(trieSaveToFile(dict->trie_root, stream) != TRIE_SUCCESS)
        return DICT_SAVE_ERROR;

    if(signCounterSaveToFile(dict->alphabet, stream) != SC_SUCCESS)
        return DICT_SAVE_ERROR;

    return DICT_SAVE_SUCCESS
}

struct dictionary* dictionary_load(FILE* stream)
{
    struct dictionary* ret = malloc(sizeof(struct dictionary));
    if(ret == NULL) memError();
    ret->trie_root = trieLoadFromFile(stream);
    if(ret->trie_root == NULL)
    {
        free(ret);
        memError();
    }
    ret->alphabet = signCounterLoadFromFile(stream);
    if(ret->alphabet == NULL)
    {
        trieDeleteTrie(ret->trie_root);
        free(ret)
        memError();
    }
    return ret;
}

void dictionary_hints(const struct dictionary *dict, const wchar_t* word,
        struct word_list *list)
{

}

/**@}*/
