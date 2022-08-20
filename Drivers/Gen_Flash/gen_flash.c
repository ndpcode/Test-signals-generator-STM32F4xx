#include "gen_flash.h"
//************************************
// 				Library FLASH for
//						STM32f407
//************************************
typedef struct
{
    uint32_t From;
    uint32_t To;
   
    uint8_t Sector;
} TSector;

static const TSector Sectors[] = 
{																				// 1 MB памяти
    { 0x08000000UL, 0x08003FFFUL,  0 }, // 16 kB
    { 0x08004000UL, 0x08007FFFUL,  1 }, // 16 kB
    { 0x08008000UL, 0x0800BFFFUL,  2 }, // 16 kB
    { 0x0800C000UL, 0x0800FFFFUL,  3 }, // 16 kB
    { 0x08010000UL, 0x0801FFFFUL,  4 }, // 64 kB
    { 0x08020000UL, 0x0803FFFFUL,  5 }, // 128 kB
    { 0x08040000UL, 0x0805FFFFUL,  6 }, // 128 kB
    { 0x08060000UL, 0x0807FFFFUL,  7 }, // 128 kB
    { 0x08080000UL, 0x0809FFFFUL,  8 }, // 128 kB
    { 0x080A0000UL, 0x080BFFFFUL,  9 }, // 128 kB
    { 0x080C0000UL, 0x080DFFFFUL, 10 }, // 128 kB
    { 0x080E0000UL, 0x080FFFFFUL, 11 }, // 128 kB
};

static const int SectorCount = sizeof(Sectors) / sizeof(Sectors[0]);
//-----------------------------------------------------------------------------
void FlashUnlock(void)
{
	FLASH->KEYR = 0x45670123;
	FLASH->KEYR = 0xCDEF89AB;
}

void FlashLock(void)
{
	FLASH->CR |= FLASH_CR_LOCK;
}

void FlashInit(void)
{   
    // Настроим количество ячеек, стираемых за раз (зависит от напряжения)
    // PSIZE(1:0) = 0b10 (32 bits)
		// 0b00 - 1 байт
		// 0b01 - 2 байта
		// 0b10 - 4 байта
		// 0b11 - 8 байт
    uint32_t Temp;
    FlashUnlock();
   
    Temp = FLASH->CR;
    Temp &= ~(FLASH_CR_PSIZE_0 | FLASH_CR_PSIZE_1);
    Temp |=  FLASH_CR_PSIZE_1;
    FLASH->CR = Temp;
   
    FlashLock();
}

uint8_t FlashReady(void)
{
	return !(FLASH->SR & FLASH_SR_BSY);
}

void FlashEraseSector(uint8_t Index)
{
	if(Index >= SectorCount) return;
	FlashUnlock();
	while(FLASH->SR & FLASH_SR_BSY) {}// Ждём готовности
	
	{
			uint32_t Temp = FLASH->CR;	// Выбираем номер сектора (SNB) и флаг, что хотим чистить (SER)
 
			Temp &= ~(0xFUL << 3); // FLASH_CR_SNB [3:6]
			Temp |=  (Index << 3);
			Temp |= FLASH_CR_SER;
			FLASH->CR = Temp;
	}    
	FLASH->CR |= FLASH_CR_STRT;// Запуск!
	while(FLASH->SR & FLASH_SR_BSY) {}// Ждём завершения операций с флешем
	FLASH->CR &= ~FLASH_CR_SER;// Сбрасываем флаг, что хотим чистить сектор
	FlashLock();
}
void FlashEraseAllPage(void) 
{
	FLASH->CR |= FLASH_CR_MER; 			//Устанавливаем бит стирания ВСЕХ страниц
	FLASH->CR |= FLASH_CR_STRT; 		//Начать стирание
	while(!FlashReady());
	FLASH->CR &= FLASH_CR_MER;
}

void FlashWriteData(const void * Src, void * Address, uint32_t Length)
{
    FlashUnlock();
    while(FLASH->SR & FLASH_SR_BSY) {}// Ждём готовности
    FLASH->CR |= FLASH_CR_PG;// Ставим флаг, что хотим программировать (PG)
   
    // Пишем по 4 байта за раз, так кк выбрали такой способ доступа
    {
        uint32_t * S = (uint32_t *)Src;
        uint32_t * D = (uint32_t *)Address; 
        uint32_t   L = (Length / sizeof(uint32_t)) ;
        uint32_t   i;
       
        for(i = 0; i < L; i++)
        {
            D[i] = S[i];
            while(FLASH->SR & FLASH_SR_BSY) {}// Ждём завершения операций с флешем
        }
    }
   
    FLASH->CR &= ~FLASH_CR_PG;// Сбрасываем флаг, что хотим программировать (PG)
    FlashLock();
}

//void FlashWriteData(uint32_t *byte_array, uint8_t size, uint32_t flash_base_addr)
//{
//	uint8_t i = 0;
//	FlashUnlock();
//	while(FLASH->SR & FLASH_SR_BSY) {}// Ждём готовности
//	if (FLASH->SR & FLASH_SR_EOP) FLASH->SR |= FLASH_SR_EOP;
//	FLASH->CR |= FLASH_CR_PG;// Ставим флаг, что хотим программировать (PG)
//	

//	for ( i = 0; i < size; i ++ )
//	{

//		*(volatile uint32_t*)(flash_base_addr + i) = (uint32_t)( byte_array[i] );

//		while ( !( FLASH->SR & FLASH_SR_EOP ) );
//		FLASH->SR |= FLASH_SR_EOP;
//	};
//	

//	FLASH->CR &= ~(FLASH_CR_PG);
//	FlashLock();
//}

uint32_t FlashReadData(uint32_t *byte_array, uint8_t size, uint32_t flash_base_addr)
{
	uint8_t i = 0;

//	if ( size == 0 ) return 0;
//	if ( ( size % 2 ) != 0 ) return 0;

	for ( i = 0; i < size; i ++/*= 2*/ )
	{
		byte_array[i] = ( *(volatile uint32_t*)(flash_base_addr+(i*sizeof(uint32_t))) ) ;
//		byte_array[i+1] = ( ( *(volatile uint16_t*)(flash_base_addr+i) ) & 0xFF00 ) >> 8;		
	};
	return 1;	
}
void flash_Read(void * Dest, const void * Address, uint32_t Length)
{
    memcpy(Dest, Address, Length);
}

