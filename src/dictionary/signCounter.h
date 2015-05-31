#ifndef SIGNCOUNTER_H_INCLUDED
#define SIGNCOUNTER_H_INCLUDED

#define SC_START_ARRAY_SIZE 1
#define SC_SUCCESS 1
#define SC_ERROR

#define SC_PRINT_ERRORS

typedef struct
{
    int arraySize;
    int signsCount;
    wchar_t* signArray;
    int* countArray;
} SignCounter;

SignCounter* newSignCounter(void); ///zwraca nowy licznik
int incrementSign(SignCounter* obj, wchar_t sign); ///<inkrementuje liczbe wystapien sign
int decrementSign(SignCounter* obj, wchar_t sign); ///<dekrementuje liczbe wystapien sign
int getSignCount(SignCounter* obj, wchar_t sign); ///<zwraca liczbe wystapien sign
const wchar_t* getSignArray(SignCounter* obj);
void deleteSignCounter(SignCounter* obj);
void signCounterSaveToFile(SignCounter* obj, FILE* file);
SignCounter* signCounterLoadFromFile(FILE* file);

///realloki obu tablic musza wystepowac razem

#endif // SIGNCOUNTER_H_INCLUDED
