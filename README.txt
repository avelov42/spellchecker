Włączone unit-testy implikują niepoprawną pracę wszystkich programów (z powodu podmienienia funkcji io, malloc, calloc i free).

Włączenie/wyłączenie unit-testów odbywa się poprzez zmianę zmiennych *_UNIT_TESTING w pliku CMakeLists.txt w katalogu ./src/dictionary.

Ustawienie UNIT_TESTING na 0 wyłącza wszystkie unit-testy.

________
IN CASE:
(Testowanie dowolnego modułu musi być wykonywane przy włączonych testach dla modułów zależnych. W przeciwnym razie w module zależnym funkcje typu malloc nie zostaną podmienione na odpowiedniki z CMOCKA, co będzie powodowało fake-failures.)
