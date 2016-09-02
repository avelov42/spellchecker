#ifndef TRIE_H_INCLUDED
#define TRIE_H_INCLUDED

/** @defgroup trie Module Trie
 * Trie implementation.
 */
/**
 * @file trie.h Header file of module trie.
 * @ingroup trie
 * @author Piotr Rybicki <pr360957@students.mimuw.edu.pl>
 * @date 2015-08
 */

#include <stdbool.h>
#include <wchar.h>
#include "array_set.h"

#define TRIE_DEBUG_FUNCTIONS ///<Switch to compile some debug functions i.e. print_trie.

#define TRIE_INSERT_MODIFIED 1 ///<Value returned after successful insertion
#define TRIE_INSERT_NOT_MODIFIED 0 ///<Value returned when given word is already in trie.

#define TRIE_WORD_FOUND 1 ///<Value returned by trie_find_word when trie contains given word.
#define TRIE_WORD_NOT_FOUND 0 ///<Value returned by trie_find_word when trie does not contain given word.

#define TRIE_WORD_DELETED 1 ///<Value returned if given word was succesfully deleted.
#define TRIE_WORD_NOT_DELETED 0 ///<Value returned if given word was unpresent in trie.

#define TRIE_SAVE_TO_FILE_SUCCESS 1 ///<Value returned after sucessful saving trie to file

#define END_OF_NODE_SIGN L' ' ///<Value used to label in file an end of node, which is not a word.
#define END_OF_WORD_NODE_SIGN L'\t' ///<Value used to label in file an end of node, which represents a word.

/**
  * Structure representing single node
  */
typedef struct Node
{
    wchar_t value; ///<Sign represented by node.
    bool is_word; ///<Bool determining whether node represents a full word.

    struct Node* parent; ///<Pointer to parent node, useful when deleting node.
    Array_Set* children; ///<Pointer to Array_Set, used to store child-nodes.
} Node;

/**
 * @brief trie_new_node Creates and initializes node to be a root.
 * @return Root-like node.
 * Root node has 0-ed value, is_word == false, parent == NULL, initialized children set.
 */
Node* trie_new_node(void);

/**
 * @brief trie_load_from_file Loades trie from file.
 * @param file File to load from.
 * @return Pointer to root of loaded trie.
 */
Node* trie_load_from_file(FILE* file);

/**
 * @brief trie_insert_word Inserts word to given trie.
 * @param root Root of the trie.
 * @param word The word.
 * @return TRIE_INSERT_MODIFIED or TRIE_INSERT_NOT_MODIFIED
 */
int trie_insert_word(Node* root, const wchar_t*  word);

/**
 * @brief trie_delete_word Removes the word from the trie.
 * @param root Root of the trie.
 * @param word The word.
 * @return TRIE_WORD_DELETED or TRIE_WORD_NOT_DELETED, when word is not in trie
 */
int trie_delete_word(Node* root, const wchar_t*  word);

/**
 * @brief trie_find_word Tests if the word is in the trie.
 * @param root Root of the trie.
 * @param word The word.
 * @return TRIE_WORD_FOUND or TRIE_WORD_NOT_FOUND
 */
int trie_find_word(Node* root, const wchar_t* word);

/**
 * @brief trie_save_to_file Saves trie starting in root to file.
 * @param node Root of the trie to be saved.
 * @param file File to save in the trie.
 * @return TRIE_SAVE_TO_FILE_SUCCESS, or crashes ^^
 */
int trie_save_to_file(Node* node, FILE* file);

/**
 * @brief trie_free_node Deallocates trie recursively.
 * @param root Root to be freed.
 */
void trie_free_node(void* root);

#ifndef NDEBUG
/**
 * @brief trie_print Prints the trie starting in node in console.
 * @param node The node!
 */
void trie_print(Node* node);
#endif //NDEBUG

#ifdef UNIT_TESTING
/**
 * @brief trie_verify Checks few conditions of correct trie.
 * @param node Trie to be verified.
 * @param is_root Logical parameter determining treating given node as a root or not.
 * @return True, if whole trie is correct, false otherwise.
 */
bool trie_verify(const Node* node, bool is_root);
#endif //UNIT_TESTING

#endif // TRIE_H_INCLUDED
