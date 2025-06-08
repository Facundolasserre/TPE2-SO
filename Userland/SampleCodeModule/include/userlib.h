#ifndef _USERLIB_H_
#define _USERLIB_H_

#include <stdint.h>
#include <colors.h>

extern const Color BLACK;
extern const Color WHITE;
extern const Color RED;
extern const Color LIGHT_BLUE;
extern const Color BLUE;
extern const Color ORANGE;
extern const Color YELLOW;
extern const Color PURPLE;
extern const Color PINK;
extern const Color GREEN;
extern const Color LIGHT_RED;
extern const Color LIGHT_PURPLE;
extern const Color LIGHT_ORANGE;
extern const Color LIGHT_YELLOW;
extern const Color LIGHT_PINK;
extern const Color LIGHT_GREEN;

typedef uint64_t (*program_t)(uint64_t argc, char *argv[]);

#define CHUNK_SIZE 10
#define CHUNK_COUNT 10
#define MAX_BUFF 254

typedef struct Note
{
    int tone;
    int duration;
} NoteType;

uint64_t create_process(int priority, program_t program, uint64_t argc, char *argv[], uint64_t fd_ids[10], uint64_t fd_count);

uint64_t create_process_foreground(int priority, program_t program, uint64_t argc, char *argv[], uint64_t fd_ids[10], uint64_t fd_count);

void write_char(char c);

void write_string(const char *str, int length);

/* Prints a char in screen */
void printc(char c);

/* Prints a string in screen */
void prints(const char *str, int length);

/* Gets a single char from keyboard */
char getChar();

/*is c a char*/
int isChar(char c);

/*is digit??*/
int isDigit(char c);

int random();

/* returns lenght of a string*/
int strlen(const char *str);

/* compares str1 with str2*/
int strcmp(const char *str1, const char *str2);

/* copies src into dest*/
char *strcpy(char *dest, const char *src);

/* copies src's into dest*/
char *strcpyForParam(char *dest, const char *src, const char *src2);

void intToStr(uint64_t value, char *str);

uint64_t strToInt(const char *str);

/*parses a num string into uint64_t*/
uint64_t charToInt(char *str);

/* prints in different bases */
void printDec(uint64_t value);
void printHex(uint64_t value);
void printBin(uint64_t value);
void printBase(uint64_t value, uint32_t base);
void printsColor(const char *str, int lenght, Color color);

int atoi(const char *str);

void triggerSpeaker(uint32_t frequence, uint64_t duration);
void playMelody(NoteType *melody, int length);

void clear_scr();
int get_scrWidth();
int get_scrHeight();
void drawRectangle(int x, int y, int x2, int y2, Color color);
void wait(uint64_t ms);
void inforeg();
void test_invopcode();
void test_zerodiv();
int print_mem(uint64_t mem);
void printcColor(char c, Color color);

void increaseScale();

void decreaseScale();

void drawCursor();

void welcome();

void registerInfo();

void cat();

void intToStr(int value, char *str);

uint64_t test_mm(uint64_t argc, char *argv[]);

#endif /* _USERLIB_H_ */