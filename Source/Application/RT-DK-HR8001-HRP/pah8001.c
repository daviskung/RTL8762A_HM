#include <stdio.h>
#include "rtl876x.h"
#include "rtl876x_gpio.h"
#include "rtl876x_i2c.h"
#include "rtl876x_uart.h"
#include "rtl_delay.h"
#include "dlps_platform.h"
#include "SimpleBLEPeripheral.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"

//#include "stk8ba50.h"
#include "pah8001.h"

#include "rtl876x_adc.h"
#include "rtl876x_lib_adc.h"

//--------------------------------------------------------------------------
// External data & function
//--------------------------------------------------------------------------

extern xQueueHandle hHeartRateQueueHandle;

extern bool is_heart_rate_service_notification_enabled;
extern bool is_heart_rate_monitor_notification_enabled;

extern bool HeartRateServiceValueNotify(void);

extern xTimerHandle hPAH8001_Timer;
extern xTimerHandle hKEYscan_Timer;



extern uint16_t adcConvertRes_HM[ARY_SIZE];
extern uint8_t	KEYscan_fun_cnt2;



//--------------------------------------------------------------------------
// Global
//--------------------------------------------------------------------------
ppg_mems_data_t _ppg_mems_data, hr_ppg_mems_data;
ppg_mems_data_t __ppg_mems_data[FIFO_SIZE];



uint8_t _frame_Count;
uint8_t _led_step;
uint8_t _led_current_change_flag;
uint8_t _touch_flag;
uint8_t _state, _state_count;
uint8_t _write_index, _read_index;
uint8_t _time_stamp;
uint8_t _sleepflag;
uint8_t _cnt_to_update_heart_rate;
uint8_t update_cnt;
uint8_t ready_flag;

//uint16_t uTxCnt;
//uint8_t uTxBuf[128];
//float _myHR;

bool KEYscan_fun(void)
{
	uint8_t _hr_event;
	//uint8_t counter;

	// Reset Timer 
	xTimerReset(hKEYscan_Timer, KEYscan_Timer_INTERVAL);
		_hr_event = EVENT_SCAN_KEY_TIMER;
	KEYscan_fun_cnt2++;
	// Send Task
	xQueueSend(hHeartRateQueueHandle, &_hr_event, 1);
		return TRUE;
}


/*--------------------------------------------------------------------------
// Function name: bool Pixart_HRD(void)
// Parameters: None
// Return: None
// Description:
//--------------------------------------------------------------------------*/
bool Pixart_HRD(void)
{
	uint8_t _hr_event;

		// Reset Timer 
		xTimerReset(hPAH8001_Timer, 2000);
		//DBG_BUFFER(MODULE_APP, LEVEL_INFO, "**  HRD RTOS timer ** ", 0);
		_hr_event = EVENT_START_HEARTRATE_CALCULATE;

		// Send Task
		xQueueSend(hHeartRateQueueHandle, &_hr_event, 1);
		return TRUE;
}


/*--------------------------------------------------------------------------
// Function name: void LED_Ctrl(void)
// Parameters: None
// Return: None
// Description:
//--------------------------------------------------------------------------*/
void LED_Ctrl(void)
{
	uint8_t _ep_l, _ep_h;
	uint16_t _exposure_line;

	// Disable Sleep Mode (Bank 0)
	//PAH8001_Write(0x05,0x98);		

	if(_sleepflag == 1)
	{
		ExitSleepMode();
	}

	if(_state_count <= STATE_COUNT_TH)
	{
		_state_count++;

		_led_current_change_flag = 0;
	}
	else
	{
		_state_count = 0;

		// Read Exposure Line
		//PAH8001_Read(0x32, &_ep_l);
		//PAH8001_Read(0x33, &_ep_h);
		_exposure_line = (_ep_h<<8)+_ep_l;

		// Change to Bank 1
		//PAH8001_Write(0x7F,0x01);

		if(_state == 0)
		{
			if((_exposure_line >= LED_CTRL_EXPO_TIME_HI_BOUND)||(_exposure_line <= LED_CTRL_EXPO_TIME_LOW_BOUND))
			{
				// Read LED Step (Bank 1)
				//PAH8001_Read(0x38, &_led_step);

				_led_step &= 0x1F;

				if((_exposure_line >= LED_CTRL_EXPO_TIME_HI_BOUND)&&(_led_step < LED_CURRENT_HI))
				{
					_state = 1;

					_led_step = _led_step+LED_INC_DEC_STEP;

					if(_led_step > LED_CURRENT_HI)
						_led_step = LED_CURRENT_HI;

					//PAH8001_Write(0x38,(_led_step|0xE0));

					_led_current_change_flag = 1;
				}
				else if((_exposure_line <= LED_CTRL_EXPO_TIME_LOW_BOUND)&&(_led_step > LED_CURRENT_LOW))
				{
					_state = 2;

					if(_led_step <= (LED_CURRENT_LOW+LED_INC_DEC_STEP))
						_led_step = LED_CURRENT_LOW;
					else
						_led_step = _led_step-LED_INC_DEC_STEP;

					//PAH8001_Write(0x38,(_led_step|0xE0));

					_led_current_change_flag = 1;
				}
				else
				{
					_state = _led_current_change_flag = 0;
				} 
			} 
			else
			{
				_led_current_change_flag = 0;
			}
		}
		else if(_state == 1)
		{
			if(_exposure_line > LED_CTRL_EXPO_TIME_HI)
			{
				_state = 1;

				_led_step = _led_step+LED_INC_DEC_STEP;

				if(_led_step >= LED_CURRENT_HI)
				{
					_state = 0;
					_led_step = LED_CURRENT_HI;
				}

				// Change LED Step (Bank 1)
				//PAH8001_Write(0x38,(_led_step|0xE0));

				 _led_current_change_flag = 1;
			}
			else
			{
				_state = _led_current_change_flag = 0;
			}
		}
		else
		{
			if(_exposure_line < LED_CTRL_EXPO_TIME_LOW)
			{
				_state = 2 ;

				if(_led_step <= (LED_CURRENT_LOW+LED_INC_DEC_STEP))
				{
					_state = 0;
					_led_step = LED_CURRENT_LOW;
				}
				else
				{
					_led_step = _led_step-LED_INC_DEC_STEP;
				}

				// Change LED Step (Bank 1)
				//PAH8001_Write(0x38,(_led_step|0xE0));

				_led_current_change_flag = 1;
			}     
			else
			{
				_state = _led_current_change_flag = 0;
			}
		}
	}
}


/*--------------------------------------------------------------------------
// Function name: void EnterSleepMode(void)
// Parameters: None
// Return: None
// Description:
//--------------------------------------------------------------------------*/
void EnterSleepMode(void)
{
	// Enable Sleep Mode (Bank 0)
	//PAH8001_Write(0x7F,0x00);
	//PAH8001_Write(0x05,0xBC);

	// Change LED Step (Bank 1)
	//PAH8001_Write(0x7F,0x01);
	//PAH8001_Write(0x38,(DEFAULT_LED_STEP|0xE0));

	// Set Suspend Mode
	//STK8BA50_Write(REG_POWMODE, SUSPEND);

	// Reset Algorithm
	PxiAlg_Close();

	_led_step = DEFAULT_LED_STEP;

	_led_current_change_flag = 0;

	_frame_Count = 0;

	_time_stamp = 0;

	//_myHR = 0;

	_sleepflag = 1;
}


/*--------------------------------------------------------------------------
// Function name: void ExitSleepMode(void)
// Parameters: None
// Return: None
// Description:
//--------------------------------------------------------------------------*/
void ExitSleepMode(void)
{
	// Change LED Step (Bank 1)
	//PAH8001_Write(0x7F,0x01);
	//PAH8001_Write(0x38,(DEFAULT_LED_STEP|0xE0));

	// Set Normal Mode
	//STK8BA50_Write(REG_POWMODE, NORMAL_PWR);

	_cnt_to_update_heart_rate = 0;

	_write_index = _read_index = 0;

	_state_count = _state = 0;

	_sleepflag = 0;
}


/*--------------------------------------------------------------------------
// Function name: uint8_t AddCntToUpdate(void)
// Parameters: None
// Return: None
// Description:
//--------------------------------------------------------------------------*/
uint8_t AddCntToUpdate(void)
{
	return(_cnt_to_update_heart_rate++);	
}


/*--------------------------------------------------------------------------
// Function name: void ClearCntToUpdate(void)
// Parameters: None
// Return: None
// Description:
//--------------------------------------------------------------------------*/
void ClearCntToUpdate(void)
{
	_cnt_to_update_heart_rate = 0;	
}


/*--------------------------------------------------------------------------
// Function name: bool isFIFOEmpty(void)
// Parameters: None
// Return: None
// Description:
//--------------------------------------------------------------------------*/
bool isFIFOEmpty(void)
{
	return(_write_index == _read_index); 
}


/*--------------------------------------------------------------------------
// Function name: bool Push(ppg_mems_data_t *data)
// Parameters: None
// Return: None
// Description:
//--------------------------------------------------------------------------*/
bool Push(ppg_mems_data_t *data)
{
	uint8_t tmp;

	tmp = _write_index;

	tmp++;

	if(tmp >= FIFO_SIZE)
		tmp = 0;

	if(tmp == _read_index)
		return FALSE;

	__ppg_mems_data[tmp] = *data;

	_write_index = tmp;

	return TRUE;
}


/*--------------------------------------------------------------------------
// Function name: bool Pop(ppg_mems_data_t *data)
// Parameters: None
// Return: None
// Description:
//--------------------------------------------------------------------------*/
bool Pop(ppg_mems_data_t *data)
{
	uint8_t tmp;

	if(isFIFOEmpty())
		return FALSE;

	*data = __ppg_mems_data[_read_index];

	tmp = _read_index + 1;

	if(tmp >= FIFO_SIZE)
		tmp = 0;

	_read_index = tmp;

	return TRUE;
}

/*--------------------------------------------------------------------------
// Function name: void Get_AR_ADC(void)
// Parameters: None
// Return: None
// Description:
//--------------------------------------------------------------------------*/


void Get_AR_ADC(void)
{

	for(uint8_t i =0; i<ARY_SIZE;i++)
	{
	   DBG_DIRECT("adcConvertRes_HM[%d] = %d ",i,adcConvertRes_HM[i]);
	   if(i%10 == 9)
	   DBG_DIRECT("\n");
	}
	
   /* start adc convert again */
				  
	DBG_DIRECT(" ********************* \n");

 	return;
	
}


/*--------------------------------------------------------------------------
// Function name: void CalculateHeartRate(void)
// Parameters: None
// Return: None
// Description:
//--------------------------------------------------------------------------*/

//#define DEBUG_PRINT_HR_RAW_DATA

void CalculateHeartRate(void)
{
	//DBG_BUFFER(MODULE_APP, LEVEL_INFO, "**  CalculateHeartRate ** ", 0);
	if(is_heart_rate_service_notification_enabled == TRUE)
	{
		
		HeartRateServiceValueNotify();

	}
		
}



