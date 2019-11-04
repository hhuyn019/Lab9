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

enum States {BEGIN, INIT, UP, DOWN, WAIT, TOGGLE} State;
double Notes[9] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25, 0};
double temp = 0.00;
//unsigned char top = 0x07;
//unsigned char bottom = 0x00;
unsigned char pos = 0x00;
unsigned char clicker = 0x00;
unsigned char tmp = 0x00;

void Tick() {
	switch(State) {
		case BEGIN:
			State = INIT;
			break;
		case INIT:
			if (input == 0x01) {
				State = UP;
				break;
			} else if (input == 0x02) {
				State = DOWN;
				break;
			} else if (input == 0x04) {
				State = TOGGLE;	
				break;
			} else {
				State = INIT;
				break;
			}
		case UP:
			temp = Notes[pos];
			State = WAIT;
			break;
		case DOWN:
			temp = Notes[pos];
			State = WAIT;
			break;
		case WAIT:
			if (input == 0x00) {
				State = INIT;
				break;
			} else {
				State = WAIT;
				break;
			}
		case TOGGLE:
			State = WAIT;
			break;
		default:
			break;
	}

	switch(State) {
		case BEGIN:
			break;
		case INIT:
			break;
		case UP:
			if ((pos+0x01) > 0x07) {
				pos = 0x07;
				break;
			} else {
				++pos;
				break;
			}
		case DOWN:
			if ((pos-0x01) < 0x00) {
				pos = 0x00;
				break;
			} else {
				--pos;
				break;
			}
		case WAIT:
			temp = Notes[pos];
			set_PWM(temp);
			break;
		case TOGGLE:
			if (clicker) {
				PWM_off();
				tmp = pos;
				pos = 0x08;
				clicker = 0x00;
				break;
			} else {
				PWM_on();
				pos = tmp;
				clicker = 0x01;
				break;
			}
		default:
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	State = BEGIN;
	PWM_on();
	clicker = 1;
    /* Insert your solution below */
    while (1) {
	Tick();
    }
}
