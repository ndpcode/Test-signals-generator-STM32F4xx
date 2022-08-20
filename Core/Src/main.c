//****************************************************************************//
//The generator 407 Firmware project
//main source file
//Created 09.08.2018
//Created by Novikov Dmitry
//****************************************************************************//

//****************************************************************************//
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "rtc.h"
#include "hal_gpio.h"
#include "display12864.h"
#include "bsc_controls.h"
#include "bsc_tree_menu.h"
#include "main_menu.h"
#include "ad9850.h"
#include "gen_timers.h"
#include "generator407.h"

//****************************************************************************//
/* Defines and constants -----------------------------------------------------*/
#define GET_RTC_SEC ( (((RTC->TR & RTC_TR_ST) >> RTC_TR_ST_Pos)*10) + ((RTC->TR & RTC_TR_SU) >> RTC_TR_SU_Pos) )
#define GET_RTS_MILLSEC ( (RTC_SYNCH_PRESCALER - RTC->SSR) * 1000/(RTC_SYNCH_PRESCALER + 1) )

//****************************************************************************//
/* variables ---------------------------------------------------------*/
D128X64_DISPLAY* Display128x64;             //main display struct
CONTROLS_EVENTS controlsEvents;             //controls struct
TREE_MENU* mainMenu;                        //main menu struct
SYS_EVENTS_DATA valcoderEvents;             //external valcoder events handler struct
uint8_t valcoderEventsBlock;                //flag - block/unblock change valcoderEvents variable
AD9850* signalSynth1;                       //ad9850 #1
AD9850* signalSynth2;                       //ad9850 #2
signalGeneratorConfig SignalGenerator;      //signal generator config
extern GeneratorTypeDef* 			genStruct;

extern uint16_t _steps;
extern uint16_t _precision;

extern uint16_t arrays_in_genArray;
extern uint16_t burst_wave_1[GEN_CORE_ARRAY_MAX_COUNT*4];
extern uint16_t burst_wave_3[GEN_CORE_ARRAY_MAX_COUNT*4];
//****************************************************************************//
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void displayInit(void);
void ad9850Init(void);
void patternGeneratorInit(void);
void signalGeneratorInit(void);
void controlsInit(void);
SYS_EVENTS_DATA updateControlsEvents(void);
void menuInit(void);
void interfaceUpdate(void);
void sysWaitUS(uint16_t timeUS);
void sysWaitNS(uint16_t timeNS); 


	uint8_t _TEST_VAR_Arr8[10];
	uint8_t _TEST_VAR_out_Arr8[10];
	
	uint32_t TestFillArray[20];
	
	TestVarStruct_ee InputDa;
	TestVarStruct_ee OutputDa;
	
	double test_var_num = 12.889;
	double roundf_Variable = 0;
	double ceil_Variable = 0;
	double floor_Variable = 0;
	
//****************************************************************************//
/* main function -------------------------------------------------------------*/
int main(void)
{
  /* MCU Configuration----------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
	
	/**************************************************************************************************/
	/* crutch */
	FLASH->ACR = 0x00000007;
	
	/* Initialize display */
	displayInit();
	
	/* Initialize AD9850 */
	ad9850Init();
	
	/*Initialize pattern generator */
	//patternGeneratorInit();
	Gen407Init();
	
	/*Initialize signal generator */
	signalGeneratorInit();
	
	/* Initialize controls: buttons, valcoder */
	controlsInit();
	
	/* Initialize menu */
	menuInit();
	
	/* Initialize Memory */
	#ifdef BOARD_MEMORY_FLASH
			FlashInit();
	#endif
	#ifdef BOARD_MEMORY_EEPROM
			EEPROM_Init();
	#endif
	
//	InputDa.Dat_1_8 =0x10111111;
//	InputDa.Dat_2_16=0x20111111;
//	InputDa.Dat_3_32=0x30111111;
//	InputDa.Dat_4_32=0x40111111;
//	InputDa.Dat_5_32=0x50111111;
//	InputDa.Dat_6_32=0x60111111;
//	InputDa.Dat_7_32=0x70111111;
//	InputDa.Dat_8_32=0x80111111;
//	
//	InputDa.Dat_9_32 =0x10222222;
//	InputDa.Dat_10_32=0x20222222;
//	InputDa.Dat_11_32=0x30222222;
//	InputDa.Dat_12_32=0x40222222;
//	InputDa.Dat_13_32=0x50222222;
//	InputDa.Dat_14_32=0x60222222;
//	InputDa.Dat_15_32=0x70222222;
//	InputDa.Dat_16_32=0x80222222;
//	
//	InputDa.Dat_17_32=0x10333333;
//	InputDa.Dat_18_32=0x20333333;
//	InputDa.Dat_19_32=0x30333333;
//	InputDa.Dat_20_32=0x40333333;
//	InputDa.Dat_21_32=0x50333333;
//	InputDa.Dat_22_32=0x60333333;
//	InputDa.Dat_23_32=0x70333333;
//	InputDa.Dat_24_32=0x80333333;
	
//	HAL_Delay(1);
//	EEPROM_Write_Data((uint8_t*)&InputDa,256, (sizeof(InputDa) / sizeof(uint8_t)) );
//	HAL_Delay(1);
//		
//	EEPROM_Read_Data((uint8_t*)&OutputDa, 256, (sizeof(InputDa) / sizeof(uint8_t)));
//	HAL_Delay(1);
	
	
//	FillArray_Test((void*)TestFillArray, 32, 20, 16);
	
	
//	Gen407TestSignal_Burst();

//SimpleGeneratorInit();
//SimpleGeneratorOn(1000, 0, 70);





//			Init_GPIO_TIM1_THREE_PHASE();
//			StepsAndPrecisionValue		(300, 30000, 9, 0, &_steps, &_precision);
//			FillArgument_THREE_PHASE	(0);
//			Init_TIM_THREE_PHASE			(_precision);		
//			Init_DMA_TIM_THREE_PHASE	(_steps);
//			ThreePhaseDeadTime				(0);
//			TIM1->CR1|= TIM_CR1_CEN;
//			DMA2_Stream5->CR |=  DMA_SxCR_EN;



//	 roundf_Variable = roundf(test_var_num);	//okruglenie
//	 ceil_Variable = ceil(test_var_num);			//in big
//	 floor_Variable = floor(test_var_num);		//in small
	 
	 
	
//	 DAC_Init();
//	 DAC_Duty_Set(297, 500);
	 
//	 for(int i =0; i<5000; i++)
//	 {
//		 for(int i =0; i<5000; i++)
//			{
//				__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//				__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//			}
//		 if(i%2==1)
//		 {
//			 DAC_Duty_Set(100, 100);__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//			 DAC_Duty_Set(200, 100);__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//			 DAC_Duty_Set(300, 100);__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//			 DAC_Duty_Set(400, 100);__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//			 DAC_Duty_Set(500, 100);__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//			 DAC_Duty_Set(600, 100);__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//			 DAC_Duty_Set(700, 100);__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//			 DAC_Duty_Set(800, 100);__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//			 DAC_Duty_Set(900, 100);__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//			 DAC_Duty_Set(1000, 500);__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
//		 }
//		 else
//		 {
//				DAC_Duty_Set(100, 1000);
//		 }
//	 }
			
//*************************************TEST_MULTI_Array_MODE_Generation*********************************************************
//			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
//			GPIOC->MODER  |=  GPIO_MODER_MODER8_0; GPIOC->MODER  &= ~GPIO_MODER_MODER8_1;   GPIOC->BSRR |=  0x00000100;
//			
//			genStruct->generatorStartFlag = 1;
//			genStruct->selectedBuffer=1;
//			genStruct->currentConfig.extDuty=500;
//			
//			Init_GPIO_TIM_All();
//			//StepsAndPrecision					(param->extFreq, param->intFreq, param->modePWM, param->countdown, &_steps, &_precision);
//			_steps = GEN_CORE_ARRAY_LENGTH_Virtual;
//			_precision = 84000000/GEN_CORE_ARRAY_LENGTH_Virtual;
//			BurstArraySelect					(1);
//			arrays_in_genArray = 			(uint16_t)ceil((double)_steps / GEN_CORE_ARRAY_MAX_COUNT);
//			FillArgument_PWM_HL_Test	((void*)burst_wave_1, 500, 0, 												 0, _steps*4, GEN_CORE_ARRAY_MAX_COUNT*4);
//			FillArgument_PWM_HL_Test	((void*)burst_wave_3, 500, 0, GEN_CORE_ARRAY_MAX_COUNT*4, _steps*4, GEN_CORE_ARRAY_MAX_COUNT*4);
//			StartGenerator						(GEN_CORE_ARRAY_MAX_COUNT, _precision, 1);
//******************************************************************************************************************************

  /* Infinite loop */
  while (1)
  {
		/* interface, menu and contols update */
		interfaceUpdate();
  }
}
//****************************************************************************//

//****************************************************************************//
/* System Clock Configuration ------------------------------------------------*/
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_HSE_DIV16;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Enables the Clock Security System 
    */
  HAL_RCC_EnableCSS();

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
//****************************************************************************//

//****************************************************************************//
/* MT12864 display init ------------------------------------------------------*/
void displayInit(void)
{
		D128X64_RESULT displayResult = 0;
		Display128x64 = 0;
	
	#ifdef BOARD_DISPLAY_MT12864
		//edit display port config
		uint8_t displayPortConfig[MT12864_DISPLAY_PIN_COUNT][2] =
		{
				'D', 0,  // MT12864_HAL_PIN_DATA_0
				'E', 3,  // MT12864_HAL_PIN_DATA_1,
				'D', 1,  // MT12864_HAL_PIN_DATA_2,
				'E', 4,  // MT12864_HAL_PIN_DATA_3,
				'D', 2,  // MT12864_HAL_PIN_DATA_4,
				'E', 5,  // MT12864_HAL_PIN_DATA_5,
				'D', 3,  // MT12864_HAL_PIN_DATA_6,
				'E', 6,  // MT12864_HAL_PIN_DATA_7,
				'C', 11, // MT12864_HAL_PIN_MODE,
				'B', 7,  // MT12864_HAL_PIN_READ_WRITE,
				'E', 2,  // MT12864_HAL_PIN_ENABLE,
				'E', 15, // MT12864_HAL_PIN_CHIP_1,
				'B', 3,  // MT12864_HAL_PIN_CHIP_2,
				'B', 6   // MT12864_HAL_PIN_RESET
		};		
		displayResult = MT12864DisplayInit((uint8_t*)&displayPortConfig, sysWaitUS, sysWaitNS, &Display128x64);
	#endif
	#ifdef BOARD_DISPLAY_ST7920
		//edit display port config
		uint8_t displayPortConfig[ST7920_DISPLAY_PIN_COUNT][2] =
		{
				'D', 0,  // ST7920_HAL_PIN_DATA_0
				'E', 3,  // ST7920_HAL_PIN_DATA_1,
				'D', 1,  // ST7920_HAL_PIN_DATA_2,
				'E', 4,  // ST7920_HAL_PIN_DATA_3,
				'D', 2,  // ST7920_HAL_PIN_DATA_4,
				'E', 5,  // ST7920_HAL_PIN_DATA_5,
				'D', 3,  // ST7920_HAL_PIN_DATA_6,
				'E', 6,  // ST7920_HAL_PIN_DATA_7,
				'C', 11, // ST7920_HAL_PIN_MODE,
				'B', 7,  // ST7920_HAL_PIN_READ_WRITE,
				'E', 2,  // ST7920_HAL_PIN_ENABLE,
				'E', 15, // ST7920_HAL_PIN_PSB,
				'B', 6   // ST7920_HAL_PIN_RESET
		};		
		displayResult = ST7920DisplayInit((uint8_t*)&displayPortConfig, sysWaitUS, sysWaitNS, &Display128x64);
	#endif
}
//****************************************************************************//

//****************************************************************************//
/* AD9850 init ------------------------------------------------------*/
void ad9850Init(void)
{
		volatile AD9850_RESULT result;
		uint8_t ad9850Synth1PortConfig[AD9850_PIN_COUNT][2] =
		{
				'A', 9,  // AD9850_HAL_PIN_DATA
				'A', 8,  // AD9850_HAL_PIN_W_CLK,
				'C', 9,  // AD9850_HAL_PIN_FQ_UD,
				'A', 10  // AD9850_HAL_PIN_RESET
		};
		uint8_t ad9850Synth2PortConfig[AD9850_PIN_COUNT][2] =
		{
				'D', 5,  // AD9850_HAL_PIN_DATA
				'D', 7,  // AD9850_HAL_PIN_W_CLK,
				'D', 6,  // AD9850_HAL_PIN_FQ_UD,
				'D', 4   // AD9850_HAL_PIN_RESET
		};
		
		signalSynth1 = 0;		
		result = AD9850Init((uint8_t*)&ad9850Synth1PortConfig, AD9850_BITBANDING_IMPLEMENTATION, AD9850_CLOCK, sysWaitNS, &signalSynth1);
		if (result != AD9850_OK) _Error_Handler("main.c : AD9850Init(&signalSynth1)", 0);
		
		signalSynth2 = 0;		
		result = AD9850Init((uint8_t*)&ad9850Synth2PortConfig, AD9850_BITBANDING_IMPLEMENTATION, AD9850_CLOCK, sysWaitNS, &signalSynth2);
		if (result != AD9850_OK) _Error_Handler("main.c : AD9850Init(&signalSynth2)", 0);
		
		result = 0;	
}
//****************************************************************************//

//****************************************************************************//
/* pattern generator init ----------------------------------------------------*/
void patternGeneratorInit(void)
{
	
}
//****************************************************************************//

//****************************************************************************//
/* signal generator init ----------------------------------------------------*/
void signalGeneratorInit(void)
{
		SignalGenerator.signalType = SSIGNAL_SINUS;
		SignalGenerator.freqPWM = 20000;
		SignalGenerator.freqSignal = 50;
		SignalGenerator.powerK = 10000;
		SignalGenerator.centerK = 0;
		SignalGenerator.pwmMinPulseLengthInNS = 50;
		SignalGenerator.pwmDeadTimeInNS = 50;
	
		SignalGenerator.timerPrescaler = GenGetPrescalerValue(SignalGenerator.freqPWM, SignalGenerator.signalType);
		SignalGenerator.timerARR = GenGetARRValueFromFreq(SignalGenerator.timerPrescaler, SignalGenerator.freqPWM, SignalGenerator.signalType);
		SignalGenerator.timerStepsCCR = GenGetStepsCCRValueFromFreq(SignalGenerator.timerPrescaler, SignalGenerator.freqPWM, SignalGenerator.freqSignal, SignalGenerator.timerARR,
                                                          SignalGenerator.powerK, SignalGenerator.pwmMinPulseLengthInNS, SignalGenerator.pwmDeadTimeInNS,
		                                                      SignalGenerator.signalType);
//		GenUpdateSignal(SignalGenerator.timerPrescaler, SignalGenerator.timerARR, SignalGenerator.timerStepsCCR, (double)SignalGenerator.powerK / 100,
//										(double)SignalGenerator.centerK / 100, SignalGenerator.pwmMinPulseLengthInNS, SignalGenerator.pwmDeadTimeInNS,
//										SignalGenerator.signalType);
	
}
//****************************************************************************//

//****************************************************************************//
/* controls init -------------------------------------------------------------*/
void controlsInit(void)
{
		//config ports and controls
		//button - up
		GPIOE_POWER_ON;
		GPIOConfigPinL(E, 7, GPIO_PIN_MODE_INPUT, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_LOW, GPIO_PIN_NOPULLUPDOWN, GPIO_PIN_ALTF_RESET);
		if ( !controlsRegNewButton('E', 7, 0, EVENT_BUTTON_UP_CLICK, 0) ) _Error_Handler("main.c : controlsRegNewButton('E', 7)", 0);
		//button - down
		GPIOConfigPinL(E, 10, GPIO_PIN_MODE_INPUT, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_LOW, GPIO_PIN_NOPULLUPDOWN, GPIO_PIN_ALTF_RESET);
		if ( !controlsRegNewButton('E', 10, 0, EVENT_BUTTON_DOWN_CLICK, 0) ) _Error_Handler("main.c : controlsRegNewButton('E', 10)", 0);
		//button - left
		GPIOC_POWER_ON;
		GPIOConfigPinL(C, 1, GPIO_PIN_MODE_INPUT, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_LOW, GPIO_PIN_NOPULLUPDOWN, GPIO_PIN_ALTF_RESET);
		if ( !controlsRegNewButton('C', 1, 0, EVENT_BUTTON_LEFT_CLICK, 0) ) _Error_Handler("main.c : controlsRegNewButton('C', 1)", 0);
		//button - right
		GPIOA_POWER_ON;
		GPIOConfigPinL(A, 4, GPIO_PIN_MODE_INPUT, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_LOW, GPIO_PIN_NOPULLUPDOWN, GPIO_PIN_ALTF_RESET);
		if ( !controlsRegNewButton('A', 4, 0, EVENT_BUTTON_RIGHT_CLICK, 0) ) _Error_Handler("main.c : controlsRegNewButton('A', 4)", 0);
		//button - ok
		GPIOConfigPinL(E, 8, GPIO_PIN_MODE_INPUT, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_LOW, GPIO_PIN_NOPULLUPDOWN, GPIO_PIN_ALTF_RESET);
		if ( !controlsRegNewButton('E', 8, 0, EVENT_BUTTON_OK_CLICK, 0) ) _Error_Handler("main.c : controlsRegNewButton('E', 8)", 0);
		//button - auto
		GPIOB_POWER_ON;
		GPIOConfigPinL(B, 11, GPIO_PIN_MODE_INPUT, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_LOW, GPIO_PIN_NOPULLUPDOWN, GPIO_PIN_ALTF_RESET);
		if ( !controlsRegNewButton('B', 11, 0, EVENT_BUTTON_AUTO_CLICK, 0) ) _Error_Handler("main.c : controlsRegNewButton('B', 11)", 0);
		//button - f1
		GPIOConfigPinL(B, 9, GPIO_PIN_MODE_INPUT, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_LOW, GPIO_PIN_NOPULLUPDOWN, GPIO_PIN_ALTF_RESET);
		if ( !controlsRegNewButton('B', 9, 0, EVENT_BUTTON_F1_CLICK, 0) ) _Error_Handler("main.c : controlsRegNewButton('B', 9)", 0);
		//button - f2
		GPIOConfigPinL(C, 12, GPIO_PIN_MODE_INPUT, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_LOW, GPIO_PIN_NOPULLUPDOWN, GPIO_PIN_ALTF_RESET);
		if ( !controlsRegNewButton('C', 12, 0, EVENT_BUTTON_F2_CLICK, 0) ) _Error_Handler("main.c : controlsRegNewButton('C', 12)", 0);
		//button - f3
		GPIOConfigPinL(E, 1, GPIO_PIN_MODE_INPUT, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_LOW, GPIO_PIN_NOPULLUPDOWN, GPIO_PIN_ALTF_RESET);
		if ( !controlsRegNewButton('E', 1, 0, EVENT_BUTTON_F3_CLICK, 0) ) _Error_Handler("main.c : controlsRegNewButton('E', 1)", 0);
		//button - f4
		GPIOConfigPinL(C, 0, GPIO_PIN_MODE_INPUT, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_LOW, GPIO_PIN_NOPULLUPDOWN, GPIO_PIN_ALTF_RESET);
		if ( !controlsRegNewButton('C', 0, 0, EVENT_BUTTON_F4_CLICK, 0) ) _Error_Handler("main.c : controlsRegNewButton('C', 0)", 0);
		//button - menu	
		GPIOConfigPinL(E, 12, GPIO_PIN_MODE_INPUT, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_LOW, GPIO_PIN_NOPULLUPDOWN, GPIO_PIN_ALTF_RESET);
		if ( !controlsRegNewButton('E', 12, 0, EVENT_BUTTON_MENU_CLICK, 0) ) _Error_Handler("main.c : controlsRegNewButton('E', 12)", 0);
		//button - valcoder
		GPIOConfigPinL(B, 10, GPIO_PIN_MODE_INPUT, GPIO_PIN_TYPE_RESET, GPIO_PIN_SPEED_LOW, GPIO_PIN_NOPULLUPDOWN, GPIO_PIN_ALTF_RESET);
		if ( !controlsRegNewButton('B', 10, 0, EVENT_VALCODER_BUTTON_CLICK, EVENT_VALCODER_BUTTON_PRESSED) ) _Error_Handler("main.c : controlsRegNewButton('B', 10)", 0);
		//valcoder
#ifdef BOARD_ENCODER_INVERSE
		if ( !controlsRegNewValcoder('B', 2, 'C', 5, 0, EVENT_VALCODER_CCW, 0, EVENT_VALCODER_CW) ) _Error_Handler("main.c : controlsRegNewValcoder()", 0);
#else
		if ( !controlsRegNewValcoder('C', 5, 'B', 2, 0, EVENT_VALCODER_CCW, 0, EVENT_VALCODER_CW) ) _Error_Handler("main.c : controlsRegNewValcoder()", 0);
#endif
		
		//init valcoder poll in interrupt
		valcoderEvents = 0;
		valcoderEventsBlock = 0;
		RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
		// f = 1000 Hz
		TIM6->PSC = 839;
		TIM6->ARR = 400;
		TIM6->CR1 = TIM_CR1_URS;
		TIM6->EGR |= TIM_EGR_UG;
		TIM6->DIER |= TIM_DIER_UIE;
		TIM6->CR1 |= TIM_CR1_CEN | TIM_CR1_OPM;
		NVIC_EnableIRQ(TIM6_DAC_IRQn);		
}
//****************************************************************************//

//****************************************************************************//
/* update buttons events ----------------------------------------------------*/
SYS_EVENTS_DATA updateControlsEvents(void)
{
		//опрос событий
		return controlsUpdateButtons(UPDEVENTS_GETEVENTS | UPDEVENTS_HANDLERS_LAUNCH);
}
//****************************************************************************//

//****************************************************************************//
/* update valcoder events - interrupt ----------------------------------------*/
void TIM6_DAC_IRQHandler()
{
		if (TIM6->SR & TIM_SR_UIF)
		{
				//clear flag
				TIM6->SR &= ~TIM_SR_UIF;
				//processing
				if (!valcoderEventsBlock)
				{
						//update, if null
						if (!valcoderEvents)
						{
								valcoderEvents = controlsUpdateValcoder(UPDEVENTS_GETEVENTS | UPDEVENTS_HANDLERS_LAUNCH);
						}
				}
				//restart timer
				TIM6->CR1 |= TIM_CR1_CEN;
		}
}
//****************************************************************************//

//****************************************************************************//
/* init menu -----------------------------------------------------------------*/
void menuInit(void)
{
		uint16_t menuID;
		// create new menu
		mainMenu = MenuCreate(MENU_DRAW_FPS, MENU_EVENTS_FPS, updateControlsEvents, 0, MENU_TRANS_TIME, Menu1_StartMenu_Draw, Menu1_StartMenu_Events,0);
		if ( !mainMenu ) return;
		//add menu pages
		menuID = MenuAddNextItem(mainMenu, 0, Menu2_MainMenu_Draw, Menu2_MainMenu_Events,0);
	
		//add submenu 1.1 - signal synthesizer
		MenuAddSubItem(mainMenu, menuID, Menu3_SignalSynth_Draw, Menu3_SignalSynth_Events,0);
		//add submenu 1.2 - signal synthesizer #1
		MenuAddNextItem(mainMenu, 0, Menu4_SignalSynth1_Draw, Menu4_SignalSynth1_Events,0);
		//add submenu 1.3 - signal synthesizer #2
		MenuAddNextItem(mainMenu, 0, Menu5_SignalSynth2_Draw, Menu5_SignalSynth2_Events,0);
	
		//add submenu 2.1
		menuID = MenuAddNextItem(mainMenu, 0, Menu6_PatternGenerator_Draw, Menu6_PatternGenerator_Events,0);

		//add submenu 3.1
		menuID = MenuAddNextItem(mainMenu, 0, Menu17_SignalGenerator_Draw, Menu17_SignalGenerator_Events,0);
		
		//add submenu 2.2
		MenuAddNextItem(mainMenu, 0, Menu_6_1_PatternGenerator_Draw, Menu_6_1_PatternGenerator_Events, Menu_6_1_PatternGenerator_InputInit);
		MenuAddNextItem(mainMenu, 0, Menu_6_2_PatternGenerator_Draw, Menu_6_2_PatternGenerator_Events, Menu_6_2_PatternGenerator_InputInit);
		MenuAddNextItem(mainMenu, 0, Menu_6_3_PatternGenerator_Draw, Menu_6_3_PatternGenerator_Events, Menu_6_3_PatternGenerator_InputInit);
		MenuAddNextItem(mainMenu, 0, Menu_6_4_PatternGenerator_Draw, Menu_6_4_PatternGenerator_Events, Menu_6_4_PatternGenerator_InputInit);		
		MenuAddNextItem(mainMenu, 0, Menu_6_5_PatternGenerator_Draw, Menu_6_5_PatternGenerator_Events, Menu_6_5_PatternGenerator_InputInit);	
		MenuAddNextItem(mainMenu, 0, Menu_6_6_PatternGenerator_Draw, Menu_6_6_PatternGenerator_Events, Menu_6_6_PatternGenerator_InputInit);
		MenuAddNextItem(mainMenu, 0, Menu_6_7_PatternGenerator_Draw, Menu_6_7_PatternGenerator_Events, Menu_6_7_PatternGenerator_InputInit);
		MenuAddNextItem(mainMenu, 0, Menu_6_8_PatternGenerator_Draw, Menu_6_8_PatternGenerator_Events, Menu_6_8_PatternGenerator_InputInit);
		MenuAddNextItem(mainMenu, 0, Menu_6_9_PatternGenerator_Draw, Menu_6_9_PatternGenerator_Events, Menu_6_9_PatternGenerator_InputInit);
		
		menuID = MenuAddNextItem(mainMenu, 0, Menu_8_FrequencyMeter_Draw, Menu_8_FrequencyMeter_Events,0);
		
		
		//reset menu global data
		ResetCommonMenuData();
		MenuGoToItemId(mainMenu, Menu2_MainMenu_ID);
}
//****************************************************************************//

//****************************************************************************//
/* update interface ----------------------------------------------------------*/
void interfaceUpdate(void)
{
		MenuUpdate(mainMenu, GET_RTC_SEC, GET_RTS_MILLSEC);
}
//****************************************************************************//

//****************************************************************************//
/* system delay us function --------------------------------------------------*/
void sysWaitUS(uint16_t timeUS)
{
	volatile uint32_t count = timeUS;
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
	TIM7->CR1 = TIM_CR1_OPM | TIM_CR1_URS;
	TIM7->EGR  |= TIM_EGR_UG;
	TIM7->DIER |= TIM_DIER_UIE;
	TIM7->PSC = 83;
	TIM7->ARR = count;
	TIM7->CR1  |= TIM_CR1_CEN;
  while ( (TIM7->CR1 & TIM_CR1_CEN) != 0 );	
}
//****************************************************************************//

//****************************************************************************//
/* system delay ns function --------------------------------------------------*/
void sysWaitNS(uint16_t timeNS)
{
	volatile uint32_t count = (uint32_t)((double)timeNS * 84 / 1000) + 1;
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
	TIM7->CR1 = TIM_CR1_OPM | TIM_CR1_URS;
	TIM7->EGR  |= TIM_EGR_UG;
	TIM7->DIER |= TIM_DIER_UIE;
	TIM7->PSC = 0;
	TIM7->ARR = count;
	TIM7->CR1  |= TIM_CR1_CEN;
  while ( (TIM7->CR1 & TIM_CR1_CEN) != 0 );	
}
//****************************************************************************//

//****************************************************************************//
/* This function is executed in case of error occurrence----------------------*/
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
//****************************************************************************//

//******************************end of main.c*********************************//
