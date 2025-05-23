#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "lib.h"

void keyboard_handler(uint8_t keyPressed);

/* Returns character detected from keyboard, Scancodes without ascii symbols will return 0 */
char getCharFromKeyboard();

/* Returns the notChar -> use for keys that are not characters */
unsigned char getNotChar();

/* Clears the notChar var */
void setCeroChar();

#endif