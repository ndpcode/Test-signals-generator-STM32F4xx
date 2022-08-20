#ifndef simpleGenerator_H_
#define simpleGenerator_H_
//---------------------------------------------------
#include "stm32f4xx.h"
#include "math.h"
#include <stdlib.h>
#include <string.h>
//-----------------DEFINEs---------------------------
#define simpleGenerator_startFrequency 36000000
#define SG_CPU_Frequency 168000000
//-----------------EXTERNAL--------------------------
void SimpleGeneratorOn				(uint32_t PWM_Frequency, uint32_t PWM_DeadTimePunkt, uint32_t PWM_Duty);
void SimpleGeneratorUpdate		(uint32_t PWM_Frequency, uint32_t PWM_DeadTimePunkt, uint32_t PWM_Duty);
void SimpleGeneratorOFF				(void);
void SimpleGeneratorInit			(void);
//-----------------INTERNAL--------------------------
void SG_SquarePWM_SignalOn					(uint32_t PWM_Frequency, uint32_t PWM_DeadTimePunkt, uint32_t PWM_Duty);
void SG_SquarePWM_SignalUpdate			(uint32_t PWM_Frequency, uint32_t PWM_DeadTimePunkt, uint32_t PWM_Duty);
void SG_Init_TIM1_SimpleGen				  (uint16_t autoreload_REG, uint16_t prescaller_REG, uint16_t PWM1_REG, uint16_t PWM2_REG, uint16_t PWM3_REG, uint16_t PWM4_REG);
void SG_FILL_TimerRegister					(uint32_t PWM_Frequency, uint32_t PWM_DeadTimePunkt, uint32_t PWM_Duty);
void SG_Hardware_DeadTime					  (uint32_t DEAD_time_count);
void SG_SquarePWM_SignalOff				(void);
void SG_Init_GPIO_Timer_1					(void);



uint16_t SG_getMaxDivider 								(uint32_t number);
uint32_t SG_SimpleGenerator_GetFrequency  (uint32_t Frequency);
//---------------------------------------------------
#endif /* simpleGenerator_H_ */
