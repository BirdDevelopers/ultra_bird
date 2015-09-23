
#ifndef UAV_TASK_H
#define UAV_TASK_H

extern "C"{
	#include "FreeRTOS.h"
	#include "task.h"
}

class uav_task
{
public:
	uav_task()
	{
	}
	
	/*void start()
	{
		xTaskCreate(task_fun, "uav task", 100, this, 2, &handle);
	}*/
	
	/*virtual void run() = 0;
	
	static void task_fun(void* parm)
	{
		static_cast<uav_task*>(parm)->run();
  }*/
	
protected:
	xTaskHandle handle;
};

#endif

