add_library(bombe_common
    bombe.h         bombe.cpp
    cli_tools.h
    enigma.h        enigma.cpp
    reflector.h     reflector.cpp
    rotor.h         rotor.cpp
    scrambler.h     scrambler.cpp
    types.h         types.cpp
)

target_include_directories(bombe_common
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
