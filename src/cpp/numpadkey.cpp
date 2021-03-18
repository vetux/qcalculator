#include "numpadkey.hpp"

#include <stdexcept>

std::string convertNumPadKeyToString(NumPadKey key) {
    switch (key) {
        case KEY_0:
            return "0";
        case KEY_1:
            return "1";
        case KEY_2:
            return "2";
        case KEY_3:
            return "3";
        case KEY_4:
            return "4";
        case KEY_5:
            return "5";
        case KEY_6:
            return "6";
        case KEY_7:
            return "7";
        case KEY_8:
            return "8";
        case KEY_9:
            return "9";
/*case KEY_A:
      return "A";
  case KEY_B:
      return "B";
  case KEY_C:
      return "C";
  case KEY_D:
      return "D";
  case KEY_E:
      return "E";
  case KEY_F:
      return "F";*/
        case KEY_DOT:
            return ".";
        case KEY_DIVIDE:
            return "/";
        case KEY_MULTIPLY:
            return "*";
        case KEY_ADD:
            return "+";
        case KEY_SUBTRACT:
            return "-";
        case KEY_OPENBRACKET:
            return "(";
        case KEY_CLOSEBRACKET:
            return ")";
        default:
            throw std::runtime_error("Unsupported key");
    }
}
