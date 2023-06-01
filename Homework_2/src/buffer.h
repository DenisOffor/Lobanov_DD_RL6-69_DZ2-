/*
 * buffer.h
 *
 *  Created on: 9 мая 2023 г.
 *      Author: denlo
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include "stm32f0xx.h"

#define BUFFER_SIZE 1024
#define DATA_SIZE 8

typedef struct RingBuf_c {
	uint8_t *buf; 				//address where buffer is physically stored
	uint16_t write_point;   	//point of writing data
	uint16_t read_point; 		//point of reading data
	uint16_t buf_size; 			//size of buffer
	uint16_t amount_data_in_buf;//number of bytes in buffer
} buf;


//functions
//initialize buffer
void InitBuffer(buf* buffer, uint8_t* address, uint16_t size);

//add data to the end of the buffer meanwhile write_point++
void Buffer_add_to_end(buf* buffer, uint8_t data);

//get data from the front of the buffer meanwhile read_point++
uint8_t Buffer_get_from_front(buf* buffer);

//check if the buffer is empty. Return 1 if empty and 0 if not
uint8_t Buffer_empty(buf* buffer);

//check if the buffer is full. Return 1 if full and 0 if not
uint8_t Buffer_is_full(buf* buffer);

//clear buffer
void Clear_buffer(buf* buffer);

#endif /* BUFFER_H_ */
