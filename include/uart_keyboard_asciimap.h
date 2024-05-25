#ifndef UART_KEYBOARD_ASCIIMAP_H
#define UART_KEYBOARD_ASCIIMAP_H

#include "mbed.h"
#include "uart_keyboard_defines.h"

#define ASCII_MAP_SIZE 55

typedef struct Scancode{
    bool release;
    uint8_t special;
    int code;
}Scancode;

enum Type {
  CHAR,
  EVENT
};

typedef struct TypeCode {
    Type type;
    uint32_t code;
}Code;

typedef struct AsciiMap{
    Scancode scancode;
    Code *none;
    Code *shift;
    Code *alt;
    Code *strg;
    Code *shiftalt;
    Code *altstrg;
    Code *strgshift;
    Code *strgaltshift;
}AsciiMap;

extern const AsciiMap asciiMapList[ASCII_MAP_SIZE];

#endif
