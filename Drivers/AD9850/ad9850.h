//*********************************************************************************************************//
//AD9850 digital synthesizer library
//Created 25.09.2018
//Created by Novikov Dmitry
//Header file
//*********************************************************************************************************//

#ifndef __AD9850_H
#define __AD9850_H

//*********************************************************************************************************//
/* includes */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "gpio.h"

#include "main.h"

//*********************************************************************************************************//
/* defines, constants and macro */

/* default config */
#define AD9850_DEFAULT_FREQUENCY 100.0 //Hz
#define AD9850_DEFAULT_PHASE     0.0   //deg
#define AD9850_DEFAULT_DUTY      0.0   //%
#define AD9850_MIN_FREQUENCY     0.0   //Hz
#define AD9850_MAX_FREQUENCY     60000000.0 //Hz
#define AD9850_MIN_PHASE         0.0   //deg
#define AD9850_MAX_PHASE         359.9 //deg
#define AD9850_MIN_DUTY          0.0   //%
#define AD9850_MAX_DUTY          100.0 //%

/* pin count for ad9850 bit-banding */
#define AD9850_PIN_COUNT 4

/* memory alloc macro set */
#define AD9850_GET_MEMORY(memSize) malloc(memSize)
#define AD9850_FREE_MEMORY(memPointer) free(memPointer)

/* bit-banding config */
#define AD9850_TIME_BEFORE_PULSE 500 //ns
#define AD9850_TIME_IN_PULSE 500 //ns
#define AD9850_TIME_AFTER_PULSE 500 //ns

//*********************************************************************************************************//
/* enumeration lists */

/* ad9850 functions result */
typedef enum
{
		AD9850_OK = 0,
		AD9850_ERROR,
		AD9850_INPUT_DATA_ERROR,
		AD9850_MEMORY_ERROR,
		AD9850_CONFIG_LOCKED
} AD9850_RESULT;

/* ad9850 implementation */
typedef enum
{
		AD9850_BITBANDING_IMPLEMENTATION = 0,
		AD9850_HARDWARE_IMPLEMENTATION
} AD9850_IMPLEMENTATION;

/* ad9850 status */
typedef enum
{
		AD9850_STATUS_BUSY = 1,
		AD9850_STATUS_ON = 2,
		AD9850_STATUS_RESET = 4,
		AD9850_STATUS_AUTO_UPDATE = 8
} AD9850_STATUS;

/* ad9850 bit-banding ports enum */
typedef enum
{
		AD9850_HAL_PIN_DATA = 0,
		AD9850_HAL_PIN_W_CLK,
		AD9850_HAL_PIN_FQ_UD,
		AD9850_HAL_PIN_RESET
} AD9850_PIN_NAME;

//*********************************************************************************************************//
/* struct declarations */

/* ad9850 one pin config struct */
typedef struct
{
		GPIO_TypeDef *portAddress;
		uint8_t pinIndex;	
} AD9850_HAL_PIN_STRUCT;

/* ad9850 hal config */
typedef struct
{
		AD9850_HAL_PIN_STRUCT* ad9850Port;
		uint8_t ad9850PinCount;
		AD9850_IMPLEMENTATION ad9850Implementation;
		void (*waitNS)(uint16_t timeNS);
} AD9850_HAL;

/* ad9850 main config */
typedef struct
{
		AD9850_HAL* ad9850Hal;
		uint32_t clockFrequency;
		uint32_t dPhase;
		uint8_t phase;
		double duty;
		AD9850_STATUS statusFlag;
		uint8_t configLocked;
} AD9850;

//*********************************************************************************************************//
/* ad9850 main functions ***********************************************************************************/
AD9850_RESULT AD9850Init(uint8_t *portTemplate, AD9850_IMPLEMENTATION ad9850Implementation, uint32_t clockFrequency,
                         void (*waitNS)(uint16_t timeNS), AD9850** ad9850);
AD9850_RESULT AD9850Destroy(AD9850** ad9850);
AD9850_RESULT AD9850SetFrequency(AD9850* ad9850, double frequency, double* outputFrequency);
AD9850_RESULT AD9850GetFrequency(AD9850* ad9850, double* outputFrequency);
AD9850_RESULT AD9850SetPhase(AD9850* ad9850, float phase, float* outputPhase);
AD9850_RESULT AD9850GetPhase(AD9850* ad9850, float* outputPhase);
AD9850_RESULT AD9850IncPhase(AD9850* ad9850, float* outputPhase);
AD9850_RESULT AD9850DecPhase(AD9850* ad9850, float* outputPhase);
AD9850_RESULT AD9850SetDuty(AD9850* ad9850, float duty, float* outputDuty);
AD9850_RESULT AD9850GetDuty(AD9850* ad9850, float* outputDuty);
AD9850_RESULT AD9850Update(AD9850* ad9850);
AD9850_RESULT AD9850SetAutoupdate(AD9850* ad9850, uint8_t autoUpdateStatus);
AD9850_RESULT AD9850SignalOnOff(AD9850* ad9850, uint8_t signalPowerStatus);
AD9850_RESULT AD9850Reset(AD9850* ad9850);


void DAC_Init(void);
void DAC_Duty_Set(float a, float b);
void DAC_Duty_Set_1(float a);
void DAC_Duty_Set_2(float b);

void Exti_Interrapt_8_Init(void);


#endif
//*********************************************end of ad9850.h*********************************************//
