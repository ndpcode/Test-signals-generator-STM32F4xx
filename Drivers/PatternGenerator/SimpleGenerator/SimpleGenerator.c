#include "simpleGenerator.h"
//-------------------------------VARIABLES------------------------------
uint32_t _K;
uint16_t _autoreload_REG;
uint16_t _prescaller_REG;
uint16_t _PWM1_REG;
uint16_t _PWM2_REG;
uint16_t _PWM3_REG;
uint16_t _PWM4_REG;
float _RealFrequency;
uint32_t SG_dead_time_ns;
//-------------------------------EXTERNAL-------------------------------
void SimpleGeneratorOn(uint32_t PWM_Frequency, uint32_t PWM_DeadTimePunkt, uint32_t PWM_Duty)
{
	SG_SquarePWM_SignalOn(PWM_Frequency, PWM_DeadTimePunkt, PWM_Duty);
}
void SimpleGeneratorOFF()
{
	SG_SquarePWM_SignalOff();
}
void SimpleGeneratorUpdate(uint32_t PWM_Frequency, uint32_t PWM_DeadTimePunkt, uint32_t PWM_Duty)
{
	SG_SquarePWM_SignalUpdate(PWM_Frequency, PWM_DeadTimePunkt, PWM_Duty);
}
void SimpleGeneratorInit()
{
	SG_Init_GPIO_Timer_1();
}
//-------------------------------INTERNAL-------------------------------
void SG_SquarePWM_SignalOn(uint32_t PWM_Frequency, uint32_t PWM_DeadTimePunkt, uint32_t PWM_Duty)
{		
			SG_FILL_TimerRegister   (PWM_Frequency, PWM_DeadTimePunkt, PWM_Duty);
			SG_Init_TIM1_SimpleGen  (_autoreload_REG, _prescaller_REG, _PWM1_REG, _PWM2_REG, _PWM3_REG, _PWM4_REG);
			SG_Hardware_DeadTime    (PWM_DeadTimePunkt);
			TIM1->CR1|= TIM_CR1_CEN;
}
void SG_SquarePWM_SignalUpdate(uint32_t PWM_Frequency, uint32_t PWM_DeadTimePunkt, uint32_t PWM_Duty)
{		
			SG_FILL_TimerRegister   (PWM_Frequency, PWM_DeadTimePunkt, PWM_Duty);
			TIM1->CR1 &= ~TIM_CR1_CEN;
			SG_Init_TIM1_SimpleGen  (_autoreload_REG, _prescaller_REG, _PWM1_REG, _PWM2_REG, _PWM3_REG, _PWM4_REG);
			SG_Hardware_DeadTime    (PWM_DeadTimePunkt);
			TIM1->CR1 |=  TIM_CR1_CEN;
}
void SG_FILL_TimerRegister(uint32_t PWM_Frequency, uint32_t PWM_DeadTimePunkt, uint32_t PWM_Duty)
{
	uint16_t z;
	
	_K = SG_CPU_Frequency / PWM_Frequency;
	
	while (SG_getMaxDivider(_K) == 0 || (_K / SG_getMaxDivider(_K)) > 65535 )
	{
		_K--;
	}
	
	_autoreload_REG = SG_getMaxDivider(_K);
	_prescaller_REG = _K / _autoreload_REG;
	z = (floor(_autoreload_REG * ((double)PWM_Duty / 1000)  ));
	
	_PWM1_REG = (z);
	_PWM2_REG = (z);
	_PWM3_REG = (z);
	_PWM4_REG = (z);
	
	if(SG_getMaxDivider(_K) > 65535 || (_K / _autoreload_REG) > 65535)
	{
		_autoreload_REG = 10;
		_prescaller_REG = 1;
		_PWM1_REG = 0;
		_PWM2_REG = 0;
		_PWM3_REG = 0;
		_PWM4_REG = 0;
	}
	_RealFrequency = SG_CPU_Frequency /(_autoreload_REG * _prescaller_REG);
}

void SG_Init_TIM1_SimpleGen(uint16_t autoreload_REG, uint16_t prescaller_REG, uint16_t PWM1_REG, uint16_t PWM2_REG, uint16_t PWM3_REG, uint16_t PWM4_REG)
{	
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

		TIM1->CCER   |= TIM_CCER_CC1E  | TIM_CCER_CC2E  | TIM_CCER_CC3E  | TIM_CCER_CC4E;
		TIM1->CCER	 |= TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE;
		TIM1->CCMR1  |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;
		TIM1->CCMR1  |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC2PE;
		TIM1->CCMR2  |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE;
		TIM1->CCMR2  |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC4PE;
		TIM1->DIER   |= TIM_DIER_UDE;
		TIM1->CR1    |= TIM_CR1_ARPE;  
		TIM1->BDTR 	 &= ~TIM_BDTR_LOCK_0 & ~TIM_BDTR_LOCK_1;
		TIM1->BDTR   |= TIM_BDTR_MOE;
		TIM1->PSC  		= prescaller_REG-1;
		TIM1->ARR 		= autoreload_REG-1;
		TIM1->EGR 	 |= TIM_EGR_UG;
		TIM1->CCR1 		= PWM1_REG;
		TIM1->CCR2 		= PWM2_REG;
		TIM1->CCR3		= PWM3_REG;
		TIM1->CCR4		= PWM4_REG;
}

void SG_Init_GPIO_Timer_1() 
{
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	
		GPIOE->MODER  &= ~(GPIO_MODER_MODER9_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0); 
		GPIOE->MODER  |=   GPIO_MODER_MODER9_1 | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1; 	
		GPIOE->OTYPER &= ~(GPIO_OTYPER_OT9 		 | GPIO_OTYPER_OT11 		| GPIO_OTYPER_OT13 	   | GPIO_OTYPER_OT14);   //0-push-pull   1-open drain
		GPIOE->OSPEEDR|= 	 GPIO_OSPEEDER_OSPEEDR9_0 | GPIO_OSPEEDER_OSPEEDR11_0 | GPIO_OSPEEDER_OSPEEDR13_0 | GPIO_OSPEEDER_OSPEEDR14_0; //00-low 01-medium 10-high 11-very high
		GPIOE->OSPEEDR|= 	 GPIO_OSPEEDER_OSPEEDR9_1 | GPIO_OSPEEDER_OSPEEDR11_1 | GPIO_OSPEEDER_OSPEEDR13_1 | GPIO_OSPEEDER_OSPEEDR14_1; //00-low 01-medium 10-high 11-very high
		GPIOE->PUPDR  &= ~(GPIO_PUPDR_PUPD9_0 | GPIO_PUPDR_PUPD11_0 | GPIO_PUPDR_PUPD13_0 | GPIO_PUPDR_PUPD14_0);
		GPIOE->PUPDR  &= ~(GPIO_PUPDR_PUPD9_1 | GPIO_PUPDR_PUPD11_1 | GPIO_PUPDR_PUPD13_1 | GPIO_PUPDR_PUPD14_1);
		GPIOE->AFR[1] |=   GPIO_AFRH_AFRH1_0 | GPIO_AFRH_AFRH3_0 | GPIO_AFRH_AFRH5_0 | GPIO_AFRH_AFRH6_0; 
		GPIOE->AFR[1] &= ~(GPIO_AFRH_AFRH1_1 | GPIO_AFRH_AFRH3_1 | GPIO_AFRH_AFRH5_1 | GPIO_AFRH_AFRH6_1); 
		GPIOE->AFR[1] &= ~(GPIO_AFRH_AFRH1_2 | GPIO_AFRH_AFRH3_2 | GPIO_AFRH_AFRH5_2 | GPIO_AFRH_AFRH6_2); 
		GPIOE->AFR[1] &= ~(GPIO_AFRH_AFRH1_3 | GPIO_AFRH_AFRH3_3 | GPIO_AFRH_AFRH5_3 | GPIO_AFRH_AFRH6_3);  
}
void SG_SquarePWM_SignalOff(void)
{
		TIM1->CCR1 = 0;
		TIM1->CCR2 = 0;
		TIM1->CCR3 = 0;
		TIM1->CCR4 = 0;
		TIM1->CR1 &= ~TIM_CR1_CEN;
		TIM1->CCMR1  &= ~TIM_CCMR1_OC1M_2 & ~TIM_CCMR1_OC1M_1 & ~TIM_CCMR1_OC1M_0 & ~TIM_CCMR1_OC1PE;		TIM1->CCMR1  |= TIM_CCMR1_OC1M_2;
		TIM1->CCMR1  &= ~TIM_CCMR1_OC2M_2 & ~TIM_CCMR1_OC2M_1 & ~TIM_CCMR1_OC2M_0 & ~TIM_CCMR1_OC2PE;		TIM1->CCMR1  |= TIM_CCMR1_OC2M_2;
		TIM1->CCMR2  &= ~TIM_CCMR2_OC3M_2 & ~TIM_CCMR2_OC3M_1 & ~TIM_CCMR2_OC3M_0 & ~TIM_CCMR2_OC3PE;		TIM1->CCMR2  |= TIM_CCMR2_OC3M_2;
		TIM1->CCMR2  &= ~TIM_CCMR2_OC4M_2 & ~TIM_CCMR2_OC4M_1 & ~TIM_CCMR2_OC4M_0 & ~TIM_CCMR2_OC4PE;		TIM1->CCMR2  |= TIM_CCMR2_OC4M_2;
		RCC->APB2ENR &= ~RCC_APB2ENR_TIM1EN;
}
void SG_Hardware_DeadTime(uint32_t DEAD_time_count)
{
	uint16_t period = 1000000000/(SG_CPU_Frequency/_prescaller_REG);
	TIM1->BDTR &= ~TIM_BDTR_LOCK_0 & ~TIM_BDTR_LOCK_1;
	if(DEAD_time_count<=127)
	{
		SG_dead_time_ns = period;
		TIM1->BDTR   &=  ~TIM_BDTR_DTG;
		TIM1->BDTR   &= 0xffffff00;
		TIM1->BDTR   |=  DEAD_time_count;
	}else if (DEAD_time_count>127 && DEAD_time_count<=127+64)
	{
		SG_dead_time_ns = 2*period;
		TIM1->BDTR   &=  ~TIM_BDTR_DTG;
		TIM1->BDTR   |=  (2<<6);
		TIM1->BDTR   |=  (DEAD_time_count);
		
	}else if (DEAD_time_count>127+64 && DEAD_time_count<=127+64+32)
	{
		SG_dead_time_ns = 8*period;
		TIM1->BDTR   &=  ~TIM_BDTR_DTG;
		TIM1->BDTR   |=  (6<<5);
		TIM1->BDTR   |=  (DEAD_time_count-(127+64));
	}else if (DEAD_time_count>127+64+32 && DEAD_time_count<127+64+32+32)
	{
		SG_dead_time_ns = 16*period;
		TIM1->BDTR   &=  ~TIM_BDTR_DTG;
		TIM1->BDTR   |=  (7<<5);
		TIM1->BDTR   |=  (DEAD_time_count-(127+64+32));
	}else if (DEAD_time_count>=127+64+32+32)
	{
		SG_dead_time_ns = 16*period;
		TIM1->BDTR   &=  ~TIM_BDTR_DTG;
		TIM1->BDTR   |=  (7<<5);
		TIM1->BDTR   |=  (127+64+32);
	}
	
}
uint16_t SG_getMaxDivider (uint32_t number)
{
	volatile uint16_t MaxDivider = 0;

	for(int i = 65535; i>2; i--)
	{
		if( (number%i)==0  &&  (number/i)!=0 )
		{
			MaxDivider = i;
			if((number/MaxDivider)>=1 && (number/MaxDivider)<65535)
			{
				i=0;
			}
		}
	}
	return MaxDivider;
}
uint32_t SG_SimpleGenerator_GetFrequency (uint32_t Frequency)
{
	uint32_t K = SG_CPU_Frequency / Frequency;
	uint32_t autoreload_REG = 0;
	uint32_t prescaller_REG = 0;

	while (SG_getMaxDivider(K) == 0 || (K / SG_getMaxDivider(K)) > 65535 )
	{
		K--;
	}
	autoreload_REG = SG_getMaxDivider(K);
	prescaller_REG = K / autoreload_REG;
 	if(SG_getMaxDivider(K) > 65535 || (K / autoreload_REG) > 65535)
	{
		autoreload_REG = 10;
		prescaller_REG = 1;
	}
	uint32_t realFrequency = SG_CPU_Frequency /(autoreload_REG * prescaller_REG);
	return realFrequency;
}
