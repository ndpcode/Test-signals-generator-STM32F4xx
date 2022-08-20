#ifndef COUNTER_H_
#define COUNTER_H_
//---------------------------------------------------
#include "stm32f4xx.h"

#include "math.h"
//---------------------------------------------------
extern volatile uint32_t  FrequencyMetr_1;
extern volatile uint32_t  FrequencyMetr_2;
extern volatile uint16_t  FrequencyMetr_TimeSample;
extern volatile FlagStatus DataReadyImpulse;
//---------------------------------------------------
void FrequencyMeter_InitTimer(void);
void FrequencyMeter_InitGPIO(void);
void FrequencyMeter_InitStart(void);
void FrequencyMeter_InitStop(void);

void TIM2_IRQHandler(void);
#endif /* COUNTER_H_ */
