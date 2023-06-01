/*
 * button_init.c
 *
 *  Created on: 15 мая 2023 г.
 *      Author: denlo
 */

#include "button_init.h"


void init_GPIO_for_Button() {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	//input mode on PA0
	GPIOA->MODER &= ~GPIO_MODER_MODER0;
	//output mode for Led PC8 and PC9
	GPIOC->MODER |= GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0;
	EXTI->IMR |= EXTI_IMR_IM0;	 //interrupt mask register
	EXTI->RTSR |= EXTI_RTSR_RT0; //rising trigger selection
	EXTI->FTSR |= EXTI_FTSR_FT0; //falling trigger selection
	NVIC_EnableIRQ(EXTI0_1_IRQn);//tune NVIC for EXTI0
	NVIC_SetPriority(EXTI0_1_IRQn, 5);
}

void init_TIM6_for_rattle_eliminating () {
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6->ARR = 4000; //tune TIM6 for 25ms
	TIM6->PSC = 50;
	//interrupt on
	TIM6->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
	NVIC_SetPriority(TIM6_DAC_IRQn, 5);
	TIM6->CR1 |= TIM_CR1_CEN;
}

void init_TIM2_for_long_pressure_check() {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->ARR = 12000; //tune TIM2 for 3sec
	TIM2->PSC = 2000;
	//interrupt on
	TIM2->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority(TIM2_IRQn, 5);
	TIM2->CR1 |= TIM_CR1_CEN;
}

void init_TIM3_for_check_result_of_press() {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->ARR = 8000;//tune TIM3 for 1sec
	TIM3->PSC = 1000;
	//interrupt on
	TIM3->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM3_IRQn);
	NVIC_SetPriority(TIM3_IRQn, 5);
	TIM3->CR1 |= TIM_CR1_CEN;
}

void init_TIM14_for_blinkLed() {
	RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
	TIM14->ARR = 4000;//tune TIM14 for 0.5sec
	TIM14->PSC = 1000;
	//interrupt on
	TIM14->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM14_IRQn);
	NVIC_SetPriority(TIM14_IRQn, 5);
}
