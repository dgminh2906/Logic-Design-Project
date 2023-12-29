/*
 * global.h
 *
 *  Created on: Dec 27, 2023
 *      Author: LENOVO
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#define INIT				0
#define MODE1				1
#define MODE2				2
#define MODE3				3
#define MODE4				4
#define MODE5 				5

#define MODE5s				11
#define INC_HOUR			41
#define INC_MIN				42
#define INC_SEC				43

#define INC_MODE			51
#define DEC_MODE			52

#define DMY_DISPLAY_TIME 	400

extern int status;
extern int dmy5stime;
extern int hour;
extern int min;
extern int sec;
extern int hour_alarm;
extern int min_alarm;
extern int sec_alarm;
extern int day;
extern int month;
extern int year;
extern int MAX_HOUR;

#endif /* INC_GLOBAL_H_ */
