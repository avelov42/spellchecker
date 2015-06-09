/** @file
  Implementacja słownika oparta na drzewie trie

  @ingroup dictionary
  @author Jakub Pawlewicz <pan@mimuw.edu.pl>
  @author Piotr Rybicki <pr360957@students.mimuw.edu.pl>
  @copyright Uniwerstet Warszawski
 */

#include "dictionary.h"
#include "trie.h"
//#include "conf.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>
#include <wctype.h>
#include <string.h>

#define START_ALPHABET_SIZE 8 ///< Początkowy rozmiar tablicy przechowującej tablicę

/** @brief Strutkura przechowująca słownik */
struct dictionary
{
    Node* trie_root;
    int letters_count;
    int array_size;
    wchar_t* alphabet;
};

/** @brief Pomocnicza funkcja dla makra wyświetlającego błędy.
*/
static void _error(int line, const char* func)
{
    fprintf(stderr, "Error at line %d (module trie) in function %s()\n", line, func);
}

#ifdef DICT_PRINT_ERRORS
#define error() {_error(__LINE__, __func__); return DICT_ERROR;} ///< Makro służące do szybkiej obsługi błędów
#define memError() {_error(__LINE__, __func__);return NULL;} ///< Makro do obsługi błędów malloc'a
#else
#define error()
#define memError()
#endif // DICT_PRINT_ERRORS

/**
 * @brief Binary-search w alfabecie dict w poszukiwaniu sign.
 * @param[in] dict - słownik, w który odbywa się bs.
 * @param[in] sign - szukany znak
 * @return Pozycja w alfabecie, na którym znajduje się sign, jeżeli faktycznie tam jest.
 * Jeżeli w alfabecie dict nie ma znaku sign - zwracana jest pozycja, na której mógłby być.
 */
static int find_letter_in_alphabet(struct dictionary* dict, wchar_t sign)
{
    assert(dict != NULL);
    int l = 0;
    int r = dict->letters_count;
    int s;
    while(l < r)
    {
        s = (l+r)/2;
        if(dict->alphabet[s] < sign) l = s+1;
        else r = s;
    }
    assert(0 <= l && l <= dict->letters_count);
    return l;
}

/**
 * @brief Oblicza rozmiar dla tablicy przechowującej alfabet w słowniku.
 * @param[in] dict - słownik, dla którego zostaje obliczony rozmiar tablicy
 * @return Rozmiar tablicy, który gwarantuje pomieszczenie liczby dict->lettersCount znaków.
 */
static int array_size_function(struct dictionary* dict)
{
    assert(dict != NULL);
    assert(dict->array_size >= 0);
    assert(dict->letters_count >= 0);
    assert(dict->array_size+1 >= dict->letters_count);
    int new_size;
    if(dict->letters_count == dict->array_size+1) //pelna tablica, poszerzamy
        new_size = dict->array_size == 0 ? 1 : dict->array_size*2;
    else if(dict->letters_count*2+1 < dict->array_size)//miesci sie, sprobujmy usunac
        new_size = dict->array_size/2; //zmniejszamy
    else
        new_size = dict->array_size; //bez zmian
    assert(0 <= new_size && dict->letters_count <= new_size);
    return new_size;
}

/**
 * @brief Dodaje znak do alfabetu w słowniku.
 * @param[out] dict - słownik
 * @param[in] sign - znak do wstawienia.
 * @return True, jeżeli znak został wstawiony, false, jeżeli już się znajdował w alfabecie.
 **/
static bool add_letter_to_alphabet(struct dictionary* dict, wchar_t sign)
{
    assert(dict != NULL);
    int pos = find_letter_in_alphabet(dict, sign);
    if(dict->alphabet[pos] == sign)
    {
        return false;
    }
    else //dodac należy do alfabetu
    {
        dict->letters_count++;
        int new_size = array_size_function(dict);
        wchar_t* new_array = malloc(sizeof(wchar_t) * new_size);

        for(int i = 0; i < pos; i++) //kopia poprzednikow pos'a
            new_array[i] = dict->alphabet[i];

        new_array[pos] = sign;

        for(int i = pos; i < dict->letters_count-1; i++)
            new_array[i+1] = dict->alphabet[i]; //kopia nastepnikow pos'a

        for(int i = dict->letters_count; i < new_size; i++)
            new_array[i] = 0; //wynullowanie reszty tablicy

        free(dict->alphabet);
        dict->alphabet = new_array;
        dict->array_size = new_size;
        return true;
    }
}

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
    dict->alphabet = malloc(sizeof(wchar_t) * START_ALPHABET_SIZE);
    dict->array_size = START_ALPHABET_SIZE;
    dict->letters_count = 0;
    for(int i = 0; i < dict->array_size; i++)
        dict->alphabet[i] = 0;
    return dict;
}

void dictionary_done(struct dictionary *dict)
{
    trieDeleteTrie(dict->trie_root);
    free(dict->alphabet);
    free(dict);
}

int dictionary_insert(struct dictionary *dict, const wchar_t *word)
{
    int len = wcslen(word);
    wchar_t* lowWord = malloc(sizeof(wchar_t) * (1+len));
    for(int i = 0; i < len; i++)
    {
        lowWord[i] = towlower(word[i]);
        add_letter_to_alphabet(dict, lowWord[i]);
    }
    lowWord[len] = 0;

    if(trieInsertWord(dict->trie_root, lowWord) == TRIE_INSERT_SUCCESS_MODIFIED)
    {
        free(lowWord);
        return DICT_WORD_INSERTED;
    }
    else
    {
        free(lowWord);
        return DICT_WORD_NOT_INSERTED;
    }
}

int dictionary_delete(struct dictionary *dict, const wchar_t *word)
{
    int len = wcslen(word);
    wchar_t* lowWord = malloc(sizeof(wchar_t) * (1+len));
    for(int i = 0; i < len; i++)
    {
        lowWord[i] = towlower(word[i]);
        add_letter_to_alphabet(dict, lowWord[i]);
    }
    lowWord[len] = 0;

    if(trieDeleteWord(dict->trie_root, lowWord) == TRIE_WORD_DELETED)
    {
        free(lowWord);
        return DICT_WORD_DELETED;
    }

    else
    {
        free(lowWord);
        return DICT_WORD_NOT_DELETED;
    }
}

bool dictionary_find(const struct dictionary *dict, const wchar_t* word)
{
    int len = wcslen(word);
    wchar_t* lowWord = malloc(sizeof(wchar_t) * (1+len));
    for(int i = 0; i < len; i++)
    {
        lowWord[i] = towlower(word[i]);
        add_letter_to_alphabet((struct dictionary* )dict, lowWord[i]);
    }
    lowWord[len] = 0;

    if(trieFindWord(dict->trie_root, lowWord) == TRIE_WORD_FOUND)
    {
        free(lowWord);
        return DICT_WORD_FOUND;
    }
    else
    {
        free(lowWord);
        return DICT_WORD_NOT_FOUND;
    }

}

int dictionary_save(const struct dictionary *dict, FILE* file)
{
    //fwprintf(file, L"dictionary");
    trieSaveToFile(dict->trie_root, file);

    fwprintf(file, L"%d", dict->letters_count);
    for(int i = 0; i < dict->letters_count; i++)
        fwprintf(file, L"%lc", dict->alphabet[i]);

    return DICT_SAVE_SUCCESS;
}

struct dictionary* dictionary_load(FILE* file)
{
    //wchar_t pw[1024];
    //fwscanf(file, L"%ls", pw);
    //if(wcscmp(pw, L"dictionary"))
    //    return NULL;
    struct dictionary* dict = malloc(sizeof(struct dictionary));

    dict->trie_root = trieLoadFromFile(file);
    fwscanf(file, L"%d", &(dict->letters_count));
    free(dict->alphabet);
    dict->alphabet = malloc(sizeof(wchar_t) * dict->letters_count);
    for(int i = 0; i < dict->letters_count; i++)
        fwscanf(file, L"%lc", &(dict->alphabet[i]));
    dict->array_size = dict->letters_count;

    return dict;
}

/**
 * @brief Tworzy nowe słowo poprzez zamianę pewnego znaku na inny.
 * @param[in] word - słowo wejściowe
 * @param[in] pos - pozycja (znak), który będzie podmieniony
 * @param[in] letter - znak, który będzie wstawiony
 * @param[in] word_len - długość słowa word, bez '\0' (w celu zmniejszenia liczby obliczeń
 * @return Wskaźnik na nowe słowo, które należy zwolnić za pomocą free()
 */
static wchar_t* replace_letter(const wchar_t* word, int pos, wchar_t letter, int word_len)
{
    assert(word != NULL);
    assert(wcslen(word) == word_len);
    assert(0 <= pos && pos < word_len);
    assert(word_len > 0);


    wchar_t* ret = malloc(sizeof(wchar_t) * (word_len+1));
    memcpy(ret, word, sizeof(wchar_t) * word_len);
    ret[pos] = letter;
    ret[word_len] = L'\0';

    assert(wcslen(ret) == word_len);
    return ret;
}

/**
 * @brief Tworzy nowe słowo poprzez wstawienie letter na pos miejsce i przesunięcie elementów pos..word_len w prawo
 * @param[in] word - słowo wejściowe
 * @param[in] pos - pozycja, na którą będzie wstawiona litera letter
 * @param[in] letter - znak, który będzie wstawiony
 * @param[in] word_len - długość słowa word, bez '\0' (w celu zmniejszenia liczby obliczeń)
 * @return Wskaźnik na nowe słowo, które należy zwolnić za pomocą free().
 */
static wchar_t* insert_letter(const wchar_t* word, int pos, wchar_t letter, int word_len)
{
    assert(word != NULL);
    assert(wcslen(word) == word_len);
    assert(0 <= pos && pos <= word_len);
    assert(word_len > 0);

    wchar_t* ret = malloc(sizeof(wchar_t) * (word_len + 1 + 1)); //1 na '\0' i 1 na nowa litere
    memcpy(ret, word, sizeof(wchar_t) * pos);
    ret[pos] = letter;
    memcpy(&(ret[pos+1]), &word[pos], sizeof(wchar_t) * (word_len - pos));
    ret[word_len+1] = L'\0';

    assert(wcslen(ret) == word_len+1);
    return ret;
}

/**
 * @brief Tworzy nowe słowo poprzez usunięcie jednego ze znaków słowa.
 * @param[in] word - słowo wejściowe
 * @param[in] pos - pozycja znaku do usunięcia, musi być mniejsza od długości słowa; 0 oznacza 1. znak.
 * @param[in] word_len - długość słowa word, bez '\0' (w celu zmniejszenia liczby obliczeń)
 * @return Wskaźnik na nowe słowo, które należy zwolnić za pomocą free()
 */
static wchar_t* remove_letter(const wchar_t* word, int pos, int word_len)
{
    assert(word != NULL);
    assert(wcslen(word) == word_len);
    assert(0 <= pos && pos < word_len);

    wchar_t* ret = malloc(sizeof(wchar_t) * word_len); //-1 za litere i +1 za '\0'
    memcpy(ret, word, sizeof(wchar_t) * pos); //kopiuje pos pierwszych znakow
    memcpy(&(ret[pos]), &(word[pos+1]), sizeof(wchar_t) * (word_len - pos -1)); //do kolejnych ret-a kopiujemy kolejne word-a, pomijaja pos-ty
    ret[word_len-1] = L'\0';

    assert(wcslen(word) == word_len);
    return ret;
}

void dictionary_hints(const struct dictionary *dict, const wchar_t* word,
                      struct word_list *list)
{
    word_list_init(list);
    int len = wcslen(word);
    wchar_t* lowWord = malloc(sizeof(wchar_t) * (1+len));
    for(int i = 0; i < len; i++)
    {
        lowWord[i] = towlower(word[i]);
        add_letter_to_alphabet((struct dictionary*) dict, lowWord[i]);
    }
    lowWord[len] = 0;

    if(dictionary_find(dict, lowWord) == DICT_WORD_FOUND)
        word_list_add(list, word);

    for(int w = 0; w < dict->letters_count; w++)
    {
        for(int i = 0; i < len; i++) //replace
        {
            if(lowWord[i] == dict->alphabet[w]) continue;
            wchar_t* replaced = replace_letter(lowWord, i, dict->alphabet[w], len);
            if(dictionary_find(dict, replaced) == DICT_WORD_FOUND)
                word_list_add(list, replaced);
        }

        for(int i = 0; i <= len; i++) //insert
        {
            wchar_t* inserted = insert_letter(lowWord, i, dict->alphabet[w], len);
            if(dictionary_find(dict, inserted) == DICT_WORD_FOUND)
                word_list_add(list, inserted);
        }
    }

    if(len > 1)
    {
        for(int i = 0; i < len; i++) //remove
        {
            wchar_t* removed = remove_letter(lowWord, i, len);
            if(dictionary_find(dict, removed) == DICT_WORD_FOUND)
                word_list_add(list, removed);
        }
    }
    //zrobimy cos brzydkiego :)
    free(lowWord);
    if(word_list_size(list) == 0)
        return;

    wchar_t** tab = word_list_get(list);

    int list_len = word_list_size(list);
    word_list_done(list);
    word_list_init(list);

    word_list_add(list, tab[0]);
    for(int i = 1; i < list_len; i++)
        if(wcscmp(tab[i], tab[i-1]))
            word_list_add(list, tab[i]);

    for(int i=0; i < list_len; i++)
        free(tab[i]);
    free(tab);


}
