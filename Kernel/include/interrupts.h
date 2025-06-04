#ifndef INTERRUPS_H_
#define INTERRUPS_H_

#include <stdint.h>

typedef uint8_t * lock_t;

void interrupt_keyboardHandler(void);
void interrupt_timerHandler(void);
void interrupt_systemCall(void);
void exception_invalidOp(void);
void exception_zeroDiv(void);

void _cli(void);

void _sti(void);

void picMasterMask(uint8_t mask);

void picSlaveMask(uint8_t mask);

// Termina la ejecución de la cpu.
void haltcpu(void);

void acquire(lock_t lock);

void release(lock_t lock);

#endif /* INTERRUPS_H_ */