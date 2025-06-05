#include <keyboard.h>
#include <pipe.h>
#include <stdint.h>

int keyboardPipeId;

static const char keyMapL[] = {

     0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 38, 0, '-', 37, 0, 39, '+', 0, 40, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

};

static const char keyMapU[] = {

    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
    '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
    '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0

};

static const char keyMapCAPS[] = {

    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']',
    '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
    0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0

};

static const char *const keyMap[] = {keyMapL, keyMapU};

/*
 * 29: left cntrl
 * 42: left shift
 * 54: right shift
 * 55: no tengo numpad
 * 56: left alt
 * 58: caps lock
 * 59 - 68: F1-F10
 * 69: numLock
 * 70: scrollLock
 *
 * Up: 0x48
 * Left: 0x4B
 * Right: 0x4D
 * Down: 0x50
 */

void initKeyboard(){
    keyboardPipeId  = pipeCreate();
}


void keyboardHandler(uint8_t keyPressed)
{
    unsigned char inputCode = 0;
    char asciiCode = 0;
    int shift = 0;
    int capsLock = 0;
    inputCode = keyPressed;

    // soltar tecla
    if (inputCode > 0x2A || inputCode == 0x36)
    {
        shift = 0;
    }
    if (inputCode == 0xAA || inputCode == 0xB6)
    {
        shift = 0;
    }
   if(inputCode == 0x3A)
    {
        capsLock = (capsLock + 1) % 2; 
    }
    if(inputCode > 0x80 || inputCode == 0x0F){
        asciiCode = 0;
    }
    else if(inputCode == 0x48 || inputCode == 0x50){
        asciiCode = inputCode;
    } else {
        asciiCode = keyMap[shift][inputCode];
    }
    pipeWrite(keyboardPipeId, asciiCode);
}

char readKeyboard(){
    return pipeRead(keyboardPipeId);
}