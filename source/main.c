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

enum states{l0,l1,l2} state;
void Tick(){
	switch(state){
		case l0:
			if(light == 0x55){
				state = l2;
			}
			else{
				state = l1;
			}
			break;
		case l1:
			state = l0;
			break;
		case l2:
			state = l0;
			break;
	}
	switch(state){
		case l0:
		break;
		
		case l1:
			light = 0x55;
		break;
		
		case l2:
			light = 0xAA;
		break;
	}
}

enum states1{l10,l11,l12} state1;
void Tick1(){
	switch(state1){
		case l10:
			if(light == 0xFF){
				state1 = l12;
			}
			else{
				state1 = l11;
			}
			break;
		case l11:
			state1 = l10;
			break;
		case l12:
			state1 = l10;
			break;
	}
	switch(state1){
		case l10:
		break;
		
		case l11:
			light = 0xFF;
		break;
		
		case l12:
			light = 0x00;
		break;
	}
}

enum states2{l20,l21,l22} state2;
void Tick2(){
	switch(state2){
		case l20:
			if(light == 0x0F){
				state2 = l22;
			}
			else{
				state2 = l21;
			}
			break;
		case l21:
			state2 = l20;
			break;
		case l12:
			state2 = l20;
			break;
	}
	switch(state2){
		case l20:
		break;
		
		case l21:
			light = 0x0F;
		break;
		
		case l22:
			light = 0xF0;
		break;
	}
}
int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xFF; PORTC = 0x07;
    TimerSet(100);
    TimerOn();
    state = l0;
    state1 = l10;
    state2 = l20;
    light = 0x0F;
    unsigned char cycle = 0x03;
    unsigned char sys = 0x00;

    /* Insert your solution below */
    while (1) {
		if((~PINA & 0x03) == 0x03){
			if(sys){
				sys = 0;
			}
			else{
				sys = 1;
			}
		}
		else if((~PINA & 0x01) == 0x01){
			if(cycle == 0x01 | cycle == 0x02){
				cycle = cycle + 0x01;
			}
			else{
				cycle = 0x01;
			}
		}
		else if((~PINA & 0x02) == 0x02){
			if(cycle == 0x03 | cycle == 0x02){
				cycle = cycle - 0x01;
			}
			else{
				cycle = 0x03;
			}
		}
		
		if(cycle == 0x01){
			Tick();
		}
		else if(cycle == 0x02){
			Tick1();
			
		}
		else if(cycle == 0x03){
			Tick2();
			
		}
		if(sys){
			transmit_data(light);
		}
		else{
			transmit_data(0x00);
		}
		
		while (!TimerFlag){}
		TimerFlag = 0;
    }
    return 1;
}
