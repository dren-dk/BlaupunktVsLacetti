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

// This maps from adc values to the remote control codes:
const struct {
	unsigned int adc;
	unsigned int code;
}adc2code[6] = {
		{1023, 0},
		{710, BLAUPUNKT_2011_MUTE},
		{532, BLAUPUNKT_2011_SEARCH},
		{356, BLAUPUNKT_2011_UP},
		{186, BLAUPUNKT_2011_VOLUMEUP},
		{0, BLAUPUNKT_2011_VOLUMEDOWN}
};

// We don't really care about unhandled interrupts.
EMPTY_INTERRUPT(__vector_default)

void delay(unsigned int us) {
	_delay_loop_2(us << 1);
}

void initADC(void){
 ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));    //16Mhz/128 = 125Khz the ADC reference clock
 ADMUX |= (1<<REFS0);                //Voltage reference from Avcc (5v)
 ADCSRA |= (1<<ADEN);                //Turn on ADC
 ADCSRA |= (1<<ADSC);                //Do an initial conversion because this one is the slowest and to ensure that everything is up and running
}

uint16_t getADC_(uint8_t channel){
 ADMUX &= 0xF0;                    //Clear the older channel that was read
 ADMUX |= channel;                //Defines the new ADC channel to be read
 ADCSRA |= (1<<ADSC);                //Starts a new conversion
 while(ADCSRA & (1<<ADSC));            //Wait until the conversion is done
 return ADCW;                    //Returns the ADC value of the chosen channel
}



unsigned int getADC(unsigned char input) {
    ADMUX = (input & 15) | _BV(REFS0); // AVcc reference + external cap.
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0) | _BV(ADIE) | _BV(ADEN);

    sei();

    if (1) {
    	ADCSRA |= 1<<ADSC;
    	while(ADCSRA & 1<<ADSC) {}
    } else {
    	SREG |= _BV(SREG_I);
    	set_sleep_mode(SLEEP_MODE_ADC);
    	sleep_enable();
    	sleep_mode();
    }

    return ADCW;
//    unsigned int result = ADCL | ADCH << 8;

//    return result;
}

unsigned int messageThrottle = 0;

unsigned int getCurrentCode() {
	unsigned int in = getADC(0);

	unsigned int best = 0;
	unsigned int bestDist = 0xffff;
	for (int i=0;i<6;i++) {
		int dist = in-adc2code[i].adc;
		if (dist < 0) {
			dist = -dist;
		}

		if (dist < bestDist) {
			best = adc2code[i].code;
			bestDist = dist;
		}
	}

	if (best != 0 || bestDist >= 10) {
		fprintf(stdout, "adc:%d code:%d dist:%d\n", in, best, bestDist);
	}

	if (bestDist < 100) {
		return best;
	} else {
		return 0;
	}
}

int main(void) {
  wdt_enable(WDTO_4S);

  initADC();
  uart_init();
  DDRC = _BV(PC1);
  PORTC |= _BV(PC1);

  fprintf(stdout, "Power up!\n");
  delay(64000);


  RemoteState rs;
/*
  char runCode = 1;
  remoteStart(&rs, &BLAUPUNKT_2011, BLAUPUNKT_2011_POWER);
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
*/


  unsigned int current = 0;
  unsigned int steady = 0;
  unsigned int activeCode = 0;
  while (1) {
	  unsigned int nc = getCurrentCode();

	  if (current == nc) {
		if (steady < 20) {
			steady++;
		}
	  } else {
		current = nc;
	  }

	  if (steady >= 3) {
		  if (activeCode != current) {

			  if (current) {
				  remoteStart(&rs, &BLAUPUNKT_2011, current);
				  fprintf(stdout, "New code: %4x\n", current);
			  }

			  activeCode = current;
		  }
	  }

	  if (activeCode) {

		  while (1) {
			  int time = remoteSignal(&rs);

			  if (time > 0) {
				  PORTC &=~ _BV(PC1);
				  delay(time);
			  } else {
				  PORTC |= _BV(PC1);
				  delay(-time);
			  }

			  if (remoteDone(&rs)) {
				  break;
			  }
		  }
	  }

	  wdt_reset();
  }
}
