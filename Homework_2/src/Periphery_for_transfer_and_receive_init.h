/*
 * Periphery_for_transfer_and_receive_init.h
 *
 *  Created on: 15 мая 2023 г.
 *      Author: denlo
 */

#ifndef PERIPHERY_FOR_TRANSFER_AND_RECEIVE_INIT_H_
#define PERIPHERY_FOR_TRANSFER_AND_RECEIVE_INIT_H_

#include "stm32f0xx.h"

#define PAR_TRANSFER_AND_SERIAL_RECEIVE
//#define SER_TRANSFER_AND_PAR_RECEIVE


#ifdef PAR_TRANSFER_AND_SERIAL_RECEIVE
//PA1 - d_send, PA2 - enable, PA3 - ready, PA4 - clock, PA5 - serial data, PB1-PB9 - par data
void init_GPIO_for_transfer_and_receive_data();
#endif

#ifdef SER_TRANSFER_AND_PAR_RECEIVE
//PA1 - d_send, PA2 - enable, PA3 - ready, PA4 - clock, PA5 - serial data, PB1-PB9 - par data
void init_GPIO_for_transfer_and_receive_data();
//timer15 init for generate clock signal
void init_TIM15_for_clock_sig();
#endif

#endif /* PERIPHERY_FOR_TRANSFER_AND_RECEIVE_INIT_H_ */
