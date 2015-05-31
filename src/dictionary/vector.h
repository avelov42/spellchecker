#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#define V_TYPE (Node*)
///w przypadku przechowywania np. wskaznikow, nalezy
///zmodyfikowac metode cmp, odpowiedzialna za porownywanie
///elementow wektora, tak aby nie porownywac samych wskaznikow
///w wiekszosci przypadkow jest to nieporządane (np. Node*)
/*

static int cmp(V_TYPE t1, V_TYPE t2)
{
    return t1->value == t2->value ?
    (t1->value < t2->value ? -1 : 1) : 0;
}
*/

///de facto ta struktura moglaby się nazywać zbiorem
///niniejsza strutkura nie pozwala dodawac takich samych elementów
typedef struct
{
    int (*cmp)(V_TYPE, V_TYPE);
    int capacity; ///<rozmiar tablicy
    int size; ///<liczba elementow w srodku
    V_TYPE* data;
} Set;

Set* getNew(int (*cmp)(V_TYPE, V_TYPE)); ///< tworzy pustą instancję zbioru wraz z komparatorem "which is less"

int insert(Set* e, V_TYPE* e); ///< Dodaje element do zbioru. Jeżeli go nie było - SET_NOT_MODIFIED, w p.p. SET_MODIFIED
int find(Set* e, V_TYPE* e, int* pos); ///< Zwraca SET_FOUND jeżeli udało się znaleźć, w p.p. SET_NOT_FOUND, ustawia wskaznik na wynik

//int deletePosition(Set* e, int pos);
int deleteElement(Set* e, V_TYPE* e); ///< Usuwa
int dispose(Set* e);



#endif // VECTOR_H_INCLUDED
