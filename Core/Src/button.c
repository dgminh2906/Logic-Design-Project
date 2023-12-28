/*
 * button.c
 *
 *  Created on: Dec 27, 2023
 *      Author: LENOVO
 */

#include "button.h"

int KeyReg0[NUM_OF_BUTTONS] = {NORMAL_STATE};
int KeyReg1[NUM_OF_BUTTONS] = {NORMAL_STATE};
int KeyReg2[NUM_OF_BUTTONS] = {NORMAL_STATE};
int KeyReg3[NUM_OF_BUTTONS] = {NORMAL_STATE};

// Create flag for normal press and long press
int normal_flag[NUM_OF_BUTTONS] = {0};
int long_flag[NUM_OF_BUTTONS] = {0};

int TimeOutForKeyPress[NUM_OF_BUTTONS] = {0};

int isButtonPressed(int index){
	if (normal_flag[index] == 1){
		normal_flag[index] = 0;
		return 1;
	}
	return 0;
}

void subKeyProcess(int index){
	normal_flag[index] = 1;
}

void getKeyInput() {
	for (int i = 0; i < NUM_OF_BUTTONS; i++) {
		KeyReg0[i] = KeyReg1[i];
		KeyReg1[i] = KeyReg2[i];

		// Read signals from buttons
		if (i == 0) {
			KeyReg2[i] = HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin);
		}
		else if (i == 1) {
			KeyReg2[i] = HAL_GPIO_ReadPin(BUTTON2_GPIO_Port, BUTTON2_Pin);
		}
		else if (i == 2) {
			KeyReg2[i] = HAL_GPIO_ReadPin(BUTTON3_GPIO_Port, BUTTON3_Pin);
		}

		if ((KeyReg1[i] == KeyReg0[i]) && (KeyReg1[i] == KeyReg2[i])){
			if (KeyReg3[i] != KeyReg2[i]){
				KeyReg3[i] = KeyReg2[i];
				if (KeyReg2[i] == PRESSED_STATE){
					subKeyProcess(i);	// Set timer_flag[i] = 1;
					if (long_flag[i] == 0) {
						TimeOutForKeyPress[i] = 2000 / TIME_CYCLE;
					}
					else {
						TimeOutForKeyPress[i] = 250 / TIME_CYCLE;
					}
				}
			} else {
				TimeOutForKeyPress[i]--;
				if (TimeOutForKeyPress[i] == 0){
					KeyReg3[i] = NORMAL_STATE;
					long_flag[i] = 1;
				}
			}
		}
		else {
			long_flag[i] = 0;
		}
	}
}

