#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#if !defined(K_TYPE) || !defined(V_TYPE)
#error "NIE PODANO TYPU MAPY"
#endif

typedef struct
{
    K_TYPE* keyArray;
    V_TYPE* valueArray;
} Map;



#endif // MAP_H_INCLUDED
