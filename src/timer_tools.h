#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <ctype.h>
#define	CTRL_FREQ		     1000		 //	Hz
#define	PRESCALER		      1
extern volatile uint64_t ticks_100microseconds;
typedef struct {
  int32_t tempo_restante,paused,ativo;
} mili_timer;
void init_timer_ms();
void start_timer(mili_timer *timer,uint16_t ms);
int32_t get_timer_time(mili_timer* timer);
int32_t get_timer(mili_timer *timer);
void pause_timer(mili_timer *timer);
void unpause_timer(mili_timer *timer);
void init100us();
void init1ms();
void init_timer_motor1();
void init_timer_motor2();
void start_timer_motor(uint32_t STEPS_PER_MINUTE);
void pause_timer_motor1();
void pause_timer_motor2();
