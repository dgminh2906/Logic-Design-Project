/*
 * scheduler.c
 *
 *  Created on: Dec 27, 2023
 *      Author: LENOVO
 */

#include "scheduler.h"

int TIME_CYCLE; // Time cycle

sTasks SCH_tasks_G[SCH_MAX_TASKS];
uint8_t current_index_task = 0;

void SCH_Init(void){
	while (current_index_task != 0) {
		SCH_Delete_Task(0);
	}
}
void SCH_Update(void){
	for(int i = 0; i < current_index_task; i++) {
		if(SCH_tasks_G[i].Delay > 0) {
			// Decrement the delay
			SCH_tasks_G[i].Delay--;
		}
		else {
			// Schedule periodic tasks to run again
			SCH_tasks_G[i].Delay = SCH_tasks_G[i].Period;
			//Inc. the ’RunMe’ flag
			SCH_tasks_G[i].RunMe += 1;
		}
	}
}
void SCH_Dispatch_Tasks(void){
	for(int i = 0; i < current_index_task; i++) {
		if(SCH_tasks_G[i].RunMe > 0) {
			SCH_tasks_G[i].RunMe--;		// Reset / reduce RunMe flag
			(*SCH_tasks_G[i].pTask)();	// Run the task
			// Periodic tasks will automatically run again
			// If this is a "one shot" task, remove it from the array
			if (SCH_tasks_G[i].Period == 0) {
				SCH_Delete_Task(i);
				i--;
			}
		}
	}
}
uint32_t SCH_Add_Task (void(*pFunction)(), uint32_t DELAY, uint32_t PERIOD){
	if(current_index_task < SCH_MAX_TASKS){

		SCH_tasks_G[current_index_task].pTask = pFunction;
		SCH_tasks_G[current_index_task].Delay = DELAY/TIME_CYCLE;
		SCH_tasks_G[current_index_task].Period = PERIOD/TIME_CYCLE;
		SCH_tasks_G[current_index_task].RunMe = 0;
		// Add to the end of array
		SCH_tasks_G[current_index_task].TaskID = current_index_task;
		// Update current_index_task
		current_index_task++;
		return SCH_tasks_G[current_index_task].TaskID;
	}
	return -1;
}
uint8_t SCH_Delete_Task(uint32_t taskID){
	// If no task in array or taskID not exist, return -1
	if (current_index_task == 0 || taskID < 0 || taskID >= SCH_MAX_TASKS) {
		return -1;
	}
	for (int i = taskID; i < SCH_MAX_TASKS-1; i++) {
		// Move task from index i+1 -> i
		SCH_tasks_G[i].pTask = SCH_tasks_G[i+1].pTask;
		SCH_tasks_G[i].Delay = SCH_tasks_G[i+1].Delay;
		SCH_tasks_G[i].Period = SCH_tasks_G[i+1].Period;
		SCH_tasks_G[i].RunMe = SCH_tasks_G[i+1].RunMe;
	}
	// Reset SCH_tasks_G[SCH_MAX_TASKS-1]
	SCH_tasks_G[SCH_MAX_TASKS-1].pTask = 0x0000;
	SCH_tasks_G[SCH_MAX_TASKS-1].Delay = 0;
	SCH_tasks_G[SCH_MAX_TASKS-1].Period = 0;
	SCH_tasks_G[SCH_MAX_TASKS-1].RunMe = 0;
	current_index_task--;
	return taskID;
}
