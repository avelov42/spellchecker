# dodajemy bibliotekę dictionary, stworzoną na podstawie pliku dictionary.c
# biblioteka będzie dołączana statycznie (czyli przez linkowanie pliku .o)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

add_library(error_handling error_handling.c)

add_library (word_list word_list.c)
target_link_libraries(word_list error_handling)


add_library (array_set array_set.c)
target_link_libraries(array_set error_handling)


add_library (trie trie.c)
target_link_libraries(trie array_set)


add_library (dictionary dictionary.c word_list.c)
target_link_libraries(dictionary trie array_set)


#Unit testy są w pewnym stopniu zależne od siebie
#Testowanie stuktury trie bez testowania array-set będzie
#sypać leak'ami, ponieważ malloc i free w array-set nie zostaną podmienione

set(UNIT_TESTING 0)
set(ARRAY_SET_UNIT_TESTING 1)
set(TRIE_UNIT_TESTING 1)
set(WORD_LIST_UNIT_TESTING 1)
set(DICTIONARY_UNIT_TESTING 1)
# ta 1 nizej jest przelacznikiem do wylaczania testowania pomimo obecnosci CMOCKA
if ((CMOCKA AND UNIT_TESTING))
    add_definitions(-DUNIT_TESTING)
    add_library(mock_io mock_io.c)

    if(ARRAY_SET_UNIT_TESTING)
        add_definitions(-DARRAY_SET_UNIT_TESTING)
        add_executable (array_set_test array_set_test.c)
        target_link_libraries(array_set_test array_set)
        target_link_libraries (array_set_test ${CMOCKA})
        target_link_libraries (array_set ${CMOCKA})
        add_test (array_set_unit_test array_set_test)
    endif (ARRAY_SET_UNIT_TESTING)

    if(TRIE_UNIT_TESTING)
        add_definitions(-DTRIE_UNIT_TESTING)
        add_executable (trie_test trie_test.c)

        target_link_libraries(trie mock_io)
        target_link_libraries(trie_test trie)
        target_link_libraries (trie_test ${CMOCKA})
        target_link_libraries(mock_io ${CMOCKA})
        add_test (trie_unit_test trie_test)
    endif (TRIE_UNIT_TESTING)

    if(WORD_LIST_UNIT_TESTING)
        # dodajemy plik wykonywalny z testem
        add_definitions(-DWORD_LIST_UNIT_TESTING)
        add_executable (word_list_test word_list_test.c)
        target_link_libraries(word_list_test word_list)
        # i linkujemy go z biblioteką do testowania
        target_link_libraries (word_list_test ${CMOCKA})
        # wreszcie deklarujemy, że to test
        add_test (word_list_unit_test word_list_test)
    endif(WORD_LIST_UNIT_TESTING)

    if(DICTIONARY_UNIT_TESTING)
        add_definitions(-DDICTIONARY_UNIT_TESTING)
        add_executable(dictionary_test dictionary_test.c)

        target_link_libraries(dictionary_test dictionary)
        target_link_libraries(dictionary mock_io)
        target_link_libraries(dictionary_test ${CMOCKA})
        add_test(dictionary_unit_test dictionary_test)
    endif(DICTIONARY_UNIT_TESTING)

endif ((CMOCKA AND UNIT_TESTING))
