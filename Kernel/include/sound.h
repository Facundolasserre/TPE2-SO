#ifndef SOUND_H
#define SOUND_H
#include <stdint.h>

void stopSpeaker();
void beep(uint32_t freq, uint64_t duration);
void triggerSpeaker(uint32_t frequence);

#endif
