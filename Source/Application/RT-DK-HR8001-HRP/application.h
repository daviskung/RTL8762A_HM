
/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "rtl_types.h"
//#include "basic_definitions.h"
#include "blueapi_types.h"

#define ARY_SIZE		100
#define BTSendOn		1
#define BTSendOff		0


extern void application_task_init(void);
extern void appBlueAPICallback(PBlueAPI_UsMessage pMsg);


extern uint8_t	NSTROBE_PWM_cnt;
extern uint8_t	NSTROBE_LOW_EndSet;
extern uint32_t GPIO_NSTROBE_value;

//const uint8_t HexNumTable[16]={
// '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};



#endif

