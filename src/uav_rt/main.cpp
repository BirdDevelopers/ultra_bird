
#include <stm32f10x.h>

/* Scheduler includes. */
extern "C"{
	#include "FreeRTOS.h"
	#include "task.h"
	#include "queue.h"
	#include <freertos/cli/FreeRTOS_CLI.h>
};

#include "led_control_task.h"
#include "../led_driver.h"
#include "../serial_stream.h"
#include "uav_device.h"
#include <string.h>


#define DEBUG 1
/* Task priorities. */
#define mainQUEUE_POLL_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3 )
#define mainFLASH_TASK_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainCOM_TEST_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainINTEGER_TASK_PRIORITY           ( tskIDLE_PRIORITY )

/* Constants related to the LCD. */
#define mainMAX_LINE						( 240 )
#define mainROW_INCREMENT					( 24 )
#define mainMAX_COLUMN						( 20 )
#define mainCOLUMN_START					( 319 )
#define mainCOLUMN_INCREMENT 				( 16 )

/* The maximum number of message that can be waiting for display at any one
time. */
#define mainLCD_QUEUE_SIZE					( 3 )

/* The check task uses the sprintf function so requires a little more stack. */
#define mainCHECK_TASK_STACK_SIZE			( configMINIMAL_STACK_SIZE + 50 )

/* Dimensions the buffer into which the jitter time is written. */
#define mainMAX_MSG_LEN						25

/* The time between cycles of the 'check' task. */
#define mainCHECK_DELAY						( ( TickType_t ) 5000 / portTICK_PERIOD_MS )

/* The number of nano seconds between each processor clock. */
#define mainNS_PER_CLOCK ( ( unsigned long ) ( ( 1.0 / ( double ) configCPU_CLOCK_HZ ) * 1000000000.0 ) )

/* Baud rate used by the comtest tasks. */
#define mainCOM_TEST_BAUD_RATE		( 115200 )

/* The LED used by the comtest tasks. See the comtest.c file for more
information. */
#define mainCOM_TEST_LED			( 3 )

uav_device * pUavDevice;

/*int fputc( int ch, FILE *f )
{
	return 0;
}*/

int stdout_putchar (int ch)
{
	return 0;	
}

int uav_rt_main();

static BaseType_t prvSetLedMode( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	const char *pLedParam, *pModeParam;
	BaseType_t led_param_len, mode_param_len;
	

	
	pLedParam = FreeRTOS_CLIGetParameter(pcCommandString, 1, &led_param_len);
	pModeParam = FreeRTOS_CLIGetParameter(pcCommandString, 2, &mode_param_len);
	
	led_base::mode_t mode = led_base::mode_t::invalid;

	if(!memcmp(pModeParam, "flash", 5))
		mode = led_base::mode_t::flash;
	else if(!memcmp(pModeParam, "dark", 4))
		mode = led_base::mode_t::dark;
	else if(!memcmp(pModeParam, "blink", 5))
		mode = led_base::mode_t::blink;
	else if(!memcmp(pModeParam, "slow_blink", 10))
		mode = led_base::mode_t::slow_blink;
		
	return pdFALSE;
}

static const CLI_Command_Definition_t xSetLedMode =
{
	"set_led_mode",
	"\r\nset_led_mode [main | radio1 | radio2] [flash | blink | slow_blink | dark ]:\r\n Sets led mode\r\n\r\n",
	prvSetLedMode, /* The function to run. */
	2 /* One parameter is expected.  Valid values are "start" and "stop". */
};

template<typename S>
void console_task( void *pvParameters )
{
	static const char * const pcWelcomeMessage = "FreeRTOS command server.\r\nType Help to view a list of registered commands.\r\n";
	
	S * pConsole = static_cast<S*>(pvParameters);
	
	*pConsole << pcWelcomeMessage << serial_stream::endl;
	
	const unsigned MAX_OUTPUT_LENGTH = 4096;
	static char pcOutputString[ MAX_OUTPUT_LENGTH ];
	
	BaseType_t xMoreDataToFollow;
	
	while(true)
	{
		if(pConsole->is_ready())
		{
			do
			{
				xMoreDataToFollow = FreeRTOS_CLIProcessCommand(pConsole->buffer(), pcOutputString, MAX_OUTPUT_LENGTH);
				*pConsole << pcOutputString;
			} while( xMoreDataToFollow != pdFALSE );
			pConsole->clear_buffer();
		}
		
		vTaskDelay(600);
	}
}


int main( void )
{
#ifdef DEBUG
//  debug();
#endif
	
	pUavDevice = new uav_device();
	//led_driver<led_driver_stm32> l1;
	//l1.set();
	
	//xTaskCreate( led_blink_task, "led_blink", mainCHECK_TASK_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL );
	//using led_t = led_driver<led_driver_stm32>;
	using led_t = led_driver_stm32;
	led_t * pl1 = new led_t;
	pl1->set_mode(led_base::mode_t::blink);
	
	xTaskCreate(led_control_task<led_t>, "led_blink", mainCHECK_TASK_STACK_SIZE, pl1, mainCHECK_TASK_PRIORITY, NULL);
	
	
	serial_stream_stm32 * p_uart0_stream = new serial_stream_stm32;
	xTaskCreate(console_task<serial_stream_stm32>, "console", mainCHECK_TASK_STACK_SIZE, p_uart0_stream, mainCHECK_TASK_PRIORITY, NULL);
	
	/*led_control_task<led_t> green_led_task(&l1);
	green_led_task.start();*/
	FreeRTOS_CLIRegisterCommand(&xSetLedMode);
	
	vTaskStartScheduler();
	
	return 0;
}
