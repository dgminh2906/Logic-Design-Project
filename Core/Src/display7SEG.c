/*
 * display7SEG.c
 *
 *  Created on: Dec 27, 2023
 *      Author: LENOVO
 */
#include "display7SEG.h"

void display7SEG(int num){
  	char led7seg[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
  	for (int i=0; i < 7; i++){
  		HAL_GPIO_WritePin(GPIOB, SEG0_Pin<<i, (led7seg[num]>>i) & 1);
  	}
}

const int MAX_LED = 6;
int index_led = 0;
int led_buffer[6] = {0, 0, 0, 0, 0, 0};
void update7SEG(int index){
	display7SEG(led_buffer[index]);
	switch(index){
		case 0:
			// Display the first 7 SEG with led_buffer [0]
			index_led = 1;
			HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, RESET);
			HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, SET);
			HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, SET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, SET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN4_Pin, SET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN5_Pin, SET);
			break;
		case 1:
			// Display the second 7 SEG with led_buffer [1]
			index_led = 2;
			HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, SET);
			HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, RESET);
			HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, SET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, SET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN4_Pin, SET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN5_Pin, SET);
			break;
		case 2:
			// Display the third 7 SEG with led_buffer [2]
			index_led = 3;
			HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, SET);
			HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, SET);
			HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, RESET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, SET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN4_Pin, SET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN5_Pin, SET);
			break;
		case 3:
			// Display the forth 7 SEG with led_buffer [3]
			index_led = 4;
			HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, SET);
			HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, SET);
			HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, SET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, RESET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN4_Pin, SET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN5_Pin, SET);
			break;
		case 4:
			// Display the forth 7 SEG with led_buffer [4]
			index_led = 5;
			HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, SET);
			HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, SET);
			HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, SET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, SET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN4_Pin, RESET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN5_Pin, SET);
			break;
		case 5:
			// Display the forth 7 SEG with led_buffer [5]
			index_led = 0;
			HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, SET);
			HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, SET);
			HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, SET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, SET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN4_Pin, SET);
			HAL_GPIO_WritePin(EN3_GPIO_Port, EN5_Pin, RESET);
			break;
		default:
			break;
	}
}

void updateLedBufferMode1(){
	if (MAX_HOUR == 24)
	{
		led_buffer[0] = (hour % MAX_HOUR) / 10;
		led_buffer[1] = (hour % MAX_HOUR) % 10;
		led_buffer[2] = min / 10;
		led_buffer[3] = min % 10;
		led_buffer[4] = sec / 10;
		led_buffer[5] = sec % 10;
	}
	if (MAX_HOUR == 12)
	{
		if (hour > 12)
		{
			led_buffer[0] = (hour % MAX_HOUR) / 10;
			led_buffer[1] = (hour % MAX_HOUR) % 10;
		}
		else{
			led_buffer[0] = hour / 10;
			led_buffer[1] = hour % 10;
		}
		led_buffer[2] = min / 10;
		led_buffer[3] = min % 10;
		led_buffer[4] = sec / 10;
		led_buffer[5] = sec % 10;

	}
}

void updateLedBufferMode2(){
	led_buffer[0] = day / 10;
	led_buffer[1] = day % 10;
	led_buffer[2] = month / 10;
	led_buffer[3] = month % 10;
	led_buffer[4] = (year - 2000) / 10;
	led_buffer[5] = (year - 2000) % 10;
}

void updateLedBufferMode4(){
	if (MAX_HOUR == 24){
		led_buffer[0] = (hour_alarm % MAX_HOUR) / 10;
		led_buffer[1] = (hour_alarm % MAX_HOUR) % 10;
		led_buffer[2] = min_alarm / 10;
		led_buffer[3] = min_alarm % 10;
		led_buffer[4] = sec_alarm / 10;
		led_buffer[5] = sec_alarm % 10;
	}
	if (MAX_HOUR == 12){
		if (hour_alarm > 12){
			led_buffer[0] = (hour_alarm % MAX_HOUR) / 10;
			led_buffer[1] = (hour_alarm % MAX_HOUR) % 10;
		}
		else{
			led_buffer[0] = hour_alarm / 10;
			led_buffer[1] = hour_alarm % 10;
		}
		led_buffer[2] = min_alarm / 10;
		led_buffer[3] = min_alarm % 10;
		led_buffer[4] = sec_alarm / 10;
		led_buffer[5] = sec_alarm % 10;

	}

}


