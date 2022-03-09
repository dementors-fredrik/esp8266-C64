#ifndef __ESP8266C64__
#define __ESP8266C64__

#define EXECUTES_PER_RUN 100
#define RAM_SIZE 1024*20
//(19049)


union CIATimer {
  unsigned short b16;
  unsigned char b[2];
};

// Move to cpu.h
uint16_t getpc();
uint8_t getop();
void exec6502(int32_t tickcount);
void reset6502();
#endif __ESP8266C64__
