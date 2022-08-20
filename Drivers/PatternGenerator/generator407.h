//*********************************************************************************************************//
// STM32F407 Generator library
// Header file
// Created 00.00.2018
//*********************************************************************************************************//
#ifndef GENERATOR407_H
#define GENERATOR407_H
//*********************************************************************************************************//
//*********************************************** includes ************************************************//
#include "stm32f4xx.h"
#include "math.h"
#include <stdlib.h>
#include <string.h>
#include "simpleGenerator.h"
//*********************************************************************************************************//
//************************************************ defines ************************************************//
#define PI 3.1415926535

/* input frequency for generator timers */
#define TIMER_INPUT_FREQ 168000000uL

/* pwm modes range */
#define GENERATOR_MODE_COUNT		((uint8_t)9)
#define GENERATOR_MODE_MIN			((uint8_t)1)
#define GENERATOR_MODE_MAX			GENERATOR_MODE_COUNT
#define GENERATOR_MODE_DEFAULT	((uint8_t)1)

/* external signal frequency range, Hz */
#define EXT_FREQ_MIN			((uint32_t)1.0)
#define EXT_FREQ_MAX			((uint32_t)2333333)
#define EXT_FREQ_DEFAULT	((uint32_t)100)

/* internal signal frequency range, Hz */
#define INT_FREQ_MIN			((uint32_t)1)
#define INT_FREQ_MAX			((uint32_t)5000000)
#define INT_FREQ_DEFAULT	((uint32_t)10000)

/* duty cycle of external signal range, (div 10)% */
#define EXT_DUTY_CYCLE_MIN			((uint16_t)0)
#define EXT_DUTY_CYCLE_MAX			((uint16_t)1000)
#define EXT_DUTY_CYCLE_DEFAULT	((uint16_t)500)

/* duty cycle of internal signal range, (div 10)% */
#define INT_DUTY_CYCLE_MIN			((uint16_t)0)
#define INT_DUTY_CYCLE_MAX			((uint16_t)1000)
#define INT_DUTY_CYCLE_DEFAULT	((uint16_t)500)

/* phase shift range, (div 10)% */
#define PHASESHIFT_MIN			((uint16_t)0)
#define PHASESHIFT_MAX			((uint16_t)360)
#define PHASESHIFT_DEFAULT	((uint16_t)120)

/* dead time range, ns */
#define DEADTIME_MIN			((uint32_t)0)
#define DEADTIME_MAX			((uint32_t)100000)
#define DEADTIME_DEFAULT	((uint32_t)0)

/* sinus shift, (div 10)% */
#define SINSHIFT_MIN			((uint16_t)0)
#define SINSHIFT_MAX			((uint16_t)100)
#define SINSHIFT_DEFAULT	((uint16_t)0)

/* ??? */	
#define COUNTDOWN_MIN			((uint32_t)1)
#define COUNTDOWN_MAX			((uint32_t)((GEN_CORE_ARRAY_LENGTH+2)/2))
#define COUNTDOWN_DEFAULT	((uint32_t)10)

/* ??? */
#define SINDIRECTION_MIN			((uint8_t)0)
#define SINDIRECTION_MAX			((uint8_t)1)
#define SINDIRECTION_DEFAULT	((uint8_t)0)

/* ??? */
#define TIMSTATUS_MIN			((uint8_t)1)
#define TIMSTATUS_MAX			((uint8_t)3)
#define TIMSTATUS_DEFAULT	((uint8_t)1)
//#define TIM1_ON ((uint8_t)1)
//#define TIM2_ON ((uint8_t)2)
//#define TIM3_ON ((uint8_t)4)

/* generator core array size */
#define GEN_CORE_ARRAY_LENGTH			((uint16_t)1000)//2100)
#define GEN_CORE_ARRAY_MIN_COUNT	((uint16_t)2)
#define GEN_CORE_ARRAY_MAX_COUNT	GEN_CORE_ARRAY_LENGTH

#define GEN_CORE_ARRAY_LENGTH_Virtual			((uint32_t)64000)

#define MODEPWM_MIN 		((uint8_t)1)
#define MODEPWM_MAX 		((uint8_t)8)
#define MODEPWM_DEFAULT ((uint8_t)1)

/* precision of timer pwm */
#define TIM_PRECISION_MIN ((uint16_t)0)
#define TIM_PRECISION_MAX ((uint16_t)65535)



///////INTERNAL DEFINES///////////
#define TIM1_Prescaller_ (2)

//*********************************************************************************************************//
//******************************************* enumeration lists *******************************************//
/* result for generator functions */
enum GEN_RESULT
{
	GEN_RESULT_OK = 0,
	GEN_RESULT_ERROR,
	GEN_VALUE_OUT_OF_RANGE,
	GEN_VALUE_CHANGE_UNAVAILABLE
};

enum PARAM_TEST_ERROR_CODE
{
		PARAM_TEST_OK,							
		PARAM_ERR_EXTFREQ_LESSTHENRANGE,	
		PARAM_ERR_EXTFREQ_MORETHENRANGE,	
		PARAM_ERR_INTFREQ_LESSTHENRANGE,	
		PARAM_ERR_INTFREQ_MORETHENRANGE,	
		PARAM_ERR_EXTDUTY_LESSTHENRANGE,
		PARAM_ERR_EXTDUTY_MORETHENRANGE,		
		PARAM_ERR_INTDUTY_LESSTHENRANGE,	
		PARAM_ERR_INTDUTY_MORETHENRANGE,
		PARAM_ERR_PHASESHIFT_LESSTHENRANGE,
		PARAM_ERR_PHASESHIFT_MORETHENRANGE,
		PARAM_ERR_DEADTIME_LESSTHENRANGE,
		PARAM_ERR_DEADTIME_MORETHENRANGE,
		PARAM_ERR_MODEPWM_LESSTHENRANGE,
		PARAM_ERR_MODEPWM_MORETHENRANGE,
		PARAM_ERR_SINSHIFT_LESSTHENRANGE,
		PARAM_ERR_SINSHIFT_MORETHENRANGE,
		PARAM_ERR_COUNTDOWN_LESSTHENRANGE,
		PARAM_ERR_COUNTDOWN_MORETHENRANGE,
		PARAM_ERR_SINDIRECTION_LESSTHENRANGE,
		PARAM_ERR_SINDIRECTION_MORETHENRANGE,
		PARAM_ERR_TIMSTATUS_LESSTHENRANGE,
		PARAM_ERR_TIMSTATUS_MORETHENRANGE,
		PARAM_ERR_STEPS_LESSTHENRANGE,
		PARAM_ERR_STEPS_MORETHENRANGE,
		PARAM_ERR_PRECISION_LESSTHENRANGE,
		PARAM_ERR_PRECISION_MORETHENRANGE
};
//*********************************************************************************************************//
//************************************************ structs ************************************************//
/* generator data struct */
typedef struct
{
		uint32_t extFreq;//216bit 54*4=216
		uint32_t intFreq;//216+8=224/32=7
		uint16_t extDuty;
		uint16_t intDuty;
		uint16_t phaseShift;
		uint8_t  modePWM;
		uint32_t deadTime;
		uint16_t sinShift;
		uint32_t countdown;
		uint8_t  sinDirection;
		uint8_t  timStatus;
		uint8_t  Nothing;
}GenParametersTypeDef;

typedef struct
{
		uint8_t configLock;
		GenParametersTypeDef currentConfig;
		uint8_t generatorStartFlag;
	
		uint8_t selectedBuffer;
		uint8_t needChangeBuffer;
}GeneratorTypeDef;

typedef struct
{
	GenParametersTypeDef mode1_All;//7*32  * 9
	GenParametersTypeDef mode2_All;
	GenParametersTypeDef mode3_All;
	GenParametersTypeDef mode4_All;
	GenParametersTypeDef mode5_All;
	GenParametersTypeDef mode6_All;
	GenParametersTypeDef mode7_All;
	GenParametersTypeDef mode8_All;
	GenParametersTypeDef mode9_All;

}GenParametersTypeDefForFlash;

//*********************************************************************************************************//
//************************************ generator functions - external *************************************//
uint16_t Gen407Init(void);
uint16_t Gen407Close(GeneratorTypeDef** genStruct);
uint16_t Gen407Start(GeneratorTypeDef* genStruct);
uint16_t Gen407Stop(GeneratorTypeDef* genStruct);
uint16_t Gen407Update(GeneratorTypeDef* genStruct);
uint16_t Gen407Update_SmoothStart(GeneratorTypeDef* genStruct, uint16_t steps, uint16_t precision);
uint16_t Gen407SetConfig(GeneratorTypeDef* genStruct, GenParametersTypeDef* inputConfig);
uint16_t Gen407GetConfig(GeneratorTypeDef* genStruct, GenParametersTypeDef* outputConfig);
uint16_t Gen407SetMode(GeneratorTypeDef* genStruct, uint8_t genMode);
uint16_t Gen407SetExtFreq(GeneratorTypeDef* genStruct, uint32_t extFreq);
uint16_t Gen407SetIntFreq(GeneratorTypeDef* genStruct, uint32_t intFreq);
uint16_t Gen407SetExtDutyCycle(GeneratorTypeDef* genStruct, uint16_t extDutyCycle);
uint16_t Gen407SetIntDutyCycle(GeneratorTypeDef* genStruct, uint16_t intDutyCycle);
uint16_t Gen407SetPhaseShift(GeneratorTypeDef* genStruct, uint16_t phaseShift);
uint16_t Gen407SetSinShift(GeneratorTypeDef* genStruct, uint16_t sinShift);
uint16_t Gen407SetCountdown(GeneratorTypeDef* genStruct, uint16_t countdown);
uint16_t Gen407SetDeadTime(GeneratorTypeDef* genStruct, uint32_t deadTime);

uint32_t Gen407_GetRealFrequency(uint32_t extFreq);
float Gen407_GetRealFrequency_Now(uint16_t steps, uint16_t precision);
float Gen407_GetPrevFrequency(uint32_t extFreq);
float Gen407_GetNextFrequency(uint32_t extFreq);
uint32_t Gen407_GetPrevInternalFrequency(uint32_t intFreq, uint32_t extFreq);
uint32_t Gen407_GetNextInternalFrequency(uint32_t intFreq, uint32_t extFreq);

uint32_t Gen407_GetRealDuty(uint16_t Duty, uint32_t Steps);
uint32_t Gen407_GetNextDuty(uint16_t Duty, uint32_t Steps);
uint32_t Gen407_GetPrevDuty(uint16_t Duty, uint32_t Steps);

uint16_t getMaxDivider (uint32_t number, uint32_t Start_Divider);
void Gen407_ThreePhase_SmoothStart(GeneratorTypeDef* genStruct);

uint16_t CheckDataErrors(GenParametersTypeDef* param, uint32_t code);
//*********************************************************************************************************//
//************************************ generator functions - internal *************************************//
void PWM_SIN_Bridge(GenParametersTypeDef* param);
void StepsAndPrecisionValue(uint32_t PWM_Freq, uint32_t MOD_Freq, uint8_t MODE_PWM, uint32_t countdown, uint16_t* steps, uint16_t* precision);
void StartGenerator(uint16_t steps, uint16_t precision, uint8_t timStatus);
void BurstArraySelect(uint8_t timStatus);


//**************************************
void Gen407TestSignal_Burst(void);//****
//**************************************





void Init_DMA_TIM( uint16_t steps, TIM_TypeDef* timer);
void Init_TIM (uint16_t precision, TIM_TypeDef* timer);

void TIM1_KILL(void);
void TIM2_KILL(void);
void TIM3_KILL(void);

void Init_GPIO_TIM1(void);
void Init_GPIO_TIM2(void);
void Init_GPIO_TIM3(void);
void Init_GPIO_TIM_All(void);

void Init_DMA_TIM_THREE_PHASE(uint16_t steps);
void Init_TIM_THREE_PHASE(uint16_t precision);
void Init_GPIO_TIM1_THREE_PHASE(void);
void ThreePhaseDeadTime(uint32_t DEAD_time_count);
uint16_t convert_ns_in_pt (uint16_t value_ns);
uint16_t convert_pt_in_ns (uint16_t value_pt);

void FillArgument_THREE_PHASE(uint16_t DUTY_Cycle, uint16_t PHASE_SHIFT);
void FillArgument_PWM_Pulse(uint16_t extDuty, uint32_t DEAD_time_count, uint8_t Tims_On);
void FillArgument_TRIANGLE(uint16_t extDuty, uint16_t intDuty, uint16_t SIN_SHIFT, uint32_t DEAD_time_count, uint8_t SIN_DIRECTION, uint8_t Tims_On);
void FillArgument_SIN_SHIFT(uint16_t extDuty, uint16_t intDuty, uint16_t SIN_SHIFT, uint32_t DEAD_time_count, uint8_t SIN_DIRECTION, uint8_t Tims_On);
void FillArgument_SIN(uint16_t extDuty, uint32_t DEAD_time_count, uint8_t Tims_On);
void FillArgument_PWM_ODNOTAKT_PHASE_LL(uint16_t extDuty, uint16_t intDuty, uint16_t PHASE_SHIFT, uint8_t Tims_On);
void FillArgument_PWM_ODNOTAKT_PHASE_HL(uint16_t extDuty, uint16_t PHASE_SHIFT, uint8_t Tims_On);
void FillArgument_PWM_LL(uint16_t extDuty, uint16_t intDuty, uint32_t DEAD_time_count, uint8_t Tims_On);
void FillArgument_PWM_HL(uint16_t extDuty, uint32_t DEAD_time_count, uint8_t Tims_On);

uint32_t FillArgument_PWM_HL_Test(void* Array, uint16_t DUTY_Cycle_1, uint32_t DEAD_time_percent, uint32_t start_step, uint32_t all_step, uint32_t one_array_steps);
void FillArray_Test(void* Array, uint8_t Type_of_Array, uint16_t Array_Size, uint16_t Precision);
//*********************************************************************************************************//
#endif /* TIM_H_ */
