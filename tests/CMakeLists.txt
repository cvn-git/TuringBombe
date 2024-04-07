add_subdirectory(doctest)

add_executable(enigma_tests
    enigma_tests.cpp
)

target_link_libraries(enigma_tests
    PUBLIC doctest
    PUBLIC bombe_common
)

add_test(NAME enigma_tests COMMAND enigma_tests)
