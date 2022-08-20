//*********************************************************************************************************//
//Standard signals for timer 1 library
//build for stm32f407
//Created 10.04.2017
//Created by Novikov Dmitry
//Source file
//*********************************************************************************************************//

//*********************************************************************************************************//
/* includes */
#include "gen_timers.h"

uint16_t timer1Prescaler;
uint16_t accuracySignalA, accuracySignalB; //�������� ���, ������� �������� ���������� �� ������������ � ������ ���
uint16_t pwmSignalStepsA, pwmSignalStepsB; //����� �������� ��� � ����� ����������� �������
uint16_t pwmSignalArrayA[SSIGNAL_MAX_SIGN_STEPS];  //������ � ��� ������� �������
uint16_t pwmSignalArrayB[SSIGNAL_MAX_SIGN_STEPS];  //������ B ��� ������� �������    
int8_t signalH;

//���������� ��� ����������� �������� ������� �� ������
uint16_t* currentSignalArray;     //��������� �� ������� ������ �������
uint16_t currentArrayPulseNumber; //����� ����������� �������� � ������� �������
uint16_t currentArrayPulseValue;  //�������� ����������� �������� � ������� �������

struct timerEventsBase
{
	unsigned currentArray : 1;   //������� ������������ ������ � ��������� ������� ( A (0) ��� B (1) )
	unsigned mayChangeArray : 1; //���� ����������� ����� ������� � ���������
	unsigned signalEnabled : 1;  //���������� �������
	unsigned timerInitType : 2;  //1 - DMA to Timer1, 2 - Timer as PWM
	unsigned needGetArrayVal : 1; //���� ������������� ��������� ���� �������� �� ������� �������
	unsigned needSetArrayVal : 1; //���� ������������� �������� ���� �������� � ������ �������
} timerEvents;

uint8_t Timer1Reset(void)
{
	TIM1->CR1 &= ~TIM_CR1_CEN;	
	
	TIM1->CR1 = 0x0000;
	TIM1->CR2 = 0x0000;
	TIM1->SMCR = 0x0000;
	TIM1->DIER = 0x0000;
	TIM1->SR = 0x0000;
	TIM1->EGR = 0x0000;
	TIM1->CCMR1 = 0x0000;
	TIM1->CCMR2 = 0x0000;
	TIM1->CCER = 0x0000;
	TIM1->CNT = 0x0000;
	TIM1->PSC = 0x0000;
	TIM1->ARR = 0x0000;
	TIM1->RCR = 0x0000;
	TIM1->CCR1 = 0x0000;
	TIM1->CCR2 = 0x0000;
	TIM1->CCR3 = 0x0000;
	TIM1->CCR4 = 0x0000;
	TIM1->BDTR = 0x0000;
	TIM1->DCR = 0x0000;
	TIM1->DMAR = 0x0000;
	
	timer1Prescaler = 0;
	
	CH1_DOWN;
	CH3_DOWN;
	
	return 1;
}

uint8_t DMA1Reset(void)
{	
  DMA2_Stream1->CR &= ~DMA_SxCR_EN;
	DMA2_Stream6->CR &= ~DMA_SxCR_EN;
	NVIC_DisableIRQ(DMA2_Stream1_IRQn);
	NVIC_DisableIRQ(DMA2_Stream6_IRQn); 
  DMA2_Stream1->CR = 0x00000000; 	
	DMA2_Stream6->CR = 0x00000000;
	DMA2_Stream1->NDTR = 0x00000000;
	DMA2_Stream6->NDTR = 0x00000000;
	DMA2_Stream1->PAR = 0x00000000;
	DMA2_Stream6->PAR = 0x00000000;
	DMA2_Stream1->M0AR = 0x00000000;
	DMA2_Stream6->M0AR = 0x00000000;
		
	return 1;
}

uint8_t GenSignalEmulStop(void)
{
	//flags off
	timerEvents.mayChangeArray = 0;
	timerEvents.signalEnabled = 0;
	timerEvents.needGetArrayVal = 0;
	timerEvents.needSetArrayVal = 0;
	//ports off
	CH1_RESET;
	CH3_RESET;
	CH2_RESET;
	CH4_RESET;
	CH1_DOWN;
	CH2_DOWN;
	CH3_DOWN;
	CH4_DOWN;
	//DMA off
	DMA1Reset();
	Timer1Reset();
	
	return 1;
}

uint8_t GenSetPorts(void)
{
	//down
	CH1_DOWN;
	CH2_DOWN;
	CH3_DOWN;
	CH4_DOWN;
	//set
	CH1_SET;
	CH3_SET;
	CH2_SET;
	CH4_SET;
	//down
	CH1_DOWN;
	CH2_DOWN;
	CH3_DOWN;
	CH4_DOWN;
	
	return 1;
}

uint8_t GenInitTimer1ForSignalEmul(void)
{
	GenSignalEmulStop();
	GenSetPorts();
	
	memset( &timerEvents, 0, sizeof(timerEvents) );
  memset( pwmSignalArrayA, 0, sizeof(uint16_t) * SSIGNAL_MAX_SIGN_STEPS );	
	memset( pwmSignalArrayB, 0, sizeof(uint16_t) * SSIGNAL_MAX_SIGN_STEPS );
	signalH = 1;
	
	//��� Timer1
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	//������������� Timer1
	TIM1->CR1 &= ~TIM_CR1_CEN;	
  //TIM1->RCR = 0;
  TIM1->CCER = TIM_CCER_CC1E | TIM_CCER_CC3E;
	TIM1->CCMR1 = TIM_CCMR1_OC1FE | TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_1 ;
  TIM1->CCMR2 = TIM_CCMR2_OC3FE | TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_1 ;
	TIM1->DIER |= TIM_DIER_CC1DE | TIM_DIER_CC3DE;
	TIM1->BDTR = TIM_BDTR_MOE;
	TIM1->PSC = timer1Prescaler;
	TIM1->ARR = 1000;
	TIM1->CCR1 = 0;
	TIM1->CCR3 = 0;
	TIM1->CR1 = TIM_CR1_ARPE; //| TIM_CR1_CMS;
	TIM1->CR1 |= TIM_CR1_CEN;
	
	//��������� ��������
	pwmSignalStepsA = 1000;
	//��� DMA, DMA->Timer1
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	//select channel 6
	//TIM1_CH1
	DMA2_Stream1->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_CHSEL_1 | DMA_SxCR_PL | DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_TCIE;
	//TIM1_CH3
	DMA2_Stream6->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_CHSEL_1 | DMA_SxCR_PL | DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_TCIE;
	DMA2_Stream1->M0AR = (uint32_t) &pwmSignalArrayA[0];
	DMA2_Stream6->M0AR = (uint32_t) &pwmSignalArrayA[0];
	DMA2_Stream1->PAR = (uint32_t) &TIM1->CCR1;
	DMA2_Stream6->PAR = (uint32_t) &TIM1->CCR3;
	DMA2_Stream1->NDTR = pwmSignalStepsA;
	DMA2_Stream6->NDTR = pwmSignalStepsA;	
	NVIC_EnableIRQ(DMA2_Stream1_IRQn);
	NVIC_EnableIRQ(DMA2_Stream6_IRQn);
	DMA2_Stream6->CR &= ~DMA_SxCR_EN;
  DMA2_Stream1->CR |= DMA_SxCR_EN;
	
	timerEvents.timerInitType = SSIGNAL_TIMER_INIT_AS_DMA;

  return 1;
}

uint8_t GenInitTimer1ForPWM(void)
{
	GenSignalEmulStop();
	GenSetPorts();
	
	//��� Timer1
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	//������������� Timer1
	TIM1->CR1 &= ~TIM_CR1_CEN;	
  TIM1->CCER = TIM_CCER_CC1E | TIM_CCER_CC3E;
	TIM1->CCMR1 = TIM_CCMR1_OC1FE | TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
  TIM1->CCMR2 = TIM_CCMR2_OC3FE | TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M;
	TIM1->BDTR = TIM_BDTR_MOE;
	TIM1->PSC = timer1Prescaler;
	TIM1->ARR = 1000;
	TIM1->CCR1 = 0;
	TIM1->CCR3 = 1000;
	TIM1->CR1 = TIM_CR1_ARPE | TIM_CR1_CMS;
	TIM1->CR1 |= TIM_CR1_CEN;

  timerEvents.timerInitType = SSIGNAL_TIMER_INIT_AS_PWM;	
	
  return 1;
}

uint8_t GenInitSignalOnTimer1(uint8_t _signal_type)
{
	//�������� ������ ���� �������
	if ( ( _signal_type > SSIGNAL_INDEX_MAX ) || ( _signal_type < SSIGNAL_INDEX_MIN ) ) return 0;
	
	if ( _signal_type == SSIGNAL_SQUARE )
	{
		//������������� ��� ��� ������
		return GenInitTimer1ForPWM();
	} else
	{
		//������������� ��� �������� ��������
		return GenInitTimer1ForSignalEmul();
	};
	
	return 0;
}

void GenTimer1SetPrescaler(void) 
{
	if ( TIM1->PSC != timer1Prescaler )
	{
		TIM1->PSC = timer1Prescaler;
	}
}

uint8_t GenTimer1ForPWMUpdate(uint16_t _tim_prescaler, uint16_t _tim_arr, uint16_t _tim_ccr,
                              uint32_t _transistorsMinTimeNS, uint32_t _transistorsDeadTimeNS)
{
	uint16_t _ccr, _ccr3, transistorsMinStep, transistorsMaxStep;
	
	//��������� ������������
	timer1Prescaler = _tim_prescaler;
	GenTimer1SetPrescaler();
	//��������� �������� ���������
	_ccr = _tim_ccr;
	//min time - NS -> � �����
	transistorsMinStep = (double)_transistorsMinTimeNS * SSIGNAL_TIMER_FREQ / ( (_tim_prescaler + 1) * 2 * 1000000000 );	
	//dead time - NS -> � �����
	transistorsMaxStep = (double)_transistorsDeadTimeNS * SSIGNAL_TIMER_FREQ / ( (_tim_prescaler + 1) * 2 * 1000000000 );
	//control compare reg
	if ( _ccr < transistorsMinStep ) _ccr = transistorsMinStep;
	if ( ( ( ( _tim_arr / 2 ) - _ccr) < transistorsMaxStep ) || ( _ccr > ( _tim_arr / 2 ) ) )
	{
		_ccr = ( _tim_arr / 2 ) - transistorsMaxStep;
	}
	_ccr3 = _tim_arr - _ccr;
	//���������� �������� ����������
	TIM1->CR1 |= TIM_CR1_ARPE | TIM_CR1_CMS;
	//����� � ���������
	TIM1->ARR = _tim_arr;
	TIM1->CCR1 = _ccr;
	TIM1->CCR3 = _ccr3;
	
	return 1;
}

void calculateSinArray(uint16_t *_sin_array, uint16_t _arr_size, uint16_t _acc_sin,
	                     double _power_k, double _center_k, uint32_t _transistorsMinTimeNS, uint32_t _transistorsDeadTimeNS)
{
	uint16_t i = 0, j = 0;
	double sin_element = 0;
	double _center_mult = 0;
	double vector = 0;
	//min time - from NS - to cicles
	uint16_t transistorsMinStep = (double)_transistorsMinTimeNS * SSIGNAL_TIMER_FREQ / 1000000000;	
	//dead time - from NS - to cicles
	uint16_t transistorsMaxStep = (double)_acc_sin - (double)_transistorsDeadTimeNS * SSIGNAL_TIMER_FREQ / 1000000000;	
	//k of pulse center - from +/- XXXX.XX - to 0.XXXXXX ... X.XXXXX
	_center_mult = ( _center_k >= 0 ) ? ( (_center_k + 100) / 100 ) : ( 100 / fabs(_center_k - 100) );
	// 1/2 up
	while ( ( sin_element >= vector ) && ( i < _arr_size ) )
	{
		vector = sin_element;
		sin_element = (double)sin( i * pi * _center_mult / _arr_size ) * _acc_sin * _power_k;		
		_sin_array[i] = sin_element;
		//�������� �� ����������� DutyCicle
		if ( _sin_array[i] < transistorsMinStep ) _sin_array[i] = 0;
		//�������� ��� DeadTime ��������, ��� ������������
		if ( _sin_array[i] > transistorsMaxStep ) _sin_array[i] = transistorsMaxStep;	
    i++;		
	}
	// 1/2 down
	if ( i < _arr_size )
	{
		if ( i > 0 ) i--;
		j = i;
		vector = pi / ( 2 * ( _arr_size - j ) );
		while ( i < _arr_size )
		{
			sin_element = (double)sin( pi / 2 + (i - j + 1) * vector ) * _acc_sin * _power_k;
		  _sin_array[i] = sin_element;
		  //�������� �� ����������� DutyCicle
		  if ( _sin_array[i] < transistorsMinStep ) _sin_array[i] = 0;
		  //�������� ��� DeadTime ��������, ��� ������������
		  if ( _sin_array[i] > transistorsMaxStep ) _sin_array[i] = transistorsMaxStep;	
      i++;
		}
	}
}

void calculateTriangleArray(uint16_t *_tri_array, uint16_t _arr_size, uint16_t _acc_tri,
		                        double _power_k, double _center_k, uint32_t _transistorsMinTimeNS, uint32_t _transistorsDeadTimeNS)
{
	uint16_t i = 0;
	double _center_mult = 0;
	double _a, _b = 0;
	//min time - from NS - to cicles
	uint16_t transistorsMinStep = (double)_transistorsMinTimeNS * SSIGNAL_TIMER_FREQ / 1000000000;	
	//dead time - from NS - to cicles
	uint16_t transistorsMaxStep = (double)_acc_tri - (double)_transistorsDeadTimeNS * SSIGNAL_TIMER_FREQ / 1000000000;	
	//k of pulse center - from +/- XXXX.XX - to 0.0001 ... 1.9999
	_center_mult = ( _center_k >= 0 ) ? ( _center_k / 100 + 1 ) : ( 1 - (fabs(_center_k) / 100) );
	if ( _center_mult < 0 ) _center_mult = 0.0001;
	if ( _center_mult > 2 ) _center_mult = 1.9999;
	//1/2 up
	//coeff
	_a = _arr_size * _center_mult / 2;
	if ( ( _a == 0 ) || ( (_arr_size - _a - 1) == 0 ) ) return;
	_b = (double)_acc_tri / _a;	
	for (i = 0; i < _a + 1; i++){
		_tri_array[i] = (double)_b * i * _power_k;
		//�������� �� ����������� DutyCicle
		if ( _tri_array[i] < transistorsMinStep ) _tri_array[i] = 0;
		//�������� ��� DeadTime ��������, ��� ������������
		if ( _tri_array[i] > transistorsMaxStep ) _tri_array[i] = transistorsMaxStep;
	};
	//1/2 down
	//coeff
	_b = (double)_acc_tri / (_arr_size - _a - 1);
	for (i = _a + 1; i < _arr_size; i++){
		_tri_array[i] = (double)( _acc_tri - _b * (i - _a) ) * _power_k;
		//�������� �� ����������� DutyCicle
		if ( _tri_array[i] < transistorsMinStep ) _tri_array[i] = 0;
		//�������� ��� DeadTime ��������, ��� ������������
		if ( _tri_array[i] > transistorsMaxStep ) _tri_array[i] = transistorsMaxStep;
	};
}

void calculatePulseArray(uint16_t *_pulse_array, uint16_t _arr_size, uint16_t _acc_pulse,
		                        double _power_k, double _center_k, uint32_t _transistorsMinTimeNS, uint32_t _transistorsDeadTimeNS)
{
	uint16_t i = 0;
	//dead time - from NS - to cicles
	uint16_t transistorsMaxStep = (double)_acc_pulse - (double)_transistorsDeadTimeNS * SSIGNAL_TIMER_FREQ / 1000000000;
	transistorsMaxStep /= 2;
	//array fill
	for (i = 0; i < _arr_size; i++)
	{
		_pulse_array[i] = transistorsMaxStep;				
	}
}

void calculateDataArray(uint16_t *_signal_array, uint16_t _arr_size, uint16_t _acc_signal,
	                      double _power_k, double _center_k, uint32_t _transistorsMinTimeNS,
                        uint32_t _transistorsDeadTimeNS, uint8_t _signal_type)
{
  switch ( _signal_type )
	{
		case SSIGNAL_SINUS: //sinus
			calculateSinArray(_signal_array, _arr_size, _acc_signal, _power_k, _center_k, _transistorsMinTimeNS, _transistorsDeadTimeNS);
		break;
		
		case SSIGNAL_TRIANGLE: //triangle
			calculateTriangleArray(_signal_array, _arr_size, _acc_signal, _power_k, _center_k, _transistorsMinTimeNS, _transistorsDeadTimeNS);
		break;
		
		case SSIGNAL_PULSE: //pulse
			calculatePulseArray(_signal_array, _arr_size, _acc_signal, _power_k, _center_k, _transistorsMinTimeNS, _transistorsDeadTimeNS);
		break;
	}		
};

uint8_t GenUpdateSignal(const uint16_t _tim_prescaler, const uint16_t _tim_arr, const uint16_t _tim_steps_ccr,
	                      const double _power_k, const double _center_k, const uint32_t _transistorsMinTimeNS,
												const uint32_t _transistorsDeadTimeNS, const uint8_t _signal_type)
{
	//�������� ������ ���� �������
	if ( ( _signal_type > SSIGNAL_INDEX_MAX ) || ( _signal_type < SSIGNAL_INDEX_MIN ) ) return 0;
	
	//��������� ��� ������� = 3 (������)
	if ( _signal_type == SSIGNAL_SQUARE )
	{
		//�������� ����������� ������ ������� � ���������
		if ( timerEvents.timerInitType != SSIGNAL_TIMER_INIT_AS_PWM )
		{
			if ( !GenInitSignalOnTimer1(SSIGNAL_SQUARE) ) return 0; 
		};
		return GenTimer1ForPWMUpdate(_tim_prescaler, _tim_arr, _tim_steps_ccr, _transistorsMinTimeNS, _transistorsDeadTimeNS);
	};
	
	//�������� ������� ������� ������������ ������� ������
	if ( _tim_steps_ccr > SSIGNAL_MAX_SIGN_STEPS ) return 0;	
	//�������� ������������ ��������
	if ( ( _power_k > SSIGNAL_MAX_POWER_K ) || ( _power_k < SSIGNAL_MIN_POWER_K ) ) return 0;
	
	//�������� ����������� ������ ������� � ���������
	if ( timerEvents.timerInitType != SSIGNAL_TIMER_INIT_AS_DMA )
	{
		if ( !GenInitSignalOnTimer1(_signal_type) ) return 0; 
	};
	
	//������������� ������� ����
	timerEvents.mayChangeArray = 0;
	//������������� ������������
	timer1Prescaler = _tim_prescaler;
	//����� ������� � ����������
	if ( timerEvents.currentArray == SSIGNAL_TIMER_CURRENT_A ) //���� ������� ����� - �
	{
		accuracySignalB = _tim_arr;
		pwmSignalStepsB = _tim_steps_ccr;
		calculateDataArray(pwmSignalArrayB, pwmSignalStepsB, accuracySignalB, _power_k/100, _center_k,
		                   _transistorsMinTimeNS, _transistorsDeadTimeNS, _signal_type);
	} else
	{ //������ ����� - B
		accuracySignalA = _tim_arr;
		pwmSignalStepsA = _tim_steps_ccr;
		calculateDataArray(pwmSignalArrayA, pwmSignalStepsA, accuracySignalA, _power_k/100, _center_k,
		                   _transistorsMinTimeNS, _transistorsDeadTimeNS, _signal_type);
	};				
	//���������� ������ � �������� ������������ �������
	//�� ���������� DMA ����� ������� ������ �������
	timerEvents.mayChangeArray = 1;
	
	return 1;
};

uint16_t GenGetPulseWidthByNumber(uint16_t pulseNum)
{
	uint32_t counter;
	
	//��������
	if (!pulseNum) return 0;
	if ( ( timerEvents.currentArray == SSIGNAL_TIMER_CURRENT_A ) && ( pulseNum > pwmSignalStepsA) ) return 0;
	if ( ( timerEvents.currentArray == SSIGNAL_TIMER_CURRENT_B ) && ( pulseNum > pwmSignalStepsB) ) return 0;
	
	//�������� ��������� �������
	counter = 3000000;
	while ( (timerEvents.mayChangeArray) && (counter--) )
	{
		__NOP();
	}
	
	//�������� ��������� �� ������� ������ �������
	if ( timerEvents.currentArray == SSIGNAL_TIMER_CURRENT_A )
	{
		currentSignalArray = pwmSignalArrayA;
	} else if ( timerEvents.currentArray == SSIGNAL_TIMER_CURRENT_B )
	{
		currentSignalArray = pwmSignalArrayB;
	}
	//���������� ����� ��������
	currentArrayPulseNumber = pulseNum - 1;
	//��������� ����
	timerEvents.needGetArrayVal = 1;
	//�������� ���������� ����������
	counter = 3000000;
	while ( (timerEvents.needGetArrayVal) && (counter--) )
	{
		__NOP();
	}
	//������� ����������
	return currentArrayPulseValue;
}

uint8_t GenSetPulseWidthByNumber(uint16_t pulseNum, uint16_t pulseVal)
{
	uint32_t counter;
	
	//��������
	if (!pulseNum) return 0;
	if ( ( timerEvents.currentArray == SSIGNAL_TIMER_CURRENT_A ) && ( pulseNum > pwmSignalStepsA) ) return 0;
	if ( ( timerEvents.currentArray == SSIGNAL_TIMER_CURRENT_B ) && ( pulseNum > pwmSignalStepsB) ) return 0;
	if ( ( timerEvents.currentArray == SSIGNAL_TIMER_CURRENT_A ) && ( pulseVal > (accuracySignalA - 1) ) ) return 0;
	if ( ( timerEvents.currentArray == SSIGNAL_TIMER_CURRENT_B ) && ( pulseVal > (accuracySignalB - 1) ) ) return 0;
	if (timerEvents.mayChangeArray) return 0;
	
	//�������� ��������� �� ������� ������ �������
	if ( timerEvents.currentArray == SSIGNAL_TIMER_CURRENT_A )
	{
		currentSignalArray = pwmSignalArrayA;
	} else if ( timerEvents.currentArray == SSIGNAL_TIMER_CURRENT_B )
	{
		currentSignalArray = pwmSignalArrayB;
	}
	//���������� ����� ��������
	currentArrayPulseNumber = pulseNum - 1;
	//���������� �������� ��������
	currentArrayPulseValue = pulseVal;
	//��������� ����
	timerEvents.needSetArrayVal = 1;
	//�������� ���������� ����������
	counter = 3000000;
	while ( (timerEvents.needSetArrayVal) && (counter--) )
	{
		__NOP();
	}
	//������� ����������
	return 1;
}

void DMA_Interrupt_Change_Signal(void)
{
	//������������� ���� DMA
	DMA2_Stream1->CR &= ~DMA_SxCR_EN;
	DMA2_Stream6->CR &= ~DMA_SxCR_EN;
	//����� �����
	if ( timerEvents.currentArray == SSIGNAL_TIMER_CURRENT_A ) //������� ����� - �
	{
		DMA2_Stream1->M0AR = (uint32_t) &pwmSignalArrayB[0];
		DMA2_Stream6->M0AR = (uint32_t) &pwmSignalArrayB[0];
	  DMA2_Stream1->NDTR = pwmSignalStepsB + 1;
	  DMA2_Stream6->NDTR = pwmSignalStepsB + 1;
	  //check prescaler
	  GenTimer1SetPrescaler();
		//set registers
		TIM1->CR1 |= TIM_CR1_ARPE;
	  TIM1->ARR = accuracySignalB;
	  TIM1->CCR1 = 0;
	  TIM1->CCR3 = 0;
		//������������ ����� �� B
		timerEvents.currentArray = SSIGNAL_TIMER_CURRENT_B;
    timerEvents.signalEnabled = 1;		
	} else 
	{
		//������� ����� - B
		DMA2_Stream1->M0AR = (uint32_t) &pwmSignalArrayA[0];
	  DMA2_Stream6->M0AR = (uint32_t) &pwmSignalArrayA[0];
	  DMA2_Stream1->NDTR = pwmSignalStepsA + 1;
	  DMA2_Stream6->NDTR = pwmSignalStepsA + 1;
	  //check prescaler
	  GenTimer1SetPrescaler();
		//set registers
		TIM1->CR1 |= TIM_CR1_ARPE;
	  TIM1->ARR = accuracySignalA;
	  TIM1->CCR1 = 0;
	  TIM1->CCR3 = 0;
		//������������ ����� �� A
		timerEvents.currentArray = SSIGNAL_TIMER_CURRENT_A;
		timerEvents.signalEnabled = 1;
	};
	// ����
	timerEvents.mayChangeArray = 0;
}

void DMA_Interrupt_Change_Period(void)
{
	if(signalH == 1) {
		//���� 2
		CH2_DOWN;
		//���� 1
		TIM1->CCMR1 &= ~TIM_CCMR1_OC1M_2;
		//��������� �������� �������
		if ( timerEvents.needSetArrayVal )
		{
			currentSignalArray[currentArrayPulseNumber] = currentArrayPulseValue;
			timerEvents.needSetArrayVal = 0;			
		}
		//������ ������
		if ( timerEvents.mayChangeArray ) DMA_Interrupt_Change_Signal();
		//������� �������� �������
		if ( timerEvents.needGetArrayVal )
		{
			currentArrayPulseValue = currentSignalArray[currentArrayPulseNumber];
			timerEvents.needGetArrayVal = 0;			
		}
		//��� 3
		TIM1->CCMR2 |=  TIM_CCMR2_OC3M_2;
		//��� 4
		if ( timerEvents.signalEnabled ) CH4_UP;
		DMA2_Stream1->CR &= ~DMA_SxCR_EN;
		DMA2_Stream6->CR |= DMA_SxCR_EN;
	}
	else  {
		//���� 4
		CH4_DOWN;
		//���� 3
		TIM1->CCMR2 &= ~TIM_CCMR2_OC3M_2;
		//��������� �������� �������
		if ( timerEvents.needSetArrayVal )
		{
			currentSignalArray[currentArrayPulseNumber] = currentArrayPulseValue;
			timerEvents.needSetArrayVal = 0;			
		}
		//������ ������
		if ( timerEvents.mayChangeArray ) DMA_Interrupt_Change_Signal();
		//������� �������� �������
		if ( timerEvents.needGetArrayVal )
		{
			currentArrayPulseValue = currentSignalArray[currentArrayPulseNumber];
			timerEvents.needGetArrayVal = 0;			
		}
		//��� 1
		TIM1->CCMR1 |=  TIM_CCMR1_OC1M_2;		
		//��� 2
		if ( timerEvents.signalEnabled ) CH2_UP;
		DMA2_Stream6->CR &= ~DMA_SxCR_EN;
		DMA2_Stream1->CR |= DMA_SxCR_EN;
	};
	signalH = 1 - signalH;
}

extern void sysWaitUS(uint16_t timeUS);
extern void sysWaitNS(uint16_t timeNS);

void DMA2_Stream1_IRQHandler(void)
{
//	if ( DMA2->LISR & DMA_LISR_TCIF1 )
//	{
//		DMA_Interrupt_Change_Period();
//	}
	DMA2->LIFCR |= DMA_LIFCR_CTCIF1 | DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CFEIF1;
	DMA_Interrupt_Change_Period();
};

void DMA2_Stream6_IRQHandler(void)
{
//	if ( DMA2->HISR & DMA_HISR_TCIF6 )
//	{
//		DMA_Interrupt_Change_Period();
//	}
	DMA2->HIFCR |= DMA_HIFCR_CTCIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTEIF6 | DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CFEIF6;
	DMA_Interrupt_Change_Period();
};

uint8_t GenCheckSignalConfig(const int32_t _tim_prescaler, const int32_t _tim_arr, const int32_t _tim_steps_ccr,
	                           const double _power_k, const double _center_k, const uint32_t _transistorsMinTimeNS,
												     const uint32_t _transistorsDeadTimeNS, const uint8_t _signal_type)
{
	uint16_t transistorsMinStep, transistorsMaxStep;
	
	if ( ( _tim_prescaler < 0 ) || ( _tim_prescaler >= USHRT_MAX ) ) return 0;
	if ( ( _tim_arr < 0 ) || ( _tim_arr > USHRT_MAX ) ) return 0;
	if ( ( _tim_steps_ccr < 0 ) || ( _tim_steps_ccr > USHRT_MAX ) ) return 0;
	if ( ( _transistorsMinTimeNS < SSIGNAL_MIN_PULSE_TIME ) || ( _transistorsMinTimeNS > SSIGNAL_MAX_PULSE_TIME ) ) return 0;
	if ( ( _transistorsDeadTimeNS < SSIGNAL_MIN_DEAD_TIME ) || ( _transistorsDeadTimeNS > SSIGNAL_MAX_DEAD_TIME ) ) return 0;
	
	//min time - NS -> � �����
	transistorsMinStep = (double)_transistorsMinTimeNS * SSIGNAL_TIMER_FREQ / ( (_tim_prescaler + 1) * 2 * 1000000000 );	
	//dead time - NS -> � �����
	transistorsMaxStep = (double)_transistorsDeadTimeNS * SSIGNAL_TIMER_FREQ / ( (_tim_prescaler + 1) * 2 * 1000000000 );
  if ( ( _signal_type == SSIGNAL_SQUARE ) && ( _tim_steps_ccr > ( ( _tim_arr / 2 ) - transistorsMaxStep ) ) ) return 0;
	if ( ( _signal_type == SSIGNAL_SQUARE ) && ( _tim_steps_ccr < transistorsMinStep ) ) return 0;
	if ( _tim_arr < 2 ) return 0;
	if ( ( ( _tim_steps_ccr < 2 ) || ( _tim_steps_ccr > SSIGNAL_MAX_SIGN_STEPS ) ) &&
		   ( ( _signal_type == SSIGNAL_SINUS ) || ( _signal_type == SSIGNAL_TRIANGLE ) || ( _signal_type == SSIGNAL_PULSE ) ) ) return 0;
	if ( ( _power_k < SSIGNAL_MIN_POWER_K ) || ( _power_k > SSIGNAL_MAX_POWER_K ) ) return 0;
	if ( ( _center_k < SSIGNAL_MIN_CENTER_K ) || ( _center_k > SSIGNAL_MAX_CENTER_K ) ) return 0;
	if ( ( _signal_type < SSIGNAL_INDEX_MIN ) || ( _signal_type > SSIGNAL_INDEX_MAX ) ) return 0;
		
	return 1;
}

uint16_t GenGetPrescalerValue(const double _freq_pwm, const uint8_t _signal_type)
{
	uint16_t _prescaler_value = 0;
	if ( _freq_pwm == 0.0 ) return USHRT_MAX;
	if ( _signal_type == SSIGNAL_SQUARE )
	{
		_prescaler_value = (uint16_t)( (double)SSIGNAL_TIMER_FREQ / ( (double)_freq_pwm * 131070 ) );		
	}
	if ( ( _signal_type == SSIGNAL_SINUS ) || ( _signal_type == SSIGNAL_TRIANGLE ) || ( _signal_type == SSIGNAL_PULSE ) )
	{
		_prescaler_value = (uint16_t)( (double)SSIGNAL_TIMER_FREQ / ( (double)_freq_pwm * 65535 ) );
	}
	return _prescaler_value;
}

uint16_t GenGetARRValueFromFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const uint8_t _signal_type)
{
	uint16_t _arr_value = 0;
	if ( _freq_pwm == 0.0 ) return 0;
	if ( _tim_prescaler == USHRT_MAX ) return 0;
	if ( _signal_type == SSIGNAL_SQUARE )
	{
		_arr_value = (uint16_t)( (double)SSIGNAL_TIMER_FREQ / ( (double)(_tim_prescaler + 1) * 2 * _freq_pwm ) );		
	}
	if ( ( _signal_type == SSIGNAL_SINUS ) || ( _signal_type == SSIGNAL_TRIANGLE ) || ( _signal_type == SSIGNAL_PULSE ) )
	{
		_arr_value = (uint16_t)( (double)SSIGNAL_TIMER_FREQ / ( (double)(_tim_prescaler + 1) * _freq_pwm ) );
	}	
	if ( _arr_value < 2 ) _arr_value = 0;
	return _arr_value;
}

uint16_t GenGetStepsCCRValueFromFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const double _freq_signal,
	                                   const uint16_t _tim_arr, const double _power_k, const uint32_t _transistorsMinTimeNS,
																		 const uint32_t _transistorsDeadTimeNS, const uint8_t _signal_type)
{
	uint16_t transistorsMinStep, transistorsMaxStep;
		
	uint16_t _steps_value = 0;
	if ( !_tim_arr ) return 0;
	if ( _freq_pwm == 0.0 ) return 0;
	if ( _freq_signal == 0.0 ) return 0;
	if ( _signal_type == SSIGNAL_SQUARE )
	{
		_steps_value = (uint16_t)( (double)_tim_arr * _power_k / 2 );
		//min time - NS -> � �����
	  transistorsMinStep = (double)_transistorsMinTimeNS * SSIGNAL_TIMER_FREQ / ( (_tim_prescaler + 1) * 2 * 1000000000 );	
	  //dead time - NS -> � �����
	  transistorsMaxStep = (double)_transistorsDeadTimeNS * SSIGNAL_TIMER_FREQ / ( (_tim_prescaler + 1) * 2 * 1000000000 );
	  if ( _steps_value < transistorsMinStep ) _steps_value = transistorsMinStep;
	  if ( ( ( ( _tim_arr / 2 ) - _steps_value) < transistorsMaxStep ) || ( _steps_value > ( _tim_arr / 2 ) ) )
	  {
		  _steps_value = ( _tim_arr / 2 ) - transistorsMaxStep;
	  }	
	}
	if ( ( _signal_type == SSIGNAL_SINUS ) || ( _signal_type == SSIGNAL_TRIANGLE ) || ( _signal_type == SSIGNAL_PULSE ) )
	{
		_steps_value = (uint16_t)( _freq_pwm / ( 2.0 * _freq_signal ) );
	  if ( _steps_value < 2 ) return 0;
	  if ( _steps_value > SSIGNAL_MAX_SIGN_STEPS ) return 0;
	}
	return _steps_value;
}

double GenGetPWMFreqValueFromTimer(const uint16_t _tim_prescaler, const uint16_t _tim_arr, const uint8_t _signal_type)
{
  if ( !GenCheckSignalConfig(_tim_prescaler, _tim_arr, 0, SSIGNAL_MIN_POWER_K,
		                         SSIGNAL_MIN_CENTER_K, SSIGNAL_MIN_PULSE_TIME, SSIGNAL_MIN_DEAD_TIME, _signal_type) ) return 0;
	
	if ( _signal_type == SSIGNAL_SQUARE )
	{
		return ( (double)SSIGNAL_TIMER_FREQ / ( (double)(_tim_prescaler + 1) * 2 * _tim_arr ) );
	}
	if ( ( _signal_type == SSIGNAL_SINUS ) || ( _signal_type == SSIGNAL_TRIANGLE ) || ( _signal_type == SSIGNAL_PULSE ) )
	{
		return ( (double)SSIGNAL_TIMER_FREQ / ( (double)(_tim_prescaler + 1) * _tim_arr ) );
	}
	return -1;
}

double GenGetSignalFreqValueFromTimer(const uint16_t _tim_prescaler, const uint16_t _tim_arr, const uint16_t _tim_steps_ccr, const uint8_t _signal_type)
{
  if ( !GenCheckSignalConfig(_tim_prescaler, _tim_arr, _tim_steps_ccr, SSIGNAL_MIN_POWER_K,
		                         SSIGNAL_MIN_CENTER_K, SSIGNAL_MIN_PULSE_TIME, SSIGNAL_MIN_DEAD_TIME, _signal_type) ) return 0;
	
	if ( _signal_type == SSIGNAL_SQUARE )
	{
		return ( (double)SSIGNAL_TIMER_FREQ / ( (double)(_tim_prescaler + 1) * 2 * _tim_arr ) );
	}
	if ( ( _signal_type == SSIGNAL_SINUS ) || ( _signal_type == SSIGNAL_TRIANGLE ) || ( _signal_type == SSIGNAL_PULSE ) )
	{
		return ( (double)SSIGNAL_TIMER_FREQ / ( (double)(_tim_prescaler + 1) * _tim_arr * 2.0 * _tim_steps_ccr ) );
	}
	return -1;
}

uint8_t GenCheckFrequencyAvailable(const double _freq_pwm, const double _freq_signal, const uint8_t _signal_type)
{
	uint32_t a = 0, b = 0, c = 0;
	a = GenGetPrescalerValue(_freq_pwm, _signal_type);
	b = GenGetARRValueFromFreq(a, _freq_pwm, _signal_type);
	c = GenGetStepsCCRValueFromFreq(a, _freq_pwm, _freq_signal, b, SSIGNAL_MIN_POWER_K, SSIGNAL_MIN_PULSE_TIME, SSIGNAL_MIN_DEAD_TIME, _signal_type);
	c = b && ( ( ( _signal_type == SSIGNAL_SQUARE ) && ( c <= ( b / 2 ) ) ) || ( ( _signal_type != SSIGNAL_SQUARE ) && c ) );
	return (uint8_t)( ( a != USHRT_MAX ) && c );
}

double GetRealAvailablePWMFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const double _freq_signal, const uint8_t _signal_type)
{
	double _new_freq = 0;
  if ( !GenCheckFrequencyAvailable(_freq_pwm, _freq_signal, _signal_type) ) return 0;
	
	_new_freq = GenGetARRValueFromFreq(_tim_prescaler, _freq_pwm, _signal_type);
	_new_freq = GenGetPWMFreqValueFromTimer(_tim_prescaler, _new_freq, _signal_type);
	if ( _new_freq <= 0.0 )
	{
		return _freq_pwm;
	} else
	{
		return _new_freq;
	}
}
double GetNextAvailablePWMFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const double _freq_signal, const uint8_t _signal_type)
{
	double _new_freq = 0;
  if ( !GenCheckFrequencyAvailable(_freq_pwm, _freq_signal, _signal_type) ) return 0;
	
	_new_freq = GenGetARRValueFromFreq(_tim_prescaler, _freq_pwm, _signal_type);
	_new_freq--;
	_new_freq = GenGetPWMFreqValueFromTimer(_tim_prescaler, _new_freq, _signal_type);
	if ( _new_freq <= 0.0 )
	{
		return _freq_pwm;
	} else
	{
		return _new_freq;
	}
}

double GetPrevAvailablePWMFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const double _freq_signal, const uint8_t _signal_type)
{
	double _new_freq = 0;
  if ( !GenCheckFrequencyAvailable(_freq_pwm, _freq_signal, _signal_type) ) return 0;
	
	_new_freq = GenGetARRValueFromFreq(_tim_prescaler, _freq_pwm, _signal_type);
	_new_freq++;
	if ( _new_freq > USHRT_MAX ) return _freq_pwm;
	_new_freq = GenGetPWMFreqValueFromTimer(_tim_prescaler, _new_freq, _signal_type);
	if ( _new_freq <= 0.0 )
	{
		return _freq_pwm;
	} else
	{
		return _new_freq;
	}
}

double GetRealAvailableSignalFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const double _freq_signal, const uint8_t _signal_type)
{
	double _new_freq = 0;
	uint16_t _arr = 0;
  if ( !GenCheckFrequencyAvailable(_freq_pwm, _freq_signal, _signal_type) ) return 0;
	if ( _signal_type == SSIGNAL_SQUARE ) return 0;
	
	_arr = GenGetARRValueFromFreq(_tim_prescaler, _freq_pwm, _signal_type);
	_new_freq = GenGetStepsCCRValueFromFreq(_tim_prescaler, _freq_pwm, _freq_signal, _arr, SSIGNAL_MIN_POWER_K, SSIGNAL_MIN_PULSE_TIME, SSIGNAL_MIN_DEAD_TIME, _signal_type);
	_new_freq = GenGetSignalFreqValueFromTimer(_tim_prescaler, _arr, _new_freq, _signal_type); 
	return _new_freq;
}

double GetNextAvailableSignalFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const double _freq_signal, const uint8_t _signal_type)
{
	double _new_freq = 0;
	uint16_t _arr = 0;
  if ( !GenCheckFrequencyAvailable(_freq_pwm, _freq_signal, _signal_type) ) return 0;
	if ( _signal_type == SSIGNAL_SQUARE ) return 0;
	
	_arr = GenGetARRValueFromFreq(_tim_prescaler, _freq_pwm, _signal_type);
	_new_freq = GenGetStepsCCRValueFromFreq(_tim_prescaler, _freq_pwm, _freq_signal, _arr, SSIGNAL_MIN_POWER_K, SSIGNAL_MIN_PULSE_TIME, SSIGNAL_MIN_DEAD_TIME, _signal_type);
	_new_freq--;
	_new_freq = GenGetSignalFreqValueFromTimer(_tim_prescaler, _arr, _new_freq, _signal_type);
  if ( _new_freq <= 0.0 )
	{
		return _freq_signal;
	} else
  {
		return _new_freq;
	}
}

double GetPrevAvailableSignalFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const double _freq_signal, const uint8_t _signal_type)
{
	double _new_freq = 0;
	uint16_t _arr = 0;
  if ( !GenCheckFrequencyAvailable(_freq_pwm, _freq_signal, _signal_type) ) return 0;
	if ( _signal_type == SSIGNAL_SQUARE ) return 0;
	
	_arr = GenGetARRValueFromFreq(_tim_prescaler, _freq_pwm, _signal_type);
	_new_freq = GenGetStepsCCRValueFromFreq(_tim_prescaler, _freq_pwm, _freq_signal, _arr, SSIGNAL_MIN_POWER_K, SSIGNAL_MIN_PULSE_TIME, SSIGNAL_MIN_DEAD_TIME, _signal_type);
	_new_freq++;
	_new_freq = GenGetSignalFreqValueFromTimer(_tim_prescaler, _arr, _new_freq, _signal_type);
  if ( _new_freq <= 0.0 )
	{
		return _freq_signal;
	} else
  {
		return _new_freq;
	}
}

//*******************************************end of gen_timers.c*******************************************//
