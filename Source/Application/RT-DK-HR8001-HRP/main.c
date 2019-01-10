/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     main.c
* @brief    this is the main file of OTA_demo project
* @details
* @author   hunter_shuai
* @date     14-July-2015
* @version  v1.0.0
******************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
******************************************************************************
*/

#include "rtl876x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "application.h"
#include "dlps_platform.h"
#include "peripheral.h"
#include "gap.h"
#include "gapbondmgr.h"
#include "profileApi.h"
#include "simpleBLEPeripheral.h"
#include "rtl876x_io_dlps.h"
#include "timers.h"

#include "ota_application.h"
#include "ota_service.h"
#include "hrm_service.h"
#include "rtl876x_adc.h"


#include "bas.h"
#include "dis.h"
#include "hrs.h"

#include <stdio.h>
#include "rtl876x_rcc.h"
#include "rtl876x_gpio.h"
#include "rtl876x_i2c.h"
#include "rtl876x_uart.h"
#include "rtl876x_nvic.h"
#include "rtl_delay.h"
//#include "stk8ba50.h"

#include "rtl876x_pwm.h"
#include "rtl876x_pinmux.h"
#include "pah8001.h"

//#include "rtl876x_PWM.h"
#include	"rtl876x_it.h"





/*
********************************************************
* parameter for btstack
*
*********************************************************
*/

#define Timer2MsgControl	1

// What is the advertising interval when device is discoverable. (units of 625us)
#define MY_ADVERTISING_INTERVAL_MIN	0x190 /* 250ms */
#define MY_ADVERTISING_INTERVAL_MAX	0x1B0 /* 270ms */


// What is the connection interval when device is connected. (units of 1.25ms)
//#define MY_CONNECTION_INTERVAL_MIN	0xC8 /* 250ms */
//#define MY_CONNECTION_INTERVAL_MAX	0xD8 /* 270ms */
#define MY_CONNECTION_INTERVAL_MIN	0x06 /* 7.5ms */
#define MY_CONNECTION_INTERVAL_MAX	0x0C80 /* 4000ms */

// service id
uint8_t gDISServiceId;
uint8_t gBASServiceId;
uint8_t gHRSServiceId;
uint8_t gOTAServiceId;
uint8_t gHRMServiceId;


extern xTimerHandle hPAH8001_Timer;
extern uint16_t adcConvertRes_HM[ARY_SIZE] = {0};
extern uint8_t	HM_100ms_cnt=0;

extern uint8_t	NSTROBE_PWM_cnt=0;
extern uint8_t	NSTROBE_LOW_EndSet=0;
extern uint32_t GPIO_NSTROBE_value = 0;



extern uint8_t PWM_chanEN_Number = 1; //NSTROBE_R1_Pin =1 ... NSTROBE_R4_Pin=4

extern uint8_t RxEndFlag;
/* globals */
extern uint8_t RxBuffer[32];
extern uint8_t RxCount;
extern uint8_t RxEndFlag;

//extern UINT8 AGC_MCP4011_Gain;  // current setting


#define  GATT_UUID128_OTA_SERVICE_ADV	0x12, 0xA2, 0x4D, 0x2E, 0xFE, 0x14, 0x48, 0x8e, 0x93, 0xD2, 0x17, 0x3C, 0xFF, 0xD0, 0x00, 0x00

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
	/* place holder for Local Name, filled by BT stack. if not present */
	/* BT stack appends Local Name.                                    */

#if 0
	0x03,	// Length
	GAP_ADTYPE_APPEARANCE,
	LO_WORD(GATT_APPEARANCE_UNKNOWN),
	HI_WORD(GATT_APPEARANCE_UNKNOWN),

	/* DIS Service */
	0x03,
	GAP_ADTYPE_16BIT_COMPLETE,
	LO_WORD(GATT_UUID_DEVICE_INFORMATION_SERVICE),
	HI_WORD(GATT_UUID_DEVICE_INFORMATION_SERVICE),

	/* HRS Service */
	0x03,
	GAP_ADTYPE_16BIT_COMPLETE,
	LO_WORD(GATT_UUID_SERVICE_HEART_RATE),
	HI_WORD(GATT_UUID_SERVICE_HEART_RATE),
#endif


	/* HRS Service */
		0x09,
		GAP_ADTYPE_16BIT_COMPLETE,
		LO_WORD(GATT_UUID_SERVICE_HEART_RATE),
		HI_WORD(GATT_UUID_SERVICE_HEART_RATE),
		LO_WORD(GATT_UUID_DEVICE_INFORMATION_SERVICE),
		HI_WORD(GATT_UUID_DEVICE_INFORMATION_SERVICE),
		LO_WORD(GATT_UUID_BATTERY),
		HI_WORD(GATT_UUID_BATTERY),
		0xE8,0xFE,					// What is this ?
	
		0x05,
		GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
		LO_WORD(MY_CONNECTION_INTERVAL_MIN),
		HI_WORD(MY_CONNECTION_INTERVAL_MIN),
		LO_WORD(MY_CONNECTION_INTERVAL_MAX),
		HI_WORD(MY_CONNECTION_INTERVAL_MAX)

	/*
	0x08,	// Length
	GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'H', 'r', 't', 'D', 'e', 'm', 'o'
	*/
	
	/* iOS藍牙設定裡顯示藍牙廣播裝置名稱 :
		差異在SCAN response data加入HRS serive ID與否，
		會在iOS的藍牙設定控制裡面顯示GAP device name. 
		Android是直接顯示廣播的名稱。
	0x14,	// Length
	GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	'H','e','a','r', 't','M','a','t','h','-','H', 'R', 'V','-','C','7','7','7','7'
	*/
};


// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
uint8_t advertData[] =
{
	/* Core spec. Vol. 3, Part C, Chapter 18 */
	/* Flags */
	0x02,
	GAP_ADTYPE_FLAGS,
	//GAP_ADTYPE_FLAGS_LIMITED|GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED, // 2019.01.07 change
	GAP_ADTYPE_FLAGS_GENERAL|GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

	
	
	0x14,	// Length
	//GAP_ADTYPE_LOCAL_NAME_COMPLETE,
	GAP_ADTYPE_LOCAL_NAME_SHORT,
	'H','e','a','r', 't','M','a','t','h','-','H', 'R', 'V','-','C','0','0','0','0'

};


/******************************************************************
 * @fn         Initial gap parameters
 * @brief      Initialize peripheral and gap bond manager related parameters
 *
 * @return     void
 */
void BtStack_Init_Gap(void)
{

	
	uint8_t mac_addr[6];

    //device name and device appearance
    uint8_t DeviceName[GAP_DEVICE_NAME_LEN] = "HeartMath-HRV";
    uint16_t Appearance = GAP_GATT_APPEARANCE_UNKNOWN;

    //default start adv when bt stack initialized
    uint8_t  advEnableDefault = TRUE;

    //advertising parameters
    uint8_t  advEventType = GAP_ADTYPE_ADV_IND;
    uint8_t  advDirectType = PEER_ADDRTYPE_PUBLIC;
    uint8_t  advDirectAddr[B_ADDR_LEN] = {0};
    uint8_t  advChanMap = GAP_ADVCHAN_ALL;
    uint8_t  advFilterPolicy = GAP_FILTER_POLICY_ALL;

    uint16_t advIntMin = MY_ADVERTISING_INTERVAL_MIN;
    uint16_t advIntMax = MY_ADVERTISING_INTERVAL_MAX;
    uint16_t conIntMin = MY_CONNECTION_INTERVAL_MIN;
    uint16_t conIntMax = MY_CONNECTION_INTERVAL_MAX;

    //GAP Bond Manager parameters
    uint8_t pairMode 	= GAPBOND_PAIRING_MODE_PAIRABLE;
    uint8_t mitm 			= GAPBOND_AUTH_YES_MITM_YES_BOND;
    uint8_t ioCap 		= GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t oobEnable = FALSE;
    uint32_t passkey = 0; // passkey "000000"

    uint8_t bUseFixedPasskey = TRUE;

    //Set device name and device appearance
    peripheralSetGapParameter(GAPPRRA_DEVICE_NAME, GAP_DEVICE_NAME_LEN, DeviceName);
    peripheralSetGapParameter(GAPPRRA_APPEARANCE, sizeof(Appearance), &Appearance);

    peripheralSetGapParameter(GAPPRRA_ADV_ENABLE_DEFAULT, sizeof ( advEnableDefault ), &advEnableDefault );

    //Set advertising parameters
    peripheralSetGapParameter(GAPPRRA_ADV_EVENT_TYPE, sizeof ( advEventType ), &advEventType );
    peripheralSetGapParameter(GAPPRRA_ADV_DIRECT_ADDR_TYPE, sizeof ( advDirectType ), &advDirectType );
    peripheralSetGapParameter(GAPPRRA_ADV_DIRECT_ADDR, sizeof ( advDirectAddr ), advDirectAddr );
    peripheralSetGapParameter(GAPPRRA_ADV_CHANNEL_MAP, sizeof ( advChanMap ), &advChanMap );
    peripheralSetGapParameter(GAPPRRA_ADV_FILTER_POLICY, sizeof ( advFilterPolicy ), &advFilterPolicy );

    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MIN, sizeof(advIntMin), &advIntMin );
    peripheralSetGapParameter(GAPPRRA_ADV_INTERVAL_MAX, sizeof(advIntMax), &advIntMax );

    peripheralSetGapParameter(GAPPRRA_MIN_CONN_INTERVAL, sizeof(conIntMin), &conIntMin );
    peripheralSetGapParameter(GAPPRRA_MAX_CONN_INTERVAL, sizeof(conIntMax), &conIntMax );

	
	GAP_GetParameter(GAPPARA_BD_ADDR, &mac_addr[0]);
	//DBG_BUFFER(MODULE_APP, LEVEL_INFO, " **S4_TEST_KEY = %d \n", 1,key_cnt);
	DBG_BUFFER(MODULE_APP,LEVEL_INFO," Davis Local BD-- 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x",6,
                    mac_addr[5],
                    mac_addr[4],
                    mac_addr[3],
                    mac_addr[2],
                    mac_addr[1],
                    mac_addr[0]);


	advertData[20] = (( mac_addr[1] & 0xf0 ) >>4) + '0';
	advertData[21] = ( mac_addr[1] & 0x0f ) + '0';
	advertData[22] = (( mac_addr[0] & 0xf0 ) >>4) + '0';
	advertData[23] = ( mac_addr[0] & 0x0f ) + '0';

    peripheralSetGapParameter(GAPPRRA_ADVERT_DATA, sizeof( advertData ), advertData );
    peripheralSetGapParameter(GAPPRRA_SCAN_RSP_DATA, sizeof ( scanRspData ), scanRspData );

    // Setup the GAP Bond Manager
    GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof ( uint8_t ), &pairMode );
    GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof ( uint8_t ), &mitm );
    GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof ( uint8_t ), &ioCap );
    GAPBondMgr_SetParameter(GAPBOND_OOB_ENABLED, sizeof ( uint8_t ), &oobEnable );

    GAPBondMgr_SetParameter(GAPBOND_PASSKEY, sizeof ( uint32_t ), &passkey );
    GAPBondMgr_SetParameter(GAPBOND_FIXED_PASSKEY_ENABLE, sizeof ( uint8_t ), &bUseFixedPasskey);
}


/******************************************************************
 * @fn         Initial profile
 * @brief      Add simple profile service and register callbacks
 *
 * @return     void
 */
void BtProfile_Init(void)
{
	gDISServiceId = DIS_AddService(AppProfileCallback);
	gBASServiceId = BAS_AddService(AppProfileCallback);
	gHRSServiceId = HRS_AddService(AppProfileCallback);
	gOTAServiceId = OTAService_AddService(AppProfileCallback);
	gHRMServiceId = HeartService_AddService(AppProfileCallback);
	ProfileAPI_RegisterCB(AppProfileCallback);
}

void Initialize_ADC(void)
{
	
	 /* turn on ADC clock */
    RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);

	
	/* ADC pad config */
	Pad_Config(P2_5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);

    /* ADC init */
    ADC_InitTypeDef adcInitStruct;

    ADC_StructInit(&adcInitStruct);

    /* use channel battery */
    //adcInitStruct.channelMap = ADC_CH_BAT;
    adcInitStruct.channelMap = ADC_CH5 ; 
	
    ADC_Init(ADC, &adcInitStruct);
    ADC_INTConfig(ADC, ADC_INT_ONE_SHOT_DONE, ENABLE); //2019.1.4 

	
	DBG_BUFFER(MODULE_APP, LEVEL_INFO, " battery ADC set OK !	\n", 0);
}

/******************************************************************
 * @fn         Initial GPIO
 * @brief      GPIO input & output setting
 * @return     void
 */
void Initialize_GPIO(void)
{
	/* turn on GPIO clock */
	RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
	
	/* GPIO Pinmux Config */
	
	Pinmux_Config(PWR_CONTROL_PIN	, GPIO_FUN);
	Pinmux_Config(STATUS_LED_PIN	, GPIO_FUN);
	
	Pinmux_Config(S4_TEST_KEY_PIN	, GPIO_FUN);
	Pinmux_Config(PWR_KEY_PIN		, GPIO_FUN);
	
	Pinmux_Config(USB_V5_IN_PIN		, GPIO_FUN);
	

	/* GPIO Pad Config */
	
	// OUTPUT mode
	Pad_Config(STATUS_LED_PIN    , PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(PWR_CONTROL_PIN   , PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(S4_TEST_KEY_PIN   , PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	
	// INPUT mode
	//Pad_Config(S4_TEST_KEY_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
	Pad_Config(PWR_KEY_PIN    , PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);

	Pad_Config(USB_V5_IN_PIN    , PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
	
	/* GPIO Parameter Config */
	{
		GPIO_InitTypeDef GPIO_InitStruct;
		
		GPIO_InitStruct.GPIO_Pin  = GPIO_STATUS_LED_PIN|GPIO_S4_TEST_KEY_PIN|GPIO_PWR_CONTROL_PIN;
    	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	    GPIO_InitStruct.GPIO_ITCmd = DISABLE;
		GPIO_Init(&GPIO_InitStruct);
				
		//GPIO_InitStruct.GPIO_Pin  = GPIO_S4_TEST_KEY_PIN|GPIO_USB_V5_IN_PIN;
		GPIO_InitStruct.GPIO_Pin  = GPIO_USB_V5_IN_PIN;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	    GPIO_InitStruct.GPIO_ITCmd = DISABLE;
		GPIO_Init(&GPIO_InitStruct);
	
		GPIO_InitStruct.GPIO_Pin  = GPIO_PWR_KEY_PIN;
	    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_ITCmd = ENABLE;
    	GPIO_InitStruct.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
	    GPIO_InitStruct.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_HIGH;
	    GPIO_InitStruct.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_ENABLE;  
	    GPIO_Init(&GPIO_InitStruct);

		// Turn Off INT frist
	    GPIO_INTConfig(GPIO_PWR_KEY_PIN, DISABLE);		
	    GPIO_MaskINTConfig(GPIO_PWR_KEY_PIN, ENABLE);
	
		DBG_BUFFER(MODULE_APP, LEVEL_INFO, " GPIO set OK !	\n", 0);
	}

}




/******************************************************************
 * @fn         Initial I2C
 * @brief      I2C peripheral
 * @return     void
 */
void Initialize_I2C(void)
{
	/* turn on I2C clock */
	RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, ENABLE);

	/* I2C Pinmux Config */
	Pinmux_Config(P2_4, I2C0_DAT);
	Pinmux_Config(P2_5, I2C0_CLK);

	/* I2C Pad Config */
	Pad_Config(P2_4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(P2_5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);

	/* I2C Parameter Config */
	{
		I2C_InitTypeDef I2C_InitStructure;

		I2C_InitStructure.I2C_ClockSpeed = 400000;
		I2C_InitStructure.I2C_DeviveMode = I2C_DeviveMode_Master;
		I2C_InitStructure.I2C_AddressMode = I2C_AddressMode_7BIT;
		I2C_InitStructure.I2C_SlaveAddress = 0;
		I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
		I2C_Init(I2C0, &I2C_InitStructure);
		I2C_Cmd(I2C0, ENABLE);
	}
}


/******************************************************************
 * @fn         Initial UART
 * @brief      UART peripheral
 * @return     void
 */
void Initialize_UART(void)
{
	/* turn on UART clock */
	RCC_PeriphClockCmd(APBPeriph_UART, APBPeriph_UART_CLOCK, ENABLE);

	/* UART Pinmux Config */
	Pinmux_Config(DTAT_UART_TX_Pin, DATA_UART_TX);
	Pinmux_Config(DTAT_UART_RX_Pin, DATA_UART_RX);

	/* UART Pad Config */
	Pad_Config(DTAT_UART_TX_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
	Pad_Config(DTAT_UART_RX_Pin, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

	/* UART Parameter Config */ 
	/* 115200-N-8-1 */
	{
		UART_InitTypeDef uartInitStruct;

		UART_StructInit(&uartInitStruct);

		UART_Init(UART, &uartInitStruct);
	}
}


/******************************************************************
 * @fn         Initial NVIC
 * @brief      Interrupt setting
 * @return     void
 */
void Initialize_NVIC(void)
{

	NVIC_InitTypeDef NVIC_InitStruct;
    
    /* ADC IRQ */  
    NVIC_InitStruct.NVIC_IRQChannel = ADC_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; //2019.1.4
    
    NVIC_Init(&NVIC_InitStruct);
	
    /* UART IRQ */
    NVIC_InitStruct.NVIC_IRQChannel = UART_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    
    NVIC_Init(&NVIC_InitStruct);

	
    /* PWR_KEY IRQ */
	NVIC_InitStruct.NVIC_IRQChannel = GPIO6To31_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);  
	
    return;
}


/**
* @brief    Board_Init() contains the initialization of pinmux settings and pad settings.
*
*           All the pinmux settings and pad settings shall be initiated in this function.
*           But if legacy driver is used, the initialization of pinmux setting and pad setting
*           should be peformed with the IO initializing.
*
* @return  void
*/
void Board_Init(void)
{
	All_Pad_Config_Default();

	Initialize_GPIO();
	Initialize_ADC();
	
	//InitGain(MIDGAIN);

	//Initialize_TIM();
	
	//Initialize_I2C();
	
	Initialize_UART();	
	Initialize_NVIC();
}


/**
* @brief   Driver_Init() contains the initialization of peripherals.
*
*          Both new architecture driver and legacy driver initialization method can be used.
*
* @return  void
*/



/**
  * @brief  HeartMonitorEnterDlpsSet() configs the pad before the system enter DLPS.
  *         Pad should be set PAD_SW_MODE and PAD_PULL_UP/PAD_PULL_DOWN.
  * @param  None
  * @retval None
  */
void HeartMonitorEnterDlpsSet(void)
{
#if 0
	DBG_BUFFER(MODULE_APP, LEVEL_INFO, "HeartMonitorEnterDlpsSet", 0);

	// Disable Interrupt
	GPIO_ClearINTPendingBit(GPIO_GetPin(P2_0));
	GPIO_INTConfig(GPIO_GetPin(P2_0), DISABLE);
	GPIO_MaskINTConfig(GPIO_GetPin(P2_0), ENABLE);

	/* set PAD of wake up pin to software mode */

	/* GPIO Pad Config */
	Pad_Config(P2_0, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);

	/* I2C Pad Config */
	Pad_Config(P2_4, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(P2_5, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);

	/* UART Pad Config */
	Pad_Config(P3_0, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(P3_1, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);

    /* Config wakeup pins which can wake up system from dlps mode */
    System_WakeUp_Pin_Enable(P2_0, 1);
#endif		
}


/**
  * @brief  HeartMonitorExitDlpsInit() config the pad after the system exit DLPS.
  *         Pad should be set PAD_PINMUX_MODE.
  * @param  None
  * @retval None
  */
void HeartMonitorExitDlpsInit(void)
{
#if 0	
	DBG_BUFFER(MODULE_APP, LEVEL_INFO, "HeartMonitorExitDlpsInit", 0);

	/* GPIO Pad Config */
	Pad_Config(P2_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);

	/* I2C Pad Config */
	Pad_Config(P2_4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(P2_5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);

	/* UART Pad Config */
	Pad_Config(P3_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	Pad_Config(P3_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);

	// Enable Interrupt
	GPIO_ClearINTPendingBit(GPIO_GetPin(P2_0));
	GPIO_INTConfig(GPIO_GetPin(P2_0), ENABLE);
	GPIO_MaskINTConfig(GPIO_GetPin(P2_0), DISABLE);
#endif	
}


/**
* @brief    PwrMgr_Init() contains the setting about power mode.
*
* @return  void
*/
void PwrMgr_Init(void)
{
	DLPS_IO_Register();

	DLPS_IO_RegUserDlpsEnterCb(HeartMonitorEnterDlpsSet);

	DLPS_IO_RegUserDlpsExitCb(HeartMonitorExitDlpsInit);

	LPS_MODE_Set(LPM_ACTIVE_MODE);
}


/**
* @brief  Task_Init() contains the initialization of all the tasks.
*
*           There are four tasks are initiated.
*           Lowerstack task and upperstack task are used by bluetooth stack.
*           Application task is task which user application code resides in.
*           Emergency task is reserved.
*
* @return  void
*/
void Task_Init(void)
{
	void lowerstack_task_init();
	void upperstack_task_init();
	void emergency_task_init();
	application_task_init();
}


/**
* @brief  main() is the entry of user code.
*
*
* @return  void
*/
int main(void)
{
	Board_Init();
	
	BtStack_Init_Peripheral();
	BtStack_Init_Gap();
	BtProfile_Init();
	PwrMgr_Init();
	Task_Init();
	vTaskStartScheduler();

	return 0;
}
