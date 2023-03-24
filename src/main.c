

#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial_printf.h"
#include <string.h>
#include "IR.h"
#include <util/delay.h>
#include <stdlib.h>
#include "Inputs.h"
#include "timer_tools.h"
#include "avr/eeprom.h"
extern volatile uint64_t ticks_100microseconds;
 
 
 Motor Motor1;
 Motor Motor2;
#define Raio 32.5
#define Pi 3.14
#define Angle 0.0314
#define frequency 62500
#define MAX_RPM (int32_t) 250
#define MicroSeconds (int32_t) 60000000
#define Steps_Per_Rotation 200
#define step1 PB1
#define dir1 PB0
#define step2 PB3
#define dir2 PB2
#define Motor2M0 PC0
#define Motor2M1 PC1
#define Motor2M2 PC2
#define Motor1M0 PD7
#define Motor1M1 PD6
#define Motor1M2 PD5
#define Motor1Sleep PB4
#define Motor2Sleep PB5
#define Buzzer PD3
#define CHMINUS (uint32_t) 3125149440
#define CH (uint32_t) 3108437760
#define CHPLUS (uint32_t) 3091726080
#define PREV (uint32_t) 3141861120
#define NEXT (uint32_t) 3208707840
#define PLAY (uint32_t) 3158572800
#define MINUS (uint32_t) 4161273600
#define PLUS (uint32_t) 3927310080
#define EQ (uint32_t) 4127850240
#define ZERO (uint32_t) 3910598400
#define ONE_HUNDRED (uint32_t) 3860463360
#define TWO_HUNDRED (uint32_t) 4061003520
#define ONE (uint32_t) 4077715200
#define TWO (uint32_t) 3877175040
#define THREE (uint32_t)2707357440
#define FOUR (uint32_t) 4144561920
#define FIVE (uint32_t) 3810328320
#define SIX (uint32_t) 2774204160
#define SEVEN (uint32_t) 3175284480
#define EIGHT (uint32_t) 2907897600
#define NINE (uint32_t) 3041591040
#define Signature (unsigned char*) 0X00
#define Total_moves_pointer (unsigned char*)0x01
#define Preset_EEPROM (unsigned char*)0x05
#define Sequencia_start_pointer (uint16_t*) 0x10
uint8_t preset=5;
uint8_t MotorState=0;
uint8_t sequence_on=0;
uint8_t sequence_state=0;
uint8_t total_moves=0;
uint8_t next_move=0;
uint8_t i=0;
uint32_t sequencia[50]={0};
uint8_t total_moves_EEPROM=0;
struct{
  int faz;
}
algo;
mili_timer T1,T2,T3,T4,T5,trepeat,Buzz,pause1,pause2;

extern volatile uint64_t ticks_100microseconds;

//counts in microsecond increments
ISR(TIMER1_COMPA_vect)
{
  ticks_100microseconds++;

  if(T1.ativo && T1.tempo_restante)
  T1.tempo_restante--;
  if(T2.ativo && T2.tempo_restante)
  T2.tempo_restante--;
  if(T3.ativo && T3.tempo_restante)
  T3.tempo_restante--;
  if(T4.ativo && T4.tempo_restante)
  T4.tempo_restante--;
  if(T5.ativo && T5.tempo_restante)
  T5.tempo_restante--;
  if(trepeat.ativo && trepeat.tempo_restante)
  trepeat.tempo_restante--;
  if(Buzz.ativo && Buzz.tempo_restante){
  Buzz.tempo_restante--;
  if(Buzz.tempo_restante==0)
  PORTD&=~(1<<Buzzer);}

}

uint8_t step1_conversion=0;
ISR(TIMER0_OVF_vect){
  if(Motor1.target_steps>0){
    step1_conversion++;
    if(step1_conversion>=(2*Motor1.Step_Size)){
    step1_conversion=0;  
    Motor1.target_steps--;}
  }
  PORTB^=(1<<step1);
  TCNT0=256-Motor1.ticks;

}
uint8_t step2_conversion=0;
ISR(TIMER2_OVF_vect){
  if(Motor2.target_steps>0){
    step2_conversion++;
    if(step2_conversion>=(2*Motor2.Step_Size)){
    step2_conversion=0;  
    Motor2.target_steps--;}
  }
  PORTB^=(1<<step2);
  TCNT2=256-Motor2.ticks;

}

void controla_motor1(Motor Motor){
  Motor1_mode();
  if(Motor1.RPM>MAX_RPM)
    Motor1.RPM=MAX_RPM;
  if(MotorState==10){
    Desacelera_motor1(0);
  }
  if(MotorState==21||MotorState==31||MotorState==41||MotorState==121||MotorState==131||MotorState==141){
    Desacelera_motor1(30);
  }
  else if(MotorState==20||MotorState==30||MotorState==120||MotorState==130){
    acelera_motor1(75);
  }
  else if(MotorState==40||MotorState==140){
    acelera_motor1(50);
  }
  
  if(MotorState==1||MotorState==20 ||MotorState==40||MotorState==120 ||MotorState==140  ){
    PORTB&=~(1<<dir1);
   
    }
    else if(MotorState==2 ||MotorState==30||MotorState==130){
      PORTB|=(1<<dir1);
      
      
      
    }
   Motor1.ticks=(frequency/2)/((Motor1.RPM*Motor1.Stepping_Rate)/60);
  
   if(Motor1.ticks>254)
   Motor1.ticks=255;
  
}
void controla_motor2(Motor Motor){
   Motor2_mode();
   if(Motor2.RPM>MAX_RPM)
    Motor2.RPM=MAX_RPM;
    if(MotorState==10){
    Desacelera_motor2(0);
  }
  if(MotorState==21||MotorState==31||MotorState==51 ||MotorState==151 ||MotorState==121 ||MotorState==131){
    Desacelera_motor2(30);
  }
  else if(MotorState==20 ||MotorState==30 || MotorState==120 ||MotorState==130){
    acelera_motor2(75);

  }
  else if(MotorState==50||MotorState==150){
    acelera_motor2(50);

  }

  if(MotorState==1 ||MotorState==20 || MotorState==50||MotorState==120 || MotorState==150){
    
   PORTB|=(1<<dir2);
    }
    else if(MotorState==2 ||MotorState==30||MotorState==130){
       PORTB&=~(1<<dir2);
      
      
      
    }
   Motor2.ticks=(frequency/2)/((Motor2.RPM*Motor2.Stepping_Rate)/60);
   if(Motor2.ticks>254)
   Motor2.ticks=255;
 
}

int main(void)
{
  sei();
  preset=eeprom_read_byte(Preset_EEPROM);
 
  init100us();
  initIR();
  DDRB=(1<<step1) | (1<<dir1) | (1<<step2) | (1<<dir2) | (1<<Motor2Sleep)| (1<<Motor1Sleep);
  DDRC= (1<<Motor2M0) | (1<<Motor2M1)| (1<<Motor2M2);
  DDRD|= (1<<Motor1M0) | (1<<Motor1M1) |(1<<Motor1M2) |(1<<Buzzer);
  start_timer(&T1,10);
  start_timer(&T2,10);
  start_timer(&T3,10);
  start_timer(&trepeat,10);
  printf_init();
  //printf("INIT");
  uint32_t Comando_IR=0;
  uint32_t buffer=0;;
  uint32_t copy=buffer;
 
  


  Motor1.RPM=0;
  Motor2.RPM=0;



  while(1){
    
  
      
      
    Comando_IR=InputIR();  
    serial_input();
    if( MotorState==0 && Comando_IR==EIGHT){
       MotorState=2;
       Motor2.RPM+=preset;
       Motor1.RPM+=preset;
       init_timer_motor1();
       init_timer_motor2();
    }
    else if( MotorState==0 &&Comando_IR==TWO)
    {
      Do_buzz();
       MotorState=1;
       Motor2.RPM+=preset;
       Motor1.RPM+=preset;
       init_timer_motor1();
       init_timer_motor2();
    }
    else if( MotorState==0 && Comando_IR==CH){
      Do_buzz();
      MotorState=20;
      Motor1.target_steps=calcula_distancia(500);
      Motor2.target_steps=calcula_distancia(500);
      init_timer_motor1();
      init_timer_motor2();
      

    }
    else if( MotorState==0 && Comando_IR==NEXT){
      Do_buzz();
      MotorState=30;
      Motor1.target_steps=calcula_distancia(500);
      Motor2.target_steps=calcula_distancia(500);
      init_timer_motor1();
      init_timer_motor2();
      

    }
    else if( MotorState==0 && Comando_IR==PLAY){
      Do_buzz();
      MotorState=40;
      Motor1.target_steps=calcula_rotacao(90); 
      motor2ToggleSleep();
      init_timer_motor1();
    }
    else if( MotorState==0 && Comando_IR==PREV){
      Do_buzz();
      MotorState=50;
      Motor2.target_steps=calcula_rotacao(90); 
      motor1ToggleSleep();
      init_timer_motor2();
    }
    else if( MotorState==0 && Comando_IR==CHMINUS){
      Do_buzz();
      MotorState=100;
      sequence_on=1;
      sequence_state=0;
    }
    else if((MotorState==1 && Comando_IR==FOUR)||(MotorState==2 && Comando_IR==SIX)){
      Do_buzz();
      if((Motor1.RPM-preset)>0){
             Motor1.RPM-=preset;
           }
    }
    else if((MotorState==1 && Comando_IR==SIX)||(MotorState==2 && Comando_IR==FOUR)){
      Do_buzz();
      if((Motor2.RPM-preset)>0){
             Motor2.RPM-=preset;
           }
    }
    else if((MotorState==1 && Comando_IR==TWO)||(MotorState==2 && Comando_IR==EIGHT)){
      Do_buzz();
      if(Motor1.RPM>Motor2.RPM)
          Motor2.RPM+=preset;
      else if (Motor1.RPM<Motor2.RPM)
          Motor1.RPM+=preset;
      else if(Motor1.RPM==Motor2.RPM){
         Motor2.RPM+=preset;
         Motor1.RPM+=preset;
            } 
    }
    else if((MotorState==1 && Comando_IR==EIGHT)||(MotorState==2 && Comando_IR==TWO)){
      Do_buzz();
      if(Motor1.RPM>Motor2.RPM){
          if(preset<Motor1.RPM){
               Motor1.RPM-=preset;
              }
            }
            else if (Motor1.RPM<Motor2.RPM){
              if(preset<Motor2.RPM){
                Motor2.RPM-=preset;
              }
            }
              else if(Motor1.RPM==Motor2.RPM){
                if(preset>Motor1.RPM){
                MotorState=0;
                pause_timer_motor2();
                pause_timer_motor1();
              }
              else{
                Motor1.RPM-=preset;
                Motor2.RPM-=preset;
              }

              }

    }
    else if(Comando_IR==CHPLUS){
      Do_buzz();
      MotorState=10;
      for (i=0;i<=total_moves;i++)
      sequencia[i]=0;
      
      sequence_on=0;
      total_moves=0;
      sequence_state=0;
      next_move=0;

    }
    else if(MotorState==10 && Motor1.RPM==0 && Motor2.RPM==0){
      MotorState=0;
       pause_timer_motor2();
       pause_timer_motor1();
    }
    else if(MotorState==20 && Motor1.target_steps<195){
      MotorState=21;
    }
    else if(MotorState==21 && Motor1.target_steps==0){
      Motor1.RPM=0;
      Motor2.RPM=0;
      pause_timer_motor2();
      pause_timer_motor1();
      MotorState=0;

    }
     else if(MotorState==30 && Motor1.target_steps<195){
      MotorState=31;
    }
    else if(MotorState==31 && Motor1.target_steps==0){
      Motor1.RPM=0;
      Motor2.RPM=0;
      pause_timer_motor2();
      pause_timer_motor1();
      MotorState=0;

    }
     else if(MotorState==40 && Motor1.target_steps<100){
      MotorState=41;
    }
    else if(MotorState==41 && Motor1.target_steps==0){
      Motor1.RPM=0;
      Motor2.RPM=0;
      pause_timer_motor2();
      pause_timer_motor1();
      MotorState=0;

    }
     else if(MotorState==50 && Motor2.target_steps<100){
      MotorState=51;
    }
    else if(MotorState==51 && Motor2.target_steps==0){
      Motor1.RPM=0;
      Motor2.RPM=0;
      pause_timer_motor2();
      pause_timer_motor1();
      MotorState=0;

    }
    else if(MotorState==100 && (Comando_IR==CH || Comando_IR==NEXT || Comando_IR==PLAY || Comando_IR==PREV)){
      Do_buzz();
      if(Comando_IR==CH){
        sequencia[sequence_state]=20;
        sequence_state++;
        MotorState=101;
      }
      else if(Comando_IR==NEXT){
        sequencia[sequence_state]=30;
        sequence_state++;
        MotorState=101;
      }
      else if(Comando_IR==PLAY){
        sequencia[sequence_state]=40;
        sequence_state++;
        MotorState=101;
      }
      else if(Comando_IR==PREV){
        sequencia[sequence_state]=50;
        sequence_state++;
        MotorState=101;
      }
      
    }
    else if(MotorState==101){
      copy=recebe_numero();
      if(copy!=0){
        sequencia[sequence_state]=copy;
        sequence_state++;
        MotorState=100;
      }
    }
    else if(MotorState==100 && Comando_IR==ONE_HUNDRED){
      Do_buzz();
      total_moves=sequence_state;
      eeprom_update_byte(Total_moves_pointer,total_moves);
      for(i=0;i<=total_moves;i++){
        eeprom_update_word(Sequencia_start_pointer+2*i,sequencia[i]);
      }
      next_move=1;
      sequence_state=0;
     
       
      }
      else if(MotorState==0 && Comando_IR==ONE_HUNDRED){
      Do_buzz();
      total_moves=eeprom_read_byte(Total_moves_pointer);
      for(i=0;i<=total_moves;i++){
        sequencia[i]=eeprom_read_word(Sequencia_start_pointer+2*i);

      }
      MotorState=100;
      next_move=1;
      sequence_state=0;
     
       
      }
      
    
    
   
    else if((MotorState==100 && (next_move==1 || Comando_IR==CHMINUS))){
      if (next_move==0){
      total_moves=sequence_state;
      sequence_state=0;
      next_move=0;}
     

     if(sequencia[sequence_state]==20){
       MotorState=120;
       sequence_state++;
       Motor1.target_steps=calcula_distancia((uint32_t)(sequencia[sequence_state]*10));
       Motor2.target_steps=calcula_distancia((uint32_t)(sequencia[sequence_state]*10));
       sequence_state++;
       init_timer_motor1();
       init_timer_motor2();
     }
     else if(sequencia[sequence_state]==30){
       MotorState=130;
       sequence_state++;
       Motor1.target_steps=calcula_distancia((uint32_t)(sequencia[sequence_state]*10));
       Motor2.target_steps=calcula_distancia((uint32_t)(sequencia[sequence_state]*10));
       sequence_state++;
       init_timer_motor1();
       init_timer_motor2();
       
     }
     else if(sequencia[sequence_state]==40){
       MotorState=140;
       sequence_state++; 
       Motor1.target_steps=calcula_rotacao((uint32_t)(sequencia[sequence_state])); 
       motor2ToggleSleep();
       init_timer_motor1();
       sequence_state++;
     }
     else if(sequencia[sequence_state]==50){
       MotorState=150;
       sequence_state++;
       Motor2.target_steps=calcula_rotacao((uint32_t)(sequencia[sequence_state])); 
       motor1ToggleSleep();
       init_timer_motor2();
       sequence_state++;
     }
    }
     else if(MotorState==120 && Motor1.target_steps<195){
      MotorState=121;
    }
    else if(MotorState==121 && Motor1.target_steps==0){
      Motor1.RPM=0;
      Motor2.RPM=0;
      pause_timer_motor2();
      pause_timer_motor1();
      if(sequence_state==total_moves){
      MotorState=0;
      for (i=0;i<=total_moves;i++)
      sequencia[i]=0;
      sequence_on=0;
      total_moves=0;
      sequence_state=0;
      next_move=0;
}
      else
      {
        next_move=1;
        MotorState=100;
      }
    }
      else if(MotorState==130 && Motor1.target_steps<195){
      MotorState=131;
    }
    else if(MotorState==131 && Motor1.target_steps==0){
      Motor1.RPM=0;
      Motor2.RPM=0;
      pause_timer_motor2();
      pause_timer_motor1();
        if(sequence_state==total_moves){
      for (i=0;i<=total_moves;i++)
      sequencia[i]=0;
      MotorState=0;
      sequence_on=0;
      total_moves=0;
      sequence_state=0;
      next_move=0;
}
      else
      {
        next_move=1;
        MotorState=100;
      }

    }
      else if(MotorState==140 && Motor1.target_steps<100){
      MotorState=141;
    }
    else if(MotorState==141 && Motor1.target_steps==0){
      Motor1.RPM=0;
      Motor2.RPM=0;
      pause_timer_motor2();
      pause_timer_motor1();
       if(sequence_state==total_moves){
      MotorState=0;
      for (i=0;i<=total_moves;i++)
      sequencia[i]=0;
      sequence_on=0;
      total_moves=0;
      sequence_state=0;
      next_move=0;
}
      else
      {
        next_move=1;
        MotorState=100;
      }
    }
     else if(MotorState==150 && Motor2.target_steps<100){
      MotorState=151;
    }
    else if(MotorState==151 && Motor2.target_steps==0){
      Motor1.RPM=0;
      Motor2.RPM=0;
      pause_timer_motor2();
      pause_timer_motor1();
      if(sequence_state==total_moves){
      for (i=0;i<=total_moves;i++)
      sequencia[i]=0;
      MotorState=0;
      sequence_on=0;
      total_moves=0;
      sequence_state=0;
      next_move=0;
}
      else
      {
        next_move=1;
        MotorState=100;
      }

    }
    else if(Comando_IR==PLUS){
      Do_buzz();
      if(preset<5){
      preset++;
      eeprom_update_byte(Preset_EEPROM,preset);}
    }
    else if(Comando_IR==MINUS){
      Do_buzz();
      if(preset>1){
      preset--;
      eeprom_update_byte(Preset_EEPROM,preset);}
    }

   

    
   
    
    controla_motor1(Motor1);
    controla_motor2(Motor2);
    
    
    
    
    
    
  
     if(get_timer(&T2)){
     printf("stop %d %d ",Motor1.stop,Motor2.stop);
     printf("MOTORSTATE %d  ",MotorState);
     printf("TICKS %d %d ",Motor1.ticks,Motor2.ticks);
     printf("RPM %ld %ld ",Motor1.RPM,Motor2.RPM);
     printf("leitura %lu",copy);
     printf("next move %u seq state %u total move  %u sequencia atual %u",next_move,sequence_state,total_moves,sequencia[sequence_state]);
     printf("  %u\n",preset);
     
     
       
       start_timer(&T2,10000);
     }
  algo.faz++;
  

    // Do stuff
    
  }
  
    
}
