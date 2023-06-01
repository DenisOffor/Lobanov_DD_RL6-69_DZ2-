/*
 * USART_DMA_setup.h
 *
 *  Created on: 18 мая 2023 г.
 *      Author: denlo
 */

#ifndef USART_DMA_SETUP_H_
#define USART_DMA_SETUP_H_

#include "stm32f0xx.h"
#include "buffer.h"


void DMA1_Channel2_3_IRQHandler(void);

//initialize GPIO for USART
void init_GPIO_for_USART();
//initialize USART1 for receive data from PC and transfer data to PC
void init_USART1();
//initialize DMA for work with USART and buffers
void init_DMA_to_work_with_USART(uint32_t transmit_page, uint32_t receive_page);
//contain all function above
void setup_USART_DMA(uint32_t transmit_page, uint32_t receive_page);
//enable DMA_channel3 for receive data from PC
void start_receive_data_from_PC();
//enable DMA_channel2 for transfer data to PC
void start_transmit_data_to_PC();

#endif /* USART_DMA_SETUP_H_ */
