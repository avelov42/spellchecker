#ifndef TRIE_H_INCLUDED
#define TRIE_H_INCLUDED

/********************************************//**
 * @file trie.h Plik nagłówkowy.
 * @ingroup trie
 * @author Piotr Rybicki <pr360957@students.mimuw.edu.pl>
 ***********************************************/

#include <stdbool.h>
#include <wchar.h>

#define TRIE_DEBUG_FUNCTIONS

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

/********************************************//**
 * @brief Zwraca wskaźnik na nowe, świeże drzewko.
 * @return W przypadku sukcesu zwraca wskaźnik na drzewo, w p.p. NULL.
 * <p>
 * Funkcja służy zarówno do uzyskania drzewa z zewnątrz,
 * jak i do wewnętrznego tworzenia nowych wierzchołków.
 * <p>
 * <strong> Funkcja ustawia parent na NULL!</strong>
 ***********************************************/
Node* trieNewNode(void);

/********************************************//**
* @brief Podejmuje próbę wczytania drzewa z pliku.
*
* @param[in] *file Wskaźnik do otwartego pliku.
* @return Wskaźnik na root-a wczytanego drzewa. W przypadku niepowodzenia - NULL.
***********************************************/
Node* trieLoadFromFile(FILE* file);

/********************************************//**
 * @brief Wstawia słowo do drzewa. (iteracyjnie)
 *
 * @param[in,out] *root Korzeń drzewa, do którego ma być wstawione słowo.
 * @param[in] *word Słowo do wstawienia, również niepusty wskaźnik.
 * @return #TRIE_INSERT_SUCCESS_MODIFIED lub #TRIE_INSERT_SUCCES_NOT_MODIFIED lub #TRIE_ERROR
 *
 ***********************************************/
int trieInsertWord(Node* root, const wchar_t*  word);

/********************************************//**
 * @brief Usuwa słowo z drzewa.
 *
 * @param[in,out] *root Korzeń drzewa.
 * @param[in] *word Słowo do usunięcia.
 * @return #TRIE_WORD_DELETED lub #TRIE_WORD_NOT_DELETED lub #TRIE_ERROR
  ***********************************************/
int trieDeleteWord(Node* root, const wchar_t*  word);

/********************************************//**
 * @brief Sprawdza, czy dane słowo znajduje się w drzewie.
 *
 * @param[in] *root Korzeń drzewa, w którym będzie szukane słowo.
 * @param[in] *word Wskaźnik na szukane słowo.
 * @return #TRIE_WORD_FOUND lub #TRIE_WORD_NOT_FOUND
 ***********************************************/
int trieFindWord(const Node* root, const wchar_t* word);

/********************************************//**
 * @brief Zapisuje drzewo do pliku.
 *
 * @param[in] *root - Drzewo do zapisania.
 * @param[in] *file - wskaźnik do strumienia docelowego
 * @return #TRIE_SUCCESS. Ta funkcja nie zwraca komunikatu o błędach.
 ***********************************************/
int trieSaveToFile(const Node* root, FILE* file);

/********************************************//**
 * @brief Usuwa drzewo rekurencyjnie (lub poddrzewo).
 * @param[in] *node Wskaźnik na drzewo, które ma zostać usunięte.
 * @return #TRIE_SUCCESS
 * <p>
 * <strong> Po użyciu należy pamiętać o wyzerowaniu wskaźnika, który był argumentem! </strong>
 ***********************************************/
int trieDeleteTrie(Node* root);

#ifdef TRIE_DEBUG_FUNCTIONS
/********************************************//**
 * @brief Wyświetla podane drzewo w konsoli.
 *
 * @param[in] *root Drzewo do wyświetlenia
 *
 ***********************************************/
void triePrint(const Node* node);

/********************************************//**
 * @brief Sprawdza poprawność drzewa.
 *
 * @param[in] *root Korzeń drzewa.
 * @param[out] *checksum Wskaźnik na int, gdzie zostanie zapisana suma kontrolna.
 * @param[in] isRoot - czy sprawdzamy od root'a
 * @param[in] countArraySize - flaga, czy do liczby wierzchołków są włączane puste liście
 * @return Jeżeli drzewo jest poprawne - zwraca liczbę wierzchołków w drzewie.
 *
 * Dodatkowo oblicza bardzo prymitywną check-sumę.
 ***********************************************/
int trieVerifyTrie(const Node* node, int* checksum, bool isRoot, bool countArraySize);
#endif //NDEBUG


#endif // TRIE_H_INCLUDED
