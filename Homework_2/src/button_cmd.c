/*
 * button.c
 *
 *  Created on: 9 мая 2023 г.
 *      Author: denlo
 */

#include <button_cmd.h>

void EXTI0_1_IRQHandler(void) {
	if( (EXTI->PR & EXTI_PR_PIF0) == EXTI_PR_PIF0) {
		EXTI->PR |= EXTI_PR_PIF0;
		//if check for RATTLE or program_state = PROGRAMMING -> return
		if(program_state == PROGRAMMING || button_variables.rattle_check == CONTROL_RATTLE_ON)
			return;
		//if enter then set CONTROL_RATTLE_ON due to eliminate rattle of button
		if(button_variables.button_state == BUTTON_UNPRESSED) {
			button_variables.button_state = BUTTON_PRESSED;     //button is pressed
			button_variables.rattle_check = CONTROL_RATTLE_ON;  //control rattle on
			button_variables.count_short_press++;               //increase number of press
			Timers_turn_on();
			return;
		}
		if(button_variables.button_state == BUTTON_PRESSED) {   //if button was unpressed
			button_variables.button_state = BUTTON_UNPRESSED;   //then it was falling front interrupt
			button_variables.rattle_check = CONTROL_RATTLE_ON;  //control rattle on
			Timers_turn_on();
		}
	}
}

void TIM6_DAC_IRQHandler(void) {
	TIM6->SR &= ~TIM_SR_UIF;
	TIM6->CR1 &= ~TIM_CR1_CEN;
	//if the timer has counted up to 25ms then disable rattle check
	button_variables.rattle_check = CONTROL_RATTLE_OFF;
}

void TIM2_IRQHandler(void) {
	TIM2->SR &= ~TIM_SR_UIF;
	TIM2->CR1 &= ~TIM_CR1_CEN;
	//if the timer has counted up to 3s then set state long press
	if(button_variables.button_state == BUTTON_PRESSED) {
		button_variables.count_short_press = 0;// throw to zero in order to count amount of press
		program_state = SETUP_MODE;			   // setup mode is set
		TIM14->CR1 |= TIM_CR1_CEN; 			   //turn on timer for blink led
	}
}

void TIM3_IRQHandler(void) {
	TIM3->SR &= ~TIM_SR_UIF;
	button_variables.press_check = CHECK_AMOUNT_OF_PRESS;
}

void check_button_press() {
	if(button_variables.press_check != CHECK_AMOUNT_OF_PRESS || button_variables.count_short_press == 0)
		return;
	button_variables.press_check = WAITING;
	//if it was long press, choose between two functions
	if(program_state == SETUP_MODE && button_variables.button_state == BUTTON_UNPRESSED) {
		switch(button_variables.count_short_press) {
		case 1:
			program_state = WRITE_DATA_INTO_FLASH;
			Turn_on_Led_PC8();                     	//if it was 1 short press
			Turn_off_Led_PC9();						//write data into flash
			Timers_turn_off();
			break;
		case 2:
			program_state = BLINK_LED_ON_OTHER_MK;	//if it was 2 short presses
			Turn_off_Led_PC8();						//blink led on another MK
			Turn_off_Led_PC9();
			Timers_turn_off();
			break;
		default:
			Turn_off_Led_PC8();
			Turn_off_Led_PC9();
			Timers_turn_off();
			Button_reset_to_be_ready_for_work(); 	//if more than 2 press was performed -> restart button
			break;
		}
		return;
	}
	//else check for short presses
	if(button_variables.button_state == BUTTON_UNPRESSED) {
		switch(button_variables.count_short_press) {
			case 1:
				program_state = TRANSFER_DATA;
				Turn_on_Led_PC8(); 			   		//if it was single short press
				Turn_on_Led_PC9(); 	   		   		//transfer data
				Timers_turn_off();
				break;
			case 2:
				program_state = RECEIVE_DATA_FROM_PC;//if it were 2 short presses
				Turn_on_Led_PC8();					 //receive data from PC via USART
				Timers_turn_off();
				break;
			case 3:
				program_state = TRANSFER_DATA_TO_PC;//if it were 3 short presses
				Turn_on_Led_PC9();					//transfer data to PC via USART
				Timers_turn_off();
				break;
			default:
				Timers_turn_off();
				Button_reset_to_be_ready_for_work();//if more than 3 press was performed -> restart button
				break;
		}
	}
}

void TIM14_IRQHandler(void) {
	if( (TIM14->DIER & TIM_DIER_UIE) == TIM_DIER_UIE ) {
			TIM14->SR &= ~TIM_SR_UIF;         //timer for blink led each 0.5s
			GPIOC->ODR ^= GPIO_ODR_8;		  // in setup mode
			GPIOC->ODR ^= GPIO_ODR_9;
			if(program_state == BLINK_MY_LED)
				count_of_blink++;
	}
}

void Button_tune() {
	init_GPIO_for_Button();
	init_TIM2_for_long_pressure_check();
	init_TIM3_for_check_result_of_press();
	init_TIM6_for_rattle_eliminating();
	init_TIM14_for_blinkLed();
	Button_reset_to_be_ready_for_work();
}

void Timers_turn_on() {
	TIM2->CNT = 0;
	TIM3->CNT = 0;
	TIM6->CNT = 0;
	TIM14->CNT = 0;
	//if SETUP mode had already set then no need to turn on timer2 for check long press
	if(program_state != SETUP_MODE)
		TIM2->CR1 |= TIM_CR1_CEN;
	TIM3->CR1 |= TIM_CR1_CEN;
	TIM6->CR1 |= TIM_CR1_CEN;
}

void Timers_turn_off() {
	TIM2->CR1 &= ~TIM_CR1_CEN;
	TIM3->CR1 &= ~TIM_CR1_CEN;
	TIM6->CR1 &= ~TIM_CR1_CEN;
	TIM14->CR1 &= ~TIM_CR1_CEN;
}

void Button_reset_to_be_ready_for_work() {
	button_variables.button_state = BUTTON_UNPRESSED;
	button_variables.rattle_check = CONTROL_RATTLE_OFF;
	button_variables.count_short_press = 0;
}

void Turn_on_Led_PC8() {
	GPIOC->ODR |= GPIO_ODR_8;
}

void Turn_off_Led_PC8() {
	GPIOC->ODR &= ~GPIO_ODR_8;
}

void Turn_on_Led_PC9(){
	GPIOC->ODR |= GPIO_ODR_9;
}

void Turn_off_Led_PC9(){
	GPIOC->ODR &= ~GPIO_ODR_9;
}

