/*
 * buffer.c
 *
 *  Created on: 9 мая 2023 г.
 *      Author: denlo
 */

#include "buffer.h"

//initialize buffer
void InitBuffer(buf* buffer, uint8_t* address, uint16_t size) {
	buffer->buf = address; //set address of buf
	//buffer is empty so read_point and write_point are in the beginning
	buffer->read_point = buffer->write_point = 0;
	//set size of buffer, in our case it is 1024 bytes
	buffer->buf_size = size;
	//buffer is empty so
	buffer->amount_data_in_buf = 0;
}


//add data to the end of the buffer meanwhile write_point++
void Buffer_add_to_end(buf* buffer, uint8_t data) {
	if(buffer->amount_data_in_buf < BUFFER_SIZE) {
		buffer->buf[buffer->write_point] = data; //write data to the end
		buffer->write_point++; //shift write_point
		buffer->amount_data_in_buf++; //increase amount of data on 1

		if(buffer->write_point >= BUFFER_SIZE) // if write point greater than BUFFER_SIZE or equal it
			buffer->write_point = 0;           // then set write_point at the beginning of buffer
	}
}


//get data from the front of the buffer meanwhile read_point++
uint8_t Buffer_get_from_front(buf* buffer) {
	if(buffer->amount_data_in_buf != 0) {//if there are data
		uint8_t data = buffer->buf[buffer->read_point]; //get data
		buffer->read_point++; //shift read_point

		if(buffer->read_point >= BUFFER_SIZE) // if read point greater than BUFFER_SIZE or equal it
			buffer->read_point = 0;           // then set read_point at the beginning of buffer

		buffer->amount_data_in_buf--;
		return data;
	}
	return -1;
}

//check if the buffer is empty. Return 1 if empty and 0 if not
uint8_t Buffer_empty(buf* buffer) {
	return ((buffer->read_point == buffer->write_point) && (buffer->amount_data_in_buf == 0));
}

uint8_t Buffer_is_full(buf* buffer) {
	return (buffer->amount_data_in_buf == BUFFER_SIZE);
}

void Clear_buffer(buf* buffer) {
	buffer->read_point = 0;
	buffer->write_point = 0;
	buffer->amount_data_in_buf = 0;
}
