/*
 * serial_receive_and_paralell_transfer.h
 *
 *  Created on: 14 мая 2023 г.
 *      Author: denlo
 */

#ifndef SERIAL_RECEIVE_AND_PARALELL_TRANSFER_H_
#define SERIAL_RECEIVE_AND_PARALELL_TRANSFER_H_

#define AMOUNT_RECEIVE_DATA 8
#include <Periphery_for_transfer_and_receive_init.h>
#include "button_cmd.h"
#include "USART_DMA_setup.h"

#ifdef SER_TRANSFER_AND_PAR_RECEIVE
//timer 15 for generate clock for serial data transfer
void TIM15_IRQHandler(void);
#endif
//falling front of clock generate interrupt in EXTI4 -> flag for read data from GPIO is set here
void EXTI4_15_IRQHandler(void);
//PA2 - signal en from this MK, PA3 - sig ready from other MK. In interrupt EXTI2 flag for transfer data is set
void EXTI2_3_IRQHandler(void);

//functions
//init buffer and write their to flash
void Setup_flash_with_data();
//initialize all peripheries needed to work of program
void Tune_peripheries();
//write data to flash using FLASH.h and FLASH.c
void write_to_flash();
//start program to depend upon which action is chosen
void manage_states();
//during receive data check if first 16 bit was key for blink led on my MK
void check_key_for_blink_led();
//if key is received then blink led 3 times
void blink_led_3_times();
//here actions for transfer 8 bit, receive bit and catch sig ready from other MK
void process_cmd();

#ifdef PAR_TRANSFER_AND_SERIAL_RECEIVE
//retrieve button to beginning state
void Restart_work_after_transfer();
//receive 1 bit from other MK
void receive_bit();
//perform actions if bytes of data are received
void check_byte_receive_complete();
//check if buf with received data is full
void check_receive_complete();
//transfer 8 bits to other MK
void set_data_on_GPIO();
#endif

#ifdef SER_TRANSFER_AND_PAR_RECEIVE
//retrieve button to beginning state
void Restart_work_after_transfer();
//transfer 1 bit on other MK
void transfer_bit();
//perform actions if bytes of data are transmitted
void check_byte_transfer_complete();
//receive 8 bits from other MK
void receive_8bit();
//if byte are received -> write in buf ->start to receive new byte of data
void check_byte_receive_complete();
//if all of data are received -> restart work of button
void check_receive_complete();
//if button was pressed 2 times in setup mode -> transfer key for blink led on other mk
void blink_led_on_other_mk();
#endif


#endif /* SERIAL_RECEIVE_AND_PARALELL_TRANSFER_H_ */
