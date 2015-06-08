#ifndef TRIE_H_INCLUDED
#define TRIE_H_INCLUDED

/********************************************//**
 * @file Plik nagłówkowy dla modułu trie.
 * @ingroup trie
 * @author Piotr Rybicki <pr360957@students.mimuw.edu.pl>
 ***********************************************/

#include <stdbool.h>
#include <wchar.h>

#define TRIE_SUCCESS 42
#define TRIE_ERROR (-1) ///<Nie ważne jaki. #TRIE_PRINT_ERRORS wyświetli tę informację na stderr.

#define TRIE_INSERT_SUCCESS_MODIFIED 1
#define TRIE_INSERT_SUCCESS_NOT_MODIFIED 0

#define TRIE_WORD_FOUND 1
#define TRIE_WORD_NOT_FOUND 0

#define TRIE_WORD_DELETED 1
#define TRIE_WORD_NOT_DELETED 0

/** @brief Struktura reprezentująca pojedynczy wierzchołek drzewa trie.
 */
typedef struct dzikaKaczka
{
    wchar_t value;
    bool isWord;
    int arraySize;
    int childrenCount;

    struct dzikaKaczka* parent;
    struct dzikaKaczka** childrenArray;
} Node;

Node* trieNewNode(void);
Node* trieLoadFromFile(FILE* file);

int trieInsertWord(Node* root, const wchar_t*  word);
int trieDeleteWord(Node* root, const wchar_t*  word);
int trieFindWord(const Node* root, const wchar_t* word);
int trieSaveToFile(const Node* root, FILE* file);
int trieDeleteTrie(Node* root);

#ifdef TRIE_DEBUG_FUNTIONS
void triePrint(const Node* node);
int trieVerifyTrie(const Node* node, int* checksum, bool isRoot, bool countArraySize);
#endif //NDEBUG


#endif // TRIE_H_INCLUDED
