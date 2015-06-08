#ifndef SORTEDVECTOR_H_INCLUDED
#define SORTEDVECTOR_H_INCLUDED

/**
Zadania:
wstawianie istniejącego elementu do środka tablicy
usuwanie dziecka, sprawdzajac czy nie ma synow - potrzeba dostepu do niego



*/

typedef struct SortedVector
{
    int size;
    int capacity;
    void** data;
    int (*comparator)(void* p1, void* p2);
} SortedVector;

/**
 @brief Tworzy wektor.
 @return Wskaźnik do zainicjalizowanego, pustego wektora
*/
SortedVector* newVector(void);

/**
 @brief Wstawia element obj na odpowiednią pozycję do wektora vect
 @param[in,out] *vect - wskaźnik do modyfikowanego wektora
 @param[in] *obj - obiekt do wstawienia
 @return Pozycja, na której znajduje się obiekt obj, SET_ALREADY_INSERTED lub SET_ERROR
 */
int insert(SortedVector* vect, void* obj); ///wstawia, zwraca pozycję. jezeli jest juz, to nie wstawia

int find(SortedVector* vect, void* obj); ///zwraca pozycje zadanego obiektu

void* at(SortedVector* vect, int pos); ///zwraca wskaźnik do obiektu na pos pozycji

int remove(SortedVector* vect, void* obj); ///usuwa obiekt obj

int remove(SortedVector* vect, int pos); ///

void deleteVector(SortedVector* vect);

#endif // SORTEDVECTOR_H_INCLUDED
