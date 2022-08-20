//*********************************************************************************************************//
//stm32fxxx input-output ports driver header file
//Created 01.06.2018
//Created by Novikov Dmitry
//*********************************************************************************************************//

//*********************************************************************************************************//
/* includes */
#include "gpio.h"

//*********************************************************************************************************//
/* methods of IO ports driver */

/* main function for init gpio */
GPIO_RESULT IOPortsInit(void)
{
	return GPIO_OK;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* get port from letter function */
GPIO_TypeDef* GetPortAddressFromLetter(uint8_t portLetter)
{
		switch(portLetter)
		{
				#ifdef GPIOA
						case 'A': return GPIOA;
				#endif
				#ifdef GPIOB
						case 'B': return GPIOB;
				#endif
				#ifdef GPIOC
						case 'C': return GPIOC;
				#endif
				#ifdef GPIOD
						case 'D': return GPIOD;
				#endif
				#ifdef GPIOE
						case 'E': return GPIOE;
				#endif
				#ifdef GPIOF
						case 'F': return GPIOF;
				#endif
				#ifdef GPIOG
						case 'G': return GPIOG;
				#endif
				#ifdef GPIOH
						case 'H': return GPIOH;
				#endif
				#ifdef GPIOI
						case 'I': return GPIOI;
				#endif
				#ifdef GPIOJ
						case 'J': return GPIOJ;
				#endif
				#ifdef GPIOK
						case 'K': return GPIOK;
				#endif
		};
		return 0;
}
//---------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------//
/* set additional parameters for port */
void STM32FXSetGPIOPower(uint8_t portLetter, uint8_t powerState)
{
		switch(portLetter)
		{
				#ifdef GPIOA
						case 'A': GPIO_POWER_SWITCH(GPIOA, powerState) return;
				#endif
				#ifdef GPIOB
						case 'B': GPIO_POWER_SWITCH(GPIOB, powerState) return;
				#endif
				#ifdef GPIOC
						case 'C': GPIO_POWER_SWITCH(GPIOC, powerState) return;
				#endif
				#ifdef GPIOD
						case 'D': GPIO_POWER_SWITCH(GPIOD, powerState) return;
				#endif
				#ifdef GPIOE
						case 'E': GPIO_POWER_SWITCH(GPIOE, powerState) return;
				#endif
				#ifdef GPIOF
						case 'F': GPIO_POWER_SWITCH(GPIOF, powerState) return;
				#endif
				#ifdef GPIOG
						case 'G': GPIO_POWER_SWITCH(GPIOG, powerState) return;
				#endif
				#ifdef GPIOH
						case 'H': GPIO_POWER_SWITCH(GPIOH, powerState) return;
				#endif
				#ifdef GPIOI
						case 'I': GPIO_POWER_SWITCH(GPIOI, powerState) return;
				#endif
				#ifdef GPIOJ
						case 'J': GPIO_POWER_SWITCH(GPIOJ, powerState) return;
				#endif
				#ifdef GPIOK
						case 'K': GPIO_POWER_SWITCH(GPIOK, powerState) return;
				#endif
		}
}
//---------------------------------------------------------------------------------------------------------//

//*******************************************end of gpio.c*************************************************//
