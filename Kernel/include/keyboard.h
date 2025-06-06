#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <stdint.h>

//inicializa el pipe del teclado
void initKeyboard();

//recibe el scancode del teclado y escribe el codigo ascii en el pipe del teclado
void keyboardHandler(uint8_t keyPressed);

//lee un caracter ascii desde el pipe del teclado
char readKeyboard();

#endif