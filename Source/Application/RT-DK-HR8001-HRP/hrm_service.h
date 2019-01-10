/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     hrm_service.h
* @brief
* @details
* @author   hunter_shuai
* @date     14-July-2015
* @version  v1.0.0
******************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2015 Realtek Semiconductor Corporation</center></h2>
******************************************************************************
*/

#ifndef _HRM_SERVICE_H_
#define _HRM_SERVICE_H_

#include "profileAPI.h"


#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */


/** @brief  Demo Profile service related UUIDs. */
#define GATT_UUID_CHAR_HR_REPORT		0xFFD5


/** @brief  Index of each characteristic in Demo Profile service database. */
#define BLE_SERVICE_CHAR_HR_REPORT_INDEX	0x02

#define READ_HEART_RATE_VALUE 1

/** @brief Write callback data type definition.*/
typedef struct _THRM_WRITE_MSG {
    uint8_t opcode;
    uint8_t value;
} THRM_WRITE_MSG;


typedef union _THRM_UPSTREAM_MSG_DATA
{
    uint8_t notification_indification_index;
    uint8_t read_value_index;
    THRM_WRITE_MSG write;
} THRM_UPSTREAM_MSG_DATA;

/**
 * @brief servic data struct for notification data to application.
 *
 * OTA service data to inform application.
*/
typedef struct _THRM_CALLBACK_DATA
{
    TSERVICE_CALLBACK_TYPE msg_type;                    /**<  @brief EventId defined upper */
    THRM_UPSTREAM_MSG_DATA msg_data;
} THRM_CALLBACK_DATA;


extern uint8_t HeartService_AddService(void *pFunc);


#ifdef __cplusplus
}
#endif

#endif
