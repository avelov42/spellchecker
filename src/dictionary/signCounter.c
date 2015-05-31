#include <assert.h>




#ifdef SC_PRINT_ERRORS
#define error() {_error(__LINE__, __func__); return SC_ERROR;}
#define memError() {_error(__LINE__, __func__);return NULL;}
#else
#define error()
#define memError()
#endif // DICT_PRINT_ERRORS

SignCounter* newSignCounter(void)
{
    SignCounter* ret = malloc(sizeof(SignCounter));
    if(ret == NULL) memError();
    ret->arrarSize = SC_START_ARRAY_SIZE;
    ret->signsCount = 0;
    ret->signArray = malloc(sizeof(wchar_t) * SC_START_ARRAY_SIZE);
    if(ret->signArray == NULL) memError();
    ret->countArray = malloc(sizoef(int) * SC_START_ARRAY_SIZE);
    if(ret->countArray == NULL) memError();
    for(int i = 0; i < arraySize; i++)
    {
        signArray[i] = 0;
        countArray[i] = 0;
    }
}

static int findPosition(SignCounter* obj, wchar_t sign)
{
    int l = 0;
    int r = obj->signsCount;
    int s;
    while(l < r)
    {
        s = (l+r)/2;
        assert(signArray[s] != 0); //jezeli beda bledy to tutaj sie pojawia
        if(signArray[s] < sign) l = s+1;
        else r = s;
        z
    }
    assert(0 <= l && l <= obj->signsCount);
    return l;
}

static wchar_t* getPartOfArray(const wchar_t* src, int begin, int end)
{
    assert(src != NULL);
    assert(begin >= 0);
    assert(end >= -1);

    if(end < begin)
        return NULL;
    int length = end - begin + 1;
    wchar_t* returnValue = malloc(sizeof(wchar_t) * length);
    if(returnValue == NULL)
        return NULL;
    memcpy(returnValue, &src[begin], sizeof(wchar_t) * length);
    return returnValue;
}

static int arraySizeFunction(const SignCounter* obj)
{
    assert(obj != NULL);
    assert(obj->arraySize >= 0);
    assert(obj->signsCount >= 0);
    assert(obj->arraySize+1 >= obj->signsCount);

    int newSize;
    if(obj->signsCount == obj->arraySize+1) //pelna tablica, poszerzamy
        newSize = obj->arraySize == 0 ? 1 : obj->arraySize*2;
    else if(obj->signsCount*2+1 < obj->arraySize)//miesci sie, sprobujmy usunac
        newSize = obj->arraySize/2; //zmniejszamy
    else
        newSize = obj->arraySize; //bez zmian
    assert(0 <= newSize && obj->signsCount <= newSize);
    return newSize;
}

static void resizeSignArray(SignCounter* obj)
{
    int newSize = arraySizeFunction(obj);
    if(newSize == obj->arraySize)
        return;
    assert(newSize >= obj->signsCount);

    wchar_t* tmp = malloc(sizeof(wchar_t) * newSize);
    if(tmp == NULL)
        return;
    free(obj->childrenArray); //nie uzywam realloca, nie chce kopiowac
    obj->childrenArray = tmp;
    obj->arraySize = newSize;

    for(int i = 0; i < obj->arraySize; i++) //nie trzeba koniecznie od 0
        obj->signArray[i] = 0;

    return;
}

//bez obslugi bledow
static void insertSign(SignCounter* obj, int position, wchar_t sign)
{
    //uzywane w przypadku, gdy nie ma w tablicy takowego elementu
    assert(position == obj->signsCount || obj->signArray[position] != sign);
    int precedingsLength = position;
    int succedingsLength = obj->signsCount - position;


    wchar_t* precSigns = getPartOfArray(obj->signArray, 0, position-1);
    int* precCounts =    getPartOfArray(obj->countArray, 0, position-1);

    wchar_t* succedings = getPartOfArray(obj->signArray, position, obj->signsCount-1);
    int* succCounts =    getPartOfArray(obj->countArray, position, obj->signsCount-1);

    obj->signsCount++;
    resizeSignsArray(obj);

    if(precSigns != NULL)
    {
        assert(precCounts != NULL);
        memcpy(&(obj->signArray[0]), &(precSigns[0]), sizeof(wchar_t) * precedingsLength);
        memcpy(&(obj->countArray[0]), &(precCounts[0], sizeof(int) * precedingsLength));
        free(precSigns);
        free(precCounts);
    }
    if(succCounts != NULL)
    {
        assert(succCounts != NULL);
        memcpy(&(obj->signArray[0]), &(succSigns[0]), sizeof(wchar_t) * succedingsLength);
        memcpy(&(obj->countArray[0]), &(succCounts[0], sizeof(int) * succedingsLength));
        free(succSigns);
        free(succCounts);
    }

    obj->signArray[position] = sign;
    obj->countArray[position] = 1;
}

int incrementSign(SignCounter* obj, wchar_t sign)
{
    int position = findSignPosition(obj, sign);
    if(obj->signArray[i] == sign) obj->countArray[i]++;
    else insertSign(obj, sign);
}

static void removeSign(SignCounter* obj, wchar_t sign)
{
        //uzywane w przypadku, gdy nie ma w tablicy takowego elementu
    assert(position == obj->signsCount || obj->signArray[position] != sign);
    int precedingsLength = position;
    int succedingsLength = obj->signsCount - position;


    wchar_t* precSigns = getPartOfArray(obj->signArray, 0, position-1);
    int* precCounts =    getPartOfArray(obj->countArray, 0, position-1);

    wchar_t* succedings = getPartOfArray(obj->signArray, position, obj->signsCount-1);
    int* succCounts =    getPartOfArray(obj->countArray, position, obj->signsCount-1);

    obj->signsCount++;
    resizeSignsArray(obj);

    if(precSigns != NULL)
    {
        assert(precCounts != NULL);
        memcpy(&(obj->signArray[0]), &(precSigns[0]), sizeof(wchar_t) * precedingsLength);
        memcpy(&(obj->countArray[0]), &(precCounts[0], sizeof(int) * precedingsLength));
        free(precSigns);
        free(precCounts);
    }
    if(succCounts != NULL)
    {
        assert(succCounts != NULL);
        memcpy(&(obj->signArray[0]), &(succSigns[0]), sizeof(wchar_t) * succedingsLength);
        memcpy(&(obj->countArray[0]), &(succCounts[0], sizeof(int) * succedingsLength));
        free(succSigns);
        free(succCounts);
    }

    obj->signArray[position] = sign;
    obj->countArray[position] = 1;
}

