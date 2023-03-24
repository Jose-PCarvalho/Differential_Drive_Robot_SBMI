#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial_printf.h"
#include <string.h>
#include "IR.h"
#include <util/delay.h>
#include <stdlib.h>

uint32_t InputIR();
void serial_input();
void Init_motores();
void Motor2_mode();
void Motor1_mode();
void Desacelera_motor1(int32_t target);
void Desacelera_motor2(int32_t target);
void acelera_motor1(int32_t target);
void acelera_motor2(int32_t target);
uint32_t recebe_numero();
uint32_t calcula_distancia(uint16_t distancia);
uint32_t calcula_rotacao(uint16_t graus);
void motor1ToggleSleep();
void motor2ToggleSleep();
void Do_buzz();
typedef struct{
    uint8_t state;
    int32_t RPM;
    uint16_t ticks;
    uint8_t Step_Size;
    uint16_t Stepping_Rate;
    uint8_t stop;
    uint32_t target_steps;

}Motor;