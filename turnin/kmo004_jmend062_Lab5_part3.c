/*
 * CS122A_Lab05_Part2.c
 *
 * Created: 10/23/2019 10:44:21 AM
 * Author : Jason Mendoza
 */ 

#include <avr/io.h>
#include "timer.h"
unsigned char shift;
unsigned char forward;
unsigned char converge;
unsigned char infinite;
unsigned char go;
unsigned char go2;

//Ensure DDRC is set up as output
void transmit_data(unsigned char data, unsigned char shiftnum){
	unsigned char complete;
	unsigned char SER;
	if(shiftnum == 0){
		PORTC = 0x01;
		complete = 0x01;
		for(int i=0; i<=7;i++){
			complete = complete & 0xFD;
			PORTC = complete;
			SER = data & 0x01;
			if(SER == 0x01){
				PORTC = complete | 0x08;
			}
			else{
				PORTC = complete & 0xF7;
			}
			PORTC = complete | 0x02;
			data = data >> 1;
		}
		PORTC = complete | 0x04;
		PORTC = 0x00;
		complete = 0x00;
	}
	if(shiftnum == 1){
		PORTD = 0x01;
		complete = 0x01;
		for(int i=0; i<=7;i++){
			complete = complete & 0xFD;
			PORTD = complete;
			SER = data & 0x01;
			if(SER == 0x01){
				PORTD = complete | 0x08;
			}
			else{
				PORTD = complete & 0xF7;
			}
			PORTD = complete | 0x02;
			data = data >> 1;
		}
		PORTD = complete | 0x04;
		PORTD = 0x00;
		complete = 0x00;
	}
}


enum pattern1 {Init, Up, Down} p1state;

void p1tick(){
	switch(p1state){
		case Init:
			shift = 0x80;
		break;
		case Up:
			shift = shift << 1;
		break;
		case Down:
			shift = shift >> 1;
		break;
	}	
	switch(p1state){
		case Init:
			p1state = Down;	
		break;
		case Up:
			if(shift < 0x80){
				p1state = Up;
			}
			else{
				p1state = Down;
			}
		break;
		case Down:
			if(shift > 0x01){
				p1state = Down;
			}
			else{
				p1state = Up;
			}
		break;
	}
}

enum pattern2 {zero, one, two, three, four} p2state;

	
void p2tick(){
	switch (p2state){
		case zero:
			p2state = one;
			forward = 0x01;
		break;
		case one:
			if(forward != 0x01){
				p2state = zero;
			}
			else {
				p2state = two;
			}
		break;
		case two:
			if(forward != 0x01){
				p2state = one;
			}
			else {
				p2state = three;
			}
		break;
		case three:
			if(forward != 0x01){
				p2state = two;
			}
			else {
				p2state = four;
			}
		break;
		case four:
			p2state = three;
			forward = 0x00;
		break;
			
	}
	switch(p2state){
		case zero:
			converge = 0x00;
		break;
		case one:
			converge = 0x81;
		break;
		case two:
			converge = 0xC3;
		break;
		case three:
			converge = 0xE7;
		break;
		case four:
			converge = 0xFF;
		break;
		
	}
}

enum pattern3 {init, edge} p3state;
	
void p3tick(){
	switch(p3state){
		case init:
			infinite = 0x55;
		break;
		case edge:
			if(infinite == 0x55){
				infinite = 0xAA;
			}
			else{
				infinite = 0x55;
			}
		break;
	}
	switch(p3state){
		case init:
			p3state = edge;
		break;
		case edge:
			p3state = edge;
		break;
	}
}


enum show{start, p1, p2, p3, off, p1_wait, off_wait } sstate;

void showtick1(){
	unsigned char button1 = (~PINA & 0x01);
	unsigned char button2 = (~PINA & 0x02);
	switch(sstate){
		case start:
		sstate = off;
		break;
		case off_wait:
		if(button1 || button2){
			sstate = off_wait;
		}
		else{
			sstate = off;
		}
		break;
		case off:
		if(button1 && button2){
			sstate = p1;
		}
		break;
		case p1_wait:
		if(button1 && button2){
			sstate = p1_wait;
		}
		else{
			sstate = p1;
		}
		case p1:
		if(button1 && button2){
			sstate = off_wait;
		}
		else if( button1 && !button2){
			sstate = p2;
		}
		else if ( !button1 && button2 ){
			sstate = p3;
		}
		else{
			sstate = p1;
		}
		break;
		case p2:
		if(button1 && button2){
			sstate = off_wait;
		}
		else if( button1 && !button2){
			sstate = p3;
		}
		else if ( !button1 && button2 ){
			sstate = p1;
		}
		else{
			sstate = p2;
		}
		break;
		case p3:
		if(button1 && button2){
			sstate = off_wait;
		}
		else if( button1 && !button2){
			sstate = p1;
		}
		else if ( !button1 && button2 ){
			sstate = p2;
		}
		else{
			sstate = p3;
		}
		break;
	}
	switch(sstate){
		case start:
		
		break;
		case off_wait:
		go = 0x00;
		break;
		case off:
		go = 0x00;
		break;
		case p1_wait:
		go = shift;
		break;
		case p1:
		go = shift;
		break;
		case p2:
		go = converge;
		break;
		case p3:
		go = infinite;
		break;
	}
}


enum show2{start2, p4, p5, p6, off1, p4_wait, off1_wait } sstate2;

void showtick2(){
	unsigned char button3 = (~PINA & 0x04);
	unsigned char button4 = (~PINA & 0x08);
	switch(sstate2){
		case start2:
		sstate2 = off1;
		break;
		case off1_wait:
		if(button3 || button4){
			sstate2 = off1_wait;
		}
		else{
			sstate2 = off1;
		}
		break;
		case off1:
		if(button3 && button4){
			sstate2 = p4_wait;
		}
		break;
		case p4_wait:
		if(button3 || button4){
			sstate2 = p4_wait;
		}
		else{
			sstate2 = p4;
		}
		case p4:
		if(button3 && button4){
			sstate2 = off1_wait;
		}
		else if( button3 && !button4){
			sstate2 = p5;
		}
		else if ( !button3 && button4 ){
			sstate2 = p6;
		}
		else{
			sstate2 = p4;
		}
		break;
		case p5:
		if(button3 && button4){
			sstate2 = off1_wait;
		}
		else if( button3 && !button4){
			sstate2 = p6;
		}
		else if ( !button3 && button4 ){
			sstate2 = p4;
		}
		else{
			sstate2 = p5;
		}
		break;
		case p6:
		if(button3 && button4){
			sstate2 = off1_wait;
		}
		else if( button3 && !button4){
			sstate2 = p4;
		}
		else if ( !button3 && button4 ){
			sstate2 = p5;
		}
		else{
			sstate2 = p6;
		}
		break;
	}
	switch(sstate2){
		case start2:
		
		break;
		case off1_wait:
		go2 = 0x00;
		break;
		case off1:
		go2 = 0x00;
		break;
		case p4_wait:
		go2 = shift;
		break;
		case p4:
		go2 = shift;
		break;
		case p5:
		go2 = converge;
		break;
		case p6:
		go2 = infinite;
		break;
	}
}

int main(void)
{
    DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
    DDRA = 0x00; PORTA = 0xFF;
    
    TimerSet(100);
    TimerOn();
	
	p1state = Init;
	p2state = zero;
	p3state = init;
	
    while (1) 
    {
		p1tick();
		p2tick();
		p3tick();
		showtick1();
		showtick2();
		transmit_data(go,0);
		transmit_data(go2,1);
		while(!TimerFlag){}
		TimerFlag = 0;
		
    }
}


