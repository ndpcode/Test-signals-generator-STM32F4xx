#include "generator407.h"
//---------------------------------------------------
uint16_t burst_wave_1[GEN_CORE_ARRAY_MAX_COUNT*4 +4];
//uint32_t burst_wave_2[GEN_CORE_ARRAY_MAX_COUNT*4];
uint16_t burst_wave_3[GEN_CORE_ARRAY_MAX_COUNT*4 +4];
uint16_t *currentArray;

GeneratorTypeDef* 			genStruct;
GenParametersTypeDef* 	inputConfig;
GenParametersTypeDefForFlash MemoryFullParameters;
uint16_t PWM_Countdown[GEN_CORE_ARRAY_MAX_COUNT/2]; 
		
uint16_t arrays_in_genArray = 0;
int16_t arrays_counter = 2;

#define Led_Off  					GPIOA->MODER  |=  GPIO_MODER_MODER5_0; GPIOA->MODER  &= ~GPIO_MODER_MODER5_1; 	GPIOA->ODR  |=  0x00000020;
#define Led_On  					GPIOA->MODER  |=  GPIO_MODER_MODER5_0; GPIOA->MODER  &= ~GPIO_MODER_MODER5_1;   GPIOA->ODR  &= ~0x00000020;

#define PowerOut_Off  		GPIOC->MODER  |=  GPIO_MODER_MODER8_0; GPIOC->MODER  &= ~GPIO_MODER_MODER8_1; 	GPIOC->BSRR |=  0x01000000;
#define PowerOut_On  			GPIOC->MODER  |=  GPIO_MODER_MODER8_0; GPIOC->MODER  &= ~GPIO_MODER_MODER8_1;   GPIOC->BSRR |=  0x00000100;

#define PowerRele_Off  		GPIOC->MODER  |=  GPIO_MODER_MODER2_0; GPIOC->MODER  &= ~GPIO_MODER_MODER2_1; 	GPIOC->BSRR |=  0x00040000;
#define PowerRele_On  		GPIOC->MODER  |=  GPIO_MODER_MODER2_0; GPIOC->MODER  &= ~GPIO_MODER_MODER2_1;   GPIOC->BSRR |=  0x00000004;

volatile uint16_t z ;

uint16_t _steps;
uint16_t _precision;
uint32_t _realFrequency;
uint32_t _dead_time_ns;


uint16_t FinalFrequency;
uint16_t FinalDutyCycle;
uint8_t  StopEngine=0;
uint8_t  SmoothFlag=0;
uint16_t HzInSec_SpdFrq=5;

uint32_t Last_steps = (GEN_CORE_ARRAY_MAX_COUNT*4)*2;
uint8_t Fill_Array_end = 1;
//*******************************Р’РќР•РЁРќРР• Р’Р«Р—Р«Р’РђР•РњР«Р• Р¤РЈРќРљР¦РР*******************************//
uint16_t Gen407Init(void)
{
	genStruct = (GeneratorTypeDef*)malloc(sizeof(GeneratorTypeDef));
	memset(genStruct, 0, (sizeof(GeneratorTypeDef)));
	inputConfig = (GenParametersTypeDef*)malloc(sizeof(GenParametersTypeDef));
	memset (inputConfig, 0, (sizeof(GenParametersTypeDef)));
	
	return GEN_RESULT_OK;
}
uint16_t Gen407Close(GeneratorTypeDef** genStruct)
{
	free(genStruct);
	return GEN_RESULT_OK;
}
uint16_t Gen407Start(GeneratorTypeDef* genStruct)
{
	genStruct->generatorStartFlag = 1;
	PowerOut_On;
	PWM_SIN_Bridge(&genStruct->currentConfig);
	
	return GEN_RESULT_OK;
}
uint16_t Gen407Stop(GeneratorTypeDef* genStruct)
{
	Last_steps=0;
	StopEngine=1;
	if(genStruct->currentConfig.timStatus==1)
	{
		if (genStruct->currentConfig.modePWM == 9 && SmoothFlag==1)
		{
			Gen407_ThreePhase_SmoothStart(genStruct);
		}else{
			TIM1_KILL();
			StopEngine=0;
			PowerOut_Off;
		}
	}
	else if(genStruct->currentConfig.timStatus==2)
	{
		TIM2_KILL();
	}
	else if(genStruct->currentConfig.timStatus==4)
	{
		TIM3_KILL();
	}
	genStruct->generatorStartFlag = 0;
	return GEN_RESULT_OK;
}
uint16_t Gen407Update(GeneratorTypeDef* genStruct)
{
	if(genStruct->currentConfig.modePWM != 10)
	{
		StepsAndPrecisionValue		(genStruct->currentConfig.extFreq, genStruct->currentConfig.intFreq, genStruct->currentConfig.modePWM, genStruct->currentConfig.countdown, &_steps, &_precision);
		if(genStruct->selectedBuffer==1)
		{
			currentArray = (uint16_t*)burst_wave_3;
			genStruct->selectedBuffer=2;
			for(int i = 0; i<(GEN_CORE_ARRAY_MAX_COUNT*4); i++)
			{	
				burst_wave_3[i] = 0;
			}
		}
		else 	if(genStruct->selectedBuffer==2)
		{
			currentArray = (uint16_t*)burst_wave_1;
			genStruct->selectedBuffer=1;
			for(int i = 0; i<(GEN_CORE_ARRAY_MAX_COUNT*4); i++)
			{	
				burst_wave_1[i] = 0;
			}
		}

			switch(genStruct->currentConfig.modePWM)
		{
			case 0x01:
				FillArgument_PWM_HL				(genStruct->currentConfig.extDuty, genStruct->currentConfig.deadTime, genStruct->currentConfig.timStatus);// _=__=__=
			break;
			case 0x02:
				FillArgument_PWM_LL				(genStruct->currentConfig.extDuty, genStruct->currentConfig.intDuty, genStruct->currentConfig.deadTime, genStruct->currentConfig.timStatus);//__|||__|||__
			break;
			case 0x03:
				FillArgument_PWM_ODNOTAKT_PHASE_HL(genStruct->currentConfig.extDuty, genStruct->currentConfig.phaseShift, genStruct->currentConfig.timStatus);// _|___|__	
			break;																										
			case 0x04:
				FillArgument_PWM_ODNOTAKT_PHASE_LL(genStruct->currentConfig.extDuty, genStruct->currentConfig.intDuty, genStruct->currentConfig.phaseShift, genStruct->currentConfig.timStatus);// _|___|__	
			break;		
			case 0x05:
				FillArgument_SIN					(genStruct->currentConfig.extDuty, genStruct->currentConfig.deadTime, genStruct->currentConfig.timStatus);//__~~__~~__
			break;
			case 0x06:
				FillArgument_SIN_SHIFT		(genStruct->currentConfig.extDuty, genStruct->currentConfig.intDuty, genStruct->currentConfig.sinShift, genStruct->currentConfig.deadTime, genStruct->currentConfig.sinDirection, genStruct->currentConfig.timStatus);//__~ ~__
			break;
			case 0x07:
				FillArgument_TRIANGLE			(genStruct->currentConfig.extDuty, genStruct->currentConfig.intDuty, genStruct->currentConfig.sinShift, genStruct->currentConfig.deadTime, genStruct->currentConfig.sinDirection, genStruct->currentConfig.timStatus);//__/\__/\__
			break;
			case 0x08:
				FillArgument_PWM_Pulse		(genStruct->currentConfig.extDuty, genStruct->currentConfig.countdown, genStruct->currentConfig.timStatus);
			break;
			case 0x09:
				FillArgument_THREE_PHASE	(genStruct->currentConfig.extDuty, genStruct->currentConfig.phaseShift);
			  ThreePhaseDeadTime				(genStruct->currentConfig.deadTime);
			break;
		}

						
		DMA2_Stream5->CR |=   DMA_SxCR_TCIE;
		DMA2->HIFCR |= DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5;
		NVIC_EnableIRQ (DMA2_Stream5_IRQn);
		return GEN_RESULT_OK;
	}else if (genStruct->currentConfig.modePWM == 10)
	{
		SimpleGeneratorUpdate(genStruct->currentConfig.extFreq, genStruct->currentConfig.deadTime, genStruct->currentConfig.extDuty);
		return GEN_RESULT_OK;
	}
	return 0;
}

uint16_t Gen407Update_SmoothStart(GeneratorTypeDef* genStruct, uint16_t steps, uint16_t precision)
{
		_steps = steps;
		_precision = precision;
		
		//StepsAndPrecisionValue		(genStruct->currentConfig.extFreq, genStruct->currentConfig.intFreq, genStruct->currentConfig.modePWM, genStruct->currentConfig.countdown, &_steps, &_precision);
		if(genStruct->selectedBuffer==1)
		{
			currentArray = (uint16_t*)burst_wave_3;
			genStruct->selectedBuffer=2;
//			for(int i = 0; i<(GEN_CORE_ARRAY_MAX_COUNT*4); i++)
//			{	
//				burst_wave_3[i] = 0;
//			}
		}
		else 	if(genStruct->selectedBuffer==2)
		{
			currentArray = (uint16_t*)burst_wave_1;
			genStruct->selectedBuffer=1;
//			for(int i = 0; i<(GEN_CORE_ARRAY_MAX_COUNT*4); i++)
//			{	
//				burst_wave_1[i] = 0;
//			}
		}

		FillArgument_THREE_PHASE	(genStruct->currentConfig.extDuty, genStruct->currentConfig.phaseShift);
		//ThreePhaseDeadTime				(genStruct->currentConfig.deadTime);

						
		DMA2_Stream5->CR |=   DMA_SxCR_TCIE;
		DMA2->HIFCR |= DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5;
		NVIC_EnableIRQ (DMA2_Stream5_IRQn);
		return GEN_RESULT_OK;

	return 0;
}
void Gen407_ThreePhase_SmoothStart(GeneratorTypeDef* genStruct)
{
	FinalFrequency = genStruct->currentConfig.extFreq;
	FinalDutyCycle = genStruct->currentConfig.extDuty;
	
	if(StopEngine==0)
	{
		genStruct->currentConfig.extFreq = HzInSec_SpdFrq;
		genStruct->currentConfig.extDuty = 100;
		Gen407Update(genStruct);
	}else{
		genStruct->currentConfig.extFreq -= 1;
		genStruct->currentConfig.extDuty -= 1;		
	}
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->PSC = 8400 + 1;
	TIM3->ARR = 10000/HzInSec_SpdFrq;// 1000-10ghz  100-100ghz  10-1000ghz
	TIM3->CR1 = TIM_CR1_URS;
	TIM3->EGR |= TIM_EGR_UG;
	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->CR1 |= TIM_CR1_CEN | TIM_CR1_OPM;
	
	NVIC_EnableIRQ(TIM3_IRQn);	
	ThreePhaseDeadTime				(genStruct->currentConfig.deadTime);
}
void TIM3_IRQHandler()
{
	if(genStruct->currentConfig.extFreq != FinalFrequency)
	{
		if ( TIM3->SR & TIM_SR_UIF) 
		{
			TIM3->SR  &= ~TIM_SR_UIF;
			TIM3->CR1 &= ~TIM_CR1_CEN;
						
			if(StopEngine==0)
			{
				genStruct->currentConfig.extFreq = genStruct->currentConfig.extFreq + 1;//HzInSec_SpdFrq;
				genStruct->currentConfig.extDuty = genStruct->currentConfig.extDuty + (FinalDutyCycle / (FinalFrequency - HzInSec_SpdFrq));
				
				if(genStruct->currentConfig.extFreq > FinalFrequency)
				{
					genStruct->currentConfig.extFreq = FinalFrequency;
				}
				if(genStruct->currentConfig.extDuty > FinalDutyCycle)
				{
					genStruct->currentConfig.extDuty = FinalDutyCycle;
				}
				
				uint16_t steps_next = roundf((8020/genStruct->currentConfig.extFreq));if(steps_next%2 >= 1){steps_next--;}
				Gen407Update_SmoothStart(genStruct, steps_next, 10500);
				//Gen407Update(genStruct);
			}
			else if (StopEngine==1)
			{
				genStruct->currentConfig.extFreq = genStruct->currentConfig.extFreq - 1;
				genStruct->currentConfig.extDuty = genStruct->currentConfig.extDuty - (FinalDutyCycle / (FinalFrequency - HzInSec_SpdFrq));
				
				if((genStruct->currentConfig.extFreq < 1) || (genStruct->currentConfig.extDuty < (FinalDutyCycle / (FinalFrequency - HzInSec_SpdFrq)))	||	_steps>GEN_CORE_ARRAY_LENGTH)
				{
					_steps = GEN_CORE_ARRAY_LENGTH;
					genStruct->currentConfig.extFreq = FinalFrequency;
					genStruct->currentConfig.extDuty = FinalDutyCycle;
					TIM1_KILL();
					StopEngine=0;
				}else{
					uint16_t steps_next = roundf((8020/genStruct->currentConfig.extFreq));if(steps_next%2 >= 1){steps_next--;}
					Gen407Update_SmoothStart(genStruct, steps_next, 10500);
					//Gen407Update(genStruct);
				}
			}		
						
			TIM3->CR1 |= TIM_CR1_CEN;
		}
	}else
	{
		TIM3->SR  &= ~TIM_SR_UIF;
		TIM3->CR1 &= ~TIM_CR1_CEN;
	}
}
void DMA2_Stream5_IRQHandler()
{

	if(DMA2->HISR & DMA_HISR_TCIF5)
  {
	TIM1->BDTR &= ~TIM_BDTR_MOE;
//	Led_Off;
	DMA2->HIFCR |= DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5;
	DMA2_Stream5->CR &=  ~DMA_SxCR_EN;
	TIM1->CCR1 = 0;
	TIM1->CCR2 = 0;
	TIM1->CCR3 = 0;
	TIM1->CCR4 = 0;
	TIM1->CR1 &= ~TIM_CR1_CEN;	
	DMA2_Stream5->CR &=  ~DMA_SxCR_TCIE;
	NVIC_DisableIRQ (DMA2_Stream5_IRQn);
	
		if(genStruct->currentConfig.timStatus==1)
		{
			if(genStruct->selectedBuffer==1)
			{
				DMA2_Stream5->M0AR = (uint32_t)(&burst_wave_1);
			}
			else if (genStruct->selectedBuffer==2)
			{
				DMA2_Stream5->M0AR = (uint32_t)(&burst_wave_3);
			}
		}
		if(genStruct->currentConfig.modePWM != 9)
		{
			TIM1->ARR  = _precision-1;
			TIM1->DCR = (3 << 8) + ((((uint32_t)(&TIM1->CCR1)) - ((uint32_t)(&TIM1->CR1))) >> 2);
			TIM1->CR1 |=  TIM_CR1_CEN;
			TIM1->BDTR |= TIM_BDTR_MOE;
			DMA2_Stream5->NDTR =  (_steps)*4;
			DMA2_Stream5->PAR  = (uint32_t)(&(TIM1->DMAR));
			DMA2_Stream5->CR |=   DMA_SxCR_EN;
		//	Led_On;
		}else if (genStruct->currentConfig.modePWM == 9)
		{
			TIM1->ARR  = _precision;
			TIM1->DCR = ((3 - 1) << 8) + ((((uint32_t)(&TIM1->CCR1)) - ((uint32_t)(&TIM1->CR1))) >> 2);
			TIM1->CR1 |=  TIM_CR1_CEN;
			TIM1->BDTR |= TIM_BDTR_MOE;
			DMA2_Stream5->NDTR =  (_steps)*3;
			DMA2_Stream5->PAR  = (uint32_t)(&(TIM1->DMAR));
			
			DMA2_Stream5->CR |=   DMA_SxCR_CIRC;    
			DMA2_Stream5->CR |=   DMA_SxCR_MINC;   
			DMA2_Stream5->CR |=   DMA_SxCR_DIR_0;   
			DMA2_Stream5->CR &=  ~DMA_SxCR_DIR_1; 
			DMA2_Stream5->CR |=   DMA_SxCR_MSIZE_0;
			DMA2_Stream5->CR &=  ~DMA_SxCR_MSIZE_1;
			DMA2_Stream5->CR |=   DMA_SxCR_PSIZE_0;
			DMA2_Stream5->CR &=  ~DMA_SxCR_PSIZE_1;
			ThreePhaseDeadTime				(genStruct->currentConfig.deadTime);
			DMA2_Stream5->CR |=   DMA_SxCR_EN;
		}
  } else
  {
		DMA2->HIFCR |= DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5;
  }
////**************************************************************************************************************************************************
//	if((DMA2->HISR & DMA_HISR_TCIF5) && (Fill_Array_end==1))
//  {
//		if(Fill_Array_end==0)
//		{
//			NVIC_DisableIRQ (DMA2_Stream5_IRQn);
//			DMA2_Stream5->CR &= ~DMA_SxCR_EN;
//			DMA2->HIFCR |= DMA_HIFCR_CTCIF5;
//		}
//		if((genStruct->selectedBuffer==1)&& (Fill_Array_end==1))
//		{
//			Fill_Array_end = 0;
//			genStruct->selectedBuffer=2;
//			if(_steps < GEN_CORE_ARRAY_MAX_COUNT)
//			{
//				Last_steps = FillArgument_PWM_HL_Test	((void*)burst_wave_1, genStruct->currentConfig.extDuty, genStruct->currentConfig.deadTime, Last_steps, _steps*4, _steps*4);
//			}else
//			{
//				Last_steps = FillArgument_PWM_HL_Test	((void*)burst_wave_1, genStruct->currentConfig.extDuty, genStruct->currentConfig.deadTime, Last_steps/*(GEN_CORE_ARRAY_MAX_COUNT*4)*arrays_counter*/, _steps*4, GEN_CORE_ARRAY_MAX_COUNT*4);
//			}
//			Fill_Array_end = 1;
//		}
//		else if ((genStruct->selectedBuffer==2)&& (Fill_Array_end==1))
//		{
//			Fill_Array_end = 0;
//			genStruct->selectedBuffer=1;
//			if(_steps < GEN_CORE_ARRAY_MAX_COUNT)
//			{
//				Last_steps = FillArgument_PWM_HL_Test	((void*)burst_wave_3, genStruct->currentConfig.extDuty, genStruct->currentConfig.deadTime, Last_steps, _steps*4, _steps*4);
//			}else
//			{
//				Last_steps = FillArgument_PWM_HL_Test	((void*)burst_wave_3, genStruct->currentConfig.extDuty, genStruct->currentConfig.deadTime, Last_steps/*(GEN_CORE_ARRAY_MAX_COUNT*4)*arrays_counter*/, _steps*4, GEN_CORE_ARRAY_MAX_COUNT*4);
//			}
//			Fill_Array_end = 1;
//		}
//		
//		if(arrays_counter>=arrays_in_genArray-1)
//		{
//			arrays_counter=0;
//		}else{
//			arrays_counter++;
//		}
//		DMA2->HIFCR |= DMA_HIFCR_CTCIF5;
//  } else
//  {
//		DMA2->HIFCR |= DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5;
//  }
////**************************************************************************************************************************************************
}

void TWO_ARRAY__DMA2_Stream5_IRQHandler()
{
	if((DMA2->HISR & DMA_HISR_TCIF5) && (Fill_Array_end==1))
  {
		if(Fill_Array_end==0)
		{
			NVIC_DisableIRQ (DMA2_Stream5_IRQn);
			DMA2_Stream5->CR &= ~DMA_SxCR_EN;
			DMA2->HIFCR |= DMA_HIFCR_CTCIF5;
		}
		if((genStruct->selectedBuffer==1)&& (Fill_Array_end==1))
		{
			Fill_Array_end = 0;
			genStruct->selectedBuffer=2;
			if(_steps < GEN_CORE_ARRAY_MAX_COUNT)
			{
				Last_steps = FillArgument_PWM_HL_Test	((void*)burst_wave_1, genStruct->currentConfig.extDuty, genStruct->currentConfig.deadTime, Last_steps, _steps*4, _steps*4);
			}else
			{
				Last_steps = FillArgument_PWM_HL_Test	((void*)burst_wave_1, genStruct->currentConfig.extDuty, genStruct->currentConfig.deadTime, Last_steps/*(GEN_CORE_ARRAY_MAX_COUNT*4)*arrays_counter*/, _steps*4, GEN_CORE_ARRAY_MAX_COUNT*4);
			}
			Fill_Array_end = 1;
		}
		else if ((genStruct->selectedBuffer==2)&& (Fill_Array_end==1))
		{
			Fill_Array_end = 0;
			genStruct->selectedBuffer=1;
			if(_steps < GEN_CORE_ARRAY_MAX_COUNT)
			{
				Last_steps = FillArgument_PWM_HL_Test	((void*)burst_wave_3, genStruct->currentConfig.extDuty, genStruct->currentConfig.deadTime, Last_steps, _steps*4, _steps*4);
			}else
			{
				Last_steps = FillArgument_PWM_HL_Test	((void*)burst_wave_3, genStruct->currentConfig.extDuty, genStruct->currentConfig.deadTime, Last_steps/*(GEN_CORE_ARRAY_MAX_COUNT*4)*arrays_counter*/, _steps*4, GEN_CORE_ARRAY_MAX_COUNT*4);
			}
			Fill_Array_end = 1;
		}
		
		if(arrays_counter>=arrays_in_genArray-1)
		{
			arrays_counter=0;
		}else{
			arrays_counter++;
		}
		DMA2->HIFCR |= DMA_HIFCR_CTCIF5;
  } else
  {
		DMA2->HIFCR |= DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5;
  }
}
	
void Gen407TestSignal_Burst(void)
{
		Init_GPIO_TIM_All();
		uint16_t PrescallerTIM1 = 2;
		_steps 				= 2;
		_precision 		= 28;

		currentArray = (uint16_t*)burst_wave_1;
		genStruct->selectedBuffer=1;
		for(int i = 0; i<(GEN_CORE_ARRAY_MAX_COUNT*4); i++)
		{	
			burst_wave_1[i] = 0;
		}
		

			burst_wave_1[6] =(uint16_t) (_precision+1)/2;
			burst_wave_1[7] =(uint16_t) (_precision+1); 
			burst_wave_1[5] =(uint16_t) (_precision+1);
			burst_wave_1[4] =(uint16_t) (_precision+1);
			burst_wave_1[2] =(uint16_t) (_precision+1)/2;
			burst_wave_1[3] =(uint16_t) (0); 
			burst_wave_1[1] =(uint16_t) (0);
			burst_wave_1[0] =(uint16_t) (0);

		_realFrequency = (168000000/PrescallerTIM1)/(_precision * _steps);

		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
		TIM1->CCER   |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
		TIM1->CCMR1  |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;
		TIM1->CCMR1  |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;
		TIM1->CCMR2  |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE;
		TIM1->CCMR2  |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4PE;
		TIM1->DIER   |= TIM_DIER_UDE;
		TIM1->CR1    |= TIM_CR1_ARPE;  
		TIM1->BDTR   |= TIM_BDTR_MOE;
		TIM1->PSC = PrescallerTIM1-1;
		TIM1->ARR  = _precision-1;
		TIM1->DCR = (3 << 8) + ((((uint32_t)(&TIM1->CCR1)) - ((uint32_t)(&TIM1->CR1))) >> 2);
		TIM1->EGR |= TIM_EGR_UG;
		
		TIM1->CR1|= TIM_CR1_CEN;
		Init_DMA_TIM(_steps, TIM1);
		DMA2_Stream5->CR |=  DMA_SxCR_EN;
}
float Gen407_GetRealFrequency_Now(uint16_t steps, uint16_t precision)
{
	float REAL_Frequency=0;
	REAL_Frequency = (168000000.0/TIM1_Prescaller_)/(precision * steps);
	
	return REAL_Frequency;
}
uint32_t Gen407_GetRealFrequency(uint32_t extFreq)
{
	uint16_t steps;
	uint16_t precision;
	uint32_t REAL_Frequency;
	uint32_t Koeff = 0;
	
	Koeff = (168000000/TIM1_Prescaller_)/extFreq;//if(Koeff%2 >= 1){Koeff++;}

	steps = getMaxDivider(Koeff, GEN_CORE_ARRAY_LENGTH);if(steps%2 >= 1){steps--;}
	if(steps==Koeff){steps = Koeff / 18;}if(steps%2 >= 1){steps--;}

	if((Koeff - (steps*18))%steps==0)
	{
		precision = 18 + (Koeff - (steps*18))/steps;
	}else
	{
		steps = 2;
		precision = 18 + (Koeff - (steps*18))/steps;
	}
	
	if((steps * 65535)<Koeff)
	{
		steps = GEN_CORE_ARRAY_LENGTH;
		precision = Koeff / GEN_CORE_ARRAY_LENGTH;
	}
		
	REAL_Frequency = (168000000.0/TIM1_Prescaller_)/(precision*steps);
	

	return REAL_Frequency;
}
float Gen407_GetPrevFrequency(uint32_t extFreq)
{
	float		 Prev_Frequency=0.0;
	uint32_t Koeff = 0;
	
	Koeff = (168000000/TIM1_Prescaller_)/extFreq;
	if(Koeff%2 >= 1){Koeff--;}
	Koeff = Koeff + 2;
	Prev_Frequency = (168000000.0/TIM1_Prescaller_)/Koeff;

	return Prev_Frequency;
}
float Gen407_GetNextFrequency(uint32_t extFreq)
{
	float 	 Next_Frequency=0.0;
	uint32_t Koeff = 0;

	Koeff = (168000000/TIM1_Prescaller_)/extFreq;
	if(Koeff%2 >= 1){Koeff--;}
	Koeff = Koeff - 2;
	Next_Frequency = (168000000.0/TIM1_Prescaller_)/Koeff;

	return Next_Frequency;
}
uint32_t Gen407_GetPrevInternalFrequency(uint32_t intFreq, uint32_t extFreq)
{
	volatile uint32_t	Prev_Frequency=0;
	volatile uint32_t Koeff = 0;
	volatile uint32_t steps = 0;
	volatile uint32_t precision = 0;
	
	if(intFreq<=  extFreq*2){return extFreq*2;}
	
	Koeff = (168000000/TIM1_Prescaller_)/extFreq;
	steps = intFreq / extFreq;if(steps%2 >= 1){steps--;}
	
	while( ((steps * precision)!= Koeff || steps>5000 )&& precision<65535)
	{
			precision = Koeff / steps;
			if( (steps * precision)!= Koeff )
			{steps = steps - 2;}
	}
	if(precision>65535) {return intFreq;}
	Prev_Frequency = steps * extFreq;
	return Prev_Frequency;
}
uint32_t Gen407_GetNextInternalFrequency(uint32_t intFreq, uint32_t extFreq)
{
	volatile uint32_t	Next_Frequency=0;
	volatile uint32_t Koeff = 0;
	volatile uint32_t steps = 0;
	volatile uint32_t precision = 0;
	volatile uint32_t Msteps = 0;
	volatile uint32_t Mprecision = 0;
		
	Koeff = (168000000/TIM1_Prescaller_)/extFreq;
	
				Msteps = getMaxDivider(Koeff, GEN_CORE_ARRAY_LENGTH);if(Msteps%2 >= 1){Msteps--;}
				if(Msteps==Koeff){Msteps = Koeff / 18;}if(Msteps%2 >= 1){Msteps--;}
				if((Koeff - (Msteps*18))%Msteps==0)
				{
					Mprecision = 18 + (Koeff - (Msteps*18))/Msteps;
				}else
				{
					Msteps = 2;
					Mprecision = 18 + (Koeff - (Msteps*18))/Msteps;
				}
				
				if((Msteps * 65535)<Koeff)
				{
					Msteps = 2100;
					Mprecision = Koeff / 2100;
				}
	
	steps = intFreq / extFreq;if(steps%2 >= 1){steps++;}
	if( intFreq % extFreq!= 0){steps=steps+2;}
	
	while( ((steps * precision)!= Koeff || precision>65535) && steps<GEN_CORE_ARRAY_MAX_COUNT/* */)
	{
			precision = Koeff / steps;
			if( (steps * precision)!= Koeff && steps<GEN_CORE_ARRAY_MAX_COUNT)
			{steps = steps + 2;}else{
				if(precision>65535)
				{
					steps = steps + 2;
				}
			}
	}
	if(steps>Msteps){steps=Msteps ;}
	Next_Frequency = steps * extFreq;
	return Next_Frequency;
}
uint32_t Gen407_GetRealDuty(uint16_t Duty, uint32_t Steps)
{
	uint16_t z = (roundf(Steps*((double)Duty / 1000)));
	uint16_t RealDuty = (roundf((double)1000*((double)z / Steps)));
		
	return RealDuty;
}
uint32_t Gen407_GetNextDuty(uint16_t Duty, uint32_t Steps)
{
	uint16_t	Next_Duty=0;
	uint16_t  Steps_In_Count;
	
	uint16_t z = (roundf(Steps*((double)Duty / 1000)));
		
	if(Duty<1000)
	{
		if(Duty == 0 && Steps%2==1)
		{
			Next_Duty = (floor(1000*((double)(z + 1) / Steps)));
		}
		else
		{
			Next_Duty = (floor(1000*((double)(z + 2) / Steps)));
			if(Duty==Next_Duty)
			{
				Next_Duty = (floor(1000*((double)(z + 2*2) / Steps)));
				if(Duty==Next_Duty)
				{
					Next_Duty = (floor(1000*((double)(z + 2*4) / Steps)));
				}
			}
		}
	}else
	{
		Next_Duty = 1000;
	}
	
	if(Next_Duty>1000)
	{
		Next_Duty = 1000;
	}
	
	return Next_Duty;
}
uint32_t Gen407_GetPrevDuty(uint16_t Duty, uint32_t Steps)
{
	uint16_t	Prev_Duty=0;
	uint16_t  Steps_In_Count;
	
	uint16_t z = (roundf(Steps*((double)Duty / 1000)));
	
	if(Duty>0)
	{
		Prev_Duty = (floor(1000*((double)(z - 2) / Steps)));
		if(Duty==Prev_Duty)
		{
			Prev_Duty = (floor(1000*((double)(z - 2*2) / Steps)));
		}
	}else
	{
		Prev_Duty = 0;
	}
	
	return Prev_Duty;
}
uint16_t Gen407SetConfig(GeneratorTypeDef* genStruct, GenParametersTypeDef* inputConfig)
{
	genStruct->configLock = 0;
	
	genStruct->currentConfig.extFreq = 			inputConfig->extFreq;
	genStruct->currentConfig.intFreq = 			inputConfig->intFreq;
	genStruct->currentConfig.extDuty = 			inputConfig->extDuty;
	genStruct->currentConfig.intDuty = 			inputConfig->intDuty;
	genStruct->currentConfig.phaseShift = 	inputConfig->phaseShift;
	genStruct->currentConfig.sinShift =			inputConfig->sinShift;
	genStruct->currentConfig.deadTime = 		inputConfig->deadTime;
	genStruct->currentConfig.modePWM = 			inputConfig->modePWM;
	genStruct->currentConfig.countdown = 		inputConfig->countdown;
	genStruct->currentConfig.sinDirection = inputConfig->sinDirection;
	genStruct->currentConfig.timStatus = 		inputConfig->timStatus;
	
	return GEN_RESULT_OK;
}
uint16_t Gen407GetConfig(GeneratorTypeDef* genStruct, GenParametersTypeDef* outputConfig)
{
	outputConfig = (GenParametersTypeDef*)malloc(sizeof(GenParametersTypeDef));
	memset(outputConfig, 0, (sizeof(GenParametersTypeDef)));
	
	outputConfig->extFreq = 			genStruct->currentConfig.extFreq;
	outputConfig->intFreq = 			genStruct->currentConfig.intFreq;
	outputConfig->extDuty = 			genStruct->currentConfig.extDuty;
	outputConfig->intDuty = 			genStruct->currentConfig.intDuty;
	outputConfig->phaseShift = 		genStruct->currentConfig.phaseShift;
	outputConfig->deadTime = 			genStruct->currentConfig.deadTime;
	outputConfig->modePWM = 			genStruct->currentConfig.modePWM;
	outputConfig->countdown = 		genStruct->currentConfig.countdown;
	outputConfig->sinDirection = 	genStruct->currentConfig.sinDirection;
	outputConfig->timStatus = 		genStruct->currentConfig.timStatus;
	
	return GEN_RESULT_OK;
}

uint16_t Gen407SetExtFreq(GeneratorTypeDef* genStruct, uint32_t extFreq)
{
	genStruct->currentConfig.extFreq = extFreq;
	if(CheckDataErrors(&genStruct->currentConfig,1 != 0))
	{
		return GEN_VALUE_OUT_OF_RANGE;
	}
	return GEN_RESULT_OK;
}

uint16_t Gen407SetIntFreq(GeneratorTypeDef* genStruct, uint32_t intFreq)
{
	genStruct->currentConfig.intFreq = intFreq;
	if(CheckDataErrors(&genStruct->currentConfig,2 != 0))
	{
		return GEN_VALUE_OUT_OF_RANGE;
	}
	return GEN_RESULT_OK;
}

uint16_t Gen407SetExtDutyCycle(GeneratorTypeDef* genStruct, uint16_t extDutyCycle)
{
	genStruct->currentConfig.extDuty = extDutyCycle;
	if(CheckDataErrors(&genStruct->currentConfig,3 != 0))
	{
		return GEN_VALUE_OUT_OF_RANGE;
	}
	return GEN_RESULT_OK;
}

uint16_t Gen407SetIntDutyCycle(GeneratorTypeDef* genStruct, uint16_t intDutyCycle)
{
	genStruct->currentConfig.intDuty = intDutyCycle;
	if(CheckDataErrors(&genStruct->currentConfig,4) != 0)
	{
		return GEN_VALUE_OUT_OF_RANGE;
	}
	return GEN_RESULT_OK;
}

uint16_t Gen407SetPhaseShift(GeneratorTypeDef* genStruct, uint16_t phaseShift)
{
	genStruct->currentConfig.phaseShift = phaseShift;
	if(CheckDataErrors(&genStruct->currentConfig,5) != 0)
	{
		return GEN_VALUE_OUT_OF_RANGE;
	}
	return GEN_RESULT_OK;
}
uint16_t Gen407SetSinShift(GeneratorTypeDef* genStruct, uint16_t sinShift)
{
	genStruct->currentConfig.sinShift = sinShift;
	if(CheckDataErrors(&genStruct->currentConfig,5) != 0)
	{
		return GEN_VALUE_OUT_OF_RANGE;
	}
	return GEN_RESULT_OK;
}
uint16_t Gen407SetCountdown(GeneratorTypeDef* genStruct, uint16_t countdown)
{
	genStruct->currentConfig.countdown = countdown;
	if(CheckDataErrors(&genStruct->currentConfig,5) != 0)
	{
		return GEN_VALUE_OUT_OF_RANGE;
	}
	return GEN_RESULT_OK;
}
uint16_t Gen407SetDeadTime(GeneratorTypeDef* genStruct, uint32_t deadTime)
{
	genStruct->currentConfig.deadTime = deadTime;
	if(CheckDataErrors(&genStruct->currentConfig,6) != 0)
	{
		return GEN_VALUE_OUT_OF_RANGE;
	}
	return GEN_RESULT_OK;
}

uint16_t Gen407SetMode(GeneratorTypeDef* genStruct, uint8_t genMode)
{
	genStruct->currentConfig.modePWM = genMode;
	if(CheckDataErrors(&genStruct->currentConfig,7 != 0))
	{
		return GEN_VALUE_OUT_OF_RANGE;
	}
	return GEN_RESULT_OK;
}

uint16_t CheckDataErrors(GenParametersTypeDef* param, uint32_t code)
{
	switch (code)
	{
		case 1:
			if (param->extFreq < EXT_FREQ_MIN) return PARAM_ERR_EXTFREQ_LESSTHENRANGE;
			if (param->extFreq > EXT_FREQ_MAX) return PARAM_ERR_EXTFREQ_MORETHENRANGE;
			break;
		case 2:
			if (param->intFreq < INT_FREQ_MIN) return PARAM_ERR_INTFREQ_LESSTHENRANGE;
			if (param->intFreq > INT_FREQ_MAX) return PARAM_ERR_INTFREQ_MORETHENRANGE;
			break;
		case 3:
			if (param->extDuty < EXT_DUTY_CYCLE_MIN) return PARAM_ERR_EXTDUTY_LESSTHENRANGE;
			if (param->extDuty > EXT_DUTY_CYCLE_MAX) return PARAM_ERR_EXTDUTY_MORETHENRANGE;
			break;
		case 4:
			if (param->intDuty < INT_DUTY_CYCLE_MIN) return PARAM_ERR_INTDUTY_LESSTHENRANGE;
			if (param->intDuty > INT_DUTY_CYCLE_MAX) return PARAM_ERR_INTDUTY_MORETHENRANGE;
			break;
		case 5:
			if (param->phaseShift < PHASESHIFT_MIN) return PARAM_ERR_PHASESHIFT_LESSTHENRANGE;
			if (param->phaseShift > PHASESHIFT_MAX) return PARAM_ERR_PHASESHIFT_MORETHENRANGE;
			break;
		case 6:
			if (param->deadTime < DEADTIME_MIN) return PARAM_ERR_DEADTIME_LESSTHENRANGE;
			if (param->deadTime > DEADTIME_MAX) return PARAM_ERR_DEADTIME_MORETHENRANGE;
			break;
		case 7:
			if (param->modePWM < MODEPWM_MIN) return PARAM_ERR_MODEPWM_LESSTHENRANGE;
			if (param->modePWM > MODEPWM_MAX) return PARAM_ERR_MODEPWM_MORETHENRANGE;
			break;
		case 8:
			if (param->sinShift < SINSHIFT_MIN) return PARAM_ERR_SINSHIFT_LESSTHENRANGE;
			if (param->sinShift > SINSHIFT_MAX) return PARAM_ERR_SINSHIFT_MORETHENRANGE;
			break;
		case 9:
			if (param->countdown < COUNTDOWN_MIN) return PARAM_ERR_COUNTDOWN_LESSTHENRANGE;
			if (param->countdown > COUNTDOWN_MAX) return PARAM_ERR_COUNTDOWN_MORETHENRANGE;
			break;
		case 10:
			if (param->sinDirection < SINDIRECTION_MIN) return PARAM_ERR_SINDIRECTION_LESSTHENRANGE;
			if (param->sinDirection > SINDIRECTION_MAX) return PARAM_ERR_SINDIRECTION_MORETHENRANGE;
			break;
		case 11:
			if (param->timStatus < TIMSTATUS_MIN) return PARAM_ERR_TIMSTATUS_LESSTHENRANGE;
			if (param->timStatus > TIMSTATUS_MAX) return PARAM_ERR_TIMSTATUS_MORETHENRANGE;
			break;		
	}
	return PARAM_TEST_OK;
}
//*******************************************************************************************//
//*******************************Р’РќРЈРўР Р•РќРќРР• Р’Р«Р—Р«Р’РђР•РњР«Р• Р¤РЈРќРљР¦РР*******************************//
void StartGenerator(uint16_t steps, uint16_t precision, uint8_t timStatus)
{		
	if(timStatus==1)
	{
		Init_TIM(precision, TIM1);
		TIM1->CR1|= TIM_CR1_CEN;
		Init_DMA_TIM(steps, TIM1);
		DMA2_Stream5->CR |=  DMA_SxCR_EN;
	}else if(timStatus==2)
	{
		Init_TIM(precision, TIM2);
		TIM2->CR1|= TIM_CR1_CEN;
		Init_DMA_TIM(steps, TIM2);
		DMA1_Stream7->CR |=  DMA_SxCR_EN;
	}else if(timStatus==4)
	{
		Init_TIM(precision, TIM3); 
		TIM3->CR1|= TIM_CR1_CEN;
		Init_DMA_TIM(steps, TIM3);
		DMA1_Stream2->CR |=  DMA_SxCR_EN;
	}
}
uint16_t getMaxDivider (uint32_t number, uint32_t Start_Divider)
{
	volatile uint16_t MaxDivider = 0;

	for(int i = Start_Divider; i>2; i-=2)
	{
		if((number%i)==0   &&   (number/i)!=0)
		{
			MaxDivider = i;
			if((number/MaxDivider)>=150)
			{
				i=0;
				return MaxDivider;
			}
		}
	}
	return MaxDivider;
}
void StepsAndPrecisionValue(uint32_t extFreq, uint32_t intFreq, uint8_t MODE_PWM, uint32_t countdown, uint16_t* steps, uint16_t* precision)
{
	uint32_t Koeff 						 = 0;
	uint32_t InternalFrequency = 0;
	double Div;
	
	switch (MODE_PWM)
	{
		case 0x01:
				Koeff = (168000000/TIM1_Prescaller_)/extFreq;
				_steps = getMaxDivider(Koeff, GEN_CORE_ARRAY_LENGTH);
		
				if(_steps==Koeff){_steps = Koeff / 18;}if(_steps%2 >= 1){_steps--;}
				
				if( ((Koeff - (_steps*18))%_steps) == 0 && (_steps*18)<=Koeff)
				{
					_precision = (uint16_t)(18 + (Koeff - (_steps*18))/_steps);
				}else
				{
					_steps = 2;
					_precision = (uint16_t)(18 + (Koeff - (_steps*18))/_steps);
				}
				
				if((_steps * 65535)<Koeff)
				{
					_steps = GEN_CORE_ARRAY_LENGTH;
					_precision = Koeff / GEN_CORE_ARRAY_LENGTH;
				}
		break;
		case 0x02:
				Koeff = (168000000/TIM1_Prescaller_)/extFreq;//if(Koeff%2 >= 1){Koeff++;}

				_steps = getMaxDivider(Koeff, GEN_CORE_ARRAY_LENGTH);if(_steps%2 >= 1){_steps--;}
				if(_steps==Koeff){_steps = Koeff / 18;}if(_steps%2 >= 1){_steps--;}
				if( ((Koeff - (_steps*18))%_steps) == 0 && (_steps*18)<=Koeff)
				{
					_precision = 18 + (Koeff - (_steps*18))/_steps;
				}else
				{
					_steps = 2;
					_precision = 18 + (Koeff - (_steps*18))/_steps;
				}
				
				if((_steps * 65535)<Koeff)
				{
					_steps = GEN_CORE_ARRAY_LENGTH;
					_precision = Koeff / GEN_CORE_ARRAY_LENGTH;
				}
				InternalFrequency = _steps * extFreq;				
				Div =((double) InternalFrequency / intFreq );
				
				if (  Div > 1.0 && intFreq!=0)
				{
					_steps      = (uint16_t)(rint((double)_steps / Div));			
					_precision  = Koeff / _steps;
				}		
//				if(_steps%2 >= 1){_steps++;}
		break;
		case 0x03:
				Koeff = (168000000/TIM1_Prescaller_)/extFreq;//if(Koeff%2 >= 1){Koeff++;}

				_steps = getMaxDivider(Koeff, GEN_CORE_ARRAY_LENGTH);if(_steps%2 >= 1){_steps--;}
				if(_steps==Koeff){_steps = Koeff / 18;}if(_steps%2 >= 1){_steps--;}
				if( ((Koeff - (_steps*18))%_steps) == 0 && (_steps*18)<=Koeff)
				{
					_precision = 18 + (Koeff - (_steps*18))/_steps;
				}else
				{
					_steps = 2;
					_precision = 18 + (Koeff - (_steps*18))/_steps;
				}
				
				if((_steps * 65535)<Koeff)
				{
					_steps = GEN_CORE_ARRAY_LENGTH;
					_precision = Koeff / GEN_CORE_ARRAY_LENGTH;
				}
		break;
		case 0x04:
				Koeff = (168000000/TIM1_Prescaller_)/extFreq;//if(Koeff%2 >= 1){Koeff++;}

				_steps = getMaxDivider(Koeff, GEN_CORE_ARRAY_LENGTH);if(_steps%2 >= 1){_steps--;}
				if(_steps==Koeff){_steps = Koeff / 18;}if(_steps%2 >= 1){_steps--;}
				if( ((Koeff - (_steps*18))%_steps) == 0 && (_steps*18)<=Koeff)
				{
					_precision = 18 + (Koeff - (_steps*18))/_steps;
				}else
				{
					_steps = 2;
					_precision = 18 + (Koeff - (_steps*18))/_steps;
				}
				
				if((_steps * 65535)<Koeff)
				{
					_steps = GEN_CORE_ARRAY_LENGTH;
					_precision = Koeff / GEN_CORE_ARRAY_LENGTH;
				}
				InternalFrequency = _steps * extFreq;				
				Div =((double) InternalFrequency / intFreq );
				
				if (  Div > 1.0 && intFreq!=0)
				{
					_steps      = (uint16_t)(rint((double)_steps / Div));			
					_precision  = Koeff / _steps;
				}		
		break;
		case 0x05:
//				Koeff = (168000000/TIM1_Prescaller_)/extFreq;//if(Koeff%2 >= 1){Koeff++;}

//				_steps = getMaxDivider(Koeff);if(_steps%2 >= 1){_steps--;}
//				if(_steps==Koeff){_steps = Koeff / 18;}if(_steps%2 >= 1){_steps--;}
//				if((Koeff - (_steps*18))%_steps==0)
//				{
//					_precision = 18 + (Koeff - (_steps*18))/_steps;
//				}else
//				{
//					_steps = 2;
//					_precision = 18 + (Koeff - (_steps*18))/_steps;
//				}
//				
//				if((_steps * 65535)<Koeff)
//				{
//					_steps = 2100;
//					_precision = Koeff / 2100;
//				}
//				InternalFrequency = _steps * extFreq;				
//				Div =((double) InternalFrequency / intFreq );
//				
//				if (  Div > 1.0 && intFreq!=0)
//				{
//					_steps      = (uint16_t)(rint((double)_steps / Div));			
//					_precision  = Koeff / _steps;
//				}		
		break;
		case 0x06:
				Koeff = (168000000/TIM1_Prescaller_)/extFreq;//if(Koeff%2 >= 1){Koeff++;}

				_steps = getMaxDivider(Koeff, GEN_CORE_ARRAY_LENGTH);if(_steps%2 >= 1){_steps--;}
				if(_steps==Koeff){_steps = Koeff / 18;}if(_steps%2 >= 1){_steps--;}
				if( ((Koeff - (_steps*18))%_steps) == 0 && (_steps*18)<=Koeff)
				{
					_precision = 18 + (Koeff - (_steps*18))/_steps;
				}else
				{
					_steps = 2;
					_precision = 18 + (Koeff - (_steps*18))/_steps;
				}
				
				if((_steps * 65535)<Koeff)
				{
					_steps = GEN_CORE_ARRAY_LENGTH;
					_precision = Koeff / GEN_CORE_ARRAY_LENGTH;
				}
				InternalFrequency = _steps * extFreq;				
				Div =((double) InternalFrequency / intFreq );
				
				if (  Div > 1.0 && intFreq!=0)
				{
					_steps      = (uint16_t)(rint((double)_steps / Div));			
					_precision  = Koeff / _steps;
				}	
		break;
		case 0x07:
				Koeff = (168000000/TIM1_Prescaller_)/extFreq;//if(Koeff%2 >= 1){Koeff++;}

				_steps = getMaxDivider(Koeff, GEN_CORE_ARRAY_LENGTH);if(_steps%2 >= 1){_steps--;}
				if(_steps==Koeff){_steps = Koeff / 18;}if(_steps%2 >= 1){_steps--;}
				if( ((Koeff - (_steps*18))%_steps) == 0 && (_steps*18)<=Koeff)
				{
					_precision = 18 + (Koeff - (_steps*18))/_steps;
				}else
				{
					_steps = 2;
					_precision = 18 + (Koeff - (_steps*18))/_steps;
				}
				
				if((_steps * 65535)<Koeff)
				{
					_steps = GEN_CORE_ARRAY_LENGTH;
					_precision = Koeff / GEN_CORE_ARRAY_LENGTH;
				}
				InternalFrequency = _steps * extFreq;				
				Div =((double) InternalFrequency / intFreq );
				
				if (  Div > 1.0 && intFreq!=0)
				{
					_steps      = (uint16_t)(rint((double)_steps / Div));			
					_precision  = Koeff / _steps;
				}	
		break;
		case 0x08:
				Koeff = (168000000/TIM1_Prescaller_)/extFreq;
				_steps = getMaxDivider(Koeff, GEN_CORE_ARRAY_LENGTH);
		
				if(_steps==Koeff){_steps = Koeff / 18;}if(_steps%2 >= 1){_steps--;}
				
				if( ((Koeff - (_steps*18))%_steps) == 0 && (_steps*18)<=Koeff)
				{
					_precision = (uint16_t)(18 + (Koeff - (_steps*18))/_steps);
				}else
				{
					_steps = 2;
					_precision = (uint16_t)(18 + (Koeff - (_steps*18))/_steps);
				}
				
				if((_steps * 65535)<Koeff)
				{
					_steps = GEN_CORE_ARRAY_LENGTH;
					_precision = Koeff / GEN_CORE_ARRAY_LENGTH;
				}
		break;
		case 0x09:
				Koeff = (168000000/TIM1_Prescaller_)/extFreq;//if(Koeff%2 >= 1){Koeff++;}

				_steps = getMaxDivider(Koeff, GEN_CORE_ARRAY_LENGTH);if(_steps%2 >= 1){_steps--;}
				if(_steps==Koeff){_steps = Koeff / 18;}if(_steps%2 >= 1){_steps--;}
				if( ((Koeff - (_steps*18))%_steps) == 0 && (_steps*18)<=Koeff)
				{
					_precision = 18 + (Koeff - (_steps*18))/_steps;
				}else
				{
					_steps = 2;
					_precision = 18 + (Koeff - (_steps*18))/_steps;
				}
				
				if((_steps * 65535)<Koeff)
				{
					_steps = GEN_CORE_ARRAY_LENGTH;
					_precision = Koeff / GEN_CORE_ARRAY_LENGTH;
				}
				InternalFrequency = _steps * extFreq;				
				Div =((double) InternalFrequency / intFreq );
				
				if (  Div > 1.0 && intFreq!=0)
				{
					_steps      = (uint16_t)(rint((double)_steps / Div));			
					_precision  = Koeff / _steps;
				}	
		break;
	}
	_realFrequency = 84000000/(_precision * _steps);
	_dead_time_ns =  (  (1000000000 / _realFrequency) / _steps);
}

void StepsAndPrecision(uint32_t extFreq, uint32_t intFreq, uint8_t MODE_PWM, uint32_t countdown, uint16_t* steps, uint16_t* precision)
{
	uint32_t Koeff 						 = 0;
	
	switch (MODE_PWM)
	{
		case 0x01:
				Koeff = (168000000/TIM1_Prescaller_)/extFreq;
				(*steps) = getMaxDivider(Koeff, GEN_CORE_ARRAY_LENGTH_Virtual);
		
				if((*steps)==Koeff){(*steps) = Koeff / 18;}if((*steps)%2 >= 1){(*steps)--;}
				
				if( ((Koeff - ((*steps)*18))%(*steps)) == 0 && ((*steps)*18)<=Koeff)
				{
					_precision = (uint16_t)(18 + (Koeff - ((*steps)*18))/(*steps));
				}else
				{
					(*steps) = 2;
					_precision = (uint16_t)(18 + (Koeff - ((*steps)*18))/(*steps));
				}
				
				if(((*steps) * 65535)<Koeff)
				{
					(*steps) = GEN_CORE_ARRAY_LENGTH;
					_precision = Koeff / GEN_CORE_ARRAY_LENGTH;
				}
		break;
	}
}
void PWM_SIN_Bridge(GenParametersTypeDef* param)
{		
	genStruct->selectedBuffer=1;
	switch(param->modePWM)
	{
		case 0x01:
			Init_GPIO_TIM_All();
			StepsAndPrecisionValue		(param->extFreq, param->intFreq, param->modePWM, param->countdown, &_steps, &_precision);
			BurstArraySelect					(param->timStatus);
			FillArgument_PWM_HL				(param->extDuty, param->deadTime, param->timStatus);
////**************************************************************************************************************		
//			StepsAndPrecision					(param->extFreq, param->intFreq, param->modePWM, param->countdown, &_steps, &_precision);
//			arrays_in_genArray = 	(uint16_t)ceil((double)_steps / GEN_CORE_ARRAY_MAX_COUNT);
//			arrays_counter=2;
//				
//			if(_steps < GEN_CORE_ARRAY_MAX_COUNT)
//			{
//				FillArgument_PWM_HL_Test	((void*)burst_wave_1, param->extDuty, param->deadTime, 0, _steps*4, _steps*4);
//				StartGenerator						(_steps, _precision, param->timStatus);
//			}else
//			{
//				FillArgument_PWM_HL_Test	((void*)burst_wave_1, param->extDuty, param->deadTime, 0, _steps*4, GEN_CORE_ARRAY_MAX_COUNT*4);
//				FillArgument_PWM_HL_Test	((void*)burst_wave_3, param->extDuty, param->deadTime, GEN_CORE_ARRAY_MAX_COUNT*4, _steps*4, GEN_CORE_ARRAY_MAX_COUNT*4);
//				StartGenerator						(GEN_CORE_ARRAY_MAX_COUNT, _precision, param->timStatus);
//			}
////**************************************************************************************************************	
		
			StartGenerator						(_steps, _precision, param->timStatus);
			
		break;
		case 0x02:
			Init_GPIO_TIM_All();
			StepsAndPrecisionValue		(param->extFreq, param->intFreq, param->modePWM, param->countdown, &_steps, &_precision);
			BurstArraySelect					(param->timStatus);
			FillArgument_PWM_LL				(param->extDuty, param->intDuty, param->deadTime, param->timStatus);//__|||__|||__
			StartGenerator						(_steps, _precision, param->timStatus);
		break;
		case 0x03:
			Init_GPIO_TIM_All();
			StepsAndPrecisionValue		(param->extFreq, param->intFreq, param->modePWM, param->countdown, &_steps, &_precision);
			BurstArraySelect					(param->timStatus);
			FillArgument_PWM_ODNOTAKT_PHASE_HL(param->extDuty, param->phaseShift, param->timStatus);// _|___|__	
			StartGenerator						(_steps, _precision, param->timStatus);
		break;																										
		case 0x04:
			Init_GPIO_TIM_All();
			StepsAndPrecisionValue		(param->extFreq, param->intFreq, param->modePWM, param->countdown, &_steps, &_precision);
			BurstArraySelect					(param->timStatus);
			FillArgument_PWM_ODNOTAKT_PHASE_LL(param->extDuty, param->intDuty, param->phaseShift, param->timStatus);// _|___|__	
			StartGenerator						(_steps, _precision, param->timStatus);
		break;		
		case 0x05:
			Init_GPIO_TIM_All();
			StepsAndPrecisionValue		(param->extFreq, param->intFreq, param->modePWM, param->countdown, &_steps, &_precision);
			BurstArraySelect					(param->timStatus);
			FillArgument_SIN					(param->extDuty, param->deadTime, param->timStatus);//__~~__~~__
			StartGenerator						(_steps, _precision, param->timStatus);
		break;
		case 0x06:
			Init_GPIO_TIM_All();
			StepsAndPrecisionValue		(param->extFreq, param->intFreq, param->modePWM, param->countdown, &_steps, &_precision);
			BurstArraySelect					(param->timStatus);
			FillArgument_SIN_SHIFT		(param->extDuty, param->intFreq, param->sinShift, param->deadTime, param->sinDirection, param->timStatus);//__~ ~__
			StartGenerator						(_steps, _precision, param->timStatus);
		break;
		case 0x07:
			Init_GPIO_TIM_All();
			StepsAndPrecisionValue		(param->extFreq, param->intFreq, param->modePWM, param->countdown, &_steps, &_precision);
			BurstArraySelect					(param->timStatus);
			FillArgument_TRIANGLE			(param->extDuty, param->intFreq, param->sinShift, param->deadTime, param->sinDirection, param->timStatus);//__/\__/\__
			StartGenerator						(_steps, _precision, param->timStatus);
		break;
		case 0x08:
			Init_GPIO_TIM_All();
			StepsAndPrecisionValue		(param->extFreq, param->intFreq, param->modePWM, param->countdown, &_steps, &_precision);
			BurstArraySelect					(param->timStatus);
			FillArgument_PWM_Pulse		(param->extDuty, param->countdown, param->timStatus);
			StartGenerator						(_steps, _precision, param->timStatus);
		break;
		case 0x09:
			Init_GPIO_TIM1_THREE_PHASE();
			if(SmoothFlag==1)
			{
				FinalFrequency = param->extFreq;
				FinalDutyCycle = param->extDuty = 1000;
				StepsAndPrecisionValue		(HzInSec_SpdFrq, param->intFreq, param->modePWM, param->countdown, &_steps, &_precision);
				BurstArraySelect					(param->timStatus);
				FillArgument_THREE_PHASE	(100, param->phaseShift);
				Init_TIM_THREE_PHASE			(_precision);		
				Init_DMA_TIM_THREE_PHASE	(_steps);
				ThreePhaseDeadTime				(param->deadTime);
				TIM1->CR1|= TIM_CR1_CEN;
				DMA2_Stream5->CR |=  DMA_SxCR_EN;
			
				genStruct->currentConfig.extFreq = FinalFrequency;
				genStruct->currentConfig.extDuty = FinalDutyCycle;
				Gen407_ThreePhase_SmoothStart(genStruct);
			}else{
				param->extDuty = 1000;
				StepsAndPrecisionValue		(param->extFreq, param->intFreq, param->modePWM, param->countdown, &_steps, &_precision);
				BurstArraySelect					(param->timStatus);
				FillArgument_THREE_PHASE	(1000, param->phaseShift);
				Init_TIM_THREE_PHASE			(_precision);		
				Init_DMA_TIM_THREE_PHASE	(_steps);
				ThreePhaseDeadTime				(param->deadTime);
				TIM1->CR1|= TIM_CR1_CEN;
				DMA2_Stream5->CR |=  DMA_SxCR_EN;
			}
		break;
		case 0xA:
//			SimpleGeneratorInit();
			Init_GPIO_TIM1_THREE_PHASE();
			SimpleGeneratorOn(param->extFreq, param->deadTime, param->extDuty);
		break;
	}
}
void BurstArraySelect(uint8_t timStatus)
{
	if(timStatus == 1){currentArray = (uint16_t*)burst_wave_1;}
	if(timStatus == 4){currentArray = (uint16_t*)burst_wave_3;}
}
void ThreePhaseDeadTime(uint32_t DEAD_time_count)
{
	uint16_t period = round((double)(1000000000/(double)TIMER_INPUT_FREQ));
	
	TIM1->BDTR &= ~TIM_BDTR_LOCK_0 & ~TIM_BDTR_LOCK_1;
	if(DEAD_time_count<=127)
	{
		_dead_time_ns = period;
		TIM1->BDTR   &=  ~TIM_BDTR_DTG;
		TIM1->BDTR   &= 0xffffff00;
		TIM1->BDTR   |=  DEAD_time_count;
	}else if (DEAD_time_count>127 && DEAD_time_count<=127+64)
	{
		_dead_time_ns = 2*period;
		TIM1->BDTR   &=  ~TIM_BDTR_DTG;
		TIM1->BDTR   |=  (2<<6);
		TIM1->BDTR   |=  (DEAD_time_count);
		
	}else if (DEAD_time_count>127+64 && DEAD_time_count<=127+64+32)
	{
		_dead_time_ns = 8*period;
		TIM1->BDTR   &=  ~TIM_BDTR_DTG;
		TIM1->BDTR   |=  (6<<5);
		TIM1->BDTR   |=  (DEAD_time_count-(127+64));
	}else if (DEAD_time_count>127+64+32 && DEAD_time_count<127+64+32+32)
	{
		_dead_time_ns = 16*period;
		TIM1->BDTR   &=  ~TIM_BDTR_DTG;
		TIM1->BDTR   |=  (7<<5);
		TIM1->BDTR   |=  (DEAD_time_count-(127+64+32));
	}else if (DEAD_time_count>=127+64+32+32)
	{
		_dead_time_ns = 16*period;
		TIM1->BDTR   &=  ~TIM_BDTR_DTG;
		TIM1->BDTR   |=  (7<<5);
		TIM1->BDTR   |=  (127+64+32);
	}
}
uint16_t convert_ns_in_pt (uint16_t value_ns)
{
	uint16_t value_pt=0;
	uint16_t period = 6;
	
	if((value_ns/period)<=128)
	{
		value_pt = (value_ns/period);
	}
	else if ((value_ns/period)<=(128+64*2))
	{
		value_pt = ((value_ns/period)-128)/2 + 128;
	}
	else if ((value_ns/period)<=(128+64*2+32*8))
	{
		value_pt = ((value_ns/period)-128-(64*2))/8 +128 + 64;
	}
	else if ((value_ns/period)<=(128+64*2+32*8+32*16))
	{
		value_pt = ((value_ns/period)-128-(64*2)-(32*8))/16 +128 + 64 + 32;
	}
	
	return value_pt;
}
uint16_t convert_pt_in_ns (uint16_t value_pt)
{
	uint16_t value_ns=0;
	uint16_t period = 6;

	if(value_pt<=128)
	{
		value_ns = value_pt * period;
	}
	else if (value_pt<=128+64)
	{
		value_ns = (value_pt-128+64) * period*2;
	}
	else if (value_pt<=128+64+32)
	{
		value_ns = (value_pt-128-64+32) * period*8;
	}
	else if (value_pt<=128+64+32+32)
	{
		value_ns = (value_pt-128-64-32+32) * period*16;
	}
	else if (value_pt>=128+64+32+32)
	{
		value_ns = 256 * period*16;
	}
	
	return value_ns;
}
//*******************************************************************************************//
//************************************Р—РђРџРћР›РќР•РќРР• РњРђРЎРЎРР’РћР’************************************//
void FillArgument_THREE_PHASE(uint16_t DUTY_Cycle, uint16_t PHASE_SHIFT)
{
	double steps_grad = (double)(360 / (double)_steps);
	double shift = (PHASE_SHIFT * PI)/180;//1.04719755
	for(int i = 0; i < _steps; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			currentArray[i*3+j] = (uint16_t)(((1+     (floor(_precision/2*((double)DUTY_Cycle / 1000)))       )*(1 + sin((double)(i*steps_grad*PI)/180 + j*shift))) );
		}
	}
}

void FillArgument_PWM_Pulse(uint16_t DUTY_Cycle_1, uint32_t Number_Pulse, uint8_t Tims_On)
{
	uint16_t z = (floor(_steps*2*((double)DUTY_Cycle_1 / 1000)));
	uint16_t d2 = ((_steps*2)-(((_steps*2)- z)/2)) - ((_steps*2)-(((_steps*2)- z)/2))%4;
	uint16_t d1 = (((_steps*2)- z)/2);
	
	volatile uint16_t max_Number_Pulse = (_steps/2);
	volatile uint16_t steps_in_one_Number_Pulse = max_Number_Pulse / Number_Pulse;
	
	uint16_t q = 0;
	
	if (Tims_On == 2)
	{
//		for(int f = 0; f<(Number_Pulse); f+=1)
//		{
//			double zz = (floor( _precision * ((double)PWM_Countdown[q] / 100)));
//			for(int i = 0; i<(steps_in_one_Number_Pulse); i+=1)
//			{
//				burst_wave_2[(((f*steps_in_one_Number_Pulse)+i)*4)] 						 = zz; 
//				burst_wave_2[(_steps*2)+(((f*steps_in_one_Number_Pulse)+i)*4)+2] = zz; 
//			}
//			q++;
//		}
//		
//		for(int i = d2; i<(_steps*2); i+=4)
//		{
//			burst_wave_2[i] 						 = 0; 
//			burst_wave_2[(_steps*2)+i+2] = 0;
//		}
//		
//		for(int i = 0; i<d1; i+=4)
//		{
//			burst_wave_2[i] 						 = 0; 
//			burst_wave_2[(_steps*2)+i+2] = 0;
//		}
				
//		for(int i = 0; i<(((_steps*2) - DEAD_time_percent*4)); i+=4)
//		{	
//			burst_wave_2[i+1] 					 = (_precision+1); 
//			burst_wave_2[(_steps*2)+i+3] = (_precision+1);
//		}
//	
//		for(int i = ((_steps*2) - DEAD_time_percent*4); i<(_steps*4); i+=4)
//		{	
//			burst_wave_2[i] 						 = 0;
//			burst_wave_2[i+1] 					 = 0; 
//			burst_wave_2[(_steps*2)+i+2] = 0; 
//			burst_wave_2[(_steps*2)+i+3] = 0;
//		}
	}
	else
	{

		if((_steps/2) > Number_Pulse)
		{
			for(int f = 0; f<(Number_Pulse); f+=1)
			{
				//double zz = (floor( _precision * ((double)PWM_Countdown[q] / 100)));
				uint16_t num_duty = (floor(steps_in_one_Number_Pulse*((double)PWM_Countdown[q] / 100)));
				for(int i = 0; i<(num_duty); i+=1)
				{
					currentArray[(((f*steps_in_one_Number_Pulse)+i)*4)] 						 = (_precision+1);//zz; 
					currentArray[(_steps*2)+(((f*steps_in_one_Number_Pulse)+i)*4)+2] = (_precision+1);//zz; 
				}
				q++;
			}
		}else
		{
			for(int i = 0; i<(_steps*2); i+=4)
			{
				double zz = (floor( _precision * ((double)PWM_Countdown[q] / 100)));
				currentArray[i] 						 = zz; 
				currentArray[(_steps*2)+i+2] = zz; 
				q++;
			}
		}
		
		for(int i = d2; i<(_steps*2); i+=4)
		{
			currentArray[i] 						 = 0; 
			currentArray[(_steps*2)+i+2] = 0;
		}
		
		for(int i = 0; i<d1; i+=4)
		{
			currentArray[i] 						 = 0; 
			currentArray[(_steps*2)+i+2] = 0;
		}
				
		uint16_t DEAD_time_percent =0;
		for(int i = 0; i<(((_steps*2) - DEAD_time_percent*4)); i+=4)
		{	
			currentArray[i+1] 					 = (_precision+1); 
			currentArray[(_steps*2)+i+3] = (_precision+1);
		}
	
		for(int i = ((_steps*2) - DEAD_time_percent*4); i<(_steps*4); i+=4)
		{	
			currentArray[i] 						 = 0;
			currentArray[i+1] 					 = 0; 
			currentArray[(_steps*2)+i+2] = 0;
			currentArray[(_steps*2)+i+3] = 0;
		}
	}
} 
void FillArgument_TRIANGLE(uint16_t DUTY_Cycle_1, uint16_t DUTY_Cycle_2, uint16_t SIN_SHIFT, uint32_t DEAD_time_percent, uint8_t SIN_DIRECTION, uint8_t Tims_On)
{
	uint16_t z = (floor(_steps*2*((double)DUTY_Cycle_1 / 1000)));
	uint16_t d2 = ((_steps*2)-(((_steps*2)- z)/2)) - ((_steps*2)-(((_steps*2)- z)/2))%4;
	uint16_t d1 = (((_steps*2)- z)/2);
	volatile double stepLenght ;
	if (SIN_DIRECTION==1)
	{
		 stepLenght =  ( ((_precision+1) * ((double)DUTY_Cycle_2 / 1000) ) / (floor(_steps*2) - (floor(_steps * ( (double)((double)(SIN_SHIFT+100) / 100) )) ))  );
	}
	else if (SIN_DIRECTION==0)
	{
		 stepLenght =  ( ((_precision+1) * ((double)DUTY_Cycle_2 / 1000) ) / (floor(_steps*2) - (floor(_steps * ( (double)((100-(double)SIN_SHIFT) / 100) )) ))  );
	}
//	if(stepLenght==0){stepLenght=1;}
//uint16_t stepLenght =  ( ((_precision+1) * ((double)DUTY_Cycle_2 / 1000) ) / (floor(_steps*2) ));
	volatile double stepLenght1 = ( ((_precision+1) * ((double)DUTY_Cycle_2 / 1000) ) / (floor(_steps * ( (double)((double)(SIN_SHIFT+100) / 100) )) ) );
  volatile double stepLenght2 = ( ((_precision+1) * ((double)DUTY_Cycle_2 / 1000) ) / (floor(_steps * ( (double)((100-(double)SIN_SHIFT) / 100) )) ) );
//	if(stepLenght1==0){stepLenght1=1;}if(stepLenght2==0){stepLenght2=1;}
	if (Tims_On == 2)
	{
//		for(int i = 0; i<(_steps*2); i+=4)
//		{
//			if ( i <=(_steps))
//			{
//				burst_wave_2[i] 								= (i * stepLenght1 ); 
//				burst_wave_2[(_steps*2)+i+2]		= (i * stepLenght1 );
//			}else{
//				burst_wave_2[i]									= ((_steps*2 - i) * stepLenght2 );
//				burst_wave_2[(_steps*2)+i+2]		= ((_steps*2 - i) * stepLenght2 );
//			}
//		}
//		
//		for(int i = d2; i<(_steps*2); i+=4)
//		{
//			burst_wave_2[i] 						 = 0; 
//			burst_wave_2[(_steps*2)+i+2] = 0;
//		}
//		
//		for(int i = 0; i<d1; i+=4)
//		{
//			burst_wave_2[i] 						 = 0; 
//			burst_wave_2[(_steps*2)+i+2] = 0;
//		}
//				
//		for(int i = 0; i<(((_steps*2) - DEAD_time_percent*4)); i+=4)
//		{	
//			burst_wave_2[i+1] 					 = (_precision+1); 
//			burst_wave_2[(_steps*2)+i+3] = (_precision+1);
//		}

//		for(int i = ((_steps*2) - DEAD_time_percent*4); i<(_steps*4); i+=4)
//		{	
//			burst_wave_2[i] 						 = 0;
//			burst_wave_2[i+1] 					 = 0; 
//			burst_wave_2[(_steps*2)+i+2] = 0;
//			burst_wave_2[(_steps*2)+i+3] = 0;
//		}
	}
	else
	{
		for(int i = 0; i<(_steps*2); i+=4)
		{
			if ( i <=(_steps)*( (double)(SIN_SHIFT+100) / 100 ) && SIN_DIRECTION==1)
			{
				currentArray[i] 								= (i * stepLenght1 ); 
				currentArray[(_steps*2)+i+2]		= (i * stepLenght1 );
//				currentArray[i] 								= (floor((i * stepLenght ) * ((double)DUTY_Cycle_2 / 1000)));
//				currentArray[(_steps*2)+i+2]		= (floor((i * stepLenght ) * ((double)DUTY_Cycle_2 / 1000)));
				
			}else if ( i <=(_steps)*( (double)(((100-(double)SIN_SHIFT) / 100)) ) && SIN_DIRECTION==0)
			{
				currentArray[i] 								= (i * stepLenght2 ); 
				currentArray[(_steps*2)+i+2]		= (i * stepLenght2 );
//				currentArray[i] 								= (floor((i * stepLenght ) * ((double)DUTY_Cycle_2 / 1000)));
//				currentArray[(_steps*2)+i+2]		= (floor((i * stepLenght ) * ((double)DUTY_Cycle_2 / 1000)));
				
			}else{
				currentArray[i]									= ((_steps*2 - i) * stepLenght );
				currentArray[(_steps*2)+i+2]		= ((_steps*2 - i) * stepLenght );
//				currentArray[i]									= (floor(((_steps*2 - i) * stepLenght ) * ((double)DUTY_Cycle_2 / 1000)));
//				currentArray[(_steps*2)+i+2]		= (floor(((_steps*2 - i) * stepLenght ) * ((double)DUTY_Cycle_2 / 1000)));
			}
		}
		
		for(int i = d2; i<(_steps*2); i+=4)// сброс в ноль от Д2 до конца полупериода
		{
			currentArray[i] 						 = 0; 
			currentArray[(_steps*2)+i+2] = 0;
		}
		
		for(int i = 0; i<d1; i+=4)//сброс в ноль от нуля до Д1 полуперода
		{
			currentArray[i] 						 = 0; 
			currentArray[(_steps*2)+i+2] = 0;
		}
				
		for(int i = 0; i<(((_steps*2) - DEAD_time_percent*4)); i+=4)// заполнение единицами 2 и 4 каналов
		{	
			currentArray[i+1] 					 = (_precision+1); 
			currentArray[(_steps*2)+i+3] = (_precision+1);
		}

		for(int i = ((_steps*2) - DEAD_time_percent*4); i<(_steps*4); i+=4)//сброс в ноль всех каналов с правого края, для обеспечения мертвого времени
		{	
			currentArray[i] 						 = 0;
			currentArray[i+1] 					 = 0; 
			currentArray[(_steps*2)+i+2] = 0;
			currentArray[(_steps*2)+i+3] = 0;
		}
	}
} 
void FillArgument_SIN_SHIFT(uint16_t DUTY_Cycle_1, uint16_t DUTY_Cycle_2, uint16_t SIN_SHIFT, uint32_t DEAD_time_percent, uint8_t SIN_DIRECTION, uint8_t Tims_On)//__~~__~~__
{
	//uint16_t zz = (floor((sin_element) * ((double)DUTY_Cycle_2 / 1000)));
	uint16_t z = (_steps*2*DUTY_Cycle_1 / 1000) - (_steps*2*DUTY_Cycle_1 / 1000)%4;//количество шагов от полупериода которые нужно сбросить в ноль 
	uint16_t d2 = ((_steps*2)-(((_steps*2)- z)/2)) - ((_steps*2)-(((_steps*2)- z)/2))%4;// точка отсчета скважности от Д2 до половины периода
	uint16_t d1 = (((_steps*2)- z)/2);// точка отсчета скважности от начала периода до Д1
	uint16_t array[2000];
	uint16_t i = 0, j = 0;
	double sin_element = 0;
	double _center_mult = 0;
	double vector = 0;
	int e = 0;
	_center_mult = ( (double)(SIN_SHIFT+100) / 100 );

	// 1/2 up
	while ( ( sin_element >= vector ) && ( i < _steps/2 ) )
	{
		vector = sin_element;
		sin_element = (double)sin( i * PI * _center_mult / (_steps/2) ) * _precision;		
		//array[i] = sin_element;
		array[i] = (floor((sin_element) * ((double)DUTY_Cycle_2 / 1000)));
		i++;		
	}
	// 1/2 down
	if ( i <= _steps/2)
	{
		if ( i > 0 ) i--;
		j = i;
		vector = PI / ( 2 * ( _steps/2 - j ) );
		while ( i <= _steps/2)
		{
			sin_element = (double)sin( PI / 2 + (i - j + 1) * vector ) * _precision;
			//array[i] = sin_element;
			array[i] = (floor((sin_element) * ((double)DUTY_Cycle_2 / 1000)));
			i++;
		}
	}
	if (Tims_On == 2)
	{
//		if(SIN_DIRECTION==0)
//		{
//				for(int k = 0; k<=_steps*2; k+=4)
//				{	
//					burst_wave_2[k] 								= array[e];
//					burst_wave_2[(_steps*2)+k+2] 		= array[e];
//					e++;
//				}
//		}else{
//				int q = _steps/2;
//				for(int k = 0; k<=_steps*2; k+=4)
//				{ 
//					burst_wave_2[k] 								= array[q];
//					burst_wave_2[(_steps*2)+k+2]		= array[q];
//					q--;
//				}
//		}
//		
//		for(int i = d2; i<(_steps*2); i+=4)// сброс в ноль от Д2 до конца полупериода
//		{
//			burst_wave_2[i] 						 = 0; 
//			burst_wave_2[(_steps*2)+i+2] = 0;
//		}
//		
//		for(int i = 0; i<d1; i+=4)//сброс в ноль от нуля до Д1 полуперода
//		{
//			burst_wave_2[i] 						 = 0; 
//			burst_wave_2[(_steps*2)+i+2] = 0;
//		}
//		
//		for(int i = 0; i<(floor((_steps*2) - DEAD_time_percent*4)); i+=4)// заполнение единицами 2 и 4 каналов
//		{	
//			burst_wave_2[i+1]							= (_precision+1); 
//			burst_wave_2[(_steps*2)+i+3] 	= (_precision+1);
//		}
//	
//		for(int i = ((_steps*2) - DEAD_time_percent*4); i<(_steps*4); i+=4)//сброс в ноль всех каналов с правого края, для обеспечения мертвого времени
//		{	
//			burst_wave_2[i] 							= 0;
//			burst_wave_2[i+1] 						= 0; 
//			burst_wave_2[(_steps*2)+i+2] 	= 0;
//			burst_wave_2[(_steps*2)+i+3] 	= 0;
//		}
	}
	else
	{
	if(SIN_DIRECTION==0)
	{
			for(int k = 0; k<=_steps*2; k+=4)
			{	
				currentArray[k] 								= array[e];
				currentArray[(_steps*2)+k+2] 		= array[e];
				e++;
			}
	}else{
				int q = _steps/2;
				for(int k = 0; k<=_steps*2; k+=4)
				{ 
					currentArray[k] 								= array[q];
					currentArray[(_steps*2)+k+2]		= array[q];
					q--;
				}
		}
		
		for(int i = d2; i<(_steps*2); i+=4)// сброс в ноль от Д2 до конца полупериода
		{
			currentArray[i] 						 = 0; 
			currentArray[(_steps*2)+i+2] = 0;
		}
		
		for(int i = 0; i<d1; i+=4)//сброс в ноль от нуля до Д1 полуперода
		{
			currentArray[i] 						 = 0; 
			currentArray[(_steps*2)+i+2] = 0;
		}
		
		for(int i = 0; i<(floor((_steps*2) - DEAD_time_percent*4)); i+=4)// заполнение единицами 2 и 4 каналов
		{	
			currentArray[i+1]							= (_precision+1); 
			currentArray[(_steps*2)+i+3] 	= (_precision+1);
		}
	
		for(int i = ((_steps*2) - DEAD_time_percent*4); i<(_steps*4); i+=4)//сброс в ноль всех каналов с правого края, для обеспечения мертвого времени
		{	
			currentArray[i] 							= 0;
			currentArray[i+1] 						= 0; 
			currentArray[(_steps*2)+i+2] 	= 0;
			currentArray[(_steps*2)+i+3] 	= 0;
		}
	}
} 

void FillArgument_SIN(uint16_t DUTY_Cycle_1, uint32_t DEAD_time_percent, uint8_t Tims_On)//__~~__~~__
{
	uint16_t z = (_steps*2*DUTY_Cycle_1 / 1000) - (_steps*2*DUTY_Cycle_1 / 1000)%4;//количество шагов от полупериода которые нужно сбросить в ноль 
	uint16_t d2 = ((_steps*2)-(((_steps*2)- z)/2)) - ((_steps*2)-(((_steps*2)- z)/2))%4;// точка отсчета скважности от Д2 до половины периода
	uint16_t d1 = (((_steps*2)- z)/2);// точка отсчета скважности от начала периода до Д1
	if (Tims_On == 2)
	{
//		for(int i = 0; i<(_steps*2); i+=4)//заполнение первой половины периода 1 и 3 канала в единицу
//		{
//			burst_wave_2[i] 						 = (uint16_t)(fabs(sin((i + 2) * PI / (_steps*2))) * _precision); 
//			burst_wave_2[(_steps*2)+i+2] = (uint16_t)(fabs(sin((i + 2) * PI / (_steps*2))) * _precision);
//		}
//		
//		for(int i = d2; i<(_steps*2); i+=4)// сброс в ноль от Д2 до конца полупериода
//		{
//			burst_wave_2[i] 						 = 0; 
//			burst_wave_2[(_steps*2)+i+2] = 0;
//		}
//		
//		for(int i = 0; i<d1; i+=4)//сброс в ноль от нуля до Д1 полуперода
//		{
//			burst_wave_2[i] 						 = 0; 
//			burst_wave_2[(_steps*2)+i+2] = 0;
//		}
//		
//		for(int i = 0; i<(floor((_steps*2) - DEAD_time_percent*4)); i+=4)// заполнение единицами 2 и 4 каналов
//		{	
//			burst_wave_2[i+1]							= (_precision+1); 
//			burst_wave_2[(_steps*2)+i+3] 	= (_precision+1);
//		}

//		for(int i = ((_steps*2) - DEAD_time_percent*4); i<(_steps*4); i+=4)//сброс в ноль всех каналов с правого края, для обеспечения мертвого времени
//		{	
//			burst_wave_2[i] 							= 0;
//			burst_wave_2[i+1] 						= 0; 
//			burst_wave_2[(_steps*2)+i+2] 	= 0;
//			burst_wave_2[(_steps*2)+i+3] 	= 0;
//		}
	}
	else
	{
		for(int i = 0; i<(_steps*2); i+=4)//заполнение первой половины периода 1 и 3 канала в единицу
		{
			currentArray[i] 						 = (uint16_t)(fabs(sin((i + 2) * PI / (_steps*2))) * _precision); 
			currentArray[(_steps*2)+i+2] = (uint16_t)(fabs(sin((i + 2) * PI / (_steps*2))) * _precision);
		}
		
		for(int i = d2; i<(_steps*2); i+=4)// сброс в ноль от Д2 до конца полупериода
		{
			currentArray[i] 						 = 0; 
			currentArray[(_steps*2)+i+2] = 0;
		}
		
		for(int i = 0; i<d1; i+=4)//сброс в ноль от нуля до Д1 полуперода
		{
			currentArray[i] 						 = 0; 
			currentArray[(_steps*2)+i+2] = 0;
		}
		
		for(int i = 0; i<(floor((_steps*2) - DEAD_time_percent*4)); i+=4)// заполнение единицами 2 и 4 каналов
		{	
			currentArray[i+1]							= (_precision+1); 
			currentArray[(_steps*2)+i+3] 	= (_precision+1);
		}

		for(int i = ((_steps*2) - DEAD_time_percent*4); i<(_steps*4); i+=4)//сброс в ноль всех каналов с правого края, для обеспечения мертвого времени
		{	
			currentArray[i] 							= 0;
			currentArray[i+1] 						= 0; 
			currentArray[(_steps*2)+i+2] 	= 0;
			currentArray[(_steps*2)+i+3] 	= 0;
		}
	}
} 
void FillArgument_PWM_ODNOTAKT_PHASE_LL(uint16_t DUTY_Cycle_1, uint16_t DUTY_Cycle_2, uint16_t PHASE_SHIFT, uint8_t Tims_On)
{
	uint16_t zz = (floor((_precision+1) * ((double)DUTY_Cycle_2 / 1000)));//значение скважности для внутреннних импульсов
	//uint16_t z = (_steps*4*DUTY_Cycle_1 / 1000) - (_steps*4*DUTY_Cycle_1 / 1000)%4;
	uint16_t z = (floor(_steps*4*((double)DUTY_Cycle_1 / 1000))) ;
	z = z - z%4;
	uint16_t shift = (_steps*4 * PHASE_SHIFT / 360) - (_steps*4 * PHASE_SHIFT / 360)%4;// номер шага с которого начинается смещение/сдвиг
	uint16_t f = ((_steps*4 - _steps*4 * PHASE_SHIFT / 360)) - ((_steps*4 - _steps*4 * PHASE_SHIFT / 360))%4;//переменная для осуществления сдвига каналов 3 4 относительно 1 2
	uint16_t q = 0;
	if (Tims_On == 2)
	{
//		for(int i = 0; i<z; i+=4)//заполнение 1 и 2 каналов
//		{	
//			burst_wave_2[i]							= (zz);
//			burst_wave_2[i+1] 					= 0;
//		}

//		for(int i = z; i<=(_steps*4); i+=4)//заполнение 1 и 2 каналов
//		{
//			burst_wave_2[i] 						= 0;
//			burst_wave_2[i+1] 					= (zz);
//		} 

//		for(int i=shift; i < (_steps*4); i+=4)//заполнение 3 и 4 каналов из 1 и 2
//		{	
//			burst_wave_2[i+2] 					= currentArray[q]; 
//			burst_wave_2[i+3]						= currentArray[q+1];
//			q+=4;
//		}
//		
//		for(int i=0 ; i<shift; i+=4)//заполнение 3 и 4 каналов из 1 и 2
//		{	
//			burst_wave_2[i+2]						= currentArray[f];
//			burst_wave_2[i+3] 					= currentArray[f+1]; 
//			f+=4; 
//		}
	}
	else
	{
		for(int i = 0; i<z; i+=4)//заполнение 1 и 2 каналов
		{	
			currentArray[i]							= (zz);
			currentArray[i+1] 					= 0;
		}

		for(int i = z; i<=(_steps*4); i+=4)//заполнение 1 и 2 каналов
		{
			currentArray[i] 						= 0;
			currentArray[i+1] 					= (zz);
		} 

		for(int i=shift; i < (_steps*4); i+=4)//заполнение 3 и 4 каналов из 1 и 2
		{	
			currentArray[i+2] 					= currentArray[q]; 
			currentArray[i+3]						= currentArray[q+1];
			q+=4;
		}
		
		for(int i=0 ; i<shift; i+=4)//заполнение 3 и 4 каналов из 1 и 2
		{	
			currentArray[i+2]						= currentArray[f];
			currentArray[i+3] 					= currentArray[f+1]; 
			f+=4; 
		}
	}
}
void FillArgument_PWM_ODNOTAKT_PHASE_HL(uint16_t DUTY_Cycle_1, uint16_t PHASE_SHIFT, uint8_t Tims_On)
{
	z = (floor(_steps*4*((double)DUTY_Cycle_1 / 1000))) ;
	z = z - z%4;
	uint16_t shift = (_steps*4 * PHASE_SHIFT / 360) - (_steps*4 * PHASE_SHIFT / 360)%4;// номер шага с которого начинается смещение/сдвиг
	uint16_t f = ((_steps*4 - _steps*4 * PHASE_SHIFT / 360)) - ((_steps*4 - _steps*4 * PHASE_SHIFT / 360))%4;//переменная для осуществления сдвига каналов 3 4 относительно 1 2
	uint16_t q = 0;
	
//	uint16_t z = (floor(_steps*2*((double)DUTY_Cycle_1 / 1000))); 
//	uint16_t d2 = ((_steps*2)-(((_steps*2)- z)/2)) - ((_steps*2)-(((_steps*2)- z)/2))%4;
//	uint16_t d1 = (((_steps*2)- z)/2);
	
	if (Tims_On == 2)
	{
//		for(int i = 0; i<z; i+=4)//заполнение 1 и 2 каналов
//		{	
//			burst_wave_2[i]							= (_precision+1);
//			burst_wave_2[i+1] 					= 0;
//		}

//		for(int i = z; i<=(_steps*4); i+=4)//заполнение 1 и 2 каналов
//		{
//			burst_wave_2[i] 						= 0;
//			burst_wave_2[i+1] 					= (_precision+1);
//		} 

//		for(int i=shift; i < (_steps*4); i+=4)//заполнение 3 и 4 каналов из 1 и 2
//		{	
//			burst_wave_2[i+2] 					= currentArray[q]; 
//			burst_wave_2[i+3]						= currentArray[q+1];
//			q+=4;
//		}
//		
//		for(int i=0 ; i<shift; i+=4)//заполнение 3 и 4 каналов из 1 и 2
//		{	
//			burst_wave_2[i+2]						= currentArray[f];
//			burst_wave_2[i+3] 					= currentArray[f+1]; 
//			f+=4; 
//		}
	}
	else
	{
		for(int i = 0; i<z; i+=4)//заполнение 1 и 2 каналов
		{	
			currentArray[i]							= (_precision+1);
			currentArray[i+1] 					= 0;
		}

		for(int i = z; i<=(_steps*4); i+=4)//заполнение 1 и 2 каналов
		{
			currentArray[i] 						= 0;
			currentArray[i+1] 					= (_precision+1);
		} 

		for(int i=shift; i < (_steps*4); i+=4)//заполнение 3 и 4 каналов из 1 и 2
		{	
			currentArray[i+2] 					= currentArray[q]; 
			currentArray[i+3]						= currentArray[q+1];
			q+=4;
		}
		
		for(int i=0 ; i<shift; i+=4)//заполнение 3 и 4 каналов из 1 и 2
		{	
			currentArray[i+2]						= currentArray[f];
			currentArray[i+3] 					= currentArray[f+1]; 
			f+=4; 
		}
	}
}
void FillArgument_PWM_LL(uint16_t DUTY_Cycle_1, uint16_t DUTY_Cycle_2, uint32_t DEAD_time_percent, uint8_t Tims_On)
{
	uint16_t z  = (floor(_steps*2*((double)DUTY_Cycle_1 / 1000)));
	uint16_t d2 = ((_steps*2)-(((_steps*2)- z)/2)) - ((_steps*2)-(((_steps*2)- z)/2))%4;
	uint16_t d1 = (((_steps*2)- z)/2);
	uint16_t zz = (floor((_precision+1) * ((double)DUTY_Cycle_2 / 1000)));
		
	if (Tims_On == 2)
	{
//		for(int i = 0; i<(_steps*2); i+=4)
//		{	
//			burst_wave_2[i] 							= (uint16_t)(zz); 
//			burst_wave_2[(_steps*2)+i+2] 	= (uint16_t)(zz);
//		}

//		for(int i = d2; i<(_steps*2); i+=4)
//		{
//			burst_wave_2[i] 							= 0; 
//			burst_wave_2[(_steps*2)+i+2]	= 0;
//		}

//		for(int i = 0; i<d1; i+=4)//сброс в ноль от нуля до Д1 полуперода
//		{
//			burst_wave_2[i] 							= 0; 
//			burst_wave_2[(_steps*2)+i+2] 	= 0;
//		}

//		for(int i = 0; i<(floor((_steps*2) - DEAD_time_percent*4)); i+=4)// заполнение единицами 2 и 4 каналов
//		{	
//			burst_wave_2[i+1]							= (_precision+1); 
//			burst_wave_2[(_steps*2)+i+3] 	= (_precision+1);
//		}

//		for(int i = ((_steps*2) - DEAD_time_percent*4); i<(_steps*4); i+=4)//сброс в ноль всех каналов с правого края, для обеспечения мертвого времени
//		{	
//			burst_wave_2[i] 							= 0;
//			burst_wave_2[i+1] 						= 0; 
//			burst_wave_2[(_steps*2)+i+2] 	= 0;
//			burst_wave_2[(_steps*2)+i+3] 	= 0;
//		}
	}
	else
	{
		for(int i = 0; i<(_steps*2); i+=4)//заполнение первой половины периода 1 и 3 канала в значение ZZ
		{	
			currentArray[i] 							= (uint16_t)(zz); 
			currentArray[(_steps*2)+i+2] 	= (uint16_t)(zz);
		}

		for(int i = d2; i<(_steps*2); i+=4)// сброс в ноль от Д2 до конца полупериода
		{
			currentArray[i] 							= 0; 
			currentArray[(_steps*2)+i+2]	= 0;
		}
		
		for(int i = 0; i<d1; i+=4)//сброс в ноль от нуля до Д1 полуперода
		{
			currentArray[i] 							= 0; 
			currentArray[(_steps*2)+i+2] 	= 0;
		}
		
		for(int i = DEAD_time_percent*4; i<(((_steps*2)-3)); i+=4)
		{	
			currentArray[i+1] 					 =(uint16_t) (_precision+1); 
			currentArray[(_steps*2)+i+3] =(uint16_t) (_precision+1);
		}

			for(int i = (0); i<(DEAD_time_percent*4); i+=4)
		{	
			currentArray[i] 						 =(uint16_t) 0;
			currentArray[i+1] 					 =(uint16_t) 0; 
			currentArray[(_steps*2)+i+2] =(uint16_t) 0;
			currentArray[(_steps*2)+i+3] =(uint16_t) 0;
		}
	}
} 
void FillArgument_PWM_HL(uint16_t DUTY_Cycle_1, uint32_t DEAD_time_percent, uint8_t Tims_On)// _=__=__=
{
	uint16_t z = (floor(_steps*2*((double)DUTY_Cycle_1 / 1000))); 
	uint16_t d2 = ((_steps*2)-(((_steps*2)- z)/2)) - ((_steps*2)-(((_steps*2)- z)/2))%4;
	uint16_t d1 = (((_steps*2)- z)/2);
	
	if (Tims_On == 2)
	{
//		for(int i = 0; i<(_steps*2); i+=4)//заполнение первой половины периода 1 и 3 канала в единицу
//		{
//			burst_wave_2[i] 						 =(uint16_t) (_precision+1); 
//			burst_wave_2[(_steps*2)+i+2] =(uint16_t) (_precision+1);
//		}
//		
//		for(int i = d2; i<(_steps*2); i+=4)// сброс в ноль от Д2 до конца полупериода
//		{
//			burst_wave_2[i] 						 =(uint16_t) 0; 
//			burst_wave_2[(_steps*2)+i+2] =(uint16_t) 0;
//		}
//		
//		for(int i = 0; i<d1; i+=4)//сброс в ноль от нуля до Д1 полуперода
//		{
//			burst_wave_2[i] 						 =(uint16_t) 0; 
//			burst_wave_2[(_steps*2)+i+2] =(uint16_t) 0;
//		}
//				
////		for(int i = 0; i<(((_steps*2) - DEAD_time_percent*4)); i+=4)// заполнение единицами 2 и 4 каналов
////		{	
////			burst_wave_2[i+1] 					 =(uint16_t) (_precision+1); 
////			burst_wave_2[(_steps*2)+i+3] =(uint16_t) (_precision+1);
////		}
////	
//		for(int i = DEAD_time_percent*4; i<(((_steps*2) )); i+=4)
//		{	
//			burst_wave_2[i+1] 					 =(uint16_t) (_precision+1); 
//			burst_wave_2[(_steps*2)+i+3] =(uint16_t) (_precision+1);
//		}
//	
////		for(int i = ((_steps*2) - DEAD_time_percent*4); i<(_steps*4); i+=4)//сброс в ноль всех каналов с правого края, для обеспечения мертвого времени
////		{	
////			burst_wave_2[i] 						 =(uint16_t) 0;
////			burst_wave_2[i+1] 					 =(uint16_t) 0; 
////			burst_wave_2[(_steps*2)+i+2] =(uint16_t) 0;
////			burst_wave_2[(_steps*2)+i+3] =(uint16_t) 0;
////		}
//		for(int i = (0); i<(DEAD_time_percent*4); i+=4)
//		{	
//			burst_wave_2[i] 						 =(uint16_t) 0;
//			burst_wave_2[i+1] 					 =(uint16_t) 0; 
//			burst_wave_2[(_steps*2)+i+2] =(uint16_t) 0;
//			burst_wave_2[(_steps*2)+i+3] =(uint16_t) 0;
//		}
	}
	else
	{
		for(int i = 0; i<(_steps*2); i+=4)//заполнение первой половины периода 1 и 3 канала в единицу
		{
			currentArray[i] 						 =(uint16_t) (_precision+1); 
			currentArray[(_steps*2)+i+2] =(uint16_t) (_precision+1);
		}
		
		for(int i = d2; i<(_steps*2); i+=4)
		{
			currentArray[i] 						 =(uint16_t) 0; 
			currentArray[(_steps*2)+i+2] =(uint16_t) 0;
		}
//		for(int i = d2; i<(_steps*4)+40; i+=4)
//		{
//			currentArray[i] 						 =(uint16_t) 0; 
//			//currentArray[(_steps*2)+i+2] =(uint16_t) 0;
//		}
		for(int i = 0; i<d1; i+=4)//сброс в ноль от нуля до Д1 полуперода
		{
			currentArray[i] 						 =(uint16_t) 0; 
			currentArray[(_steps*2)+i+2] =(uint16_t) 0;
		}
				
//		for(int i = 0; i<(((_steps*2) - DEAD_time_percent*4)); i+=4)// заполнение единицами 2 и 4 каналов
//		{	
//			currentArray[i+1] 					 =(uint16_t) (_precision+1); 
//			currentArray[(_steps*2)+i+3] =(uint16_t) (_precision+1);
//		}
		for(int i = DEAD_time_percent*4; i<(((_steps*2)-3)); i+=4)
		{	
			currentArray[i+1] 					 =(uint16_t) (_precision+1); 
			currentArray[(_steps*2)+i+3] =(uint16_t) (_precision+1);
		}
//		for(int i = ((_steps*2) - DEAD_time_percent*4); i<(_steps*4); i+=4)//сброс в ноль всех каналов с правого края, для обеспечения мертвого времени
//		{	
//			currentArray[i] 						 =(uint16_t) 0;
//			currentArray[i+1] 					 =(uint16_t) 0; 
//			currentArray[(_steps*2)+i+2] =(uint16_t) 0;
//			currentArray[(_steps*2)+i+3] =(uint16_t) 0;
//		}
			for(int i = (0); i<(DEAD_time_percent*4); i+=4)
		{	
			currentArray[i] 						 =(uint16_t) 0;
			currentArray[i+1] 					 =(uint16_t) 0; 
			currentArray[(_steps*2)+i+2] =(uint16_t) 0;
			currentArray[(_steps*2)+i+3] =(uint16_t) 0;
		}
	}
} 

uint32_t  FillArgument_PWM_HL_Test(void* Array, uint16_t DUTY_Cycle_1, uint32_t DEAD_time_percent, uint32_t start_step, uint32_t all_step, uint32_t one_array_steps)
{
	uint32_t z  = (floor((double)(all_step/2)*((double)DUTY_Cycle_1 / 1000))); 
	uint32_t d2 = ((all_step/2)-(((all_step/2)- z)/2)) - ((all_step/2)-(((all_step/2)- z)/2))%4;
	uint32_t d1 = (((all_step/2)- z)/2);
	uint32_t stop_step   = (start_step+one_array_steps);
	uint32_t shift_steps = 0;
	uint32_t Last_Step   = 0;
	
	for(uint32_t i = start_step; i<stop_step; i+=4)
	{
		/*********************************************************************************/
		if( (all_step-start_step)<one_array_steps	&&	(i>=(all_step))	)
		{
			shift_steps = (all_step-start_step);
			stop_step   = one_array_steps - (all_step-start_step);
			start_step  = 0;
			i=0;
		}
		/*********************************************************************************/
		if(i<d1)
		{
			((uint16_t*)Array)[i  -start_step+shift_steps] 						 =(uint16_t) 0; 
			((uint16_t*)Array)[i+1-start_step+shift_steps] 						 =(uint16_t) (_precision+1);
			((uint16_t*)Array)[i+2-start_step+shift_steps] 						 =(uint16_t) 0;
			((uint16_t*)Array)[i+3-start_step+shift_steps]						 =(uint16_t) 0;
		}
		if(i>=d1 && i<=d2) 
		{
			((uint16_t*)Array)[i  -start_step+shift_steps] 						 =(uint16_t) (_precision+1); 
			((uint16_t*)Array)[i+1-start_step+shift_steps] 						 =(uint16_t) (_precision+1);
			((uint16_t*)Array)[i+2-start_step+shift_steps] 						 =(uint16_t) 0;
			((uint16_t*)Array)[i+3-start_step+shift_steps]						 =(uint16_t) 0;
		}
		if(i>d2 && i<=(all_step/2))
		{
			((uint16_t*)Array)[i  -start_step+shift_steps] 						 =(uint16_t) 0; 
			((uint16_t*)Array)[i+1-start_step+shift_steps] 						 =(uint16_t) (_precision+1);
			((uint16_t*)Array)[i+2-start_step+shift_steps] 						 =(uint16_t) 0;
			((uint16_t*)Array)[i+3-start_step+shift_steps]						 =(uint16_t) 0;
		}
		if(i<(DEAD_time_percent*4))
		{
			((uint16_t*)Array)[i  -start_step+shift_steps] 						 =(uint16_t) 0;
			((uint16_t*)Array)[i+1-start_step+shift_steps] 						 =(uint16_t) 0; 		
		}
/*---------------------------------------------------------------*/
		if(i>(all_step/2) && i<d1+(all_step/2))
		{
			((uint16_t*)Array)[i  -start_step+shift_steps] 						 =(uint16_t) 0; 
			((uint16_t*)Array)[i+1-start_step+shift_steps] 						 =(uint16_t) 0;
			((uint16_t*)Array)[i+2-start_step+shift_steps] 						 =(uint16_t) 0;
			((uint16_t*)Array)[i+3-start_step+shift_steps]						 =(uint16_t) (_precision+1);
		}
		if(i>=(d1+(all_step/2)) && i<=(d2+(all_step/2)) )
		{		
			((uint16_t*)Array)[i  -start_step+shift_steps] 						 =(uint16_t) 0; 
			((uint16_t*)Array)[i+1-start_step+shift_steps] 						 =(uint16_t) 0;
			((uint16_t*)Array)[i+2-start_step+shift_steps] 						 =(uint16_t) (_precision+1);
			((uint16_t*)Array)[i+3-start_step+shift_steps]						 =(uint16_t) (_precision+1);
		}
		if(i>(d2+(all_step/2)) && i<(all_step))
		{
			((uint16_t*)Array)[i  -start_step+shift_steps] 						 =(uint16_t) 0; 
			((uint16_t*)Array)[i+1-start_step+shift_steps] 						 =(uint16_t) 0;
			((uint16_t*)Array)[i+2-start_step+shift_steps] 						 =(uint16_t) 0;
			((uint16_t*)Array)[i+3-start_step+shift_steps]						 =(uint16_t) (_precision+1);
		}
		if(i<(DEAD_time_percent*4)+(all_step/2) )
		{
			((uint16_t*)Array)[i+2-start_step+shift_steps] 						 =(uint16_t) 0;
			((uint16_t*)Array)[i+3-start_step+shift_steps] 						 =(uint16_t) 0; 		
		}
	}
	Last_Step = stop_step;
	return Last_Step;
}
void FillArray_Test(void* Array, uint8_t Type_of_Array, uint16_t Array_Size, uint16_t Precision)
{
//		uint8_t byte_in_cell=2;
//		switch (Type_of_Array)
//		{
//			case 16:
//				byte_in_cell=2;
//			break;
//			case 32:
//				byte_in_cell=4;
//			break;
//		}
//		
//		for(int i = 0; i<(Array_Size/2); i+=1)
//		{
//			*(&Array + i*byte_in_cell) = (uint32_t) (Precision);
//		}
		
		for(int i = 0; i<(Array_Size/2); i+=1)
		{
			switch (Type_of_Array)
			{
				case 16:
					((uint16_t*)Array)[i] = (uint16_t) (Precision);
				break;
				case 32:
					((uint32_t*)Array)[i] = (uint16_t) (Precision);
				break;
			}
		}
		
		for(int i = (Array_Size/2); i<(Array_Size); i+=1)
		{
			switch (Type_of_Array)
			{
				case 16:
					((uint16_t*)Array)[i] = (uint16_t) (0);
				break;
				case 32:
					((uint32_t*)Array)[i] = (uint16_t) (0);
				break;
			}
		}
} 
//*******************************************************************************************//
//*************************************РРќРР¦РРђР›РР—РђР¦РРЇ РџР”Рџ*************************************//
void Init_DMA_TIM( uint16_t steps, TIM_TypeDef* timer)
{
		DMA_Stream_TypeDef* DMA_Timers;

		if(timer==TIM1)
		{
			RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
			DMA_Timers=DMA2_Stream5;
			DMA_Timers->CR = (DMA_Timers->CR & ~DMA_SxCR_CHSEL) |  DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_2;

				DMA_Timers->M0AR = (uint32_t)(&burst_wave_1);
			
//				DMA2_Stream5->CR &= ~DMA_SxCR_CT;
//				if(_steps >= GEN_CORE_ARRAY_MAX_COUNT)
//				{
//					DMA_Timers->M1AR = (uint32_t)(&burst_wave_3);
//					DMA_Timers->CR |=   DMA_SxCR_DBM;
//				}else
//				{
//					DMA_Timers->CR &=  ~DMA_SxCR_DBM;
//				}

			
			
		
			DMA_Timers->CR |=   DMA_SxCR_MSIZE_0; 
			DMA_Timers->CR &=  ~DMA_SxCR_MSIZE_1; 
			DMA_Timers->CR |=   DMA_SxCR_PSIZE_0; 
			DMA_Timers->CR &=  ~DMA_SxCR_PSIZE_1; 
		}
		else if(timer==TIM2)
		{
//			RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
//			DMA_Timers=DMA1_Stream7;
//			DMA_Timers->CR = (DMA_Timers->CR & ~DMA_SxCR_CHSEL) |  DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_0;
//			DMA_Timers->M0AR = (uint32_t)(&burst_wave_2);
//			DMA_Timers->CR &=  ~DMA_SxCR_MSIZE_0; 
//			DMA_Timers->CR |=   DMA_SxCR_MSIZE_1; 
//			DMA_Timers->CR &=  ~DMA_SxCR_PSIZE_0; 
//			DMA_Timers->CR |=   DMA_SxCR_PSIZE_1; 
		}
		else if(timer==TIM3)
		{
			RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
			DMA_Timers=DMA1_Stream2;
			DMA_Timers->CR = (DMA_Timers->CR & ~DMA_SxCR_CHSEL) |  DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_2;
			DMA_Timers->M0AR = (uint32_t)(&burst_wave_3);
			
			DMA_Timers->M1AR = (uint32_t)(&burst_wave_3);			
			DMA_Timers->CR |=   DMA_SxCR_MSIZE_0; 
			DMA_Timers->CR &=  ~DMA_SxCR_MSIZE_1; 
			DMA_Timers->CR |=   DMA_SxCR_PSIZE_0; 
			DMA_Timers->CR &=  ~DMA_SxCR_PSIZE_1; 
		}
	
		DMA_Timers->CR |=   DMA_SxCR_CIRC;    
		DMA_Timers->CR |=   DMA_SxCR_MINC;   
		DMA_Timers->CR |=   DMA_SxCR_DIR_0;   
		DMA_Timers->CR &=  ~DMA_SxCR_DIR_1; 
		
		DMA_Timers->CR |=		DMA_SxCR_PL_1 | DMA_SxCR_PL_0;//PriorityLevel
		
		DMA_Timers->CR |=   DMA_SxCR_TCIE;
		NVIC_EnableIRQ (DMA2_Stream5_IRQn);

		DMA_Timers->NDTR =  (steps)*4;
		DMA_Timers->PAR  =	(uint32_t)(&(timer->DMAR));
}
void Init_DMA_TIM_THREE_PHASE( uint16_t steps)
{
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	
		DMA2_Stream5->CR = (DMA2_Stream5->CR & ~DMA_SxCR_CHSEL) |  DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_2;
		DMA2_Stream5->PAR  = (uint32_t)(&(TIM1->DMAR));
		DMA2_Stream5->M0AR = (uint32_t)(&burst_wave_1[0]);

		DMA2_Stream5->NDTR= steps*3;

		DMA2_Stream5->CR |=   DMA_SxCR_CIRC;    
		DMA2_Stream5->CR |=   DMA_SxCR_MINC;   
		DMA2_Stream5->CR |=   DMA_SxCR_DIR_0;   
		DMA2_Stream5->CR &=  ~DMA_SxCR_DIR_1; 

		DMA2_Stream5->CR |=   DMA_SxCR_MSIZE_0;
		DMA2_Stream5->CR &=  ~DMA_SxCR_MSIZE_1;
		DMA2_Stream5->CR |=   DMA_SxCR_PSIZE_0;
		DMA2_Stream5->CR &=  ~DMA_SxCR_PSIZE_1;
	
}
//*******************************************************************************************//
//***********************************РРќРР¦РРђР›РР—РђР¦РРЇ РўРђР™РњР•Р РћР’**********************************//
void Init_TIM(uint16_t precision, TIM_TypeDef* timer)
{	
		if(timer==TIM1){RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;}else if(timer==TIM2){RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;}else if(timer==TIM3){RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;}

		timer->CCER   |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
		timer->CCMR1  |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;
		timer->CCMR1  |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE;
		timer->CCMR2  |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3PE;
		timer->CCMR2  |= TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4PE;
		timer->DIER   |= TIM_DIER_UDE;
		timer->CR1    |= TIM_CR1_ARPE;  
		if(timer==TIM1){TIM1->BDTR   |= TIM_BDTR_MOE;}
		if(timer==TIM1){timer->PSC = TIM1_Prescaller_-1;}else{timer->PSC = 0;}
		timer->ARR  = precision-1;
		timer->DCR = (3 << 8) + ((((uint32_t)(&timer->CCR1)) - ((uint32_t)(&timer->CR1))) >> 2);
		timer->EGR |= TIM_EGR_UG;
}

void Init_TIM_THREE_PHASE(uint16_t precision)
{
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
		TIM1->PSC  = 1;
		TIM1->ARR  = precision;

		TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE; // PWM Mode 1 channel 1
		TIM1->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2PE; // PWM Mode 1 channel 2
		TIM1->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3PE; // PWM Mode 1 channel 3

		TIM1->CCER  |=  TIM_CCER_CC1E;   // Capture/Compare 1 output enable
		TIM1->CCER	|=  TIM_CCER_CC1NE;  // Capture/Compare 1 complementary output enable
		TIM1->CCER  |=  TIM_CCER_CC2E;   // Capture/Compare 2 output enable
		TIM1->CCER  |=  TIM_CCER_CC2NE;  // Capture/Compare 2 complementary output enable
		TIM1->CCER  |=  TIM_CCER_CC3E;   // Capture/Compare 3 output enable
		TIM1->CCER  |=  TIM_CCER_CC3NE;  // Capture/Compare 3 complementary output enable

		TIM1->DCR = ((3 - 1) << 8) + ((((uint32_t)(&TIM1->CCR1)) - ((uint32_t)(&TIM1->CR1))) >> 2);

		TIM1->BDTR 	|= TIM_BDTR_MOE;
		TIM1->DIER 	|= TIM_DIER_UDE;
		TIM1->CR1 	|= TIM_CR1_ARPE;
}
//*******************************************************************************************//
//*************************************РРќРР¦РРђР›РР—РђР¦РРЇ GPIO************************************//
void Init_GPIO_TIM1() 
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
void Init_GPIO_TIM2() 
{		
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	GPIOA->MODER  &= ~(GPIO_MODER_MODER5_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0); 
  GPIOA->MODER  |=   GPIO_MODER_MODER5_1 | GPIO_MODER_MODER1_1 | GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1; 	
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT5 		 | GPIO_OTYPER_OT1 		| GPIO_OTYPER_OT2 	   | GPIO_OTYPER_OT3);   //0-push-pull   1-open drain
	GPIOA->OSPEEDR|= 	 GPIO_OSPEEDER_OSPEEDR5_0 | GPIO_OSPEEDER_OSPEEDR1_0 | GPIO_OSPEEDER_OSPEEDR2_0 | GPIO_OSPEEDER_OSPEEDR3_0; //00-low 01-medium 10-high 11-very high
	GPIOA->OSPEEDR|= 	 GPIO_OSPEEDER_OSPEEDR5_1 | GPIO_OSPEEDER_OSPEEDR1_1 | GPIO_OSPEEDER_OSPEEDR2_1 | GPIO_OSPEEDER_OSPEEDR3_1; //00-low 01-medium 10-high 11-very high
	GPIOA->PUPDR  &= ~(GPIO_PUPDR_PUPD5_0 | GPIO_PUPDR_PUPD1_0 | GPIO_PUPDR_PUPD2_0 | GPIO_PUPDR_PUPD3_0);
	GPIOA->PUPDR  &= ~(GPIO_PUPDR_PUPD5_1 | GPIO_PUPDR_PUPD1_1 | GPIO_PUPDR_PUPD2_1 | GPIO_PUPDR_PUPD3_1);
	GPIOA->AFR[0] |=   GPIO_AFRH_AFRH5_0 | GPIO_AFRH_AFRH1_0 | GPIO_AFRH_AFRH2_0 | GPIO_AFRH_AFRH3_0; 
	GPIOA->AFR[0] &= ~(GPIO_AFRH_AFRH5_1 | GPIO_AFRH_AFRH1_1 | GPIO_AFRH_AFRH2_1 | GPIO_AFRH_AFRH3_1); 
	GPIOA->AFR[0] &= ~(GPIO_AFRH_AFRH5_2 | GPIO_AFRH_AFRH1_2 | GPIO_AFRH_AFRH2_2 | GPIO_AFRH_AFRH3_2); 
 	GPIOA->AFR[0] &= ~(GPIO_AFRH_AFRH5_3 | GPIO_AFRH_AFRH1_3 | GPIO_AFRH_AFRH2_3 | GPIO_AFRH_AFRH3_3);       
}
void Init_GPIO_TIM3() 
{	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	GPIOA->MODER  &= ~(GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0); 
  GPIOA->MODER  |=   GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1; 	
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT6 		 | GPIO_OTYPER_OT7);   //0-push-pull   1-open drain
	GPIOA->OSPEEDR|=   GPIO_OSPEEDER_OSPEEDR6_0 | GPIO_OSPEEDER_OSPEEDR7_0; //00-low 01-medium 10-high 11-very high
	GPIOA->OSPEEDR|=   GPIO_OSPEEDER_OSPEEDR6_1 | GPIO_OSPEEDER_OSPEEDR7_1; //00-low 01-medium 10-high 11-very high
	GPIOA->PUPDR  &= ~(GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0);
	GPIOA->PUPDR  &= ~(GPIO_PUPDR_PUPD6_1 | GPIO_PUPDR_PUPD7_1);
	GPIOA->AFR[0] &= ~(GPIO_AFRH_AFRH6_0 | GPIO_AFRH_AFRH7_0); 
	GPIOA->AFR[0] |=   GPIO_AFRH_AFRH6_1 | GPIO_AFRH_AFRH7_1; 
	GPIOA->AFR[0] &= ~(GPIO_AFRH_AFRH6_2 | GPIO_AFRH_AFRH7_2); 
 	GPIOA->AFR[0] &= ~(GPIO_AFRH_AFRH6_3 | GPIO_AFRH_AFRH7_3);  
	
	GPIOB->MODER  &= ~(GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0); 
  GPIOB->MODER  |=   GPIO_MODER_MODER0_1 | GPIO_MODER_MODER1_1; 	
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT0 		 | GPIO_OTYPER_OT1);   //0-push-pull   1-open drain
	GPIOB->OSPEEDR|=   GPIO_OSPEEDER_OSPEEDR0_0 | GPIO_OSPEEDER_OSPEEDR1_0; //00-low 01-medium 10-high 11-very high
	GPIOB->OSPEEDR|=	 GPIO_OSPEEDER_OSPEEDR0_1 | GPIO_OSPEEDER_OSPEEDR1_1; //00-low 01-medium 10-high 11-very high
	GPIOB->PUPDR  &= ~(GPIO_PUPDR_PUPD0_0 | GPIO_PUPDR_PUPD1_0);
	GPIOB->PUPDR  &= ~(GPIO_PUPDR_PUPD0_1 | GPIO_PUPDR_PUPD1_1);
	GPIOB->AFR[0] &= ~(GPIO_AFRH_AFRH0_0 | GPIO_AFRH_AFRH1_0); 
	GPIOB->AFR[0] |=   GPIO_AFRH_AFRH0_1 | GPIO_AFRH_AFRH1_1; 
	GPIOB->AFR[0] &= ~(GPIO_AFRH_AFRH0_2 | GPIO_AFRH_AFRH1_2); 
 	GPIOB->AFR[0] &= ~(GPIO_AFRH_AFRH0_3 | GPIO_AFRH_AFRH1_3); 
}
void Init_GPIO_TIM1_THREE_PHASE()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

	GPIOE->MODER  &= ~(GPIO_MODER_MODER9_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER13_0); 
  GPIOE->MODER  |=   GPIO_MODER_MODER9_1 | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER13_1; 	
	GPIOE->OTYPER &= ~(GPIO_OTYPER_OT9 		 | GPIO_OTYPER_OT11 		| GPIO_OTYPER_OT13);   //0-push-pull   1-open drain
	GPIOE->OSPEEDR|= 	 GPIO_OSPEEDER_OSPEEDR9_0 | GPIO_OSPEEDER_OSPEEDR11_0 | GPIO_OSPEEDER_OSPEEDR13_0; //00-low 01-medium 10-high 11-very high
	GPIOE->OSPEEDR|= 	 GPIO_OSPEEDER_OSPEEDR9_1 | GPIO_OSPEEDER_OSPEEDR11_1 | GPIO_OSPEEDER_OSPEEDR13_1; //00-low 01-medium 10-high 11-very high
	GPIOE->PUPDR  &= ~(GPIO_PUPDR_PUPD9_0 | GPIO_PUPDR_PUPD11_0 | GPIO_PUPDR_PUPD13_0);
	GPIOE->PUPDR  &= ~(GPIO_PUPDR_PUPD9_1 | GPIO_PUPDR_PUPD11_1 | GPIO_PUPDR_PUPD13_1);
	GPIOE->AFR[1] |=   GPIO_AFRH_AFRH1_0 | GPIO_AFRH_AFRH3_0 | GPIO_AFRH_AFRH5_0; 
	GPIOE->AFR[1] &= ~(GPIO_AFRH_AFRH1_1 | GPIO_AFRH_AFRH3_1 | GPIO_AFRH_AFRH5_1); 
	GPIOE->AFR[1] &= ~(GPIO_AFRH_AFRH1_2 | GPIO_AFRH_AFRH3_2 | GPIO_AFRH_AFRH5_2); 
 	GPIOE->AFR[1] &= ~(GPIO_AFRH_AFRH1_3 | GPIO_AFRH_AFRH3_3 | GPIO_AFRH_AFRH5_3);       

	GPIOA->MODER  &= ~(GPIO_MODER_MODER7_0); 
  GPIOA->MODER  |=   GPIO_MODER_MODER7_1; 	
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT7);   //0-push-pull   1-open drain
	GPIOA->OSPEEDR|=   GPIO_OSPEEDER_OSPEEDR7_0; //00-low 01-medium 10-high 11-very high
	GPIOA->OSPEEDR|=   GPIO_OSPEEDER_OSPEEDR7_1; //00-low 01-medium 10-high 11-very high
	GPIOA->PUPDR  &= ~(GPIO_PUPDR_PUPD7_0);
	GPIOA->PUPDR  &= ~(GPIO_PUPDR_PUPD7_1);
	GPIOA->AFR[0] |=  (GPIO_AFRH_AFRH7_0); 
	GPIOA->AFR[0] &= ~(GPIO_AFRH_AFRH7_1); 
	GPIOA->AFR[0] &= ~(GPIO_AFRH_AFRH7_2); 
 	GPIOA->AFR[0] &= ~(GPIO_AFRH_AFRH7_3);  
	
	GPIOB->MODER  &= ~(GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0); 
  GPIOB->MODER  |=   GPIO_MODER_MODER0_1 | GPIO_MODER_MODER1_1; 	
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT0 		 | GPIO_OTYPER_OT1);   //0-push-pull   1-open drain
	GPIOB->OSPEEDR|= 	 GPIO_OSPEEDER_OSPEEDR0_0 | GPIO_OSPEEDER_OSPEEDR1_0; //00-low 01-medium 10-high 11-very high
	GPIOB->OSPEEDR|= 	 GPIO_OSPEEDER_OSPEEDR0_1 | GPIO_OSPEEDER_OSPEEDR1_1; //00-low 01-medium 10-high 11-very high
	GPIOB->PUPDR  &= ~(GPIO_PUPDR_PUPD0_0 | GPIO_PUPDR_PUPD1_0);
	GPIOB->PUPDR  &= ~(GPIO_PUPDR_PUPD0_1 | GPIO_PUPDR_PUPD1_1);
	GPIOB->AFR[0] |=  (GPIO_AFRH_AFRH0_0 | GPIO_AFRH_AFRH1_0); 
	GPIOB->AFR[0] &= ~(GPIO_AFRH_AFRH0_1 | GPIO_AFRH_AFRH1_1); 
	GPIOB->AFR[0] &= ~(GPIO_AFRH_AFRH0_2 | GPIO_AFRH_AFRH1_2); 
 	GPIOB->AFR[0] &= ~(GPIO_AFRH_AFRH0_3 | GPIO_AFRH_AFRH1_3); 
}
void Init_GPIO_TIM_All() 
{
		Init_GPIO_TIM1();
		Init_GPIO_TIM2();	
		Init_GPIO_TIM3();
}
//*******************************************************************************************//
//**************************************РЎР‘Р РћРЎ РўРђР™РњР•Р РћР’***************************************//
void TIM1_KILL(void)
{
		DMA2_Stream5->CR &= 0xFFFFFFFE;
		TIM1->CCR1 = 0;
		TIM1->CCR2 = 0;
		TIM1->CCR3 = 0;
		TIM1->CCR4 = 0;
		TIM1->CCER	&= ~TIM_CCER_CC1NE;  // Capture/Compare 1 complementary output enable
		TIM1->CCER  &= ~TIM_CCER_CC2NE;  // Capture/Compare 2 complementary output enable
		TIM1->CCER  &= ~TIM_CCER_CC3NE;  // Capture/Compare 3 complementary output enable
		TIM1->CR1 	 &= ~TIM_CR1_CEN;
		TIM1->CCMR1  &= ~TIM_CCMR1_OC1M_1 & ~TIM_CCMR1_OC1M_0 & ~TIM_CCMR1_OC1PE;
		TIM1->CCMR1  &= ~TIM_CCMR1_OC2M_1 & ~TIM_CCMR1_OC2M_0 & ~TIM_CCMR1_OC2PE;
		TIM1->CCMR2  &= ~TIM_CCMR2_OC3M_1 & ~TIM_CCMR2_OC3M_0 & ~TIM_CCMR2_OC3PE;
		TIM1->CCMR2  &= ~TIM_CCMR2_OC4M_1 & ~TIM_CCMR2_OC4M_0 & ~TIM_CCMR2_OC4PE;
		RCC->AHB1ENR &= ~RCC_AHB1ENR_DMA2EN;
		RCC->APB2ENR &= ~RCC_APB2ENR_TIM1EN;
		for(int i = 0; i<(GEN_CORE_ARRAY_MAX_COUNT*4); i++)
		{	
			burst_wave_1[i] = 0;
		}
}
void TIM2_KILL(void)
{
		DMA1_Stream7->CR &= 0x00000000;
		TIM2->CCR1 = 0;
		TIM2->CCR2 = 0;
		TIM2->CCR3 = 0;
		TIM2->CCR4 = 0;
		TIM2->CR1 &= 0x00000000;
		TIM2->CCMR1  &= ~TIM_CCMR1_OC1M_1 & ~TIM_CCMR1_OC1M_0 & ~TIM_CCMR1_OC1PE;
		TIM2->CCMR1  &= ~TIM_CCMR1_OC2M_1 & ~TIM_CCMR1_OC2M_0 & ~TIM_CCMR1_OC2PE;
		TIM2->CCMR2  &= ~TIM_CCMR2_OC3M_1 & ~TIM_CCMR2_OC3M_0 & ~TIM_CCMR2_OC3PE;
		TIM2->CCMR2  &= ~TIM_CCMR2_OC4M_1 & ~TIM_CCMR2_OC4M_0 & ~TIM_CCMR2_OC4PE;
		TIM2->CR1 &= ~TIM_CR1_CEN;
		RCC->AHB1ENR &= ~RCC_AHB1ENR_DMA1EN;
		RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN;

//	 	for(int i = 0; i<(GEN_CORE_ARRAY_MAX_COUNT*4); i++)
//		{	
//			burst_wave_2[i] = 0;
//		}
}
void TIM3_KILL(void)
{
		DMA1_Stream2->CR &= 0x00000000;
		TIM3->CCR1 = 0;
		TIM3->CCR2 = 0;
		TIM3->CCR3 = 0;
		TIM3->CCR4 = 0;
		TIM3->CR1 &= 0x00000000;
		TIM3->CCMR1  &= ~TIM_CCMR1_OC1M_1 & ~TIM_CCMR1_OC1M_0 & ~TIM_CCMR1_OC1PE;
		TIM3->CCMR1  &= ~TIM_CCMR1_OC2M_1 & ~TIM_CCMR1_OC2M_0 & ~TIM_CCMR1_OC2PE;
		TIM3->CCMR2  &= ~TIM_CCMR2_OC3M_1 & ~TIM_CCMR2_OC3M_0 & ~TIM_CCMR2_OC3PE;
		TIM3->CCMR2  &= ~TIM_CCMR2_OC4M_1 & ~TIM_CCMR2_OC4M_0 & ~TIM_CCMR2_OC4PE;
		TIM3->CR1 &= ~TIM_CR1_CEN;
		RCC->AHB1ENR &= ~RCC_AHB1ENR_DMA1EN;
		RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN;
		for(int i = 0; i<(GEN_CORE_ARRAY_MAX_COUNT*4); i++)
		{	
			burst_wave_3[i] = 0;
		}
}
//*******************************************************************************************//
void TIM1_UP_TIM10_IRQHandler(void)
{
	TIM10->SR &= ~TIM_SR_UIF;
	
	NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
	NVIC_EnableIRQ (EXTI9_5_IRQn);
	GPIOB-> ODR&=   ~ ( (0x1<<(12 * 1)) | (0x1<<(13 * 1)) );
	Gen407Update(genStruct);
	
}

