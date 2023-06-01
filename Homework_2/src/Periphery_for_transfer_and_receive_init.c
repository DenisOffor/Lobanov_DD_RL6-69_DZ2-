/*
 * Periphery_for_transfer_and_receive_init.c
 *
 *  Created on: 15 мая 2023 г.
 *      Author: denlo
 */

#include <Periphery_for_transfer_and_receive_init.h>

#ifdef PAR_TRANSFER_AND_SERIAL_RECEIVE
void init_GPIO_for_transfer_and_receive_data() {
	//RCC on
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	//turn on external interrupt for PA3-PA5
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA | SYSCFG_EXTICR1_EXTI3_PA;
	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PA | SYSCFG_EXTICR2_EXTI5_PA;
	//PB1-PB9 are set to output mode
	GPIOB->MODER |= GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0 |
				GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0 | GPIO_MODER_MODER8_0;
	//PA1 is set to input mode as signal d_send from other MK
	GPIOA->MODER &= ~GPIO_MODER_MODER1;
	//PA2 is set to output mode as signal en to other MK
	GPIOA->MODER |= GPIO_MODER_MODER2_0;
	//PA3 is set to input mode as signal ready from other MK
	GPIOA->MODER &= ~GPIO_MODER_MODER3;
	//PA4 for receive clock from other MK
	GPIOA->MODER &= ~GPIO_MODER_MODER4;
	//PA5 for receive serial data
	GPIOA->MODER &= ~GPIO_MODER_MODER5;

	//interrupt on for Px2, Px3 and Px4
	EXTI->IMR |= EXTI_IMR_IM2 | EXTI_IMR_IM3 | EXTI_IMR_IM4;
	//falling front for Px3 and Px4
	EXTI->FTSR |= EXTI_FTSR_FT3 | EXTI_FTSR_FT4;

	NVIC_EnableIRQ(EXTI2_3_IRQn);
	NVIC_SetPriority(EXTI2_3_IRQn, 9);
	NVIC_EnableIRQ(EXTI4_15_IRQn);
	NVIC_SetPriority(EXTI4_15_IRQn, 1);
}
#endif

#ifdef SER_TRANSFER_AND_PAR_RECEIVE
void init_GPIO_for_transfer_and_receive_data() {
	//RCC on
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PA;
	//PB1-PB9 are set to input mode for receive parallel data
	GPIOB->MODER &= ~GPIO_MODER_MODER1;
	GPIOB->MODER &= ~GPIO_MODER_MODER2;
	GPIOB->MODER &= ~GPIO_MODER_MODER3;
	GPIOB->MODER &= ~GPIO_MODER_MODER4;
	GPIOB->MODER &= ~GPIO_MODER_MODER5;
	GPIOB->MODER &= ~GPIO_MODER_MODER6;
	GPIOB->MODER &= ~GPIO_MODER_MODER7;
	GPIOB->MODER &= ~GPIO_MODER_MODER8;
	//PA1 is set to output mode as signal d_send
	GPIOA->MODER |= GPIO_MODER_MODER1_0;
	//PA2 is set to input mode as signal en from other MK
	GPIOA->MODER &= ~GPIO_MODER_MODER2;
	//PA3 is set to output mode as signal ready for other MK
	GPIOA->MODER |= GPIO_MODER_MODER3_0;
	//PA4 for generate clock
	GPIOA->MODER |= GPIO_MODER_MODER4_0;
	//PA5 for transfer data
	GPIOA->MODER |= GPIO_MODER_MODER5_0;

	//interrupt on for Px2 and Px4
	EXTI->IMR |= EXTI_IMR_IM2 | EXTI_IMR_IM4;
	//rising front in en sig for read data and clock sig to set data
	EXTI->RTSR |= EXTI_RTSR_RT2 | EXTI_RTSR_RT4;

	//signal d_send initially is set to "1"
	GPIOA->ODR |= GPIO_ODR_1;

	NVIC_EnableIRQ(EXTI4_15_IRQn);
	NVIC_SetPriority(EXTI4_15_IRQn, 1);
	NVIC_EnableIRQ(EXTI2_3_IRQn);
	NVIC_SetPriority(EXTI2_3_IRQn, 9);
}

void init_TIM15_for_clock_sig() {
	//RCC on
	RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
	//tune TIM14 for 1000 Hz
	TIM15->ARR = 700;
	TIM15->PSC = 4;
	//interrupt on
	TIM15->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM15_IRQn);
	NVIC_SetPriority(TIM15_IRQn, 4);
}
#endif

