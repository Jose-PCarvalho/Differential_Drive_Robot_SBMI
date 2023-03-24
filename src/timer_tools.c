#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <ctype.h>
#include "serial_printf.h"
#include "timer_tools.h"
#include "Inputs.h"
#define Motor1Sleep PB4
#define Motor2Sleep PB5
#define frequency 62500
extern mili_timer pause1,pause2;
extern Motor Motor1, Motor2;
void init_timer_motor1(){
  cli();
  TCNT0= 0;
  PORTB|=(1<<Motor1Sleep);
  TIFR0|=(1<<TOV0);
  TIMSK0|=(1<<TOIE0);
  TCCR0B|=(1<<CS02);
  
  sei();
}
void pause_timer_motor1(){

	TCCR0B&=~(1<<CS02);
  PORTB&=~(1<<Motor1Sleep);
  
}
void init_timer_motor2(){
  cli();
  TCNT2=0;
  PORTB|=(1<<Motor2Sleep);
  TIFR2|=(1<<TOV2);
  TIMSK2|=(1<<TOIE2);
  TCCR2B|=(1<<CS22)|(1<<CS21);
  
  sei();
}
void pause_timer_motor2(){

	TCCR2B&=~(1<<CS21); 
  TCCR2B&=~(1<<CS22) ;
  PORTB&=~(1<<Motor2Sleep);
}


void start_timer(mili_timer *timer,uint16_t ms){
    timer->ativo=1;
    timer->tempo_restante=ms;
    
}
void pause_timer(mili_timer *timer){
    timer->ativo=0;
    
}
void unpause_timer(mili_timer *timer){
    timer->ativo=1;
    
}


int32_t get_timer_time(mili_timer* timer)
{
  int32_t restante;
   ATOMIC_BLOCK(ATOMIC_FORCEON){
    restante = timer->tempo_restante;
  }
    return restante;
     
   
    
  
}
int32_t get_timer(mili_timer *timer){
    return (!(timer->tempo_restante));
}

void init100us()
{
	TCCR1B=(1<<WGM12); //modo ctc
   OCR1A=1600;
   TIMSK1=(1<<OCIE1A); //Interrupt por comparação com A
   sei(); 
   TCCR1B|=(1<<CS10); //prescaler 1
}





