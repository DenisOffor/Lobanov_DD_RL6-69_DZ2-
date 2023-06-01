/*
 * serial_receive_and_paralell_transfer.c
 *
 *  Created on: 14 мая 2023 г.
 *      Author: denlo
 */


#include "serial_receive_and_paralell_transfer.h"

static uint8_t count_bit_for_tr = 0;
static uint8_t count_bit_for_rec = 0;
static uint8_t data_for_tr = 0;
static uint8_t data_for_rec = 0;

void EXTI2_3_IRQHandler(void) {
	if( (EXTI->PR & EXTI_PR_PIF2) == EXTI_PR_PIF2 ) {
		EXTI->PR |= EXTI_PR_PIF2;
#ifdef PAR_TRANSFER_AND_SERIAL_RECEIVE
		transfer_state = TRANSFER_8_BIT;
#endif
#ifdef SER_TRANSFER_AND_PAR_RECEIVE
		receive_state =  RECEIVE_8_BIT;
#endif
	}
	if( (EXTI->PR & EXTI_PR_PIF3) == EXTI_PR_PIF3 ) {
		EXTI->PR |= EXTI_PR_PIF3;
#ifdef PAR_TRANSFER_AND_SERIAL_RECEIVE
		transfer_state = CATCH_SIG_READY;
#endif
	}
}

void EXTI4_15_IRQHandler(void) {
	if( (EXTI->PR & EXTI_PR_PIF4) == EXTI_PR_PIF4 ) {
		EXTI->PR |= EXTI_PR_PIF4;
#ifdef PAR_TRANSFER_AND_SERIAL_RECEIVE
		receive_state = RECEIVE_BIT;
#endif
#ifdef SER_TRANSFER_AND_PAR_RECEIVE
		transfer_state =  TRANSFER_BIT;
#endif
	}
}



#ifdef SER_TRANSFER_AND_PAR_RECEIVE
void TIM15_IRQHandler(void) {    //in interrupts timer15 clock signal is generated
	if( (TIM15->DIER & TIM_DIER_UIE) == TIM_DIER_UIE ) {
		TIM15->SR &= ~TIM_SR_UIF;
		GPIOA->ODR ^= GPIO_ODR_4;//generate clock on PA4
	}
}

void Restart_work_after_transfer() {
	GPIOA->ODR &= ~GPIO_ODR_4;   //generate falling front for receive last bit in other MK
	for(int i = 0; i < 20; i++); //delay in order sig d_send have time to reach other MK
	TIM15->CR1 &= ~TIM_CR1_CEN;	 //and then set d_send to "1"
	GPIOA->ODR |= GPIO_ODR_1;	 //PA1 is set to 1(d_send signal) as transfer ended
	 //Buf for transfer again full of data
	Buf_for_transfer.amount_data_in_buf = BUFFER_SIZE;
	Turn_off_Led_PC8();
	Turn_off_Led_PC9();
	Button_reset_to_be_ready_for_work();
	program_state = WAITING_FOR_ACTION;
}

void transfer_bit() {
	//if count_bit_for_tr = 0 then get byte of data from buffer
	if(count_bit_for_tr == 0)
		data_for_tr = Buffer_get_from_front(&Buf_for_transfer);
	GPIOA->ODR &= ~GPIO_ODR_5; //clear bit for data transfer
	//write 1 bit to PA5, for that get specific bit from byte and shift it to ODR5 (<< 5)
	GPIOA->ODR |= (( (data_for_tr >> count_bit_for_tr) & 0x0001 ) << 5);
	count_bit_for_tr++;
}

void check_byte_transfer_complete() {
	//if byte of data are transfered then throw to zero count_bit_for_tr in order to
	if(count_bit_for_tr == DATA_SIZE) {
		count_bit_for_tr = 0;				//transfer next byte of data
		//if it was last element in transfer buf then transfer complete
		if(Buffer_empty(&Buf_for_transfer))
			Restart_work_after_transfer();
	}
}

void receive_8bit() {
	data_for_rec |= ((GPIOB->IDR >> 1) &  0x00FF); //receive 8 bit from PB1-PB9
}

void check_byte_receive_complete() {
	//if 8 bit was received write data to buf and throw to zero variables for receive
	Buffer_add_to_end(&Buf_For_receive, data_for_rec);
	count_bit_for_rec = 0;
	data_for_rec = 0;
	check_key_for_blink_led();	//check if first 16 bit was key to blink led
}

void check_receive_complete() {
	if(Buffer_is_full(&Buf_For_receive)) {
		receive_state = RECEIVE_COMPLETE; //if buf for receive is full then receive complete
		Clear_buffer(&Buf_For_receive);   //artificially buf again is empty
	}
}

void blink_led_on_other_mk() {
	GPIOA->ODR &= ~GPIO_ODR_5; //clear bit for data transfer
	 //write 1 bit to PA5, for that get specific bit from byte and shift it to ODR5 (<< 5)
	GPIOA->ODR |= (( (KEY_FOR_BLINK_LED >> count_bit_for_tr) & 0x0001 ) << 5);
	count_bit_for_tr++;
	if(count_bit_for_tr == (DATA_SIZE * 2) ) {
		count_bit_for_tr = 0;
		Restart_work_after_transfer();
	}
}

#endif



#ifdef PAR_TRANSFER_AND_SERIAL_RECEIVE
void receive_bit() {
	//get bit from GPIOA->IDR PA5 and write in temp variable
	data_for_rec |= (((GPIOA->IDR >> 5) &  0x0001 ) << count_bit_for_rec);
	count_bit_for_rec++; //increase the number of bits have already received
}

void check_byte_receive_complete() {
	if(count_bit_for_rec == DATA_SIZE) {
		Buffer_add_to_end(&Buf_For_receive, data_for_rec); //if 8 bit are received
		data_for_rec = 0;								   //then start to receive new bits
		count_bit_for_rec = 0;
		check_key_for_blink_led(); //check if first 16 bit was key to blink led
	}
}

void check_receive_complete() {
	if(Buffer_is_full(&Buf_For_receive)) {  //if buffer is full then all data are received
			receive_state = RECEIVE_COMPLETE;
			Clear_buffer(&Buf_For_receive);	//artificially buf again is empty
	}
}

void set_data_on_GPIO() {
	if(program_state == KEY_TRANSFER) {
		GPIOB->ODR &= (0xFE01);   //clear GPIO in order to set right bit for transfer
		//set bits on PB1-PB9
		GPIOB->ODR |= ( ((KEY_FOR_BLINK_LED >> count_bit_for_tr) & 0x00FF ) << 1);
		count_bit_for_tr += AMOUNT_RECEIVE_DATA;
		return;
	}
	//if it is ordinary transfer, not key transfer
	data_for_tr = Buffer_get_from_front(&Buf_for_transfer); //get element from buf
	GPIOB->ODR &= (0xFE01);	//clear GPIO in order to set right bit for transfer
	GPIOB->ODR |= ( data_for_tr << 1 ); //set bits on PB1-PB9
}

void Restart_work_after_transfer() {
	GPIOA->ODR &= ~GPIO_ODR_2;	//reset GPIO to beginning state because of beauty :)
	//Buffer for transfer again full of data artificially
	Buf_for_transfer.amount_data_in_buf = BUFFER_SIZE;
	Turn_off_Led_PC8();
	Turn_off_Led_PC9();
	Button_reset_to_be_ready_for_work();
	program_state = WAITING_FOR_ACTION;
}
#endif

void Setup_flash_with_data() {
	InitBuffer(&Buf_for_transfer, &array_tr[0], BUFFER_SIZE);
	InitBuffer(&Buf_For_receive, &array_rec[0], BUFFER_SIZE);
	Write_data_to_flash(PAGE_FOR_TRANSFER, Buf_for_transfer.buf, BUFFER_SIZE); //write empty buffer to flash
	Write_data_to_flash(PAGE_FOR_RECEIVE, Buf_For_receive.buf, BUFFER_SIZE);   //in order to check correctness
	Buf_for_transfer.amount_data_in_buf = BUFFER_SIZE;						   //of data received from PC or other MK
}

void check_key_for_blink_led() {
	if(Buf_For_receive.amount_data_in_buf == 2) { //if 2 element are received (key consist of 16 bit)
		if( (Buf_For_receive.buf[0] | (Buf_For_receive.buf[1] << 8)) != KEY_FOR_BLINK_LED )
			return;
		program_state = BLINK_MY_LED;
		Clear_buffer(&Buf_For_receive);
	}
}

void write_to_flash() {
	if(receive_state == RECEIVE_COMPLETE){ 	 //if data was received, write data to flash
		Write_data_to_flash(PAGE_FOR_RECEIVE, Buf_For_receive.buf, BUFFER_SIZE);
		Button_reset_to_be_ready_for_work();  //reset button
		Turn_off_Led_PC8();
		receive_state = WAITING_FOR_ACTION;	  //ready to receive new data
		program_state = WAITING_FOR_ACTION;
	}
	else {
		Turn_off_Led_PC8();					 //else if data wasn't received
		Button_reset_to_be_ready_for_work(); // just reset button
		program_state = WAITING_FOR_ACTION;
	}
}

void blink_led_3_times() {
	TIM14->CR1 |= TIM_CR1_CEN; //turn on timer for 0.5 sec
	if(count_of_blink == 6) {  //and wait while led blink 3 times
		program_state = WAITING_FOR_ACTION;
		TIM14->CR1 &= ~TIM_CR1_CEN;
		count_of_blink = 0;
	}
}

//contain function for initialize all periphery for program
void Tune_peripheries() {
	Button_tune();
	init_GPIO_for_transfer_and_receive_data();
#ifdef SER_TRANSFER_AND_PAR_RECEIVE
	init_TIM15_for_clock_sig();
#endif
	Setup_flash_with_data();
	setup_USART_DMA((uint32_t)(&Buf_for_transfer.buf[0]), PAGE_FOR_RECEIVE);
}


void process_cmd() {
#ifdef PAR_TRANSFER_AND_SERIAL_RECEIVE
	if(receive_state == RECEIVE_BIT) {
		receive_state = WAITING_FOR_ACTION; 		//change receive state
		if((GPIOA->IDR & GPIO_IDR_1) == GPIO_IDR_1) //if signal d_send "1" so data isn't transfered
			return;									//so return because it's fake :(
		receive_bit();								//else if signal on PA1 (d_send) "0" -> data is transfered
		check_byte_receive_complete();				//so receive this data
		check_receive_complete();
	}

	if(transfer_state == TRANSFER_8_BIT) {		//change transfer state
		transfer_state = WAITING_FOR_ACTION;
		GPIOA->ODR &= ~GPIO_ODR_2; 				//set 0 on PA2
		set_data_on_GPIO();						//set 8 bit on GPIOx
		GPIOA->ODR |= GPIO_ODR_2; 				//set 1 on PA2 -> rising front (signal enable) for PA2 other MK
	}

	if(transfer_state == CATCH_SIG_READY) {
		transfer_state = WAITING_FOR_ACTION;
		if( Buffer_empty(&Buf_for_transfer) ) {
			Restart_work_after_transfer();  //if buf empty -> all data are transfered
			return; 						//no need to SWI bit set so restart work and return
		}
		if(program_state == KEY_TRANSFER && count_bit_for_tr == (DATA_SIZE * 2)) {
			count_bit_for_tr = 0;           //if key to blink led on other MK is transfered ->
			Restart_work_after_transfer();  //no need to SWI bit set so restart work and return
			return;
		}
		EXTI->SWIER |= EXTI_SWIER_SWI2; //if falling front came then carry out new 8 bits transfer
	}
#endif

#ifdef SER_TRANSFER_AND_PAR_RECEIVE
	if(receive_state == RECEIVE_8_BIT) {
		receive_state = WAITING_FOR_ACTION; 	//change receive state
		GPIOA->ODR |= GPIO_ODR_3; 				//set 1 on PA3 (signal ready)
		receive_8bit();
		check_byte_receive_complete();
		check_receive_complete();
		GPIOA->ODR &= ~GPIO_ODR_3; 				//set 0 on PA3 (signal ready) providing falling for PA3 other MK
	}
	if(transfer_state == TRANSFER_BIT) {
		transfer_state = WAITING_FOR_ACTION;    //change transfer state
		if(program_state == KEY_TRANSFER) {
			blink_led_on_other_mk();
			return;
		}
		transfer_bit();
		check_byte_transfer_complete();
	}
#endif
}


//contain switch in which by using program state actions are chosen
void manage_states() {
	switch(program_state) {
		case TRANSFER_DATA:
			program_state = PROGRAMMING;
			ReadFromFlash(PAGE_FOR_TRANSFER, Buf_for_transfer.buf, BUFFER_SIZE);
#ifdef PAR_TRANSFER_AND_SERIAL_RECEIVE
			EXTI->SWIER |= EXTI_SWIER_SWI2; //call interrupt on EXTI2 and start transfer data
#endif
#ifdef SER_TRANSFER_AND_PAR_RECEIVE
			GPIOA->ODR &= ~GPIO_ODR_1;   //start transfer data so set PA1(d_send) to zero level
			for(int i = 0; i < 20; i++); //delay in order sig d_send have time to reach other MK
			TIM15->CR1 |= TIM_CR1_CEN;	 //TIM15 on in order to generate clock
#endif
			break;
		case WRITE_DATA_INTO_FLASH:
			program_state = PROGRAMMING;
			write_to_flash();
			break;
		case BLINK_LED_ON_OTHER_MK:
			program_state = KEY_TRANSFER;
#ifdef PAR_TRANSFER_AND_SERIAL_RECEIVE
			EXTI->SWIER |= EXTI_SWIER_SWI2;
#endif
#ifdef SER_TRANSFER_AND_PAR_RECEIVE
			GPIOA->ODR &= ~GPIO_ODR_1;    //start transfer data so set PA1(d_send) to zero level
			for(int i = 0; i < 20; i++);  //delay in order sig d_send have time to reach other MK
			TIM15->CR1 |= TIM_CR1_CEN;	  //TIM15 on in order to generate clock
#endif
			break;
		case BLINK_MY_LED:
			blink_led_3_times();
			break;
		case RECEIVE_DATA_FROM_PC:
			start_receive_data_from_PC();
			while(DMA1_Channel3->CCR & DMA_CCR_EN);  //wait while data receive
			Write_data_to_flash(PAGE_FOR_TRANSFER, Buf_for_transfer.buf, BUFFER_SIZE); //write transfered data to flash
			Turn_off_Led_PC8();
			Button_reset_to_be_ready_for_work();	 //reset button
			program_state = WAITING_FOR_ACTION;
			break;
		case TRANSFER_DATA_TO_PC:
			start_transmit_data_to_PC();
			while(DMA1_Channel2->CCR & DMA_CCR_EN);  //wait while data transfer
			Turn_off_Led_PC9();
			Button_reset_to_be_ready_for_work();     //reset button
			program_state = WAITING_FOR_ACTION;
			break;
		default:
			break;
	}
}




