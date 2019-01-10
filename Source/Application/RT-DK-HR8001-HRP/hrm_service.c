/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     hr_service.c
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

#include "trace.h"
#include "endialig.h"
#include "FreeRTOS.h"
#include "gatt.h"
#include "profileApi.h"
#include "hrm_service.h"
#include "gap.h"

#define HEART_RATE_NOTIFY_ENABLE	1
#define HEART_RATE_NOTIFY_DISABLE	2
#define GATT_HEART_RATE_CHAR_CCCD_INDEX		3


/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
/**<  Function pointer used to send event to application from BWPS extended profile. */
/**<  Initiated in BWPSExtended_AddService. */
pfnAPPHandleInfoCB_t pfnHeartExtendedCB = NULL;

uint8_t heart_rate_value[64];
uint16_t heart_rate_size;

const uint8_t GATT_UUID_HEART_SERVICE[16] = { 0x2E, 0x74, 0x72, 0x61, 0x65, 0x48, 0x2D, 0x65, 0x6E, 0x6F, 0x74, 0x53, 0x79, 0x6C, 0x6F, 0x48};

uint8_t gReadHeartRateValuePending = FALSE;


/**< @brief  profile/service definition.
*   here is an example of OTA service table
*   including Write
*/
const TAttribAppl gattHeartServiceTable[] =
{
	/* <<Primary Service>>, .. 0 */
	{
		(ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),	//* wFlags */
		{
			LO_WORD(GATT_UUID_PRIMARY_SERVICE),	//* bTypeValue */
			HI_WORD(GATT_UUID_PRIMARY_SERVICE),	//* bTypeValue */
		},
		UUID_128BIT_SIZE,						//* bValueLen */
		(void*)GATT_UUID_HEART_SERVICE,			//* pValueContext */
		GATT_PERM_READ							//* wPermissions */
	},
	/* <<Characteristic1>>, .. 1 Heart Rate Report */
	{
		ATTRIB_FLAG_VALUE_INCL,					/* wFlags */
		LO_WORD(GATT_UUID_CHARACTERISTIC),		/* bTypeValue */
		HI_WORD(GATT_UUID_CHARACTERISTIC),		/* bTypeValue */
		(GATT_CHAR_PROP_READ|
		GATT_CHAR_PROP_NOTIFY),					/* characteristic properties */
		1,										/* bValueLen */
		NULL,
	    GATT_PERM_READ							/* wPermissions */
	},
	/* Characteristic value 2 */
	{
		ATTRIB_FLAG_VALUE_APPL,					/* wFlags */
		{
			LO_WORD(GATT_UUID_CHAR_HR_REPORT),	/* bTypeValue */
			HI_WORD(GATT_UUID_CHAR_HR_REPORT),	/* bTypeValue */
		},
		1,										/* variable size */
		(void *)NULL,
		GATT_PERM_READ							/* wPermissions */
	},
	/* Client Characteristic Configuration */
	{
		ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* wFlags */
		{
			LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),		/* bTypeValue */
			HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),		/* bTypeValue */
			LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT),	/* client char. config. bit field */
			HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT),	/* client char. config. bit field */
		},
		2,												/* bValueLen */
		NULL,
		(GATT_PERM_READ | GATT_PERM_WRITE)				/* wPermissions */
	}
};


/**
 * @brief read characteristic data from service.
 *
 * @param ServiceId          ServiceID of characteristic data.
 * @param iAttribIndex       Attribute index of getting characteristic data.
 * @param iOffset            Used for Blob Read.
 * @param piLength           length of getting characteristic data.
 * @param ppValue            data got from service.
 * @return Profile procedure result
*/
TProfileResult HeartRateServiceAttrReadCb(uint8_t ServiceId, uint16_t iAttribIndex, uint16_t iOffset, uint16_t *piLength, uint8_t **ppValue)
{
	TAppResult appResult = AppResult_Success;

	TProfileResult wCause = ProfileResult_Success;

	switch(iAttribIndex)
	{
		default:
			DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "<-- HeartRate_AttrRead, Attr not found, index=%d", 1, iAttribIndex);
			wCause  = ProfileResult_AttrNotFound;
			break;
		case BLE_SERVICE_CHAR_HR_REPORT_INDEX:
			{
				THRM_CALLBACK_DATA callback_data;

				callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
				callback_data.msg_data.read_value_index = READ_HEART_RATE_VALUE;
				appResult = pfnHeartExtendedCB(ServiceId, (void*)&callback_data);

				if(appResult == AppResult_Pending)
				{
					gReadHeartRateValuePending = TRUE;
				}

				wCause = ProfileAPI_GetProfileResult(appResult);

				*ppValue  = (uint8_t *)&heart_rate_value[0];
				*piLength = heart_rate_size;
			}
		break;
	}

	return(wCause);
}


/**
 * @brief update CCCD bits from stack.
 *
 * @param ServiceId          Service ID.
 * @param Index              Attribute index of characteristic data.
 * @param wCCCBits           CCCD bits from stack.
 * @return None
*/
void HeartRateServiceCccdUpdateCb(uint8_t ServiceId, uint16_t Index, uint16_t wCCCBits)
{
	THRM_CALLBACK_DATA callback_data;

	callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
	bool bHandle = TRUE;

	DBG_BUFFER(MODULE_PROFILE, LEVEL_INFO, "HeartRate_CccdUpdateCb  Index = %d wCCCDBits %x", 2, Index, wCCCBits);

	switch(Index)
	{
		case GATT_HEART_RATE_CHAR_CCCD_INDEX:
			{
				if(wCCCBits & GATT_CCCD_NOTIFICATION_BIT)
				{
					// Enable Notification
					callback_data.msg_data.notification_indification_index = HEART_RATE_NOTIFY_ENABLE;
				}
				else
				{
					callback_data.msg_data.notification_indification_index = HEART_RATE_NOTIFY_DISABLE;
				}
				break;
			}
		default:
			{
				bHandle = FALSE;
				break;
			}
	}

	/* Notify Application. */
	if (pfnHeartExtendedCB && (bHandle == TRUE))
	{
	    pfnHeartExtendedCB(ServiceId, (void*)&callback_data);
	}

	return;
}


CONST gattServiceCBs_t HeartServiceCBs =
{
    HeartRateServiceAttrReadCb,		// Read callback function pointer
	NULL,							// Write callback function pointer
    HeartRateServiceCccdUpdateCb	// CCCD update callback function pointer
};


/**
 * @brief  add ble service to application.
 *
 * @param  pFunc          pointer of app callback function called by profile.
 * @return service ID auto generated by profile layer.
 * @retval ServiceId
*/
uint8_t HeartService_AddService(void *pFunc)
{
	uint8_t ServiceId;

	if(FALSE == ProfileAPI_AddService(	&ServiceId,
										(uint8_t *)gattHeartServiceTable,
										sizeof(gattHeartServiceTable),
										HeartServiceCBs))
	{
		DBG_BUFFER(MODULE_PROFILE, LEVEL_ERROR, "Heart_Service_AddService: ServiceId %d", 1, ServiceId);
		ServiceId = 0xff;
		return ServiceId;
	}

	pfnHeartExtendedCB = (pfnAPPHandleInfoCB_t)pFunc;

	return ServiceId;
}

