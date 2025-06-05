#ifndef _utils_
#define _utils_

#include <stdint.h>

int strcmp(const char *str1, const char *str2);
uint64_t charToInt(char *str);
int strlen(const char *str);
char *strcpy(char *dest, const char *src, uint64_t size);
char *strcpyForParam(char *dest, const char *src, const char *src2);
void intToStr(int value, char * str);

#endif