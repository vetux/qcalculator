#ifndef NUMPADKEY_HPP
#define NUMPADKEY_HPP

#include <string>

enum NumPadKey {
    START,

    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,

    KEY_DOT,

    KEY_DIVIDE,
    KEY_MULTIPLY,
    KEY_ADD,
    KEY_SUBTRACT,

    KEY_OPENBRACKET,
    KEY_CLOSEBRACKET,

    KEY_EQUAL,

    END
};

std::string convertNumPadKeyToString(NumPadKey key);

#endif // NUMPADKEY_HPP
