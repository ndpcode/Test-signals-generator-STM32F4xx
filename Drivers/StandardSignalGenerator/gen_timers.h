//*********************************************************************************************************//
//Standard signals for timer 1 library
//build for stm32f407
//Created 10.04.2017
//Created by Novikov Dmitry
//Header file
//*********************************************************************************************************//

#ifndef __GEN_STANDARD_SIGNALS_H
#define __GEN_STANDARD_SIGNALS_H

//*********************************************************************************************************//
/* includes */

#include <stdint.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include "main.h"
#include "gpio.h"

//*********************************************************************************************************//
/* defines, constants and macro */

/* user: limits for data array */
#define SSIGNAL_MAX_SIGN_STEPS 3000

/* user: frequency */
#define SSIGNAL_TIMER_FREQ 168000000UL //??????? ??
#define SSIGNAL_FREQ_EXT_MIN 0
#define SSIGNAL_FREQ_EXT_MAX 10000000UL
#define SSIGNAL_FREQ_INT_MIN 0
#define SSIGNAL_FREQ_INT_MAX 10000000UL

/* user: signal config */
#define SSIGNAL_MIN_POWER_K 000000 //??????????? ???????? ???????????? ????????, %, div 100
#define SSIGNAL_MAX_POWER_K 170000 //???????????? ???????? ???????????? ????????, %, div 100
#define SSIGNAL_MIN_CENTER_K -50000 //??????????? ???????? ?-?? ???????? ??????, div 100
#define SSIGNAL_MAX_CENTER_K 50000  //???????????? ???????? ?-?? ???????? ??????, div 100
#define SSIGNAL_MIN_DEAD_TIME 0     //??????????? dead time, ??
#define SSIGNAL_MAX_DEAD_TIME 10000 //???????????? dead time, ??
#define SSIGNAL_MIN_PULSE_TIME 10     //??????????? min duty time, ??
#define SSIGNAL_MAX_PULSE_TIME 1000   //???????????? min duty time, ??

/* user: TIMER1 ports config */
#define CH1_SET GPIOE_POWER_ON; GPIOConfigPinL(E, 9, GPIO_PIN_MODE_ALTFUNC, GPIO_PIN_TYPE_PUSHPULL, GPIO_PIN_SPEED_VERYHIGH, GPIO_PIN_NOPULLUPDOWN, 1)
#define CH2_SET GPIOE_POWER_ON; GPIOConfigPinL(E, 11, GPIO_PIN_MODE_OUTPUT, GPIO_PIN_TYPE_PUSHPULL, GPIO_PIN_SPEED_VERYHIGH, GPIO_PIN_NOPULLUPDOWN, 0)
#define CH3_SET GPIOE_POWER_ON; GPIOConfigPinL(E, 13, GPIO_PIN_MODE_ALTFUNC, GPIO_PIN_TYPE_PUSHPULL, GPIO_PIN_SPEED_VERYHIGH, GPIO_PIN_NOPULLUPDOWN, 1)
#define CH4_SET GPIOE_POWER_ON; GPIOConfigPinL(E, 14, GPIO_PIN_MODE_OUTPUT, GPIO_PIN_TYPE_PUSHPULL, GPIO_PIN_SPEED_VERYHIGH, GPIO_PIN_NOPULLUPDOWN, 0)
#define CH1_RESET GPIOConfigPinL(E, 9, GPIO_PIN_MODE_RESET, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_RESET, GPIO_PIN_PULL_RESET, 0)
#define CH2_RESET GPIOConfigPinL(E, 11, GPIO_PIN_MODE_RESET, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_RESET, GPIO_PIN_PULL_RESET, 0)
#define CH3_RESET GPIOConfigPinL(E, 13, GPIO_PIN_MODE_RESET, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_RESET, GPIO_PIN_PULL_RESET, 0)
#define CH4_RESET GPIOConfigPinL(E, 14, GPIO_PIN_MODE_RESET, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_RESET, GPIO_PIN_PULL_RESET, 0)
#define CH1_DOWN GPIOResetPinL(E, 9)
#define CH2_DOWN GPIOResetPinL(E, 11)
#define CH3_DOWN GPIOResetPinL(E, 13)
#define CH4_DOWN GPIOResetPinL(E, 14)
#define CH2_UP GPIOSetPinL(E, 11)
#define CH4_UP GPIOSetPinL(E, 14)

/* signal types */
#define SSIGNAL_SINUS 1
#define SSIGNAL_TRIANGLE 2
#define SSIGNAL_SQUARE 3
#define SSIGNAL_PULSE 4
#define SSIGNAL_INDEX_MIN SSIGNAL_SINUS
#define SSIGNAL_INDEX_MAX SSIGNAL_PULSE

/* type of timer implementation */
#define SSIGNAL_TIMER_INIT_AS_DMA 1
#define SSIGNAL_TIMER_INIT_AS_PWM 2

/* array A and array B defines */
#define SSIGNAL_TIMER_CURRENT_A 0
#define SSIGNAL_TIMER_CURRENT_B 1

/* define - pi */
#define pi		3.1415926535897932384626433832795

//*********************************************************************************************************//
/* struct declarations */

/* generator config struct */
typedef struct 
{
	uint8_t signalType;
	double freqPWM;
	double freqSignal;
	int32_t powerK;
	int32_t centerK;
	int32_t pwmMinPulseLengthInNS;
	int32_t pwmDeadTimeInNS;
	uint16_t timerPrescaler;
	uint16_t timerARR;
	uint16_t timerStepsCCR;
} signalGeneratorConfig;

//*********************************************************************************************************//
/* standard signals main functions *************************************************************************/

uint8_t GenInitTimer1ForSignalEmul(void);
uint8_t GenInitTimer1ForPWM(void);
uint8_t GenInitSignalOnTimer1(uint8_t _signal_type);
uint8_t GenUpdateSignal(const uint16_t _tim_prescaler, const uint16_t _tim_arr, const uint16_t _tim_steps_ccr,
	                      const double _power_k, const double _center_k, const uint32_t _transistorsMinTimeNS,
												const uint32_t _transistorsDeadTimeNS, const uint8_t _signal_type);
uint16_t GenGetPulseWidthByNumber(uint16_t pulseNum);
uint8_t GenSetPulseWidthByNumber(uint16_t pulseNum, uint16_t pulseVal);
uint8_t GenCheckSignalConfig(const int32_t _tim_prescaler, const int32_t _tim_arr, const int32_t _tim_steps_ccr,
	                           const double _power_k, const double _center_k, const uint32_t _transistorsMinTimeNS,
												     const uint32_t _transistorsDeadTimeNS, const uint8_t _signal_type);
uint16_t GenGetPrescalerValue(const double _freq_pwm, const uint8_t _signal_type);
uint16_t GenGetARRValueFromFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const uint8_t _signal_type);
uint16_t GenGetStepsCCRValueFromFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const double _freq_signal,
	                                   const uint16_t _tim_arr, const double _power_k, const uint32_t _transistorsMinTimeNS,
																		 const uint32_t _transistorsDeadTimeNS, const uint8_t _signal_type);
double GenGetPWMFreqValueFromTimer(const uint16_t _tim_prescaler, const uint16_t _tim_arr, const uint8_t _signal_type);
double GenGetSignalFreqValueFromTimer(const uint16_t _tim_prescaler, const uint16_t _tim_arr, const uint16_t _tim_steps_ccr, const uint8_t _signal_type);																		 
uint8_t GenCheckFrequencyAvailable(const double _freq_pwm, const double _freq_signal, const uint8_t _signal_type);
																		 
double GetRealAvailablePWMFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const double _freq_signal, const uint8_t _signal_type);
double GetNextAvailablePWMFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const double _freq_signal, const uint8_t _signal_type);
double GetPrevAvailablePWMFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const double _freq_signal, const uint8_t _signal_type);
double GetRealAvailableSignalFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const double _freq_signal, const uint8_t _signal_type);
double GetNextAvailableSignalFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const double _freq_signal, const uint8_t _signal_type);
double GetPrevAvailableSignalFreq(const uint16_t _tim_prescaler, const double _freq_pwm, const double _freq_signal, const uint8_t _signal_type);

#endif

//*******************************************end of gen_timers.h*******************************************//
