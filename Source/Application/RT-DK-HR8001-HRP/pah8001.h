#ifndef __PAH8001_H__
#define __PAH8001_H__

#include "rtl876x.h"

//--------------------------------------------------------------------------
// Constant Define
//--------------------------------------------------------------------------
#define PXIALG_API

#define FIFO_SIZE			32

#define LED_INC_DEC_STEP	2
#define LED_CTRL_EXPO_TIME_HI_BOUND		496
#define LED_CTRL_EXPO_TIME_LOW_BOUND	32
#define LED_CTRL_EXPO_TIME_HI	420
#define LED_CTRL_EXPO_TIME_LOW	64
#define LED_CURRENT_HI		31
#define LED_CURRENT_LOW		1

#define STATE_COUNT_TH		3

#define DEFAULT_LED_STEP	4

#define EVERY_N_PPG_SAMPLES_TO_UPDATE_CHART	23

#define SENSOR_PAH8001_INTERVAL 500    // 500 msec
#define SENSOR_PAH8001_TIMER_ID 3552

#define KEYscan_Timer_INTERVAL 	250    //  250ms
#define KEYscan_Timer_ID		3554


#define EVENT_START_HEARTRATE_CALCULATE 1
//#define EVENT_START_AR_ADC				2
//#define EVENT_ADC_CONVERT_BUF_FULL		3
#define EVENT_SCAN_KEY_TIMER			4
#define EVENT_RxEndFlag_SET				5
#define EVENT_PWR_KEY_PUSH_SET			6
#define EVENT_GAPSTATE_CONNECTED		7

#define BTCONN_GAPSTATE_ADVERTISING			8
#define BTCONN_GAPSTATE_CONNECTED		9


#define USB_5V_IN_SET	1
#define BAT_IN_SET		2


#define ARY_SIZE		100

#define NO_SIGNAL_SHUTDOWN_CNT_SET			35

/* Defines ------------------------------------------------------------------*/

//#define LED1_Pin            P0_3	// LOG_UART_TX can not use
//#define GPIO_LED1_Pin       GPIO_GetPin(LED1_Pin)

#define S4_TEST_KEY_PIN            P4_2
#define GPIO_S4_TEST_KEY_PIN       GPIO_GetPin(S4_TEST_KEY_PIN)
//#define GPIOTestIntrHandler Gpio1IntrHandler

#define PWR_KEY_PIN                P4_3
#define GPIO_PWR_KEY_PIN           GPIO_GetPin(PWR_KEY_PIN)
//#define GPIOTestIntrHandler Gpio1IntrHandler

#define STATUS_LED_PIN             P0_2
#define GPIO_STATUS_LED_PIN        GPIO_GetPin(STATUS_LED_PIN)

#define BAT_VOLT_IN_CONT_PIN       P2_0
#define GPIO_BAT_VOLT_IN_CONT_PIN  GPIO_GetPin(BAT_VOLT_IN_CONT_PIN)


#define PWR_CONTROL_PIN            P3_2
#define GPIO_PWR_CONTROL_PIN       GPIO_GetPin(PWR_CONTROL_PIN)

#define USB_V5_IN_PIN              P2_7
#define GPIO_USB_V5_IN_PIN         GPIO_GetPin(USB_V5_IN_PIN)

#define DTAT_UART_TX_Pin            P4_0
#define DTAT_UART_RX_Pin            P4_1

#define HCI_UART_TX_Pin            P3_0
#define HCI_UART_RX_Pin            P3_1






/* TIM Defines ------------------------------------------------------------------*/
#define TIM_ID                  TIM2







//--------------------------------------------------------------------------
//  Structure
//--------------------------------------------------------------------------
typedef struct
{
	uint8_t HRD_Data[13];
	float MEMS_Data[3];
}ppg_mems_data_t;


//--------------------------------------------------------------------------
//  Enum
//--------------------------------------------------------------------------
typedef enum
{
	FLAG_DATA_READY = 0,
	FLAG_DATA_NOT_READY,
	FLAG_DATA_LOSS,
	FLAG_NO_TOUCH,
	FLAG_DATA_ERROR,
	FLAG_POOR_SIGNAL,
	FLAG_FIFO_ERROR,
	FLAG_TIMING_ERROR
}PXI_STATUS_FLAG;


//--------------------------------------------------------------------------
// Prototypes
//--------------------------------------------------------------------------
void PAH8001_Settings(void);
bool Pixart_HRD(void);
void LED_Ctrl(void);
void EnterSleepMode(void);
void ExitSleepMode(void);
uint8_t AddCntToUpdate(void);
void ClearCntToUpdate(void);
void PushHeart(float myHR);

bool isFIFOEmpty(void);
bool Push(ppg_mems_data_t *data);
bool Pop(ppg_mems_data_t *data);

void CalculateHeartRate(void);

bool AR_ADC_CH1(void);
void Get_AR_ADC(void);
bool KEYscan_fun(void);


void GainDelay (void);	// changing GUD or GCS requires a delay depending on digital pot chip
void GainStepUp (void);
void GainStepDown (void);
void InitGain(UINT8 val);
void GainUp (void);
void GainDown (void);






//--------------------------------------------------------------------------
// Library API
//--------------------------------------------------------------------------
/**
 * @brief When HRD and MEMS data are ready, call this function to do the algorithm processing
 *
 * @param[in] HRD_Data   Pointer to the buffer where HRD data (13 Bytes) is stored.
 * @param[in] MEMS_Data  Pointer to the buffer where MEMS data (3*sizeof(float) Bytes) is stored.
 *
 * @return Return one of the PXI_STATUS_FLAG types.
 */
PXIALG_API int32_t PxiAlg_Process(unsigned char *HRD_Data, float *MEMS_Data);

/**
 * @brief Call this function to get Heart Rate
 *
 * @param[out] hr   Pointer to a float variable where heart rate is stored
 *
 * @return None
 */
PXIALG_API void PxiAlg_HrGet(float *hr);

/**
 * @brief Call this function to determine the version of the algorithm
 *
 *
 * @return Version of the algorithm
 */
PXIALG_API int32_t PxiAlg_Version(void);

/**
 * @brief Call this funtion to get Ready_Flag
 *
 * @return Return Ready_Flag
 */
PXIALG_API unsigned char PxiAlg_GetReadyFlag(void);

/**
 * @brief Call this funtion to get Motion_Flag
 *
 * @return Return Motion_Flag
 */
PXIALG_API unsigned char PxiAlg_GetMotionFlag(void);

/**
 * @brief Call this function to notify algorithm the MEMS Scale of Motion Sensor
 *
 * @param[in] scale The MEMS Scale of Motion Sensor. Only 0(2G),1(4G~16G) are supported.
 *
 * @return 1 for success. 0 for failure.
 */
PXIALG_API uint8_t PxiAlg_SetMemsScale(int scale);

/**
 * @brief Call this function to get PPG Signal Grade
 *
 * @param[out] grade	Pointer to a float variable where signal grade is stored.
 *
 * @return  Return 1 when Signal Grade is ready. Otherwise, return 0.
 */
PXIALG_API uint8_t PxiAlg_GetSigGrade(float *grade);

/**
 * @brief Call this function to set PPG Signal Grade Threshold
 *
 * @param[in] threshold	The PPG Signal Grade Threshold. Its value ranges from 0 to 100.
 *
 * @return 1 for success. 0 for failure.
 */
PXIALG_API uint8_t PxiAlg_SetSigGradeThrd(float thrd);

/**
 * @brief Call this function to enable or disable fast output mode
 *
 * @param[in] en The flag of fast output mode.
 */
PXIALG_API void PxiAlg_EnableFastOutput(uint8_t en);

/**
 * @brief Call this function to enable or disable motion mode
 *
 * @param[in] en The flag of motion mode.
 */
PXIALG_API void PxiAlg_EnableMotionMode(uint8_t en);

/**
 * @brief Call this function to enable or disable auto mode
 * @param[in] enable The flag of auto mode.
 */
PXIALG_API void PxiAlg_EnableAutoMode(uint8_t en);

/**
 * @brief Call this function to open algorithm
 */
PXIALG_API void PxiAlg_Open(void);

/**
 * @brief Call this function to close/reset algorithm
 */
PXIALG_API void PxiAlg_Close(void);

/**
 * @brief Call this function to get the alarm flag of fast output
* @param[out] get the alarm flag of fast output
 * @return  Return 1 when the flag is set. Otherwise, return 0.
 */
PXIALG_API uint8_t PxiAlg_GetFastOutAlarmFlag(void);

/**
 * @brief Call this function to enable or disable mems0 signal grade mode
 * @param[in] 1 : enable. 0 : disable.
 */
PXIALG_API void PxiAlg_EnableMEMS0SigGrade(uint8_t en);

/**
 * @brief Call this function to get the alarm flag of signal bad
 * @param[out] get the alarm flag of bad signal
 * @return  Return 1 when the flag is set. Otherwise, return 0.
 */
PXIALG_API uint8_t PxiAlg_GetSignalBadAlarm(void);

/**  * @brief Call this function to set PPG buffer pointer and size
 * @param[in] ppg_buffer   Pointer to a float buffer where the PPG data would be stored
 * @param[in] size   PPG buffer size  
 * @return None  */ 
PXIALG_API void PxiAlg_SetPPGBuffer(float *ppg_buffer, int32_t size);

#endif
