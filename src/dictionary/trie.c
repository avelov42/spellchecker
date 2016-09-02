/** @file
    Implementation of trie.
    @ingroup trie
    @author Piotr Rybicki
    @date 2015-08
  */

#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>

#include "trie.h"
#include "array_set.h"
#include "error_handling.h"

#ifdef TRIE_UNIT_TESTING
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

#endif // TRIE_UNIT_TESTING

void trie_free_node(void* node)
{
    assert(node != NULL);
    set_free(((Node*)node)->children);
    free(node);
} //ok?

///Node comparison function, compares node->value using < operator.
static int cmp_node(void* a, void* b)
{
    Node* x = a;
    Node* y = b;
    if(x->value < y->value) return -1;
    if(x->value > y->value) return 1;
    return 0;
}

///Package of functions to operate on nodes, which is delegated to set when created.
static Set_Functions node_set_functions = {.cmp = cmp_node, .dispose = trie_free_node};

///Helper function used mainly in assertions. Also defines a necessary and sufficient condition for node to be null.
inline static bool is_root(Node* node)
{
    return node->value == 0;
}

Node* trie_new_node(void)
{
    Node* ret = malloc(sizeof(Node));
    if(ret == NULL) report_error(MEMORY);
    memset(ret, 0, sizeof(Node));
    ret->children = set_new(&node_set_functions);
    //set_new always returns valid or reports_error
    return ret;
}

int trie_insert_word(Node* root, const wchar_t* word)
{
    assert(root != NULL);
    assert(is_root(root));
    assert(word != NULL);

    Node* current_node = root;
    Node* current_letter_node;
    Node* found_child;
    int len = wcslen(word);
    bool modified = false;
    assert(len > 0);

    for(int i = 0; i < len; i++)
    {
        current_letter_node = trie_new_node();
        current_letter_node->value = word[i];
        current_letter_node->parent = current_node;
        current_letter_node->is_word = (i == len-1);

        found_child = set_find(current_node->children, current_letter_node);
        if(found_child == NULL) //no such letter
        {
            modified = true;
            set_add(current_node->children, current_letter_node);
            current_node = current_letter_node;
            //do not free current_letter_node, it's become a part of trie
        }
        else //found proper child
        {
            modified = !found_child->is_word;
            found_child->is_word = found_child->is_word || current_letter_node->is_word;
            current_node = found_child;
            trie_free_node(current_letter_node); //c_l_n will be not used
            //optimization left for volunteers
        }

        if(i == len-1)
            return modified ? TRIE_INSERT_MODIFIED : TRIE_INSERT_NOT_MODIFIED;
    }
    return 42;
}

/**
 * @brief jump_to_word_node Finds node which represents given word.
 * @param root The trie.
 * @param word The word.
 * @return Pointer to node which represents given word in given trie, or null, if there is no such word in the trie.
 */
static Node* jump_to_word_node(Node* root, const wchar_t* word)
{
    assert(root != NULL);
    assert(is_root(root));
    assert(word != NULL);

    int len = wcslen(word);
    assert(len > 0);

    Node* current_node = root;
    Node* compare_node = trie_new_node();

    for(int i = 0; i < len; i++)
    {
        compare_node->value = word[i];
        current_node = set_find(current_node->children, compare_node);
        if(current_node == NULL) //proper child not found, trie does not contain word
        {
            trie_free_node(compare_node);
            return NULL;
        }
    }
    trie_free_node(compare_node);
    return current_node->is_word ? current_node : NULL;
}

int trie_find_word(Node* root, const wchar_t* word)
{
    return jump_to_word_node(root, word) == NULL ? TRIE_WORD_NOT_FOUND : TRIE_WORD_FOUND;
}

/**
 * @brief fix_after_delete Removes all unused nodes in trie after operation of delete.
 * @param node A node that was corresonding to a word that was deleted from trie.
 * The function deallocates nodes that can be removed and calls itself for removed node parent.
 * Stops when reaching root.
 */
static void fix_after_delete(Node* node)
{
    assert(node != NULL);

    if(is_root(node))
        return;
    if(set_size(node->children) > 0) //cannot delete node
        return;

    Node* current_node = node;
    Node* current_node_parent;

    while(!is_root(current_node) && !current_node->is_word && set_size(current_node->children) == 0)
    {
        current_node_parent = current_node->parent;
        set_remove(current_node_parent->children, current_node);
        current_node = current_node_parent;
    }
}

int trie_delete_word(Node* root, const wchar_t* word)
{
    assert(root != NULL);
    assert(is_root(root));
    assert(word != NULL);

    Node* word_node = jump_to_word_node(root, word);
    if(word_node == NULL)
        return TRIE_WORD_NOT_DELETED;
    else //word_node found
    {
        assert(word_node->is_word);
        word_node->is_word = false;
        fix_after_delete(word_node);
        return TRIE_WORD_DELETED;
    }
}

/**
 * @brief fill_node_from_file Function fill's given node with it's childs and is_word.
 * @param file File to read from.
 * @param filled Node with set value, but unset is_word and children.
 * @param parent Parent of node which will be read. Needed to set correctly node->parent.
 * @return 0 if success, <0 otherwise.
 *
 */
static int fill_node_from_file(FILE* file, Node* filled, Node* parent)
{
    assert(file != NULL);
    assert(filled != NULL);

    //assuming that filled->value is already set.
    filled->parent = parent;

    wchar_t sign;
    //let's read all children of filled node
    Node* filled_child;
    while(true)
    {

        sign = fgetwc(file);
        if(sign == WEOF)
            return -1;
        if(sign == END_OF_NODE_SIGN)
        {
            break;
        }
        else if(sign == END_OF_WORD_NODE_SIGN)
        {
            filled->is_word = true;
            break;
        }
        else //normal letter, new child
        {
            filled_child = trie_new_node();
            filled_child->value = sign;
            set_add(filled->children, filled_child);
        }
    }
    //if end of line, then filling children of filled

    for(int i = 0; i < filled->children->element_count; i++)
        fill_node_from_file(file, (Node*)filled->children->storage[i], filled);
    return 0;

}

Node* trie_load_from_file(FILE* file)
{
    assert(file != NULL);
    Node* root = trie_new_node();
    if(fill_node_from_file(file, root, NULL) == 0)
        return root;
    else
        return NULL;
}

/**
 * How trie is saved:
 * ->Values of node's children are written to file.
 * ->If node is_word, then END_OF_WORD_NODE_SIGN is written
 * ->If not, then END_OF_NODE_SIGN is written
 * ->Then, all children of node are saved in the same manner.
 */
int trie_save_to_file(Node* node, FILE* file)
{
    assert(node != NULL);
    assert(file != NULL);

    for(int i = 0; i < node->children->element_count; i++)
        fputwc(((Node*)node->children->storage[i])->value, file);
    if(node->is_word)
        fputwc(END_OF_WORD_NODE_SIGN, file);
    else
        fputwc(END_OF_NODE_SIGN, file);

    for(int i = 0; i < node->children->element_count; i++)
        trie_save_to_file(node->children->storage[i], file);
    return TRIE_SAVE_TO_FILE_SUCCESS;
}

#ifndef NDEBUG
///Helper function drawing indention in console.
static void indent(int n)
{
    for(int i = 0; i < n; i++) printf("-");
}

///Helper function drawing trie in console.
static void trie_print_rec(Node* node, int level)
{
    assert(level >= 0);
    indent(level);
    if(node == NULL)
    {
        printf("&\n");
        return;
    }
    printf("%c (%d/%d)", node->value, node->children->element_count, node->children->array_size);

    if(node->is_word)
        printf("*");

    printf(" : ");
    for(int i = 0; i < node->children->array_size; i++)
    {
        if(node->children->storage[i] != NULL)
            printf("(%d)%c, ", i, (*(Node*)node->children->storage[i]).value);
        else
            printf("(%d)(~), ", i);
    }

    printf("\n");
    for(int i = 0; i < node->children->array_size; i++)
        trie_print_rec(node->children->storage[i], level+1);
    return;
}

void trie_print(Node* node)
{
    assert(node != NULL);
    assert(is_root(node));
    trie_print_rec(node, 0);
    return;
}
#endif //ndebug
#ifdef TRIE_UNIT_TESTING
bool trie_verify(const Node* node, bool is_root)
{
    //we assume that node is root
    if(node == NULL) return false;
    if(is_root)
    {
        if(node->value != 0) return false;
        if(node->parent != NULL) return false;
        if(node->is_word) return false;
    }
    else
    {
        if(node->value == 0) return false;
        if(node->parent == NULL) return false;
    }
    if(node->children == NULL) return false;

    if(!set_check_correctness(node->children)) return false;
    for(int i = 0; i < node->children->element_count; i++)
        if(!trie_verify(node->children->storage[i], false)) return false;
    return true;

}
#endif //TRIE_UNIT_TESTING
