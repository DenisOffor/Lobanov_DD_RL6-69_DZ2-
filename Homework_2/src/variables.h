/*
 * variables.h
 *
 *  Created on: 14 мая 2023 г.
 *      Author: denlo
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_

#include "buffer.h"

#define KEY_FOR_BLINK_LED ('B' + 'L' + 'I' + 'N' + 'K')

typedef enum ProgramStates {
	WAITING_FOR_ACTION,
	SETUP_MODE,
	TRANSFER_DATA,
	TRANSFER_BIT,
	TRANSFER_8_BIT,
	CATCH_SIG_READY,
	WRITE_DATA_INTO_FLASH,
	BLINK_LED_ON_OTHER_MK,
	KEY_TRANSFER,
	BLINK_MY_LED,
	RECEIVE_BIT,
	RECEIVE_8_BIT,
	RECEIVE_COMPLETE,
	PROGRAMMING,
	RECEIVE_DATA_FROM_PC,
	TRANSFER_DATA_TO_PC
} ProgramStates;

ProgramStates program_state;
ProgramStates receive_state;
ProgramStates transfer_state;

//variable to count amount of blink led when appropriate command was got
uint8_t count_of_blink;

uint8_t array_tr[BUFFER_SIZE];
uint8_t array_rec[BUFFER_SIZE];
buf Buf_for_transfer;
buf Buf_For_receive;


#endif /* VARIABLES_H_ */
