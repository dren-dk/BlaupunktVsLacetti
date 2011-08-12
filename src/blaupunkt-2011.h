#ifndef BLAUPUNKT_2011_H
#define BLAUPUNKT_2011_H

#include "remote.h"

extern const Remote BLAUPUNKT_2011;

const Remote BLAUPUNKT_2011 = {
  107115,	// gap Time between each code start.
  16,	// fixedBits (aka pre_data_bits) fixed Bits in each code, max 16.
  0x41BE,	// fixed (aka pre_data) The actual fixed bits.
  16,	// bits Unique bits in each code, max 16.
  9066, 4370, //header: Signal for the header which is sent before every code
  612, 1600, //one: Signal for a binary 1
  612, 523, //zero: Signal for a binary 0
  9067, 2123, //repeat: Signal for a held button
  638	// tailMark (aka ptrail) Tail mark sent after each code
};

//Key codes:
#define BLAUPUNKT_2011_DOWN 0xE817
#define BLAUPUNKT_2011_UP 0x6897
#define BLAUPUNKT_2011_POWER 0xF00F
#define BLAUPUNKT_2011_MUTE 0x28D7
#define BLAUPUNKT_2011_GREEN 0x08F7
#define BLAUPUNKT_2011_MENU 0xA857
#define BLAUPUNKT_2011_ESC 0x708F
#define BLAUPUNKT_2011_1 0x807F
#define BLAUPUNKT_2011_VOLUMEDOWN 0xD02F
#define BLAUPUNKT_2011_5 0xA05F
#define BLAUPUNKT_2011_4 0x20DF
#define BLAUPUNKT_2011_RIGHT 0x10EF
#define BLAUPUNKT_2011_SEARCH 0xC837
#define BLAUPUNKT_2011_VOLUMEUP 0x30CF
#define BLAUPUNKT_2011_2 0x40BF
#define BLAUPUNKT_2011_LEFT 0x906F
#define BLAUPUNKT_2011_RED 0x48B7
#define BLAUPUNKT_2011_3 0xC03F

#endif
