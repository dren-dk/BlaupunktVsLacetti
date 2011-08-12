#include <ctype.h>
#include <inttypes.h>

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <util/twi.h>
#include <avr/sleep.h>

#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <util/delay_basic.h>

#include "uart.h"
#include "blaupunkt-2011.h"

#define MIN_DELAY 18

void delay(unsigned int us) {
	_delay_loop_2(us << 1);
}

int main(void) {
  wdt_enable(WDTO_4S);

  uart_init();
  DDRC = _BV(PC1);
  PORTC |= _BV(PC1);

  fprintf(stdout, "Power up!\n");
  delay(64000);
  fprintf(stdout, "Done sleeping...\n");


  RemoteState rs;

  char runCode = 1;
  remoteStart(&rs, &BLAUPUNKT_2011, BLAUPUNKT_2011_POWER);
/*
  fprintf(stdout, "Done starting code...\n");
  while (!remoteDone(&rs)) {
    int time = remoteSignal(&rs);
    if (time < 0) {
    	fprintf(stdout, "Space: %4d  state:%d  codestate:%d\n", -time, rs.state, rs.codeState);
    } else {
    	fprintf(stdout, "Mark:  %4d  state:%d  codestate:%d\n", time, rs.state, rs.codeState);
    }
  }
  fprintf(stdout, "Done printing codes...\n");
*/
  while (1) {

	  if (runCode) {
		  int time = remoteSignal(&rs);

		  if (time > 0) {
			  PORTC &=~ _BV(PC1);
			  delay(time);
		  } else {
			  PORTC |= _BV(PC1);
			  delay(-time);
		  }

		  if (remoteDone(&rs)) {
			  runCode = 0;
		  }
	  }

	  wdt_reset();
  }
}
