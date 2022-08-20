//*********************************************************************************************************//
//AD9850 digital synthesizer library
//Created 25.09.2018
//Created by Novikov Dmitry
//Source file
//*********************************************************************************************************//

//*********************************************************************************************************//
/* includes */
#include "ad9850.h"

//*********************************************************************************************************//
/* defines and constants */
#define AD9850_PHASE_STEPS_COUNT 5

#define PowerRele_Off  		GPIOC->MODER  |=  GPIO_MODER_MODER2_0; GPIOC->MODER  &= ~GPIO_MODER_MODER2_1; 	GPIOC->BSRR |=  0x00040000;
#define PowerRele_On  		GPIOC->MODER  |=  GPIO_MODER_MODER2_0; GPIOC->MODER  &= ~GPIO_MODER_MODER2_1;   GPIOC->BSRR |=  0x00000004;
//*********************************************************************************************************//
/* Private variables --------------------------------------------------------------------------------------*/
const float phaseSteps[AD9850_PHASE_STEPS_COUNT] = {11.25, 22.5, 45.0, 90.0, 180.0};
//*********************************************************************************************************//
/* ad9850 main functions */

//---------------------------------------------------------------------------------------------------------//
/* bit-banding function send bit */
static void ad9850SendBit(AD9850* ad9850, uint8_t bit)
{
		//w_clk = 0
		GPIOResetPinP(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_W_CLK].portAddress, ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_W_CLK].pinIndex);
		//data = bit
		if (bit)
		{
				GPIOSetPinP(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_DATA].portAddress, ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_DATA].pinIndex);				
		} else
		{
				GPIOResetPinP(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_DATA].portAddress, ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_DATA].pinIndex);				
		}
		//wait
		ad9850->ad9850Hal->waitNS(AD9850_TIME_BEFORE_PULSE);
		//w_clk = 1
		GPIOSetPinP(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_W_CLK].portAddress, ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_W_CLK].pinIndex);
		//wait
		ad9850->ad9850Hal->waitNS(AD9850_TIME_IN_PULSE);
		//data = 0
		GPIOResetPinP(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_DATA].portAddress, ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_DATA].pinIndex);	
		//w_clk = 0
		GPIOResetPinP(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_W_CLK].portAddress, ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_W_CLK].pinIndex);
		//wait
		ad9850->ad9850Hal->waitNS(AD9850_TIME_AFTER_PULSE);
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* function for sending config to AD9850 */
static void ad9850SendConfig(AD9850* ad9850)
{
		uint32_t dPhase;
		uint8_t configByte;
		volatile uint8_t i, j;
		volatile uint8_t data;
	
		if (ad9850->ad9850Hal->ad9850Implementation == AD9850_BITBANDING_IMPLEMENTATION)
		{
				//delta phase
				dPhase = ad9850->dPhase;
				//calculate configByte
				//phase
				configByte = ad9850->phase << 3;
				//power on/off
				if ( !(ad9850->statusFlag & AD9850_STATUS_ON) )
				{
						configByte |= 0x04;			
				}
				
				//write delta phase
				for (i = 0; i < 4; i++)
				{
						data = dPhase & 0xFF;
						for (j = 0; j < 8; j++)
						{
								ad9850SendBit(ad9850, data & 0x01);
								data >>= 1;						
						}
						dPhase >>= 8;
				}
				//write configByte
				for (j = 0; j < 8; j++)
				{
						ad9850SendBit(ad9850, configByte & 0x01);
						configByte >>= 1;						
				}			
		}
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* AD9850 Init */
AD9850_RESULT AD9850Init(uint8_t *portTemplate, AD9850_IMPLEMENTATION ad9850Implementation, uint32_t clockFrequency,
                         void (*waitNS)(uint16_t timeNS), AD9850** ad9850)
{
		AD9850* newAD9850Driver;
		AD9850_HAL* newAD9850HalDriver;
		uint8_t i;
		AD9850_RESULT funcResult;
	
		//check input data
		if ( !portTemplate ) return AD9850_INPUT_DATA_ERROR;
		if ( (ad9850Implementation != AD9850_BITBANDING_IMPLEMENTATION) &&
		     (ad9850Implementation != AD9850_HARDWARE_IMPLEMENTATION) ) return AD9850_INPUT_DATA_ERROR;
		if ( !waitNS ) return AD9850_INPUT_DATA_ERROR;
		if ( !ad9850 ) return AD9850_INPUT_DATA_ERROR;
		if ( *ad9850 ) return AD9850_MEMORY_ERROR;
		
		DAC_Init();
		Exti_Interrapt_8_Init();
		PowerRele_On
		
		if (ad9850Implementation == AD9850_BITBANDING_IMPLEMENTATION)
		{
				//get memory for ad9850 struct, check and clear
				newAD9850Driver = (AD9850*)AD9850_GET_MEMORY(sizeof(AD9850));
				if (!newAD9850Driver) return AD9850_MEMORY_ERROR;
				memset(newAD9850Driver, 0, sizeof(AD9850));
			
				//get memory for ad9850 hal struct, check and clear
				newAD9850HalDriver = (AD9850_HAL*)AD9850_GET_MEMORY(sizeof(AD9850_HAL));
				if ( !newAD9850HalDriver )
				{
						AD9850Destroy(&newAD9850Driver);
						return AD9850_MEMORY_ERROR;
				}
				memset(newAD9850HalDriver, 0, sizeof(AD9850_HAL));
				//set
				newAD9850Driver->ad9850Hal = newAD9850HalDriver;
				
				//get memory for ad9850 pin struct config, check and clear
				newAD9850HalDriver->ad9850Port =
						(AD9850_HAL_PIN_STRUCT*)AD9850_GET_MEMORY(sizeof(AD9850_HAL_PIN_STRUCT)*AD9850_PIN_COUNT);
				if ( !newAD9850HalDriver->ad9850Port )
				{
						AD9850Destroy(&newAD9850Driver);
						return AD9850_MEMORY_ERROR;
				}
				memset(newAD9850HalDriver->ad9850Port, 0, sizeof(AD9850_HAL_PIN_STRUCT)*AD9850_PIN_COUNT);
				
				//set ad9850 driver port config
				for (i = 0; i < AD9850_PIN_COUNT; i++)
				{
						//get gpio address and pin index
						newAD9850HalDriver->ad9850Port[i].portAddress = GetPortAddressFromLetter(portTemplate[i*2]);
						newAD9850HalDriver->ad9850Port[i].pinIndex = portTemplate[i*2 + 1];
						//check
						if ( ( !newAD9850HalDriver->ad9850Port[i].portAddress ) ||
						     ( newAD9850HalDriver->ad9850Port[i].pinIndex > (GPIO_MAX_PIN_COUNT - 1) ) )
						{
								//clear and exit
								AD9850Destroy(&newAD9850Driver);
								return AD9850_INPUT_DATA_ERROR;
						}
						//gpio power ON
						STM32FXSetGPIOPower(portTemplate[i*2], 1); 
						//set port as push-pull
						GPIOConfigPinP(newAD9850HalDriver->ad9850Port[i].portAddress, newAD9850HalDriver->ad9850Port[i].pinIndex,
						               GPIO_PIN_MODE_OUTPUT, GPIO_PIN_TYPE_PUSHPULL, GPIO_PIN_SPEED_VERYHIGH, GPIO_PIN_NOPULLUPDOWN, GPIO_PIN_ALTF_RESET);
						//set all pins to 0
						GPIOResetPinP(newAD9850HalDriver->ad9850Port[i].portAddress, newAD9850HalDriver->ad9850Port[i].pinIndex);
				}				
				//set config
				newAD9850Driver->ad9850Hal->ad9850Implementation = AD9850_BITBANDING_IMPLEMENTATION;
				newAD9850Driver->ad9850Hal->ad9850PinCount = AD9850_PIN_COUNT;
				newAD9850Driver->ad9850Hal->waitNS = waitNS;
				newAD9850Driver->clockFrequency = clockFrequency;
				newAD9850Driver->dPhase = AD9850_DEFAULT_FREQUENCY * 4294967296.0 / newAD9850Driver->clockFrequency;
				newAD9850Driver->phase = AD9850_DEFAULT_PHASE;
				newAD9850Driver->duty = AD9850_DEFAULT_DUTY;
				
				//return
				*ad9850 = newAD9850Driver;
				
				//reset
				funcResult = AD9850Reset(newAD9850Driver);
				if (funcResult != AD9850_OK)
				{
						AD9850Destroy(&newAD9850Driver);
						return funcResult;					
				}
				
				//update
				funcResult = AD9850Update(newAD9850Driver);
				if (funcResult != AD9850_OK)
				{
						AD9850Destroy(&newAD9850Driver);
						return funcResult;					
				}			
		}
		
		return AD9850_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* AD9850 clean and free memory */
AD9850_RESULT AD9850Destroy(AD9850** ad9850)
{
		//check input data
		if (!ad9850) return AD9850_INPUT_DATA_ERROR;	
		if (!*ad9850) return AD9850_INPUT_DATA_ERROR;
		
		//free ad9850 port struct 
		if ((*ad9850)->ad9850Hal->ad9850Port)
		{
				AD9850_FREE_MEMORY((*ad9850)->ad9850Hal->ad9850Port);
		}
		//free ad9850 hal struct
		if ((*ad9850)->ad9850Hal)
		{
				AD9850_FREE_MEMORY((*ad9850)->ad9850Hal);
		}
		//free ad9850 main struct
		AD9850_FREE_MEMORY(*ad9850);
		//zero
		*ad9850 = 0;	
		return AD9850_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* AD9850 set frequency */
AD9850_RESULT AD9850SetFrequency(AD9850* ad9850, double frequency, double* outputFrequency)
{
		uint32_t dPhase;
		double newFrequency;
		
		//check input data
		if (!ad9850) return AD9850_INPUT_DATA_ERROR;	
		if (ad9850->configLocked) return AD9850_CONFIG_LOCKED;
	
		//check frequency
		dPhase = frequency * 4294967296.0 / ad9850->clockFrequency;
		newFrequency = (double)dPhase * ad9850->clockFrequency / 4294967296.0;
		if ( (newFrequency < AD9850_MIN_FREQUENCY) || (newFrequency > AD9850_MAX_FREQUENCY) ) return AD9850_INPUT_DATA_ERROR;
		
		//lock config
		ad9850->configLocked = 1;
		//calc new delta phase
		ad9850->dPhase = dPhase;
		//return new frequnecy
		if (outputFrequency)
		{
				*outputFrequency = newFrequency;
		}
		//unlock config
		ad9850->configLocked = 0;
		//update frequency
		if (ad9850->statusFlag & AD9850_STATUS_AUTO_UPDATE)
		{
				return AD9850Update(ad9850);			
		}	
		return AD9850_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* AD9850 get frequency */
AD9850_RESULT AD9850GetFrequency(AD9850* ad9850, double* outputFrequency)
{
		//check input data
		if (!ad9850) return AD9850_INPUT_DATA_ERROR;	
		if (!outputFrequency) return AD9850_INPUT_DATA_ERROR;
		if (ad9850->configLocked) return AD9850_CONFIG_LOCKED;
	
		//lock config
		ad9850->configLocked = 1;
		*outputFrequency = (double)ad9850->dPhase * ad9850->clockFrequency / 4294967296.0;
		//unlock config
		ad9850->configLocked = 0;
		return AD9850_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* AD9850 set phase shift */
AD9850_RESULT AD9850SetPhase(AD9850* ad9850, float phase, float* outputPhase)
{
		uint8_t newPhaseWord;
		float newPhaseValue;
		uint8_t i;
		
		//check input data
		if (!ad9850) return AD9850_INPUT_DATA_ERROR;	
		if (ad9850->configLocked) return AD9850_CONFIG_LOCKED;
	
		//calc new phase
		newPhaseWord = 0;
		newPhaseValue = 0;
		for (i = AD9850_PHASE_STEPS_COUNT; i > 0; i--)
		{
				if ( (newPhaseValue + phaseSteps[i-1]) <= phase )
				{
						newPhaseWord |= 1 << (i - 1);
						newPhaseValue += phaseSteps[i-1];
				}				
		}		
		//check new phase
		if ( (newPhaseValue < AD9850_MIN_PHASE) || (newPhaseValue > AD9850_MAX_PHASE) ) return AD9850_INPUT_DATA_ERROR;	
		//lock config
		ad9850->configLocked = 1;
		//save new phase
		ad9850->phase = newPhaseWord;
		//return new phase
		if (outputPhase)
		{
				*outputPhase = newPhaseValue;
		}
		//unlock config
		ad9850->configLocked = 0;
		//update phase
		if (ad9850->statusFlag & AD9850_STATUS_AUTO_UPDATE)
		{
				return AD9850Update(ad9850);			
		}	
		return AD9850_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* AD9850 get phase shift */
AD9850_RESULT AD9850GetPhase(AD9850* ad9850, float* outputPhase)
{
		float phaseValue;
		uint8_t i;
		
		//check input data
		if (!ad9850) return AD9850_INPUT_DATA_ERROR;	
		if (!outputPhase) return AD9850_INPUT_DATA_ERROR;
		if (ad9850->configLocked) return AD9850_CONFIG_LOCKED;
	
		//lock config
		ad9850->configLocked = 1;
		//calc phase
		phaseValue = 0;
		for (i = AD9850_PHASE_STEPS_COUNT; i > 0; i--)
		{
				if ( ad9850->phase & (1 << (i-1)) )
				{
						phaseValue += phaseSteps[i-1];
				}				
		}
		//return
		*outputPhase = phaseValue;
		//unlock config
		ad9850->configLocked = 0;
		return AD9850_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* AD9850 inc phase shift */
AD9850_RESULT AD9850IncPhase(AD9850* ad9850, float* outputPhase)
{
		uint8_t newPhaseWord;
		float newPhaseValue;
		uint8_t i;
		
		//check input data
		if (!ad9850) return AD9850_INPUT_DATA_ERROR;	
		if (ad9850->configLocked) return AD9850_CONFIG_LOCKED;
	
		newPhaseWord = ad9850->phase;
		//inc phase
		if (newPhaseWord < 31)
		{
				newPhaseWord++;
		}
		//calc phase
		newPhaseValue = 0;
		for (i = AD9850_PHASE_STEPS_COUNT; i > 0; i--)
		{
				if ( newPhaseWord & (1 << (i-1)) )
				{
						newPhaseValue += phaseSteps[i-1];
				}				
		}
		//check new phase
		if ( (newPhaseValue < AD9850_MIN_PHASE) || (newPhaseValue > AD9850_MAX_PHASE) ) return AD9850_INPUT_DATA_ERROR;
		//lock config
		ad9850->configLocked = 1;
		//save new phase
		ad9850->phase = newPhaseWord;
		//return new phase
		if (outputPhase)
		{
				*outputPhase = newPhaseValue;
		}
		//unlock config
		ad9850->configLocked = 0;
		//update phase
		if (ad9850->statusFlag & AD9850_STATUS_AUTO_UPDATE)
		{
				return AD9850Update(ad9850);			
		}
		return AD9850_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* AD9850 dec phase shift */
AD9850_RESULT AD9850DecPhase(AD9850* ad9850, float* outputPhase)
{
		uint8_t newPhaseWord;
		float newPhaseValue;
		uint8_t i;
		
		//check input data
		if (!ad9850) return AD9850_INPUT_DATA_ERROR;	
		if (ad9850->configLocked) return AD9850_CONFIG_LOCKED;
	
		newPhaseWord = ad9850->phase;
		//inc phase
		if (newPhaseWord > 0)
		{
				newPhaseWord--;
		}
		//calc phase
		newPhaseValue = 0;
		for (i = AD9850_PHASE_STEPS_COUNT; i > 0; i--)
		{
				if ( newPhaseWord & (1 << (i-1)) )
				{
						newPhaseValue += phaseSteps[i-1];
				}				
		}
		//check new phase
		if ( (newPhaseValue < AD9850_MIN_PHASE) || (newPhaseValue > AD9850_MAX_PHASE) ) return AD9850_INPUT_DATA_ERROR;
		//lock config
		ad9850->configLocked = 1;
		//save new phase
		ad9850->phase = newPhaseWord;
		//return new phase
		if (outputPhase)
		{
				*outputPhase = newPhaseValue;
		}
		//unlock config
		ad9850->configLocked = 0;
		//update phase
		if (ad9850->statusFlag & AD9850_STATUS_AUTO_UPDATE)
		{
				return AD9850Update(ad9850);			
		}
		return AD9850_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* AD9850 set duty */
AD9850_RESULT AD9850SetDuty(AD9850* ad9850, float duty, float* outputDuty)
{
		//check input data
		if (!ad9850) return AD9850_INPUT_DATA_ERROR;	
		if ( (duty < AD9850_MIN_DUTY) || (duty > AD9850_MAX_DUTY) ) return AD9850_INPUT_DATA_ERROR;
		if (ad9850->configLocked) return AD9850_CONFIG_LOCKED;

		//lock config
		ad9850->configLocked = 1;	
		//save new duty
		ad9850->duty = duty;
		//return new duty
		if (outputDuty)
		{
				*outputDuty = duty;
		}
		//unlock config
		ad9850->configLocked = 0;
		//update duty
		if (ad9850->statusFlag & AD9850_STATUS_AUTO_UPDATE)
		{
				return AD9850Update(ad9850);			
		}	
		return AD9850_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* AD9850 get duty */
AD9850_RESULT AD9850GetDuty(AD9850* ad9850, float* outputDuty)
{
		//check input data
		if (!ad9850) return AD9850_INPUT_DATA_ERROR;	
		if (!outputDuty) return AD9850_INPUT_DATA_ERROR;
		if (ad9850->configLocked) return AD9850_CONFIG_LOCKED;
	
		//lock config
		ad9850->configLocked = 1;
		*outputDuty = ad9850->duty;
		//unlock config
		ad9850->configLocked = 0;
		return AD9850_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* AD9850 update module */
AD9850_RESULT AD9850Update(AD9850* ad9850)
{
		//check input data
		if (!ad9850) return AD9850_INPUT_DATA_ERROR;
		if (ad9850->configLocked) return AD9850_CONFIG_LOCKED;
	
		//lock config
		ad9850->configLocked = 1;
		//send data to ad9850
		ad9850SendConfig(ad9850);
		//update!
		//wait
		ad9850->ad9850Hal->waitNS(AD9850_TIME_BEFORE_PULSE);
		//fq_ud pin up
		GPIOSetPinP(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_FQ_UD].portAddress, ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_FQ_UD].pinIndex);
		//wait
		ad9850->ad9850Hal->waitNS(AD9850_TIME_IN_PULSE);
		//fq_ud pin down
		GPIOResetPinP(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_FQ_UD].portAddress, ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_FQ_UD].pinIndex);	
		//wait
		ad9850->ad9850Hal->waitNS(AD9850_TIME_AFTER_PULSE);
		
		if(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_FQ_UD].pinIndex == 9)
		{
			DAC_Duty_Set_1((ad9850->duty)*10);
		}
		if(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_FQ_UD].pinIndex == 6)
		{
			DAC_Duty_Set_2((ad9850->duty)*10);
		}

		//unlock config
		ad9850->configLocked = 0;
		return AD9850_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* AD9850 config autoupdate flag */
AD9850_RESULT AD9850SetAutoupdate(AD9850* ad9850, uint8_t autoUpdateStatus)
{
		//check input data
		if (!ad9850) return AD9850_INPUT_DATA_ERROR;
		if (ad9850->configLocked) return AD9850_CONFIG_LOCKED;
		
		//lock config
		ad9850->configLocked = 1;
		//set autoupdate
		if (autoUpdateStatus)
		{
				ad9850->statusFlag |= AD9850_STATUS_AUTO_UPDATE;
		} else
		{
				ad9850->statusFlag &= ~AD9850_STATUS_AUTO_UPDATE;				
		}		
		//unlock config
		ad9850->configLocked = 0;
		return AD9850_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* AD9850 config power on/off */
AD9850_RESULT AD9850SignalOnOff(AD9850* ad9850, uint8_t signalPowerStatus)
{
		//check input data
		if (!ad9850) return AD9850_INPUT_DATA_ERROR;
		if (ad9850->configLocked) return AD9850_CONFIG_LOCKED;
		
		//lock config
		ad9850->configLocked = 1;
		//set power status
		if (signalPowerStatus)
		{
				ad9850->statusFlag |= AD9850_STATUS_ON;
				PowerRele_On
		} else
		{
				ad9850->statusFlag &= ~AD9850_STATUS_ON;
				//PowerRele_Off
		}		
		//unlock config
		ad9850->configLocked = 0;
		return AD9850_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* AD9850 reset */
AD9850_RESULT AD9850Reset(AD9850* ad9850)
{
		//check input data
		if (!ad9850) return AD9850_INPUT_DATA_ERROR;
		if (ad9850->configLocked) return AD9850_CONFIG_LOCKED;
	
		//lock config
		ad9850->configLocked = 1;
		//switch AD9850 to reset state
		ad9850->statusFlag |= AD9850_STATUS_RESET;
		GPIOSetPinP(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_RESET].portAddress, ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_RESET].pinIndex);
		//wait
		ad9850->ad9850Hal->waitNS(AD9850_TIME_IN_PULSE);	
		//AD9850 reset state off
		GPIOResetPinP(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_RESET].portAddress, ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_RESET].pinIndex);
		ad9850->statusFlag &= ~AD9850_STATUS_RESET;
		//wait
		ad9850->ad9850Hal->waitNS(AD9850_TIME_AFTER_PULSE);
		//enable ad9850 serial data mode
		//w_clk = 1
		GPIOSetPinP(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_W_CLK].portAddress, ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_W_CLK].pinIndex);
		//wait
		ad9850->ad9850Hal->waitNS(AD9850_TIME_IN_PULSE);
		//w_clk = 0
		GPIOResetPinP(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_W_CLK].portAddress, ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_W_CLK].pinIndex);
		//wait
		ad9850->ad9850Hal->waitNS(AD9850_TIME_AFTER_PULSE);
		//fq_ud = 1
		GPIOSetPinP(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_FQ_UD].portAddress, ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_FQ_UD].pinIndex);
		//wait
		ad9850->ad9850Hal->waitNS(AD9850_TIME_IN_PULSE);
		//fq_ud = 0
		GPIOResetPinP(ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_FQ_UD].portAddress, ad9850->ad9850Hal->ad9850Port[AD9850_HAL_PIN_FQ_UD].pinIndex);
		//wait
		ad9850->ad9850Hal->waitNS(AD9850_TIME_AFTER_PULSE);
		//unlock config
		ad9850->configLocked = 0;	
		return AD9850_OK;
}
//---------------------------------------------------------------------------------------------------------//

void DAC_Init(void)
{
//	if((RCC->AHB1ENR & RCC_AHB1ENR_GPIOAEN) == 0)
//	{
//		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
//	}
//  if((RCC->APB1ENR & RCC_APB1ENR_DACEN) == 0)
//	{
//		RCC->APB1ENR |= RCC_APB1ENR_DACEN;
//	}
//	
//	GPIOA->MODER   |= GPIO_MODER_MODER4_1	|	GPIO_MODER_MODER4_0	| GPIO_MODER_MODER5_1	|	GPIO_MODER_MODER5_0;//Alternate Funk
//	GPIOA->PUPDR   &=~(GPIO_PUPDR_PUPDR4_1	|	GPIO_PUPDR_PUPDR4_0	| GPIO_PUPDR_PUPDR5_1	|	GPIO_PUPDR_PUPDR5_0);//No pull-up, pull-down
//	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR4_1 | GPIO_OSPEEDER_OSPEEDR5_1; 
//	
////	DAC->CR |= DAC_CR_DMAUDRIE2;//DAC channel2 DMA underrun interrupt enabled
////	DAC->CR |= DAC_CR_DMAUDRIE1;//DAC channel1 DMA underrun interrupt enabled
////	DAC->CR |= DAC_CR_DMAEN2;//DAC channel2 DMA mode enabled
////	DAC->CR |= DAC_CR_DMAEN1;//DAC channel1 DMA mode enabled
//	DAC->CR	&= ~(DAC_CR_WAVE2_0 | DAC_CR_WAVE2_1);//wave generation disabled
//	DAC->CR	&= ~(DAC_CR_WAVE1_0 | DAC_CR_WAVE1_1);//wave generation disabled
//	
////	DAC->CR |= DAC_CR_TSEL2_0	|	DAC_CR_TSEL2_1	|	DAC_CR_TSEL2_2;//Software trigger	// Выбор события
////	DAC->CR |= DAC_CR_TSEL1_0	|	DAC_CR_TSEL1_1	|	DAC_CR_TSEL1_2;//Software trigger	// Выбор события
//	
//	DAC->CR &= ~DAC_CR_BOFF2;//DAC channel2 output buffer enabled
//	DAC->CR &= ~DAC_CR_BOFF1;//DAC channel1 output buffer enabled
//	
////	DAC->CR |= DAC_CR_TEN2;//DAC channel2 trigger enable	// включение вывода данных от события
////	DAC->CR |= DAC_CR_TEN1;//DAC channel1 trigger enable	// включение вывода данных от события
//	
//	DAC->CR |= DAC_CR_EN2;//DAC channel2 enabled
//	DAC->CR |= DAC_CR_EN1;//DAC channel1 enabled
//	
////	DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG2;//DAC channel2 software trigger
////	DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;//DAC channel1 software trigger
}

void DAC_Duty_Set(float a, float b)
{
	uint32_t value_a = (roundf		( ((double)(a / 1000))*4095 )		);
	uint32_t value_b = (roundf		( ((double)(b / 1000))*4095 )		);

	DAC->DHR12RD = value_a | (value_b<<16)		;	
}
void DAC_Duty_Set_1(float a)
{
	uint32_t value_a = (roundf		( ((double)(a / 1000))*4095 )		);

	DAC->DHR12RD &= ~0xFFF;
	DAC->DHR12RD |= value_a;	
}
void DAC_Duty_Set_2(float b)
{
	uint32_t value_b = (roundf		( ((double)(b / 1000))*4095 )		);

	DAC->DHR12RD &= ~0xFFF0000;
	DAC->DHR12RD |= (value_b<<16)		;	
}

void Exti_Interrapt_8_Init(void)
{

	if((RCC->AHB1ENR & RCC_AHB1ENR_GPIODEN) == 0)
	{
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	}
	GPIOD-> MODER  &= ~ ( (0x3<<(8 * 2)) );// PD8 Input
	GPIOD-> OTYPER &= ~ ( (0x1<< 8 ) );// Тип вывода: двухтактный
	GPIOD-> OSPEEDR|=   ( (0x3<<(8 * 2)) );
	GPIOD-> PUPDR &= ~  ( (0x3<<(8 * 2)) );// Очистка

	GPIOB-> MODER  |= ( (0x1<<(12 * 2)) | (0x1<<(13 * 2)) | (0x1<<(14 * 2)) );
	
	SYSCFG->EXTICR[2] |= 0x3;
	
	EXTI->RTSR 	|=(EXTI_FTSR_TR8);
	EXTI->IMR		|=(EXTI_IMR_MR8);
	
	NVIC_EnableIRQ (EXTI9_5_IRQn);
	
	__enable_irq ();
}

void EXTI9_5_IRQHandler(void)
{

	
		RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
		TIM10->PSC = 8400 + 1;
		TIM10->ARR = 1000;// 1000-10ghz  100-100ghz  10-1000ghz
		TIM10->CR1 = TIM_CR1_URS;
		TIM10->EGR |= TIM_EGR_UG;
		TIM10->DIER |= TIM_DIER_UIE;
		TIM10->CR1 |= TIM_CR1_CEN | TIM_CR1_OPM;
		
		NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);	
		NVIC_DisableIRQ (EXTI9_5_IRQn);

	
		
		GPIOB-> ODR|=   ( (0x1<<(12 * 1)) | (0x1<<(13 * 1)) | (0x1<<(14 * 1)) );
		EXTI->PR |= EXTI_PR_PR8;
		
}

//void TIM1_UP_TIM10_IRQHandler(void)
//{
//	TIM10->SR &= ~TIM_SR_UIF;
//	
//	NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
//	NVIC_EnableIRQ (EXTI9_5_IRQn);
//	GPIOB-> ODR&=   ~ ( (0x1<<(12 * 1)) | (0x1<<(13 * 1)) );
//	Gen407Update(genStruct);
//	
//}





//*********************************************end of ad9850.c*********************************************//
