#include "spi_eeprom.h"
//***************************************************************************************************************//
void Delay(uint32_t nTime)
{
	volatile	int j = 0;
	for(int i =0; i<nTime;i++)
	{
		j+=2;
	}	
}

//***************************************************************************************************************//
//***************************************************************************************************************//
void CMSIS_SPI1_Init(void) 
{
		if((RCC->APB2ENR & RCC_APB2ENR_SPI1EN) == 0)
		{
			RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; 
		}
		
		SPI1-> CR1 &=			 ~( SPI_CR1_SPE ) ; // Убедитесь, что периферийное устройство выключено, и перезагрузите его.
		RCC-> APB2RSTR |=   ( RCC_APB2RSTR_SPI1RST ) ;
		RCC-> APB2RSTR &=	 ~( RCC_APB2RSTR_SPI1RST ) ;
	
		SPI1->CR1 &= ~SPI_CR1_BIDIMODE;//если этот бит установлен, то прием и передача данных осуществляются по одному проводу (не считая SCK)
		SPI1->CR1 &= ~SPI_CR1_CRCEN;//аппаратный модуль расчёт контрольной суммы. 0 - выкл, 1 - вкл
		SPI1->CR1 &= ~SPI_CR1_DFF;//если бит сброшен, то SPI модуль передает/принимает данные по 8 бит, в противном случаее передается/принимается сразу по 16 бит
		SPI1->CR1 &= ~SPI_CR1_LSBFIRST;//0 - сначала передается старший бит 
		
		SPI1-> CR1 &=  ~( SPI_CR1_CPOL |	SPI_CR1_CPHA ) ;// Установка полярности и фазы.
		SPI1-> CR1 |=   ( SPI_CR1_MSTR ) ;// Установить STM32 для работы в качестве хост-устройства.
		SPI1-> CR1 |=	  ( SPI_CR1_SSM )  ;// Установить программный пин-код Chip Select.
		SPI1-> CR1 |=   ( SPI_CR1_SSI )  ;// Установить внутренний сигнал «Chip Select» в высокий уровень.
		SPI1-> CR1 |=   ( SPI_CR1_BR  )  ;// Задают скорость приема/передачи (частоту SCK).
//		SPI1-> CR1 &=  ~( SPI_CR1_BR_0  )  ;
		
		SPI1-> CR2 &=  ~( SPI_CR2_SSOE  );//Если бит выставлен, то SPI модуль (в режиме мастера разумеется) сам управляет выводом NSS
		
		SPI1-> CR2 &=  ~( SPI_CR2_TXDMAEN );//разрешает/запрещает запрос DMA по завершению передачи
		SPI1-> CR2 &=  ~( SPI_CR2_RXDMAEN );//разрешает/запрещает запрос DMA по завершению приема
		
		SPI1-> CR1 |=   ( SPI_CR1_SPE )  ;// Включить периферию.
}

void CMSIS_GPIO_for_SPI1(void) 
{
	//PB3 _SPI_SCK
	//PB4 _SPI_MISO
	//PB5 _SPI_MOSI
	//PA15_SPI_CS
		if((RCC->AHB1ENR & RCC_AHB1ENR_GPIOAEN) == 0)
		{
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; 
		}
		if((RCC->AHB1ENR & RCC_AHB1ENR_GPIOBEN) == 0)
		{
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; 
		}
		//GPIOA->AFR[1] |= (0x05<<7*4);//AF5 PA15
		GPIOB->AFR[0] |= (0x05<<3*4);//AF5 PB3 
		GPIOB->AFR[0] |= (0x05<<4*4);//AF5 PB4
		GPIOB->AFR[0] |= (0x05<<5*4);//AF5 PB5
			
		GPIOA-> MODER  &= ~ ( (0x3<<(PA_CS * 2)) );																								// Предварительная очистка
		GPIOB-> MODER  &= ~ ( (0x3<<(PB_MOSI * 2)) | (0x3<<(PB_SCK * 2)) | (0x3<<(PB_MISO * 2)) );// Предварительная очистка
		//GPIOA-> MODER  |= 	( (0x2<<(PA_CS * 2)) );//CS__AF_SPI
		GPIOA-> MODER  |= 	( (0x1<<(PA_CS * 2)) );//CS__Output
		GPIOB-> MODER  |= 	( (0x2<<(PB_MOSI * 2)) | (0x2<<(PB_SCK * 2)) | (0x2<<(PB_MISO * 2)) );// Установите контакты MOSI и SCK в альтернативный режим работы 0.
		GPIOA-> OTYPER &= ~ ( (0x1<< PA_CS ) );																										// Тип вывода: двухтактный
		GPIOB-> OTYPER &= ~ ( (0x1<< PB_MOSI ) 	   | (0x1<< PB_SCK    )	 | (0x1<< PB_MISO    )  );// Тип вывода: двухтактный
		GPIOA-> OSPEEDR|=   ( (0x3<<(PA_CS * 2)) );
		GPIOB-> OSPEEDR|=   ( (0x3<<(PB_MOSI * 2)) | (0x3<<(PB_SCK * 2)) | (0x3<<(PB_MISO * 2)) );
		GPIOA-> PUPDR &= ~  ( (0x3<<(PA_CS * 2)) );																								// Очистка
		GPIOB-> PUPDR &= ~  ( (0x3<<(PB_MOSI * 2)) | (0x3<<(PB_SCK * 2)) | (0x3<<(PB_MISO * 2)) );// Очистка
}

void CMSIS_SPI1_Send_8  (uint8_t  dat)
{   
  while ((SPI1->SR & SPI_SR_TXE)==0) {}; // Ждем TXE.
  *(uint8_t*)&(SPI1->DR ) = dat;// Отправить байт.
}

uint8_t CMSIS_SPI1_Read_8  (void)
{   
	while(!(SPI1->SR & SPI_SR_RXNE));//ждём пока придёт ответ     
	uint8_t Data = SPI1->DR;
	return Data;//считываем полученные данные
}  

void CMSIS_SPI1_Send_16 (uint16_t dat)
{   
  CMSIS_SPI1_Send_8((uint8_t)((dat>>8)&0xFF));
	CMSIS_SPI1_Send_8((uint8_t)((dat)&0xFF));
}
//***************************************************************************************************************//
void EEPROM_CS_Level(uint8_t Level)
{
	if(Level == 1)
	{
		GPIOA->BSRR = (1UL << (PA_CS & 0x0F));
	}
	else
	{
		GPIOA->BSRR = (1UL << ((PA_CS & 0x0F)+0x10) );
	}
}

void EEPROM_Write_Instr(enum EEPROM_Instr Instruct)
{
	CMSIS_SPI1_Send_8(Instruct);
}

void EEPROM_Write_Unlock(void)
{
	EEPROM_CS_Level(0);
	EEPROM_Write_Instr(WREN);
	
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
		
	while ((SPI1->SR  & SPI_SR_BSY)) {};	
	while (!(SPI1->SR & SPI_SR_RXNE)){};
	while (!(SPI1->SR & SPI_SR_TXE)) {};
		
	Delay(20);
	EEPROM_CS_Level(1);
}

void EEPROM_Write_Lock(void)
{
	EEPROM_CS_Level(0);
	EEPROM_Write_Instr(WRDI);
	
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	
	while ((SPI1->SR  & SPI_SR_BSY)) {};	
	while (!(SPI1->SR & SPI_SR_RXNE)){};
	while (!(SPI1->SR & SPI_SR_TXE)) {};
	
	EEPROM_CS_Level(1);
}

void EEPROM_Init(void)
{
	CMSIS_GPIO_for_SPI1();
	EEPROM_CS_Level(1);
	CMSIS_SPI1_Init();
}

uint8_t EEPROM_Read_Status_Reg(void)
{
	EEPROM_CS_Level(0);
	EEPROM_Write_Instr(RDSR);
	
	EEPROM_Write_Instr(NULL8);
	
	uint8_t StatusReg = CMSIS_SPI1_Read_8();
		
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();	
	__NOP();
	
	while ((SPI1->SR & SPI_SR_BSY)) {};
	while (!(SPI1->SR & SPI_SR_RXNE)) {};
	Delay(20);
	EEPROM_CS_Level(1);
		
	//return StatusReg;
	return SPI1->DR;
}

uint8_t EEPROM_Ready(void)
{
	uint8_t StatusReg = EEPROM_Read_Status_Reg();
	if( (StatusReg&0x1) ) // если бит в 1, неготов
	{
		return 0;
	}
	else if( !(StatusReg&0x1) ) // если бит в 0, готов
	{
		return 1;
	}
	return 0;
}

//void EEPROM_Write_Data(const void * Src, uint16_t Address, uint32_t Length)
//{
//	EEPROM_Write_Unlock();
//	Delay(100);
//	EEPROM_CS_Level(0);
//	
//	EEPROM_Write_Instr(WRITE);
//	CMSIS_SPI1_Send_16(Address);
//	
//	uint8_t * S = (uint8_t *)Src; 
//	uint32_t  L = (Length / sizeof(uint8_t)) ;
//		
//	for(uint32_t i = 0; i < L; i++)
//	{
//			CMSIS_SPI1_Send_8(S[i]);
//	}
//	while ((SPI1->SR  & SPI_SR_BSY)) {};
//	while (!(SPI1->SR & SPI_SR_RXNE)){};
//	while (!(SPI1->SR & SPI_SR_TXE)) {};
//	
//	Delay(20);
//	EEPROM_CS_Level(1);
//}

void EEPROM_Write_Data(const void * Src, uint16_t Address, uint32_t Length)
{
	uint8_t * S = (uint8_t *)Src; 
	uint32_t  L = (Length / sizeof(uint8_t)) ;
	
	for(uint32_t i = 0; i < L; i++)
	{
			EEPROM_Write_Unlock();
			Delay(100);
			EEPROM_CS_Level(0);
			
			EEPROM_Write_Instr(WRITE);
			CMSIS_SPI1_Send_16(Address +i);
			CMSIS_SPI1_Send_8(S[i]);
			
			while ((SPI1->SR  & SPI_SR_BSY)) {};
			while (!(SPI1->SR & SPI_SR_RXNE)){};
			while (!(SPI1->SR & SPI_SR_TXE)) {};
			
			Delay(20);
			EEPROM_CS_Level(1);
			
			Delay(200);		
			while(!EEPROM_Ready()){__NOP();__NOP();__NOP();Delay(200);};
			Delay(200);
	}
}

//void EEPROM_Write_Data(const void * Src, uint16_t Address, uint32_t Length)
//{
//	uint8_t * S 	= (uint8_t *)Src; 
//	uint32_t  L 	= (Length / sizeof(uint8_t)) ;
//	uint32_t  L_p = (Length / 64);
//	
//	for(uint32_t i = 0; i < L; i++)
//	{
//		
//	}
//}

void EEPROM_Clear_Data(void)
{
	uint32_t  L = (0x7FFF) ;
	
	for(uint32_t i = 0; i < L; i++)
	{
			EEPROM_Write_Unlock();
			Delay(100);
			EEPROM_CS_Level(0);
			
			EEPROM_Write_Instr(WRITE);
			CMSIS_SPI1_Send_16(0+i);
				
			CMSIS_SPI1_Send_8(0xFF);
			
			while ((SPI1->SR  & SPI_SR_BSY)) {};
			while (!(SPI1->SR & SPI_SR_RXNE)){};
			while (!(SPI1->SR & SPI_SR_TXE)) {};
			
			Delay(20);
			EEPROM_CS_Level(1);
			Delay(300);
	}
}

void EEPROM_Read_Data(const void * Src, uint16_t Address, uint32_t Length)
{
	EEPROM_CS_Level(0);
	
	EEPROM_Write_Instr(READ);
	CMSIS_SPI1_Send_16(Address);
		
	uint8_t * S = (uint8_t *)Src;
	uint32_t  L = (Length / sizeof(uint8_t)) ;
	
	while ((SPI1->SR  & SPI_SR_BSY)) {__NOP();};
	S[0] = CMSIS_SPI1_Read_8();

	for (uint32_t i = 0; i < L; i++)
	{
		EEPROM_Write_Instr(NULL8);
		S[i] = CMSIS_SPI1_Read_8();
	};
	
	while (!(SPI1->SR & SPI_SR_TXE)) {__NOP();};
	while ((SPI1->SR  & SPI_SR_BSY)) {__NOP();};
	
	Delay(20);
	EEPROM_CS_Level(1);
}

//***************************************************************************************************************//
//***************************************************************************************************************//
