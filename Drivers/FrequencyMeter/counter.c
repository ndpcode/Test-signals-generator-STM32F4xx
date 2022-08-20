#include "counter.h"
//--------------------------------------------------------
 volatile uint32_t  FrequencyMetr_1;
 volatile uint32_t  FrequencyMetr_2;
 volatile uint16_t  FrequencyMetr_TimeSample = 10000;
 
 volatile FlagStatus DataReadyImpulse;
//--------------------------------------------------------
void FrequencyMeter_InitTimer(void)
{
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN | RCC_APB1ENR_TIM5EN;
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN | RCC_APB2ENR_TIM8EN;

		//------------------------------------------------------------------------------------------
		//TIM2 master counter, period = 1sec.
		//------------------------------------------------------------------------------------------
		TIM2->PSC       = 8400 - 1; //new clock = 10kHz
		TIM2->ARR       = FrequencyMetr_TimeSample - 1; //period = 1sec
		TIM2->CR1       |= TIM_CR1_DIR; //used as downcounter
		TIM2->CR1       |= TIM_CR1_OPM; //counter stops counting at the next update event(clearing the bit CEN) отключение таймера по переполнению, сброс CEN
		TIM2->CR2       |= TIM_CR2_MMS_0; //COUNTER_ENABLE signal to TIM1, used as trigger output (TRGO)
		TIM2->DIER      |= TIM_DIER_UIE; //Update interrupt enabled

		//------------------------------------------------------------------------------------------
		//TIM8 slave counter, TIM8_ETR pin is input
		//------------------------------------------------------------------------------------------
		TIM8->PSC       = 0;
		TIM8->CR1       &= ~TIM_CR1_DIR; //used as upcounter
		TIM8->CR1       &= ~TIM_CR1_OPM; //is not stopped at update event
		TIM8->CR2       |= TIM_CR2_MMS_1; //update event is selected as trigger output to TIM4
		TIM8->SMCR      |= TIM_SMCR_ECE; //ext. clock mode2 enabled, counter is clocked by ETRF signal
		TIM8->SMCR      &= ~TIM_SMCR_ETPS; //no external trigger prescaller
		TIM8->SMCR      &= ~TIM_SMCR_ETF; //no external trigger filter

		TIM8->SMCR      |= TIM_SMCR_TS_0; //internal trigger_1 (ITR1) from TIM2

		TIM8->SMCR      |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_2; //while trigger input (TRGI) is high, counter is on

		//------------------------------------------------------------------------------------------
		//TIM4 slave counter, TIM4_ETR pin is input
		//------------------------------------------------------------------------------------------
		TIM4->PSC       = 0;
		TIM4->CR1       &= ~TIM_CR1_DIR; //used as upcounter
		TIM4->CR1       &= ~TIM_CR1_OPM; //is not stopped at update event
		TIM4->CR2       |= TIM_CR2_MMS_1; //update event is selected as trigger output to TIM4
		TIM4->SMCR      |= TIM_SMCR_ECE; //ext. clock mode2 enabled, counter is clocked by ETRF signal
		TIM4->SMCR      &= ~TIM_SMCR_ETPS; //no external trigger prescaller
		TIM4->SMCR      &= ~TIM_SMCR_ETF; //no external trigger filter

		TIM4->SMCR      |= TIM_SMCR_TS_0; //internal trigger_1 (ITR1) from TIM2

		TIM4->SMCR      |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_2; //while trigger input (TRGI) is high, counter is on

		//------------------------------------------------------------------------------------------
		//TIM5 additional counter for TIM8
		//------------------------------------------------------------------------------------------
		TIM5->PSC       = 0;
		TIM5->CR1       &= ~TIM_CR1_DIR; //used as upcounter
		TIM5->SMCR      &= ~TIM_SMCR_ETPS; //no external trigger prescaller
		TIM5->SMCR      &= ~TIM_SMCR_ETF; //no external trigger filter
		TIM5->SMCR      |= TIM_SMCR_TS_1 | TIM_SMCR_TS_0; //internal trigger_3 (ITR3) from TIM8
		TIM5->SMCR      |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_2; //TIM1 (TRGI) clock the counter

		//------------------------------------------------------------------------------------------
		//TIM3 additional counter for TIM4
		//------------------------------------------------------------------------------------------
		TIM3->PSC       = 0;
		TIM3->CR1       &= ~TIM_CR1_DIR; //used as upcounter
		TIM3->SMCR      &= ~TIM_SMCR_ETPS; //no external trigger prescaller
		TIM3->SMCR      &= ~TIM_SMCR_ETF; //no external trigger filter
		TIM3->SMCR      |= TIM_SMCR_TS_1 | TIM_SMCR_TS_0; //internal trigger_3 (ITR3) from TIM4
		TIM3->SMCR      |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_2; //TIM1 (TRGI) clock the counter
}				
void FrequencyMeter_InitGPIO(void)
{
		//------------------------------------------------------------------------------------------
		//GPIO PE0 Input AF TIM4 ___Freq_1
		//------------------------------------------------------------------------------------------
		/*
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
		GPIO_InitTypeDef initFrequencyMeterGpio1;
		initFrequencyMeterGpio1.GPIO_Mode = GPIO_Mode_AF;
		initFrequencyMeterGpio1.GPIO_OType = GPIO_OType_PP;
		initFrequencyMeterGpio1.GPIO_Pin = GPIO_Pin_0;
		initFrequencyMeterGpio1.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(GPIOE,&initFrequencyMeterGpio1);
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource0,GPIO_AF_TIM4);
		*/
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	
		GPIOE->MODER  &= ~(GPIO_MODER_MODER0_0); 
		GPIOE->MODER  |=   GPIO_MODER_MODER0_1; 	
		GPIOE->OSPEEDR|= 	 GPIO_OSPEEDER_OSPEEDR0_0;
		GPIOE->OSPEEDR|= 	 GPIO_OSPEEDER_OSPEEDR0_1;
		GPIOE->AFR[0] &= ~(GPIO_AFRL_AFRL0_0); 
		GPIOE->AFR[0] |=  (GPIO_AFRL_AFRL0_1); 
		GPIOE->AFR[0] &= ~(GPIO_AFRH_AFRH0_2); 
		GPIOE->AFR[0] &= ~(GPIO_AFRH_AFRH0_3);  
		//------------------------------------------------------------------------------------------
		//GPIO PA0 Input AF TIM8 ___Freq_2
		//------------------------------------------------------------------------------------------
		/*
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
		GPIO_InitTypeDef initFrequencyMeterGpio2;
		initFrequencyMeterGpio2.GPIO_Mode = GPIO_Mode_AF;
		initFrequencyMeterGpio2.GPIO_OType = GPIO_OType_PP;
		initFrequencyMeterGpio2.GPIO_Pin = GPIO_Pin_0;
		initFrequencyMeterGpio2.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(GPIOA,&initFrequencyMeterGpio2);
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM8);*/
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
		GPIOA->MODER  &= ~(GPIO_MODER_MODER0_0); 
		GPIOA->MODER  |=   GPIO_MODER_MODER0_1; 	
		GPIOA->OSPEEDR|= 	 GPIO_OSPEEDER_OSPEEDR0_0;
		GPIOA->OSPEEDR|= 	 GPIO_OSPEEDER_OSPEEDR0_1;
		GPIOA->AFR[0] |=  (GPIO_AFRL_AFRL0_0); 
		GPIOA->AFR[0] |=  (GPIO_AFRL_AFRL0_1); 
		GPIOA->AFR[0] &= ~(GPIO_AFRH_AFRH0_2); 
		GPIOA->AFR[0] &= ~(GPIO_AFRH_AFRH0_3);  

}

void FrequencyMeter_InitStart(void)
{
		NVIC_EnableIRQ(TIM2_IRQn);
		TIM3->CR1       |= TIM_CR1_CEN;
		TIM5->CR1       |= TIM_CR1_CEN;
		TIM4->CR1       |= TIM_CR1_CEN;
		TIM8->CR1       |= TIM_CR1_CEN;
		TIM2->CR1       |= TIM_CR1_CEN;
}
void FrequencyMeter_InitStop(void)
{
		NVIC_DisableIRQ(TIM2_IRQn);
		TIM3->CR1       &= ~TIM_CR1_CEN;
		TIM5->CR1       &= ~TIM_CR1_CEN;
		TIM4->CR1       &= ~TIM_CR1_CEN;
		TIM8->CR1       &= ~TIM_CR1_CEN;
		TIM2->CR1       &= ~TIM_CR1_CEN;
}
void TIM2_IRQHandler(void)
{
	if(TIM2->SR & TIM_SR_UIF)
	{
		TIM2->SR &= ~TIM_SR_UIF; //reset interrupt flag

		FrequencyMetr_1 = ((uint32_t)(TIM3->CNT) << 16) | ((uint32_t)(TIM4->CNT));
		FrequencyMetr_1 = (FrequencyMetr_1 * (10000 / FrequencyMetr_TimeSample));
		//FrequencyMetr_1 = ((uint32_t)(TIM3->CNT)*65535) + ((uint32_t)(TIM4->CNT));
		FrequencyMetr_2 = ((uint32_t)(TIM5->CNT) << 16) | ((uint32_t)(TIM8->CNT));
		FrequencyMetr_2 = (FrequencyMetr_2 * (10000 / FrequencyMetr_TimeSample));
		
		DataReadyImpulse = SET;
		TIM8->CNT = 0;
		TIM4->CNT = 0;
		TIM3->CNT = 0;
		TIM5->CNT = 0;

		TIM2->CR1 |= TIM_CR1_CEN; //another circle
	}
}
