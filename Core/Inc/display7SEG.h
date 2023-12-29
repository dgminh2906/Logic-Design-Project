/*
 * display7SEG.h
 *
 *  Created on: Dec 27, 2023
 *      Author: LENOVO
 */

#ifndef INC_DISPLAY7SEG_H_
#define INC_DISPLAY7SEG_H_

#include "main.h"
#include "global.h"

extern int index_led;

void display7SEG(int num);
void update7SEG(int index);

void updateLedBufferMode1();
void updateLedBufferMode2();
void updateLedBufferMode4();

#endif /* INC_DISPLAY7SEG_H_ */
