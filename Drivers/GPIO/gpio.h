//*********************************************************************************************************//
//stm32fxxx input-output ports driver header file
//Created 01.06.2018
//Created by Novikov Dmitry
//NOTE: configured for stm32f4xx
//*********************************************************************************************************//

#ifndef __GPIO_DRIVER_H
#define __GPIO_DRIVER_H

//*********************************************************************************************************//
/* includes */
#include "stm32f407xx.h"

//*********************************************************************************************************//
/* defines, constatnts and macro */

#define GPIO_MAX_PIN_COUNT 16U

/* defines for port power control */
#ifdef GPIOA
		#define GPIOA_POWER_ON 	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN
		#define GPIOA_POWER_OFF RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOAEN
#endif

#ifdef GPIOB
		#define GPIOB_POWER_ON 	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN
		#define GPIOB_POWER_OFF RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOBEN
#endif

#ifdef GPIOC
		#define GPIOC_POWER_ON 	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN
		#define GPIOC_POWER_OFF RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOCEN
#endif

#ifdef GPIOD
		#define GPIOD_POWER_ON 	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN
		#define GPIOD_POWER_OFF RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIODEN
#endif

#ifdef GPIOE
		#define GPIOE_POWER_ON 	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN
		#define GPIOE_POWER_OFF RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOEEN
#endif

#ifdef GPIOF
		#define GPIOF_POWER_ON 	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN
		#define GPIOF_POWER_OFF RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOFEN
#endif

#ifdef GPIOG
		#define GPIOG_POWER_ON 	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN
		#define GPIOG_POWER_OFF RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOGEN
#endif

#ifdef GPIOH
		#define GPIOH_POWER_ON 	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN
		#define GPIOH_POWER_OFF RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOHEN
#endif

#ifdef GPIOI
		#define GPIOI_POWER_ON 	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN
		#define GPIOI_POWER_OFF RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOIEN
#endif

#ifdef GPIOJ
		#define GPIOJ_POWER_ON 	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN
		#define GPIOJ_POWER_OFF RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOJEN
#endif

#ifdef GPIOK
		#define GPIOK_POWER_ON 	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN
		#define GPIOK_POWER_OFF RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOKEN
#endif

/* macro for port power control */
#define GPIO_POWER_SWITCH(GPIOX, powerStatus) if (powerStatus) { GPIOX##_POWER_ON; } else { GPIOX##_POWER_OFF; }

/* defines for port modes */
#define GPIO_PIN_MODE_INPUT     0U
#define GPIO_PIN_MODE_OUTPUT    1U
#define GPIO_PIN_MODE_ALTFUNC   2U
#define GPIO_PIN_MODE_ANALOG    3U
#define GPIO_PIN_MODE_RESET     GPIO_PIN_MODE_INPUT
#define GPIO_PIN_MODE_MASK      3U
#define GPIO_PIN_TYPE_PUSHPULL  0U
#define GPIO_PIN_TYPE_OPENDRAIN 1U
#define GPIO_PIN_TYPE_RESET     GPIO_PIN_TYPE_PUSHPULL
#define GPIO_PIN_TYPE_MASK      1U
#define GPIO_PIN_SPEED_LOW      0U
#define GPIO_PIN_SPEED_MEDIUM   1U
#define GPIO_PIN_SPEED_HIGH     2U
#define GPIO_PIN_SPEED_VERYHIGH 3U
#define GPIO_PIN_SPEED_RESET    GPIO_PIN_SPEED_LOW
#define GPIO_PIN_SPEED_MASK     3U
#define GPIO_PIN_NOPULLUPDOWN   0U
#define GPIO_PIN_PULLUP         1U
#define GPIO_PIN_PULLDOWN       2U
#define GPIO_PIN_PULL_RESET     0U
#define GPIO_PIN_PULL_MASK      3U
#define GPIO_PIN_ALTF_MASK      0xFU
#define GPIO_PIN_ALTF_RESET     0U

/* macro for port config */
#define GPIOConfigPinP(pin_port, pin_number, pin_mode, pin_type, pin_speed, pin_pull, pin_altfnum)\
										pin_port->MODER &= ~(GPIO_PIN_MODE_MASK << pin_number * 2);\
										pin_port->MODER |= (pin_mode << pin_number * 2);\
										pin_port->OTYPER &= ~(GPIO_PIN_TYPE_MASK << pin_number);\
										pin_port->OTYPER |= (pin_type << pin_number);\
										pin_port->OSPEEDR &= ~(GPIO_PIN_SPEED_MASK << pin_number * 2);\
										pin_port->OSPEEDR |= (pin_speed << pin_number * 2);\
										pin_port->PUPDR &= ~(GPIO_PIN_PULL_MASK << pin_number * 2);\
										pin_port->PUPDR |= (pin_pull << pin_number * 2);\
										if ( pin_number < 8 )\
										{\
											pin_port->AFR[0] &= ~(GPIO_PIN_ALTF_MASK << pin_number * 4);\
											pin_port->AFR[0] |= pin_altfnum << pin_number * 4;\
										} else\
										{\
											pin_port->AFR[1] &= ~(GPIO_PIN_ALTF_MASK << ( pin_number - 8) * 4);\
											pin_port->AFR[1] |= pin_altfnum << ( pin_number - 8) * 4;\
										}
#define GPIOConfigPinL(pin_letter, pin_number, pin_mode, pin_type, pin_speed, pin_pull, pin_altfnum)\
										GPIO##pin_letter->MODER &= ~(GPIO_PIN_MODE_MASK << pin_number * 2);\
										GPIO##pin_letter->MODER |= (pin_mode << pin_number * 2);\
										GPIO##pin_letter->OTYPER &= ~(GPIO_PIN_TYPE_MASK << pin_number);\
										GPIO##pin_letter->OTYPER |= (pin_type << pin_number);\
										GPIO##pin_letter->OSPEEDR &= ~(GPIO_PIN_SPEED_MASK << pin_number * 2);\
										GPIO##pin_letter->OSPEEDR |= (pin_speed << pin_number * 2);\
										GPIO##pin_letter->PUPDR &= ~(GPIO_PIN_PULL_MASK << pin_number * 2);\
										GPIO##pin_letter->PUPDR |= (pin_pull << pin_number * 2);\
										if ( pin_number < 8 )\
										{\
											GPIO##pin_letter->AFR[0] &= ~(GPIO_PIN_ALTF_MASK << pin_number * 4);\
											GPIO##pin_letter->AFR[0] |= pin_altfnum << pin_number * 4;\
										} else\
										{\
											GPIO##pin_letter->AFR[1] &= ~(GPIO_PIN_ALTF_MASK << ( pin_number - 8) * 4);\
											GPIO##pin_letter->AFR[1] |= pin_altfnum << ( pin_number - 8) * 4;\
										}
										
/* macro for set and reset pin */
#define GPIOSetPinP(pin_port, pin_number) pin_port->BSRR |= 1 << pin_number
#define GPIOResetPinP(pin_port, pin_number) pin_port->BSRR |= 1 << ( pin_number + 16 )
#define GPIOSetPinL(pin_letter, pin_number) GPIO##pin_letter->BSRR |= 1 << pin_number
#define GPIOResetPinL(pin_letter, pin_number) GPIO##pin_letter->BSRR |= 1 << ( pin_number + 16 )
										
/* macro for read pin level */
#define GPIOReadPinP(pin_port, pin_number) (uint8_t)((pin_port->IDR & (1 << pin_number)) != 0)
#define GPIOReadPinL(pin_letter, pin_number) (uint8_t)((GPIO##pin_letter->IDR & (1 << pin_number)) != 0)
										
//*********************************************************************************************************//
/* enumeration lists*/
typedef enum
{
	GPIO_OK = 0,
	GPIO_ERROR = 1
} GPIO_RESULT;

//*********************************************************************************************************//
/* methods of IO ports driver */

/* main function for init gpio */
GPIO_RESULT IOPortsInit(void);
GPIO_TypeDef* GetPortAddressFromLetter(uint8_t portLetter);
void STM32FXSetGPIOPower(uint8_t portLetter, uint8_t powerState);

#endif
//*******************************************end of gpio.h*************************************************//
