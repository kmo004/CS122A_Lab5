/*	Author: kmo004
 *  Partner(s) Name: Jason Mendoza
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
#include "timer.h"
#endif

void transmit_data(unsigned char data){
	unsigned char output;
	unsigned char SER;
	unsigned char i;
	PORTC = 0x01;
	output = 0x01;
	
	for ( i = 0; i <= 7; i++){
		output = output & 0xFD;
		PORTC = output;
		SER = data & 0x01;
		if(SER == 0x01){
			PORTC = output | 0x08;
		}
		else{
			PORTC = output & 0xF7;
		}
		PORTC = output | 0x02;
		data = data >> 1;
	}
	PORTC = output | 0x04;
	PORTC = 0x00;
	output = 0x00;
		
}
unsigned char light = 0x00;
enum states{wait,inc,dec,reset} state;
void Tick(){
	switch(state){
		case wait:
		
			if((~PINA & 0x03) == 0x03){
				state = reset;
			}
			else if((~PINA & 0x01) == 0x01){
				state = inc;
			}
			else if((~PINA & 0x02) == 0x02){
				state = dec;
			}
			else{
				state = wait;
			}
			break;
		case inc:
			state = wait;
			break;
		case dec:
			state = wait;
			break;
		case reset:
			state = wait;
			break;
	}
	switch(state){
		case wait:
		break;
		
		case inc:
			if(light < 0xFF){
				light = light + 0x01;
			}
		break;
		
		case dec:
			if(light > 0x00){
				light = light - 0x01;
			}
		break;
		
		case reset:
			light = 0x00;
		break;
	}
}
int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xFF; PORTC = 0x07;
    TimerSet(100);
    TimerOn();
    state = wait;
    light = 0x00;

    /* Insert your solution below */
    while (1) {
		Tick();
		/*
		if((~PINA & 0x01) == 0x01){
			if(light < 0xFF){
				light = light + 1;
			}
		}
		else if ((~PINA & 0x02) == 0x02){
			if(light > 0x00)
			light = light - 1;
		}
		*/
		transmit_data(light);
		
		while (!TimerFlag){}
		TimerFlag = 0;
    }
    return 1;
}
