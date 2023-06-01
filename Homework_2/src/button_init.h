/*
 * button_init.h
 *
 *  Created on: 15 мая 2023 г.
 *      Author: denlo
 */

#ifndef BUTTON_INIT_H_
#define BUTTON_INIT_H_

#include "variables.h"

typedef enum ButtonStates {
	WAITING,
	CHECK_AMOUNT_OF_PRESS,
	BUTTON_PRESSED,
	BUTTON_UNPRESSED,
	CONTROL_RATTLE_ON,
	CONTROL_RATTLE_OFF,
} ButtonStates;

typedef struct Variables_for_button_functioning{
ButtonStates button_state;
ButtonStates rattle_check;
ButtonStates press_check;
uint8_t count_short_press;
} Variables_for_button_functioning;

Variables_for_button_functioning button_variables;

//functions
//initialize GPIOA for button on PA0
void init_GPIO_for_Button();
//timer for delay to eliminate rattle of button
void init_TIM6_for_rattle_eliminating();
//timer for long press check (3sec press)
void init_TIM2_for_long_pressure_check();
//Every second check whether the button was pressed or not. If it was, then a choice of subsequent actions occurs
void init_TIM3_for_check_result_of_press();
//timer for blink led in setup mode
void init_TIM14_for_blinkLed();

#endif /* BUTTON_INIT_H_ */
