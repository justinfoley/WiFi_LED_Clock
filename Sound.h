//#include "i2s.h"
#include "wavspiffs.h"

// Non-blocking I2S write for left and right 16-bit PCM
bool ICACHE_FLASH_ATTR i2s_write_lr_nb(int16_t left, int16_t right);

void wav_stopPlaying();

bool wav_playing();

void wav_setup();

void wav_loop();

void wav_startPlayingFile(const char *wavfilename);
