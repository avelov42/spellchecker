/** @defgroup trie Moduł trie
 Rekurencyjno-iteracyjna implementacja drzewa trie.
*/

/** @file
 Implementacja nieskompresowanego drzewa trie z funkcją zapisu/odczytu do/z pliku.
 @ingroup trie
 @author Piotr Rybicki <pr360957@students.mimuw.edu.pl>
 @todo Podziwiać.
 <p>
 <strong>Zdefiniowanie makra #TRIE_PRINT_ERRORS włącza wyświetlanie komunikatów o błędach.</strong>
 <p>
 @todo Wyodrębnienie vector-a z trie
 @todo Przeniesc dokumentacje do nagłówku.
 */

#include <wchar.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wctype.h>

/* MIEJSCE NA MAKRODEFINICJE */
#define TRIE_DEBUG_FUNTIONS
#define TRIE_PRINT_ERRORS
/* ************************* */

#include <assert.h>
#include "trie.h"

/* STALE MAKRODEFINICJE */
//czy tutaj nie powinno być >1?
//implementation-defined behaviour
#define TRIE_CHILDREN_ARRAY_START_SIZE 1
#define TRIE_EMPTY_NODE_VALUE 0
/* ******************** */

static Node* addChild(Node* node, wchar_t sign, bool isWordArg, bool* wasAdded);

static int deleteChild(Node* node, wchar_t sign);
static int deleteObsoleteNodes(Node* node);

#ifdef TRIE_DEBUG_FUNTIONS
static bool verifyChildrenArray(const Node* node);
static void printRec(const Node* node, int level);
static void indent(int n);
#endif // TRIE_DEBUG_FUNTIONS

static int arraySizeFunction(const Node* node);
static int findSignPosition(const Node* node,  wchar_t sign);
static int resizeChildrenArray(Node* node);

static Node* findNode(const Node* root, const wchar_t* word);
static Node** getPartOfArray(const Node** src, int begin, int end, bool* success);

static int saveNodeToFileRec(const Node* node, FILE* file);
static Node* loadNodeFromFileRec(Node* parent, FILE* file);

static void _error(const char* func, int line)
{
    //fprintf(stderr, "**************************************************************************\n");
    fprintf(stderr, "Exception at line %d in %s() in module trie\n", line, func);
}
#ifdef TRIE_PRINT_ERRORS
#define error() _error(__func__, __LINE__)
#else
#define error() ;
#endif // TRIE_PRINT_ERRORS


/********************************************//**
 * @brief Zwraca wskaźnik na nowe, świeże drzewko.
 * @return W przypadku sukcesu zwraca wskaźnik na drzewo, w p.p. NULL.
 * <p>
 * Funkcja służy zarówno do uzyskania drzewa z zewnątrz,
 * jak i do wewnętrznego tworzenia nowych wierzchołków.
 * <p>
 * <strong> Funkcja ustawia parent na NULL!</strong>
 ***********************************************/
Node* trieNewNode(void) //~konstruktor
{
    Node* ret = malloc(sizeof(Node));
    if(ret == NULL)
    {
        error();
        return NULL;
    }
    ret->value = 0;
    ret->isWord = false;
    ret->childrenCount = 0;

    ret->arraySize = TRIE_CHILDREN_ARRAY_START_SIZE;
    ret->parent = NULL;
    ret->childrenArray = malloc(sizeof(Node*) * TRIE_CHILDREN_ARRAY_START_SIZE);
    if(ret->childrenArray == NULL)
    {
        error();
        return NULL;
    }

    for(int i = 0; i < ret->arraySize; i++)
        ret->childrenArray[i] = NULL;

    return ret;
}

/********************************************//**
 * @brief Wstawia słowo do drzewa. (iteracyjnie)
 *
 * @param[in,out] *root Korzeń drzewa, do którego ma być wstawione słowo.
 * @param[in] *word Słowo do wstawienia, również niepusty wskaźnik.
 * @return #TRIE_INSERT_SUCCESS_MODIFIED lub #TRIE_INSERT_SUCCES_NOT_MODIFIED lub #TRIE_ERROR
 *
 ***********************************************/
int trieInsertWord(Node* root, const wchar_t* word)
{
    assert(root != NULL);
    assert(root->value == TRIE_EMPTY_NODE_VALUE);
    assert(word != NULL);

    Node* currentNode = root;
    bool wasAdded;
    for(int i = 0; word[i] != 0; i++)
        if((currentNode = addChild(currentNode, word[i], (word[i+1] == 0), &wasAdded)) == NULL)
        {
            error();
            return TRIE_ERROR;
        }
    return wasAdded ? TRIE_INSERT_SUCCESS_MODIFIED : TRIE_INSERT_SUCCESS_NOT_MODIFIED;
}

/********************************************//**
 * @brief Pomocnicza funkcja dodająca dziecko w wierzchołku.
 *
 * @param[in,out] *node Wierzchołek, w którym zostanie dodane dziecko.
 * @param[in] sign Znak (identyfikator dziecka), które ma być dodane.
 * @param[in] isWordArg Wartość isWord, która zostanie nadana wierzchołkowi dziecka.
 * @param[out] *wasAdded True, jeżeli dziecka wcześniej nie było i zostało dodane.
 * @return Wskaźnik do dodanego dziecka, NULL w przypadku błędów.
 *
 ***********************************************/
static Node* addChild(Node* node, wchar_t sign, bool isWordArg, bool* wasAdded)
{
    // KAZDY RETURN MUSI BYC POPRZEDZONY WASADDED
    assert(node != NULL);
    assert(sign != TRIE_EMPTY_NODE_VALUE);
    assert(wasAdded != NULL);

    int signPosition = findSignPosition(node, sign);

    assert(0 <= signPosition && signPosition <= node->childrenCount);

    if(signPosition < node->childrenCount && node->childrenArray[signPosition]->value == sign)
    {
        if(!node->childrenArray[signPosition]->isWord && isWordArg)
            *wasAdded = true;
        else
            *wasAdded = false;
        node->childrenArray[signPosition]->isWord =
            node->childrenArray[signPosition]->isWord || isWordArg; ///@bug brak ||..


        return node->childrenArray[signPosition];
    }
    else
    {
        int precedingsLength = signPosition;
        int  succedingsLength = node->childrenCount - signPosition;
        assert(precedingsLength >= 0);
        assert(succedingsLength >= 0);

        bool precedingsSuccess, succedingsSuccess; //kopia aktualnych dzieci
        //jezeli w ktorejkolwiek z ponizszych tablic pojawi sie null, oznacza to
        //blad, lub (czesciej) zerowa dlugosc zadanej tablicy
        Node** precedings = getPartOfArray((const Node**) node->childrenArray, 0,
                                           signPosition-1, &precedingsSuccess);
        Node** succedings = getPartOfArray((const Node**) node->childrenArray, signPosition,
                                           node->childrenCount-1, &succedingsSuccess);
        if(!precedingsSuccess || !succedingsSuccess) //jezeli wystapil memErr, to musimy skonczyc
        {
            if(precedings != NULL)
                free(precedings);
            if(succedings != NULL)
                free(succedings);
            *wasAdded = false;
            error();
            return NULL;
        }

        node->childrenCount++; //ustawiamy porządaną liczbę dzieci w wierzchołku
        if(resizeChildrenArray(node) != TRIE_SUCCESS)
        {
            if(precedings != NULL)
                free(precedings);
            if(succedings != NULL)
                free(succedings);
            *wasAdded = false; //no effect
            error();
            return NULL;
        }
        assert(node->childrenCount <= node->arraySize); //mamy pewnosc ze mozemy dodac?

        //nullowatosc tego wskaznika nie jest bledem!
        if(precedings != NULL)
        {
            memcpy(&(node->childrenArray[0]),
                   &(precedings[0]),
                   sizeof(Node*) * precedingsLength);

            ///@bug leaks, leaks, tak na pamiątkę.
            free(precedings);
            precedings = NULL;
            //length zostawiam, obiecuje dalej nie uzywac ;)
        }

        if(succedings != NULL)
        {
            memcpy(&(node->childrenArray[signPosition+1]),
                   &(succedings[0]),
                   sizeof(Node*) * succedingsLength);

            free(succedings);
            succedings = NULL;
        }

        node->childrenArray[signPosition]= trieNewNode();
        if(node->childrenArray[signPosition] == NULL)
        {
            *wasAdded = false;
            error();
            return NULL;
        }
        node->childrenArray[signPosition]->value = sign;
        node->childrenArray[signPosition]->isWord = isWordArg;
        node->childrenArray[signPosition]->parent = node;

        *wasAdded = true;
        return node->childrenArray[signPosition];
    }
}

/********************************************//**
 * @brief Usuwa słowo z drzewa.
 *
 * @param[in,out] *root Korzeń drzewa.
 * @param[in] *word Słowo do usunięcia.
 * @return #TRIE_WORD_DELETED lub #TRIE_WORD_NOT_DELETED lub #TRIE_ERROR
 ***********************************************/
int trieDeleteWord(Node* root, const wchar_t* word)
{
    assert(root != NULL);
    assert(root->value == TRIE_EMPTY_NODE_VALUE);
    assert(word != NULL);

    Node* node = findNode(root, word);
    if(node == NULL) //tutaj potencjalnie umykaja jakies bledy
        return TRIE_WORD_NOT_DELETED; //ale to bardzo potencjalnie ;)

    assert(node->isWord);
    node->isWord = false;


    if(deleteObsoleteNodes(node) == TRIE_SUCCESS)
        return TRIE_WORD_DELETED;
    else
    {
        error();
        return TRIE_ERROR; //to sie nigdy nie wydarzy, ale ciezko wyleczyc pedantyzm
    }
}

/********************************************//**
 * @brief Sprząta zbędne wierzchołki po usunięciu słowa.
 *
 * @param[in,out] *node Wskaźnik na wierzchołek, od którego rozpoczyna się sprzątanie.
 * @return #TRIE_SUCCESS lub #TRIE_ERROR
 ***********************************************/
static int deleteObsoleteNodes(Node* node)
{
    assert(node != NULL); //to nie moze byc root ;)
    assert(node->value != TRIE_EMPTY_NODE_VALUE);
    assert(node->isWord == false);

    Node* parent;
    int ret;
    while(node->value != TRIE_EMPTY_NODE_VALUE && !node->isWord && node->childrenCount == 0)
    {
        assert(node->parent != NULL);
        parent = node->parent; //istnieje, bo node nie jest root'em
        if((ret = deleteChild(parent, node->value)) != TRIE_SUCCESS) //deletChild usuwa dziecko z pamieci
        {
            error();
            return TRIE_ERROR;
        }
        node = parent;
    }
    return TRIE_SUCCESS;
}

/********************************************//**
 * @brief Usuwa dziecko sign z wierzchołka node.
 *
 * @param[in,out] *node Wierzchołek do usunięcia z niego dziecka.
 * @param[in] sign Znak dziecka.
 * @return #TRIE_SUCCESS lub #TRIE_ERROR
 ***********************************************/
static int deleteChild(Node* node, wchar_t sign)
{
    assert(node != NULL);
    assert(node->childrenCount > 0);

    int signPosition = findSignPosition(node, sign);
    assert(0 <= signPosition && signPosition < node->childrenCount);
    assert(node->childrenArray[signPosition]->value == sign); //mamy to zagwarantowane

    int precedingsLength = signPosition;
    int succedingsLength = node->childrenCount - signPosition -1;
    assert(precedingsLength >= 0);
    assert(succedingsLength >= 0);

    bool precedingsSuccess, succedingsSuccess;
    //jezeli w ktorejkolwiek z ponizszych tablic pojawi sie null, oznacza to
    //blad, lub (czesciej) zerowa dlugosc zadanej tablicy
    Node** precedings = getPartOfArray((const Node**) node->childrenArray, 0,
                                       precedingsLength-1, &precedingsSuccess);
    Node** succedings = getPartOfArray((const Node**) node->childrenArray, signPosition+1,
                                       node->childrenCount-1, &succedingsSuccess);
    if(!precedingsSuccess || !succedingsSuccess)
    {
        if(precedings != NULL)
            free(precedings);
        if(succedings != NULL)
            free(succedings);
        error();
        return TRIE_ERROR;
    }
    //raczej nie chcemy usuwac dzieci, ktore maja dzieci, to bardzo nieetyczne ;c
    assert(node->childrenArray[signPosition]->childrenCount == 0);

    if(trieDeleteTrie(node->childrenArray[signPosition]) != TRIE_SUCCESS)//kasujemy dziecie
    {
        error();
        return TRIE_ERROR;
    }
    node->childrenArray[signPosition] = NULL;
    node->childrenCount--;

    int ret;
    if((ret = resizeChildrenArray(node)) != TRIE_SUCCESS) //inteligentna funkcja opiekuje sie rozmiarem
    {
        if(precedings != NULL)
            free(precedings);
        if(succedings != NULL)
            free(succedings);
        error();
        return TRIE_ERROR;
    }

    if(precedings != NULL)
    {
        memcpy(&(node->childrenArray[0]),
               &(precedings[0]),
               sizeof(Node*) * precedingsLength);
        free(precedings);
        //precedings == NULL;
        //statement with no effect
    }

    if(succedings != NULL)
    {
        memcpy(&(node->childrenArray[signPosition]),
               &(succedings[0]),
               sizeof(Node*) * succedingsLength);
        free(succedings);
        //succedings == NULL; //j/w
    }
    assert(node->childrenCount == precedingsLength+succedingsLength);
    assert(findSignPosition(node, sign) == node->childrenCount ||
           node->childrenArray[findSignPosition(node, sign)]->value != sign);

    return TRIE_SUCCESS;
}

/********************************************//**
 * @brief Sprawdza, czy dane słowo znajduje się w drzewie.
 *
 * @param[in] *root Korzeń drzewa, w którym będzie szukane słowo.
 * @param[in] *word Wskaźnik na szukane słowo.
 * @return #TRIE_WORD_FOUND lub #TRIE_WORD_NOT_FOUND
 ***********************************************/
int trieFindWord(const Node* root, const wchar_t* word)
{
    assert(root != NULL);
    assert(root->value == TRIE_EMPTY_NODE_VALUE);
    assert(word != NULL);
    return findNode(root, word) == NULL ? TRIE_WORD_NOT_FOUND : TRIE_WORD_FOUND;
}

/********************************************//**
 * @brief Zwraca wskaźnik na wierzchołek reprezentujący słowo word.
 *
 * @param[in] *root Wierzchołek startowy. Zazwyczaj będzie to szukanie od root'a, więc w tej wersji
 * biblioteki, argument musi być root'em.
 * @param[in] *word Słowo, które jest 'szukane'.
 * @return Zwraca wskaźnik do wierzchołka, który reprezentuje szukane słowo. Jeżeli takiego słowa
 * nie ma w drzewie, zwracany jest NULL.
 ***********************************************/
static Node* findNode(const Node* root, const wchar_t* word)
{
    assert(root != NULL);
    assert(root->value == TRIE_EMPTY_NODE_VALUE);
    assert(word != NULL);

    const Node* currentNode = root;
    int signPosition;
    for(int i = 0; word[i] != 0; i++) //warunek nie jest potrzebny
    {
        signPosition = findSignPosition(currentNode, word[i]);
        assert(0 <= signPosition && signPosition <= currentNode->childrenCount);
        if(signPosition == currentNode->childrenCount)
            return NULL;

        assert(currentNode->childrenArray[signPosition] != NULL);

        if(currentNode->childrenArray[signPosition]->value != word[i])
            return NULL; //nie ma odp. dziecka.

        currentNode = currentNode->childrenArray[signPosition];
        //wchodzimy do dziecka
        if(word[i+1] == 0 && currentNode->isWord)
            return (Node*) currentNode; //pozbywam sie const-a, zeby p
        if(word[i+1] == 0 && !currentNode->isWord)
            return NULL;
    }
    assert(wcslen(word) == 0);
    return NULL;
}

/********************************************//**
 * @brief Zapisuje drzewo do pliku.
 *
 * @param[in] *root - Drzewo do zapisania.
 * @param[in] *file - wskaźnik do strumienia docelowego
 * @return #TRIE_SUCCESS. Ta funkcja nie zwraca komunikatu o błędach.
 * @todo Dodac obsluge bledow tu i w load.
 ***********************************************/
int trieSaveToFile(const Node* root, FILE* file)
{
    assert(root != NULL);
    assert(root->value == TRIE_EMPTY_NODE_VALUE);
    assert(file != NULL);
    //zapisanie
    saveNodeToFileRec(root, file); //mozna dodac obsluge bledow przy save/laod
    return TRIE_SUCCESS;
    ///pod koniec tej funkcji wskaźnik pokazuje za stopkę, za którą umieszcze w dictionary alfabet.
    ///plik pozostaje otwarty
}

/********************************************//**
 * @brief Rekurencyjna funkcja pomocnicza zapisująca wierzchołek do pliku.
 *
 * @param[in] *node Wierzchołek do zapisania
 * @param[in] *file Wskaźnik do pliku.
 * @return #TRIE_SUCCESS
 ***********************************************/
static int saveNodeToFileRec(const Node* node, FILE* file)
{
    assert(node != NULL);
    assert(file != NULL);


    fwprintf(file, L"%lc", node->value);
    fwprintf(file, L"%c", (char) node->isWord);
    fwprintf(file, L"%d", node->childrenCount);

    for(int i = 0; i < node->childrenCount; i++)
        saveNodeToFileRec(node->childrenArray[i], file);
    return TRIE_SUCCESS;
}

/********************************************//**
* @brief Podejmuje próbę wczytania drzewa z pliku.
*
* @param[in] *file Wskaźnik do otwartego pliku.
* @return Wskaźnik na root-a wczytanego drzewa. W przypadku niepowodzenia - NULL.
***********************************************/
Node* trieLoadFromFile(FILE* file)
{
    assert(file != NULL);
    Node* returnValue = loadNodeFromFileRec(NULL, file);
    if(returnValue == NULL)
    {
        error();
        return NULL;
    }
    return returnValue;
}

/********************************************//**
 * @brief Pomocnicza funkcja rekursywna wczytująca z pliku wierzchołek wraz z jego dziećmi.
 *
 * @param[in] *parent Rodzic, która ma zostać przypisany wczytanemu wierzchołkowi.
 * @param[in] *file Plik wejściowy.
 * @return Zwraca wskaźnik na wczytane dziecko. W przypadku niepowodzenia - NULL.
 *
 ***********************************************/
static Node* loadNodeFromFileRec(Node* parent, FILE* file)
{
    assert(file != NULL);

    Node* node = trieNewNode();
    node->parent = parent;
    fwscanf(file, L"%lc", &(node->value));
    fwscanf(file, L"%c", &(node->isWord));
    fwscanf(file, L"%d", &(node->childrenCount));
    //w tym momencie musze niestety samemu zajac sie alokacja, bo resizeChildrenArray(node)
    //nie zagwarantuje odpowiedniego zachowania
    free(node->childrenArray); //zwolnienie default-owo zaalokowanej tablicy
    node->childrenArray = malloc(sizeof(Node*) * node->childrenCount);

    node->arraySize = node->childrenCount;
    for(int i = 0; i < node->childrenCount; i++)
        node->childrenArray[i] = NULL; //pedantyzm, ale tez konieczne ze wzgledu na assert

    for(int i = 0; i < node->childrenCount; i++)
        node->childrenArray[i] = loadNodeFromFileRec(node, file);




    return node;
}

/********************************************//**
 * @brief Usuwa drzewo rekurencyjnie (lub poddrzewo).
 * @param[in] *node Wskaźnik na drzewo, które ma zostać usunięte.
 * @return #TRIE_SUCCESS
 * <p>
 * <strong> Po użyciu należy pamiętać o wyzerowaniu wskaźnika, który był argumentem! </strong>
 ***********************************************/
int trieDeleteTrie(Node* node)
{
    assert(node != NULL);
    assert(node->childrenArray != NULL);

    for(int i = 0; i < node->childrenCount; i++)
    {
        assert(node->childrenArray[i] != NULL);
        trieDeleteTrie(node->childrenArray[i]); //tutaj nie ma mozliwosci zepsucia.
        node->childrenArray[i] = NULL;
    }
    free(node->childrenArray);
    free(node);
    return TRIE_SUCCESS;
}

/********************************************//**
 * @brief Prywatna funkcja wyszukująca przez bisekcję zadane dziecko.
 *
 * @param[in] *node Wierzchołek, którego dziecko szukamy.
 * @param[in] sign Symbol szukanego dziecko.
 * @return W przypadku, gdy wierzchołek posiada dziecko, którego value == sign, zwracana jest
 * jego pozycja w node->childrenArray. W przeciwnym przypadku, zwracana jest pozycja, na której
 * znajdowałoby się owe dziecko, gdyby tam było.
 * <p>
 * <strong>W szczególności może zostać zwrócony indeks,
 * który znajduje się bezpośrednio za ostatnim elementem tablicy, ale nie dalej.</strong>
 *
 ***********************************************/
static int findSignPosition(const Node* node, wchar_t sign)
{
    assert(node != NULL);

    int l = 0;
    int r = node->childrenCount;
    int s;
    while(l < r)
    {
        s = (l+r)/2;
        assert(node->childrenArray[s] != NULL); //jezeli beda bledy (null) w synach, to tutaj sie pojawia
        if(node->childrenArray[s]->value < sign) l = s+1;
        else r = s;
    }
    assert(0 <= l && l <= node->childrenCount);
    return l;
}

/********************************************//**
 * @brief Funkcja odpowiada na pytanie jaki rozmiar powinna mieć tablica childrenArray, aby
 * mieściła childrenCount i nie używała zbędnego miejsca.
 *
 * @param[in] *node Wskaźnik do rozważanego wierzchołka.
 * @return Zwraca nowy rozmiar tablicy.
 *
 * Funkcja opiera się na wartościach childrenCount i arraySize. Zwrócona wartość będzie odpowiednia
 * dla ich faktycznych wartości. Oznacza to, że dodając dziecko do tablicy <strong>najpierw
 * zwiększamy liczbę dzieci, a potem wywołujemy tę funkcję.</strong>
 * <p>
 * W przypadku delete analogicznie.
 *
 ***********************************************/
static int arraySizeFunction(const Node* node)
{
    assert(node != NULL);
    assert(node->arraySize >= 0);
    assert(node->childrenCount >= 0);
    assert(node->arraySize+1 >= node->childrenCount);
    int newSize;
    if(node->childrenCount == node->arraySize+1) //pelna tablica, poszerzamy
        newSize = node->arraySize == 0 ? 1 : node->arraySize*2;
    else if(node->childrenCount*2+1 < node->arraySize)//miesci sie, sprobujmy usunac
        newSize = node->arraySize/2; //zmniejszamy
    else
        newSize = node->arraySize; //bez zmian
    assert(0 <= newSize && node->childrenCount <= newSize);
    return newSize;
}

/********************************************//**
 * @brief Zwraca wskaźnik do kopii fragmentu tablicy.
 *
 * @param[in] **src Tablica źródłowa (childrenArray)
 * @param[in] begin - początek (inkluzywnie)
 * @param[in] end - koniec (inkluzywnie)
 * @param[out] *success - flaga powodzenia. False gdy błąd alokacji.
 * @return Zwraca wskaźnik do kopii fragmentu tablicy lub null, w przypadku gdy zadany fragment
 * ma ujemną długość.
 * @todo src może być typu void*
 ***********************************************/
static Node** getPartOfArray(const Node** src, int begin, int end, bool* success)
{
    assert(src != NULL);
    assert(begin >= 0);
    assert(end >= -1);

    if(end < begin)
    {
        *success = true;
        return NULL;
    }

    int length = end - begin + 1;
    Node** returnValue = malloc(sizeof(Node*) * length);
    if(returnValue == NULL) //obsługa błędu
    {
        *success = false;
        error();
        return NULL;
    }
    memcpy(returnValue, &src[begin], sizeof(Node*) * length);
    *success = true;
    return returnValue;
}

/********************************************//**
 * @brief Alokuje nową, pustą tablicę childrenArray w *node.
 *
 * @param[in,out] *node Wierzchołek, w którym będzie realokowana tablica childrenArray.
 * @return #TRIE_SUCCESS lub #TRIE_ERROR
 * <p>
 * <strong>Po zakończeniu, tablica childrenArray jest wypełniona NULL'ami.</strong><br>
 * Dlatego przed wywołaniem tej funkcji należy mieć kopię zapasową tablicy.
 ***********************************************/
static int resizeChildrenArray(Node* node)
{
    int newSize = arraySizeFunction(node);
    if(newSize == node->arraySize)
    {
        for(int i = node->childrenCount; i < node->arraySize; i++)
            node->childrenArray[i] = NULL; //to skomplikowane.
        //jezeli delete to dzieci przesuna sie w lewo, a po prawej nie bedzie null.
        return TRIE_SUCCESS;
    }
    assert(newSize >= node->childrenCount);

    Node** tmp = malloc(sizeof(Node*) * newSize);
    if(tmp == NULL)
    {
        error();
        return TRIE_ERROR;
    }
    free(node->childrenArray); //nie uzywam realloca, nie chce kopiowac
    node->childrenArray = tmp;
    node->arraySize = newSize;

    for(int i = 0; i < node->arraySize; i++) //nie trzeba koniecznie od 0
        node->childrenArray[i] = NULL;

    return TRIE_SUCCESS;
}
#ifdef TRIE_DEBUG_FUNTIONS

/********************************************//**
 * @brief Wyświetla podane drzewo w konsoli.
 *
 * @param[in] *root Drzewo do wyświetlenia
 *
 ***********************************************/
void triePrint(const Node* root)
{
    assert(root != NULL);
    printRec(root, 0);
}

/********************************************//**
 * @brief Pomocnicza funkcja wyświetlająca drzewo.
 *
 * @param[in] *node Wierzchołek do wyświetlenia.
 * @param[in] level Określa głębokość rekurencji, wpływa na wcięcie.
 *
 * Ta funkcja wygrała konkurs na najbardziej paskudny kod w tym module.
 ***********************************************/
static void printRec(const Node* node, int level)
{
    assert(level >= 0);
    indent(level);
    if(node == NULL)
    {
        wprintf(L"&\n");
        return;
    }
    wprintf(L"%lc (%d/%d)", node->value, node->childrenCount, node->arraySize);

    if(node->isWord)
        wprintf(L"*");

    wprintf(L" : ");
    for(int i = 0; i<node->arraySize; i++)
    {
        if(node->childrenArray[i] != NULL)
            wprintf(L"(%d)%lc, ", i, node->childrenArray[i]->value);
        else
            wprintf(L"(%d)(~), ", i);
    }

    wprintf(L"\n");
    for(int i = 0; i < node->arraySize; i++)
    {
        printRec(node->childrenArray[i], level+1);
    }
}

/********************************************//**
 * @brief Pisze wcięcie ze znaków '-' na stdin.
 *
 * @param[in] n Długość wcięcia.
 *
 ***********************************************/
static void indent(int n)
{
    for(int i = 0; i < n; i++) wprintf(L"-");
}

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
int trieVerifyTrie(const Node* node, int *checksum, bool isRoot, bool countArraySize)
{
    //Poprawność root-a sprawdzam oddzielnie.
    assert(node != NULL); //niepustosc
    if(isRoot)
    {
        assert(node->value == TRIE_EMPTY_NODE_VALUE); //poprawny root
        assert(node->parent == NULL); //nie ma rodzica
        assert(!node->isWord); //nie jest slowem
    }
    else
    {
        assert(node->value != TRIE_EMPTY_NODE_VALUE);
        assert(node->parent != NULL);

        *checksum += node->value;
        *checksum += node->isWord;
        *checksum += node->parent->value;
    }

    if(verifyChildrenArray(node))
    {
        *checksum += (node->childrenCount);
        //*checksum += (node->arraySize);
        //usuwamy ten czynnik z powodu zapisu do pliku
    }

    int sum = 1;
    for(int i = 0; i < node->childrenCount; i++)
        sum += trieVerifyTrie(node->childrenArray[i], checksum, false, countArraySize);
    if(countArraySize)
        sum += node->arraySize - node->childrenCount;

    return sum;
}

/********************************************//**
 * @brief Sprawdza poprawność danych związanych z dziećmi.
 *
 * @param *node Wierzchołek do sprawdzenia.
 * @return Zwraca prawdę w przypadku powodzenia.
 *
 ***********************************************/
static bool verifyChildrenArray(const Node* node)
{
    assert(node != NULL);
    assert(node->childrenArray != NULL);
    assert(node->childrenCount >= 0 || node->arraySize >= 0);
    assert(node->arraySize >= node->childrenCount); //nie ma wiecej dzieci niz miejsca

    for(int i = 0; i < node->childrenCount; i++)
        assert(node->childrenArray[i] != NULL); //brak dziur

    for(int i = node->childrenCount; i < node->arraySize; i++)
        assert(node->childrenArray[i] == NULL); //nie ma podejrzanych rzeczy dalej

    for(int i = 0; i+1 < node->childrenCount; i++) //dzieci sa uporzadkowane
        assert(node->childrenArray[i]->value < node->childrenArray[i+1]->value);
    return true;
}
#endif // DEBUG_FUNCTIONS
