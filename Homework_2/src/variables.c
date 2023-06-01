/*
 * variables.c
 *
 *  Created on: 14 мая 2023 г.
 *      Author: denlo
 */

#include "variables.h"

ProgramStates program_state = WAITING_FOR_ACTION;
ProgramStates receive_state = WAITING_FOR_ACTION;
ProgramStates transfer_state = WAITING_FOR_ACTION;

uint8_t count_of_blink = 0; //variable to count amount of blink led when appropriate command was got
