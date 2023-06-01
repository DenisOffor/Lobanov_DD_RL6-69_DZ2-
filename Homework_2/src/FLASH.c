/*
 * FLASH.c
 *
 *  Created on: 11 мая 2023 г.
 *      Author: denlo
 */


#include "FLASH.h"

void Set_protection_of_flash() {
	FLASH->CR |= FLASH_CR_LOCK;
}

void Flash_unlock() {
	while( (FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY); //check if flash is busy
	//unlock flash if it was locked
	if( (FLASH->CR & FLASH_CR_LOCK ) == FLASH_CR_LOCK) {
		FLASH->KEYR = FLASH_KEY1;
		FLASH->KEYR = FLASH_KEY2;
	}
}

void Erase_Page(uint32_t Page_for_erase) {
	Flash_unlock();

	//check if flash is busy
	while( (FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);
	//page erase register
	FLASH->CR |= FLASH_CR_PER;
	//set address of page to erase
	FLASH->AR = Page_for_erase;
	//start erase
	FLASH->CR |= FLASH_CR_STRT;
	//wait while page is erasing
	while( (FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);
	//reset bit "end of operation"
	if( (FLASH->SR & FLASH_SR_EOP) == FLASH_SR_EOP )
		FLASH->SR |= FLASH_SR_EOP;
	//reset page erase register
	FLASH->CR &= ~FLASH_CR_PER;

	Set_protection_of_flash();
}

void Write_data_to_flash(uint32_t Page_adress, uint8_t* data, uint16_t data_size) {
	Erase_Page(Page_adress);
	Flash_unlock();

	//set programming bit
	FLASH->CR |= FLASH_CR_PG;
	//write data to the flash address, in our case - 1064 byte (1 page)
	uint16_t data_for_write = 0;
	for (int i = 0; i < data_size / 2; i++) {
		data_for_write = 0;
		data_for_write |= data[i * 2];
		data_for_write |= ((data[i * 2 + 1]) << 8);
	     *(__IO uint16_t*)(Page_adress + i*2) = data_for_write;
	 	while( (FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);
	}
    //wait while data is writing
	//reset bit "end of operation"
	if( (FLASH->SR & FLASH_SR_EOP) == FLASH_SR_EOP )
		FLASH->SR |= FLASH_SR_EOP;
	//reset programming bit
	FLASH->CR &= ~FLASH_CR_PG;

	Set_protection_of_flash();
}

void ReadFromFlash(uint32_t Page_adress, uint8_t* data, uint16_t data_size){
	uint16_t data_for_read = 0;
	for(int i = 0; i < data_size / 2; i++) {
	  data_for_read = *(uint16_t*)(Page_adress + i*2);
      while( (FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY);
      data[i * 2] = data_for_read & 0x00FF;
      data[i * 2 + 1] = (data_for_read >> 8) & 0x00FF;
	}
}

