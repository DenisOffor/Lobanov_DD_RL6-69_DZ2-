/*
 * FLASH.h
 *
 *  Created on: 11 мая 2023 г.
 *      Author: denlo
 */

#ifndef FLASH_H_
#define FLASH_H_

#define PAGE_FOR_TRANSFER (uint32_t)0x0800F800  //page 62
#define PAGE_FOR_RECEIVE (uint32_t)0x0800FC00   //page 63

#include "stm32f0xx.h"

void Set_protection_of_flash();
void Flash_unlock();
void Erase_Page(uint32_t Page_for_erase);
void Write_data_to_flash(uint32_t Page_adress, uint8_t* data, uint16_t data_size);
void ReadFromFlash(uint32_t Page_adress, uint8_t* data, uint16_t data_size);


#endif /* FLASH_H_ */
