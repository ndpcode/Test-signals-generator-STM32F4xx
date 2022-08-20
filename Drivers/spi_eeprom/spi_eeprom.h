#ifndef SPI_EEPROM_INIT_H
#define SPI_EEPROM_INIT_H
#include "stm32f4xx.h"
#include "stdint.h"
//***************************************************************************************************************//
#define PB_MOSI 	(5)
#define PB_MISO 	(4)//no use
#define PB_SCK 		(3)
#define PA_CS 		(15)
//         WP     3.3v
//***************************************************************************************************************//
enum EEPROM_Instr {
	WREN = 0b00000110,//Set Write Enable Latch				//6
	WRDI = 0b00000100,//Reset Write Enable Latch			//4
	RDSR = 0b00000101,//Read Status Register					//5
	WRSR = 0b00000001,//Write Status Register					//1
	READ = 0b00000011,//Read Data from Memory Array		//3
	WRITE= 0b00000010,//Write Data to Memory Array		//2
	NULL8= 0b00000000
};
//адресса памяти побайтно 0000 – 7FFF


//***************************************************************************************************************//
void CMSIS_SPI1_Init	(void);
void CMSIS_GPIO_for_SPI1(void);
void CMSIS_SPI1_Send_8  (uint8_t  dat);
uint8_t CMSIS_SPI1_Read_8  (void);
void CMSIS_SPI1_Send_16 (uint16_t dat);
void EEPROM_Write_Unlock(void);
//***************************************************************************************************************//
void EEPROM_Init(void);
uint8_t EEPROM_Ready(void);
uint8_t EEPROM_Read_Status_Reg(void);
void EEPROM_Write_Data(const void * Src, uint16_t Address, uint32_t Length);
void EEPROM_Read_Data (const void * Src, uint16_t Address, uint32_t Length);

void EEPROM_Clear_Data(void);

//***************************************************************************************************************//
//***************************************************************************************************************//
#endif
