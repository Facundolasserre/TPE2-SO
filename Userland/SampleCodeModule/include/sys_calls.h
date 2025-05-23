#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>
#include <colors.h>

/*
 * Pasaje de parametros en C:
   %rdi %rsi %rdx %rcx %r8 %r9
 */

uint64_t sys_scrWidth();

uint64_t sys_drawRectangle(int x, int y, int x2, int y2, Color color);

uint64_t sys_wait(uint64_t ms);

uint64_t sys_registerInfo(uint64_t reg[17]);

uint64_t sys_printmem(uint64_t mem);

uint64_t sys_read(uint64_t fd, char *buf);

uint64_t sys_write(uint64_t fd, const char buf);

uint64_t sys_writeColor(uint64_t fd, char buffer, Color color);

uint64_t sys_clear();

uint64_t sys_getHours();

uint64_t sys_pixelPlus();

uint64_t sys_pixelMinus();

uint64_t sys_playSpeaker(uint32_t frequence, uint64_t duration);

uint64_t sys_stopSpeaker();

uint64_t sys_getMinutes();

uint64_t sys_getSeconds();

uint64_t sys_scrHeight();

uint64_t sys_drawCursor();

#endif
