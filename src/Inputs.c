#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial_printf.h"
#include "timer_tools.h"
#include <string.h>
#include "IR.h"
#include "Inputs.h"
#include <util/delay.h>
#include <stdlib.h>
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
#define Motor1Sleep PB4
#define Motor2Sleep PB5
#define MAX_RPM (int32_t)500
#define Base_Stepping_Rate 200
#define Motor2M0 PC0
#define Motor2M1 PC1
#define Motor2M2 PC2
#define Motor1M0 PD7
#define Motor1M1 PD6
#define Motor1M2 PD5
#define Buzzer PD3
#define Motor1Sleep PB4
#define Motor2Sleep PB5
#define Raio 31.6
#define Pi 3.14
#define Raio_Rot 142.6
#define Angle 0.031416
#define Circ_Rotation Raio_Rot*2*Pi
extern mili_timer trepeat;
extern uint8_t preset;

extern Motor Motor1; //motor esquerdo
extern Motor Motor2; //motor direito
extern mili_timer Buzz;

 uint8_t preset2=10;


 uint32_t InputIR(){
    static uint32_t copy=0;
    uint8_t new=0;
    uint32_t buffer;
    static uint8_t repeat=0;
    static uint8_t old_repeat=0;
    repeat=read_repeat();
    
    buffer=readIR();
    
     
    if(buffer!=0 && buffer!=copy){
        copy=buffer;
        new=1;    
    }
     if(old_repeat!=repeat && repeat!=0){        

      new=1;}
      if (new){
         old_repeat=repeat;
         
        return copy;
      }
      
      old_repeat=repeat;
    
    
    
     return 0;
 }
    


void serial_input(){
    static char string[10]={0};
    static uint8_t serial_data;
   static int pos=0;
    

    
 
if (serial_receive_ready()){
    serial_data = serial_receive(); 
      if(serial_data=='.'){
        string[pos]=serial_data;
        string[pos+1]='\0';
        
        
        
      }
      else{
        string[pos]=serial_data;
        pos++;
      }
    

  }
     
       if(strcmp(string,"P1")==0){
        Motor1.RPM=0;
        string[0]='\0';
        while(pos!=0){
         pos--;
         string[pos]='\0';
       }
      }
       
      
      else if(strcmp(string,"P2")==0){
       Motor2.RPM=0;
        string[0]='\0';
           while(pos!=0){
         pos--;
         string[pos]='\0';
       }
         
      }
      else if(string[pos]=='.'){
        
        string[0]='\0';
        while(pos!=0){
         pos--;
         string[pos]='\0';
       }
             
      }         
         
      
      else if(strcmp(string,"8")==0){
        if(Motor1.RPM>Motor2.RPM){
          Motor2.RPM+=preset2;
        }
        if(Motor1.RPM<Motor2.RPM){
          Motor1.RPM+=preset2;
        }
        if(Motor1.RPM==Motor2.RPM){
          Motor1.RPM+=preset2;
          Motor2.RPM+=preset2;
        }
        string[0]='\0';
        while(pos!=0){
         pos--;
         string[pos]='\0';
        }    
      }
      else if(strcmp(string,"2")==0){
        if(Motor1.RPM>Motor2.RPM){
          Motor1.RPM-=preset2;
        }
        if(Motor1.RPM<Motor2.RPM){
          Motor2.RPM-=preset2;
        }
        if(Motor1.RPM==Motor2.RPM){
          Motor1.RPM-=preset2;
          Motor2.RPM-=preset2;
        }
        string[0]='\0';
        while(pos!=0){
         pos--;
         string[pos]='\0';
        }    
      }
      else if(strcmp(string,"4")==0){
      if(Motor1.RPM>0)
       Motor1.RPM-=preset2;
        string[0]='\0';
        while(pos!=0){
         pos--;
         string[pos]='\0';
        }    
      }
      else if(strcmp(string,"6")==0){
      if(Motor2.RPM>0)
       Motor2.RPM-=preset2;
        string[0]='\0';
        while(pos!=0){
         pos--;
         string[pos]='\0';
        }    
      }
    
      if(Motor1.RPM>MAX_RPM){
        Motor1.RPM=MAX_RPM;
      }
      if(Motor2.RPM>MAX_RPM){
        Motor2.RPM=MAX_RPM;
      }
      if(Motor1.RPM<(-1*MAX_RPM)){
        Motor1.RPM=-MAX_RPM;
      }
       if(Motor2.RPM<(-1*MAX_RPM)){
        Motor2.RPM=-MAX_RPM;
      }

}
extern uint8_t step1_conversion;
 void Motor1_mode(){
   static uint8_t old_step;
  if( Motor1.RPM>313){
    Motor1.Step_Size=1;
    if(old_step!=Motor1.Step_Size){
      PORTD&=~(1<<Motor1M0) & ~(1<<Motor1M1) & ~(1<<Motor1M2);

    }

  }
  else if(Motor1.RPM>157){
    Motor1.Step_Size=2;
    if(old_step!=Motor1.Step_Size){
    PORTD&= ~(1<<Motor1M1) & ~(1<<Motor1M2);
    PORTD|=(1<<Motor1M0);}


  }
  else if(Motor1.RPM>79){
    Motor1.Step_Size=4;
    if(old_step!=Motor1.Step_Size){
    PORTD&= ~(1<<Motor1M0) & ~(1<<Motor1M2);
    PORTD|=(1<<Motor1M1);}

  }
  else {
    Motor1.Step_Size=8;
    if(old_step!=Motor1.Step_Size){
    PORTD&= ~(1<<Motor1M2);
    PORTD|=(1<<Motor1M1)|(1<<Motor1M0);}

  }
  old_step=Motor1.Step_Size;
  Motor1.Stepping_Rate=Base_Stepping_Rate*Motor1.Step_Size;
 }
 extern uint8_t step2_conversion;
 void Motor2_mode(){
    static uint8_t old_step_2=0;
  if( Motor2.RPM>313){
    Motor2.Step_Size=1;
     if(old_step_2!=Motor2.Step_Size){    
      PORTC&=~(1<<Motor2M0) & ~(1<<Motor2M1) & ~(1<<Motor2M2);

    }
    

  }
  else if(Motor2.RPM>157){
     Motor2.Step_Size=2;
     if(old_step_2!=Motor2.Step_Size){  
    PORTC&= ~(1<<Motor2M1) & ~(1<<Motor2M2);
    PORTC|=(1<<Motor2M0);}

  }
  else if(Motor2.RPM>79){
     Motor2.Step_Size=4;
      if(old_step_2!=Motor2.Step_Size){    
      PORTC&= ~(1<<Motor2M0) & ~(1<<Motor2M2);
      PORTC|=(1<<Motor2M1);}

  }

  
  else {
     Motor2.Step_Size=8;
     if(old_step_2!=Motor1.Step_Size){ 
      PORTC&= ~(1<<Motor2M2);
      PORTC|=(1<<Motor2M1)|(1<<Motor2M0);}
    
  }
  Motor2.Stepping_Rate=Base_Stepping_Rate*Motor2.Step_Size;
 }
 void Init_motores(){
   Motor1.RPM=0;
   Motor1.state=0;
   Motor1.Step_Size=8;
   Motor1.Stepping_Rate=8*200;
   Motor1.stop=1;
   Motor1.ticks=0;
   Motor2.RPM=0;
   Motor2.state=0;
   Motor2.Step_Size=8;
   Motor2.Stepping_Rate=8*200;
   Motor2.stop=1;
   Motor2.ticks=0;
 }
 extern mili_timer T1,T3,T5,T4;
 void Desacelera_motor1(int32_t target){
   if(get_timer(&T1)){
     if (Motor1.RPM>target){
     Motor1.RPM--;
    start_timer(&T1,500/preset);}
   }
   
 }
 void Desacelera_motor2(int32_t target){
   if(get_timer(&T3)){
     if (Motor2.RPM>target){
     Motor2.RPM--;
    start_timer(&T3,500/preset);}
   }
   
 }
  void acelera_motor1(int32_t target){
   if(get_timer(&T4)){
     if (Motor1.RPM<target){
     Motor1.RPM++;
    start_timer(&T4,1000/preset);}
   }
   
 }
 void acelera_motor2(int32_t target){
   if(get_timer(&T5)){
     if (Motor2.RPM<target){
     Motor2.RPM++;
    start_timer(&T5,1000/preset);}
   }
   
 }
 uint32_t recebe_numero(){
   static char string[100]={0};
   static uint8_t pos=0;
    static uint32_t numero=0;
    uint8_t i=0;
    uint8_t new=0;
    uint32_t buffer;
     uint8_t repeat=0;
    static uint8_t old_repeat=0;
    
    
    buffer=readIR();
    repeat=read_repeat();
    if(buffer!=0 && buffer!=numero&& repeat!=0){
        numero=buffer;
        new=1;    
    }
    if(old_repeat!=repeat && repeat==1)
     new=1;
    
     
  
   if(new){
   if(numero==ONE){
    Do_buzz();
     string[pos]='1';
     pos++;

   }
   else if(numero==TWO){
     Do_buzz();
     string[pos]='2';
     pos++;
     

   }
   else if(numero==THREE){
     Do_buzz();
     string[pos]='3';
     pos++;

   }
   else if(numero==FOUR){
     Do_buzz();
     string[pos]='4';
     pos++;

   }
   else if(numero==FIVE){
     Do_buzz();
     string[pos]='5';
     pos++;

   }
   else if(numero==SIX){
     Do_buzz();
     string[pos]='6';
     pos++;

   }
    else if(numero==SEVEN){
      Do_buzz();
     string[pos]='7';
     pos++;

   }
   else if(numero==EIGHT){
     Do_buzz();
     string[pos]='8';
     pos++;

   }
   else if(numero==NINE){
     Do_buzz();
     string[pos]='9';
     pos++;

   }
   else if(numero==ZERO){
     Do_buzz();
     string[pos]='0';
     pos++;

   }
   else if(numero==EQ){
     Do_buzz();
     string[pos]='f';
     pos++;
     int32_t resultado=strtoul(string,NULL,10);
     for(i=0;i<pos;i++){
       string[i]=0;

     }
     pos=0;
     old_repeat=repeat;
     return resultado;
   }
   else if(numero==MINUS){
     Do_buzz();
     pos++;
     for(i=0;i<pos;i++){
       string[i]=0;

     }
     pos=0;
   }
   }
   old_repeat=repeat;
   return 0;


 }


 uint32_t calcula_distancia(uint16_t distancia){
   double passos;
   passos= (double) distancia/(Angle*Raio);
   uint32_t res;
   res=(uint32_t) passos;
   return res;


 }
 uint32_t calcula_rotacao(uint16_t graus){
   double passos;
   double distancia;
   distancia= Raio_Rot*(((double) graus)*Pi/180.0);

   passos= (double) distancia/(Angle*Raio);
   uint32_t res;
   res=(uint32_t) passos;
   return res;


 }
 void motor1ToggleSleep(){
   PORTB^=(1<<Motor1Sleep);
 }
 void motor2ToggleSleep(){
   PORTB^=(1<<Motor2Sleep);
 }
 void Do_buzz(){
   PORTD|=(1<<Buzzer);
   start_timer(&Buzz,200);
 }
 