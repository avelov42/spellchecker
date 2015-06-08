# dodajemy bibliotekę dictionary, stworzoną na podstawie pliku dictionary.c
# biblioteka będzie dołączana statycznie (czyli przez linkowanie pliku .o)

add_library (trie trie.c)
add_library (word_list word_list.c)
add_library (dictionary dictionary.c word_list.c)

target_link_libraries (dictionary word_list)
target_link_libraries (dictionary trie)



#if (CMOCKA)
#    # dodajemy plik wykonywalny z testem    
#    add_executable (word_list_test word_list.c word_list_test.c)
#
#    # i linkujemy go z biblioteką do testowania
#    target_link_libraries (word_list_test ${CMOCKA})
#
#    # wreszcie deklarujemy, że to test
#    add_test (word_list_unit_test word_list_test)
#endif (CMOCKA)

