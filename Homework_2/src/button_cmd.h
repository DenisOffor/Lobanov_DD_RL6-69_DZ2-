/*
 * button.h
 *
 *  Created on: 9 мая 2023 г.
 *      Author: denlo
 */

#ifndef BUTTON_CMD_H_
#define BUTTON_CMD_H_

#include "stm32f0xx.h"
#include "button_init.h"
#include "FLASH.h"

//interrupts:
void EXTI0_1_IRQHandler(void);
void TIM6_DAC_IRQHandler(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM14_IRQHandler(void);

//function
//turn on or turn off all timers for button functioning
void Timers_turn_on();
void Timers_turn_off();
//restart button variables in order to get new press
void Button_reset_to_be_ready_for_work();
//contain all functions for initialization
void Button_tune();
//each 1 second check how many press was done
void check_button_press();
//turn on or turn off leds (just for visual control of program)
void Turn_on_Led_PC8();
void Turn_off_Led_PC8();
void Turn_on_Led_PC9();
void Turn_off_Led_PC9();

#endif /* BUTTON_CMD_H_ */
