/** @file
  Dictionary implementation.
  @ingroup dictionary
  @author Piotr Rybicki
  @date 2015-08
 */


#include "../conf.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wctype.h>
#include <string.h>
#include <dirent.h>
#include <wchar.h>
#include "trie.h"

#include "error_handling.h"
#include "dictionary.h"

#define _GNU_SOURCE


#ifdef DICTIONARY_UNIT_TESTING
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

#define static

#ifdef assert
#undef assert
#define assert(expression) mock_assert((int)(expression), #expression, __FILE__, __LINE__);
#endif

#ifdef fgetwc
#undef fgetwc
#endif //fgetwc
#define fgetwc testing_fgetwc

#ifdef fputwc
#undef fputwc
#endif //fputwc
#define fputwc testing_fputwc

extern wchar_t testing_fputwc (wchar_t sign, FILE * stream);
extern wchar_t testing_fgetwc (FILE *stream);


#endif //DICTIONARY_UNIT_TESTING


///Comparison function to create set containing letters in alphabet.
static int wchar_t_cmp(void* a, void* b)
{
    wchar_t* x = a;
    wchar_t* y = b;
    if(*x < *y) return -1;
    if(*x > *y) return 1;
    return 0;
}

///Wrapper function for free to dispose wchars from alphabet, crucial for tests with cmocka.
static void free_letter(void* ptr)
{
    free(ptr);
}

///Package of functions to compare and dispose letters in alphabet.
static Set_Functions alphabet_set_functions = {.cmp = wchar_t_cmp, .dispose = free_letter};

/**
 * @brief dict_non_null Tests whether dict and it's ingredients are non-null.
 * @param dict Dictionary to check.
 * @return True if everything is OK!
 */
static bool dict_non_null(const Dictionary* dict)
{
    return dict != NULL && dict->trie_root != NULL && dict->alphabet != NULL;
}

/**
 * @brief word_valid Tests if given word is non-null and non-zero-lenght.
 * @param word Word to test.
 * @return True if these conditions are met.
 */
static bool word_valid(const wchar_t* word)
{
    return word != NULL && wcslen(word) > 0;
}

/**
 * @brief update_alphabet Ensures that every letter in word is in alphabet of given dictionary.
 * @param dict The dictionary.
 * @param word The word.
 * O(n(logk)), where n - length of word, k - size of the alphabet.
 */
static void update_alphabet(Dictionary* dict, const wchar_t* word)
{
    wchar_t* wch;
    for(int i = 0; word[i] != 0; i++)
    {
        wch = malloc(sizeof(wchar_t));
        *wch = word[i];
        if(wch == NULL) report_error(MEMORY);
        if(!set_add(dict->alphabet, wch))//if letter belong to alphabet, then nothing happens
            free(wch);
    }
    return;
}

/**
 * @brief new_low_wstring Generates new, lower-cased version of word.
 * @param word Original word.
 * @return Pointer to lower-case version of given word.
 */
static wchar_t* new_low_wstring(const wchar_t* word)
{
    int len = wcslen(word);
    wchar_t* low_word = malloc(sizeof(wchar_t) * (len+1));
    if(low_word == NULL) report_error(MEMORY);
    for(int i = 0; i <= len; i++)
        low_word[i] = (wchar_t) towlower((wint_t) word[i]);
    low_word[len] = L'\0';
    return low_word;
}

/**
 * @brief save_alphabet_to_file Saves alphabet of given dict to given file.
 * @param dict The dictionary.
 * @param file The file.
 */
static void save_alphabet_to_file(const Dictionary* dict, FILE* file)
{
    for(int i = 0; i < dict->alphabet->element_count; i++)
        fputwc(*(wchar_t*)dict->alphabet->storage[i], file);
    return;
}

/**
 * @brief load_alphabet_from_file Creates empty set and copies into it stored in file letters.
 * @param file File to load from.
 * @return An Array_Set containing loaded letters from file.
 */
static Array_Set* load_alphabet_from_file(FILE* file)
{
    Array_Set* ret = set_new(&alphabet_set_functions);
    wchar_t* alloc = malloc(sizeof(wchar_t));
    if(alloc == NULL) report_error(MEMORY);
    while((*alloc = fgetwc(file)) != WEOF)
    {
        set_add(ret, alloc);
        alloc = malloc(sizeof(wchar_t));
        if(alloc == NULL) report_error(MEMORY);
    }
    free(alloc);
    return ret;
}

// Interface

Dictionary* dictionary_new()
{
    Dictionary* ret = malloc(sizeof(Dictionary));
    if(ret == NULL) report_error(MEMORY);
    ret->alphabet = set_new(&alphabet_set_functions);
    ret->trie_root = trie_new_node();
    return ret;
}

void dictionary_done(struct dictionary *dict)
{
    assert(dict_non_null(dict));

    set_free(dict->alphabet);
    trie_free_node(dict->trie_root);
    free(dict);
    return;
}

int dictionary_insert(struct dictionary *dict, const wchar_t *word)
{
    assert(dict_non_null(dict));
    assert(word_valid(word));

    if(!dict_non_null(dict) || !word_valid(word)) return TRIE_INSERT_NOT_MODIFIED;

    wchar_t* low_word = new_low_wstring(word);

    update_alphabet(dict, low_word);
    int ret = trie_insert_word(dict->trie_root, low_word) == TRIE_INSERT_MODIFIED ? DICTIONARY_INSERT_MODIFIED : DICTIONARY_INSERT_NOT_MODIFIED;
    free(low_word);
    return ret;
}

int dictionary_delete(struct dictionary *dict, const wchar_t *word)
{
    assert(dict_non_null(dict));
    assert(word_valid(word));

    if(!dict_non_null(dict) || !word_valid(word)) return TRIE_WORD_NOT_DELETED;

    wchar_t* low_word = new_low_wstring(word);

    int ret = trie_delete_word(dict->trie_root, low_word) == TRIE_WORD_DELETED ? DICTIONARY_WORD_DELETED : DICTIONARY_WORD_NOT_DELETED;
    free(low_word);
    return ret;
}

bool dictionary_find(const struct dictionary *dict, const wchar_t* word)
{
    assert(dict_non_null(dict));
    assert(word_valid(word));

    if(!dict_non_null(dict) || !word_valid(word)) return TRIE_WORD_NOT_FOUND;

    wchar_t* low_word = new_low_wstring(word);

    int ret = trie_find_word(dict->trie_root, low_word) == TRIE_WORD_FOUND ? DICTIONARY_WORD_FOUND : DICTIONARY_WORD_NOT_FOUND;
    free(low_word);
    return ret;
}

int dictionary_save(const struct dictionary *dict, FILE* file)
{
    trie_save_to_file(dict->trie_root, file);
    save_alphabet_to_file(dict, file);
    return DICTIONARY_SAVE_SUCCESS;
}

Dictionary* dictionary_load(FILE* file)
{
    Dictionary* ret = malloc(sizeof(Dictionary));
    if(ret == NULL) report_error(MEMORY);
    ret->trie_root = trie_load_from_file(file);
    ret->alphabet = load_alphabet_from_file(file);
    return ret;
}

/**
 * @brief replace_letter Creates new word by exchanging one letter with another.
 * @param word Model word.
 * @param pos Position in word to be replaced.
 * @param letter Letter, which will be substituted at pos in word.
 * @param word_len Length of the word.
 * @return Pointer to a new word, indepenedent from model word.
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
 * @brief insert_letter Creates a new word by inserting 'letter' onto position 'pos' and moving the rest of letters to right.
 * @param word Model word.
 * @param pos Position, where to insert letter.
 * @param letter Letter which will be inserted.
 * @param word_len Length of model word.
 * @return Pointer to a new word, independent from model word.
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
 * @brief remove_letter Creates new word by removal one of the letters.
 * @param word Model word.
 * @param pos Position of letter to be removed.
 * @param word_len Length of the model word.
 * @return Pointer to created word.
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

#ifndef NDEBUG
/**
 * @brief dictionary_print Prints trie and alphabet of given dict.
 * @param dict Dictionary to be printed.
 */
static void dictionary_print(Dictionary* dict)
{
    trie_print(dict->trie_root);
    for(int i = 0; i < dict->alphabet->element_count; i++)
        printf("[%d]", *((int*) (dict->alphabet->storage[i])));
}
#endif

void dictionary_hints(const struct dictionary *dict, const wchar_t* word,
                      struct word_list *list)
{
    assert(dict_non_null(dict));
    assert(word_valid(word));
    assert(list != NULL);

    if(!dict_non_null(dict) || !word_valid(word) || list == NULL) return;

    word_list_init(list);
    int len = wcslen(word);
    wchar_t* low_word = new_low_wstring(word);

    if(dictionary_find(dict, low_word) == DICTIONARY_WORD_FOUND)
        word_list_add(list, word);

    for(int w = 0; w < dict->alphabet->element_count; w++)
    {
        for(int i = 0; i < len; i++) //replace
        {
            if(low_word[i] == *(wchar_t*)(dict->alphabet->storage[w])) continue;
            wchar_t* replaced = replace_letter(low_word, i, *(wchar_t*)(dict->alphabet->storage[w]), len);
            if(dictionary_find(dict, replaced) == DICTIONARY_WORD_FOUND)
                word_list_add(list, replaced);
            free(replaced); //freeing, because list copies when adding.
        }
        for(int i = 0; i <= len; i++) //insert
        {
            wchar_t* inserted = insert_letter(low_word, i, *(wchar_t*)(dict->alphabet->storage[w]), len);
            if(dictionary_find(dict, inserted) == DICTIONARY_WORD_FOUND)
                word_list_add(list, inserted);
            free(inserted);
        }
    }

    if(len > 1)
    {
        for(int i = 0; i < len; i++) //remove
        {
            wchar_t* removed = remove_letter(low_word, i, len);
            if(dictionary_find(dict, removed) == DICTIONARY_WORD_FOUND)
                word_list_add(list, removed);
            free(removed);
        }
    }

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
    free(low_word);
}

int dictionary_lang_list(char** list, size_t *list_len)
{
    DIR* main_dir = opendir(CONF_PATH);
    if(main_dir == NULL) return -1;

    Word_List* buffer = word_list_new();
    struct dirent* current_element;
    *list_len = 0;
    while((current_element = readdir(main_dir)) != NULL)
    {
        if(current_element->d_type != DT_REG) //if not file, then skipping
            continue;

        int name_len = strlen(current_element->d_name);
        wchar_t* wname = malloc(sizeof(wchar_t) * (name_len + 1));
        if(wname == NULL) report_error(MEMORY);
        for(int i = 0; i <= name_len; i++) //copy including \0
            wname[i] = current_element->d_name[i];

        *list_len += name_len + 1;
        word_list_add(buffer, wname);
    }
    //for example list may contain
    //"pl_PL\0en_US\0"
    *list = malloc(sizeof(char) * (*list_len));
    if(*list == NULL) report_error(MEMORY);
    int pos_in_list = 0;
    int curr_word_len;
    struct word_node* current_node = buffer->first;
    while(current_node != NULL) //travel by all nodes in list
    {
        curr_word_len = wcslen(current_node->word);
        for(int i = 0; i <= curr_word_len; i++)
            (*list)[pos_in_list++] = current_node->word[i];
        current_node = current_node->next;
    }
    assert(pos_in_list == *list_len);
    return 0;
}

/**
 * @brief strcat3 Concatenate three strings.
 * @param str1 String one.
 * @param str2 String two.
 * @param str3 String three.
 * @return str1+str2+str3
 */
static char* strcat3(char* str1, char* str2, char* str3)
{
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    int len3 = strlen(str3);
    char* ret = malloc(sizeof(char) * (len1+len2+len3+1));
    if(ret == NULL) report_error(MEMORY);
    int global_pos = 0;
    for(int i = 0; i < len1; i++)
        ret[global_pos + i] = str1[i];
    global_pos += len1;
    for(int i = 0; i < len2; i++)
        ret[global_pos + i] = str2[i];
    global_pos += len2;
    for(int i = 0; i < len3; i++)
        ret[global_pos + i] = str3[i];
    global_pos += len3;
    ret[global_pos] = 0;
    return ret;

}

Dictionary* dictionary_load_lang(const char* lang)
{
    DIR* main_dir = opendir(CONF_PATH);
    if(main_dir == NULL)
        return NULL;

    struct dirent* current_element;

    while((current_element = readdir(main_dir)) != NULL)
    {
        if(current_element->d_type != DT_REG) //skipping not-files
            continue;
        if(strcmp(current_element->d_name, lang) == 0) //if name matches, load it
        {
            char* full_path = strcat3(CONF_PATH, "/", current_element->d_name);
            FILE* dict_file = fopen(full_path, "r");
            free(full_path);
            if(dict_file == NULL)
                return NULL;
            Dictionary* ret = dictionary_load(dict_file);
            fclose(dict_file);
            return ret;
        }
    }
    return NULL; //no such dictionary
}

int dictionary_save_lang(const Dictionary* dict, const char *lang)
{
    char* full_path = strcat3(CONF_PATH, "/", (char*) lang);
    FILE* dict_file = fopen(full_path, "w");
    free(full_path);
    if(dict_file == NULL)
        return -1;
    dictionary_save(dict, dict_file);
    fclose(dict_file);
    return 0;
}
