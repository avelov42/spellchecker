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
#include <assert.h>
#include <wchar.h>

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

#define START_ALPHABET_SIZE 256

struct dictionary
{
    Node* trie_root;
    int letters;
    int asize;
    wchar_t* alphabet;
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
    /*
    dict->alphabet = newSignCounter();
    if(dict->alphabet == NULL)
    {
        trieDeleteTrie(dict->trie_root); //po uzyciu tej funkcji nie ma potrzeby zwalniac trie_root'a
        free(dict);
        memError();
    }
    */
    dict->alphabet = malloc(sizeof(wchar_t) * START_ALPHABET_SIZE);
    dict->asize = START_ALPHABET_SIZE;
    dict->letters = 0;
    return dict;
}

void dictionary_done(struct dictionary *dict)
{
    trieDeleteTrie(dict->trie_root);
    free(dict->alphabet);
    free(dict);
}

static bool findInAlphabet(struct dictionary* dict, wchar_t sign)
{
    for(int i = 0; i < dict->asize; i++)
        if(dict->alphabet[i] == sign)
            return true;
    return false;
}

static void appendLetterToAlphabet(struct dictionary* dict, wchar_t sign)
{
    if(dict->asize == dict->letters)
        dict->alphabet = realloc(dict->alphabet, sizeof(wchar_t) * 2*dict->asize);
    assert(dict->letters < dict->asize);

    dict->alphabet[dict->letters] = sign;
    dict->letters++;
}

int dictionary_insert(struct dictionary *dict, const wchar_t *word)
{
    int len = wcslen(word);
    for(int i = 0; i < len; i++)
        if(!findInAlphabet(dict, word[i]))
            appendLetterToAlphabet(dict, word[i]);
    //koszmarne rozwiazanie, ale brak czasu na truskawki

    if(trieInsertWord(dict->trie_root, word) == TRIE_INSERT_SUCCESS_MODIFIED)
        return DICT_WORD_INSERTED;
    else
        return DICT_WORD_NOT_INSERTED;
}

int dictionary_delete(struct dictionary *dict, const wchar_t *word)
{

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

    fwrite(&(dict->letters), sizeof(dict->letters), 1, stream);
    fwrite(dict->alphabet, sizeof(dict->alphabet[0]), dict->letters, stream);

    return DICT_SAVE_SUCCESS;
}

struct dictionary* dictionary_load(FILE* stream)
{
    struct dictionary* ret = malloc(sizeof(struct dictionary));

    ret->trie_root = trieLoadFromFile(stream);

    int size;
    fread(&size, sizeof(size), 1, stream);
    ret->alphabet = malloc(sizeof(wchar_t) * size);
    ret->asize = ret->letters = size;

    fread(ret->alphabet, sizeof(wchar_t), size, stream);

    return ret;
}

static wchar_t* insertLetter(wchar_t* word, wchar_t letter, int position)
{
    int wordlen = wcslen(word);
    wchar_t* ret = malloc(wordlen + 1); //1 na wstawiona litere, drugi na \0
    memcpy(ret, word, position);
    ret[position] = word;
    memcpy(ret, &word[position+1], wordlen-position);
}


void dictionary_hints(const struct dictionary *dict, const wchar_t* word,
        struct word_list *list)
{
    //generuj slowa przez replace, dodaj je do listy
    //generuj slowa przez insert, dodaj je do listy
    //generuj slowa przez delete, dodaj je do listy

}

/**@}*/
