/*	Author: hhuyn019
 *  Partner(s) Name: Badr Asi
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#define input (~PINA & 0x07)

void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }
	
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		else if (frequency > 31250) { OCR3A = 0x0000; }
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum States {BEGIN, INIT, ON, OFF) State;

void Tick() {
	switch(State) {
		case BEGIN:
			State = INIT;
			break;
		case INIT:
			if (input  != 0x00) {
				State = ON;
				break;
			} else {
				State = INIT;
				break;
			}
		case ON:
			if (input != 0x00) {
				State = ON;
				break;
			} else if ((input == 0x03) || (input == 0x05) || (input == 0x06) || (input == 0x07)) {
				State = OFF;
				break;
			} else {
				State = OFF;
				break;
			}
		case OFF:
			State = INIT;
			break;
		default:
			break;
	}

	switch(State) {
		case BEGIN:
			break;
		case INIT:
			set_PWM(0);
			break;
		case ON:
			if (input == 0x01) {
				set_PWM(261.63);
				break;
			} else if (input == 0x02) {
				set_PWM(293.66);
				break;
			} else if (input == 0x04) {
				set_PWM(329.63);
				break;
			}
		case OFF:
			set_PWM(0);
			break;
		default:
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	State = BEGIN;
	PWM_on();
    /* Insert your solution below */
    while (1) {
	Tick();
    }
}
