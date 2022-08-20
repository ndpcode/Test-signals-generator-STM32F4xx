
#ifndef GEN_FLASH
#define GEN_FLASH

#include <stdint.h>
#include "stm32f4xx.h"



#define LastPage  			0x800FC00






void FlashUnlock  (void);
void FlashLock	  (void);
void FlashInit		(void);

uint8_t FlashReady(void);
	
void FlashEraseSector(uint8_t Index);
void FlashEraseAllPage	(void) ;

void FlashWriteData(const void * Src, void * Address, uint32_t Length);
//void FlashWriteData(uint32_t *byte_array, uint8_t size, uint32_t flash_base_addr);
//uint8_t FlashReadData (uint8_t *byte_array, uint8_t size, uint32_t flash_base_addr);
uint32_t FlashReadData(uint32_t *byte_array, uint8_t size, uint32_t flash_base_addr);

#endif
