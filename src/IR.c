#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <ctype.h>
#include "timer_tools.h"
#include "serial_printf.h"
#include "IR.h"
#include <stdio.h>
#define REST 0
#define START 1
#define READING 2
#define START_BURST 130
#define REPEAT_TIME 110
#define RECEBE_1 20
#define RECEBE_0 9
#define PACKET_TIME 1150

 volatile uint64_t ticks_100microseconds;

struct
{
	uint8_t state;
	uint64_t timePassed;
	uint64_t timeStart;
	uint32_t buffer;
	uint8_t shifts;
	uint8_t finished:1;
	uint8_t repeat;
	
	
} ir;


//will setup counters, and pin interrupt
void initIR()
{
	uint8_t tmpSREG = 0;

	tmpSREG = SREG;
	cli();

	//setup initial values for IR_DATA
	ir.state = REST;
	ir.buffer = 0;
	ir.timePassed = 0;
	ir.timeStart = 0;
	ir.shifts = 0;
	ir.repeat =0;
	//setup change of pin state interrupt
	DDRD&=~(1<<PD2); 
    PORTD|=(1<<PD2); 
	EICRA|=(1<<ISC00); 
	EIMSK|=(1<<INT0);

	SREG = tmpSREG;

	sei();
}
uint32_t readIR(){
	
	if(ir.finished){
		
	
		return ir.buffer;
	}
	return 0;
}
uint8_t read_repeat(){
	return ir.repeat;
}
 ISR(INT0_vect)
{
 uint8_t pinState = (PIND & (1 << PD2));


	
	if (pinState == 0)
	{
		ir.timePassed = ticks_100microseconds - ir.timeStart;

		if(ir.timePassed > PACKET_TIME)
		{
			ir.shifts = 0;
			ir.repeat = 0;
			ir.state = REST;
		}

		
		
		if(ir.state==REST){
			ir.state = START;
			ir.timeStart = ticks_100microseconds;

		}
		else if(ir.state==READING)
		ir.timeStart = ticks_100microseconds;
	}
	else
	{	

		if(ir.state==START){
			ir.state = READING;
			
			}
		
		else if(ir.state==READING){
			if(ir.timePassed > START_BURST)
			{
				ir.finished=0;
				ir.repeat = 0;
			}
			else if (ir.timePassed > REPEAT_TIME)
			{
				ir.repeat++;
				ir.finished=1;
				ir.state = REST;
			}
			else if (ir.timePassed > RECEBE_1)
			{
				
				ir.buffer |= ((uint32_t)1 << ir.shifts);
				ir.shifts++;
			}
			else if (ir.timePassed > RECEBE_0)
			{
				ir.buffer &= ~((uint32_t)1 << ir.shifts);
				ir.shifts++;
			}
			if(ir.shifts >= 32)
			{
				ir.shifts = 0;
				ir.finished=1;
				ir.repeat=0;
				ir.state = REST;
			}
				
			}
	}
}