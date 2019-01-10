#include "rtl876x.h"
#include "rtl876x_i2c.h"
#include "rtl_delay.h"
#include "stk8ba50.h"

/*--------------------------------------------------------------------------
// Function name: STK8BA50_Settings
// Parameters: None
// Return: None
// Description: 
// Send commands to set registers of the STK8BA50.
--------------------------------------------------------------------------*/
void STK8BA50_Settings(void)
{
	// Software Reset
	STK8BA50_Write(REG_SWRST, 0xB6);
	delayMS(10);

	//--------------------------------------------------------------------------
	//	Normal Setting

	// Set Normal Mode
	STK8BA50_Write(REG_POWMODE, NORMAL_PWR);

	// Set Data Filter
	STK8BA50_Write(REG_DATASRC, ALL_FILTER);

	// Set Sensing Range +/- 2G
	STK8BA50_Write(REG_RANGESEL, RANGE_2G);

	// Set Actual Bandwidth 37Hz
	STK8BA50_Write(REG_BWSEL, BWSEL_37);

	// Set Data output filtered & Disable the data protection
	STK8BA50_Write(REG_DATASETUP, DATA_FILTER | DATA_UNPROTECT);

	// Set I2C Watchdog Enable
	STK8BA50_Write(REG_INTFCFG, I2C_WDT_EN | I2C_WDT_50MS);

	//--------------------------------------------------------------------------
	//	Single Tag Detectoin Setting

	#if 0
	// Enable single-tap interrupt
	STK8BA50_Write(REG_INTEN1, STAP_EN);

	// Set single-tap interrupt to INT1
	STK8BA50_Write(REG_INTMAP1, STAP2INT1);

	// Set INT1 active high
	STK8BA50_Write(REG_INTCFG1, INT1_HIGH);

	// Set Interrupt temporary 50mS
	STK8BA50_Write(REG_INTCFG2, INT_LATCH_50MS);

	// Set tap time window
	STK8BA50_Write(REG_TAPTIME, TAP_QUIET_30MS | TAP_SHOCK_75MS);

	// Tap threshold 62.5mg * 5 = 312mg, 8 samples
	STK8BA50_Write(REG_TAPCFG, TAP_SAMP_8 | 0x05);
	#endif

	//--------------------------------------------------------------------------
	//	End of Setting

	// Set Suspend Mode
	STK8BA50_Write(REG_POWMODE, SUSPEND);
}


/*--------------------------------------------------------------------------
// Function name: STK8BA50_Write
// Parameters:
// uint8_t Write_Addr - the address to be written to
// uint8_t Write_Data - the data to be written to the specified address.
// Return: None
// Description:
--------------------------------------------------------------------------*/
void STK8BA50_Write(uint8_t Write_Addr, uint8_t Write_Data)
{
	uint8_t WriteBuf[2] = {Write_Addr, Write_Data};

	if((I2C0->IC_TAR&0x3FF) != STK8BA50_ADR)
	{
		// Delay for previous device to finish work
		// Can not less than 300uS
		delayUS(300);

		// Change I2C Driver Address 
		I2C_SetSlaveAddress(I2C0, STK8BA50_ADR);
	}

	I2C_MasterWrite(I2C0, WriteBuf, 2);		
}


/*--------------------------------------------------------------------------
// Function name: STK8BA50_Read
// Parameters: 
// uint8_t Read_Addr - the address to be read
// uint8_t *Read_Data - the data buffer point
// Return: None
// Description:
//--------------------------------------------------------------------------*/
void STK8BA50_Read(uint8_t Read_Addr, uint8_t *Read_Data)
{
	if((I2C0->IC_TAR&0x3FF) != STK8BA50_ADR)
	{
		// Delay for previous device to finish work
		// Can not less than 300uS
		delayUS(300);

		// Change I2C Driver Address 
		I2C_SetSlaveAddress(I2C0, STK8BA50_ADR);
	}

	I2C_RepeatRead(I2C0, &Read_Addr, 1, Read_Data, 1);		
}


/*--------------------------------------------------------------------------
// Function name: STK8BA50_Burst_Read
// Parameters:
// uint8_t Read_Addr - the address to be read
// uint8_t *Read_Data - the data buffer point
// uint8_t Read_Size - the read szie 
// Return: None
// Description:
//--------------------------------------------------------------------------*/
void STK8BA50_Burst_Read(uint8_t Read_Addr, uint8_t *Read_Data, uint8_t Read_Size)
{
	if((I2C0->IC_TAR&0x3FF) != STK8BA50_ADR)
	{
		// Delay for previous device to finish work
		// Can not less than 300uS
		delayUS(300);

		// Change I2C Driver Address 
		I2C_SetSlaveAddress(I2C0, STK8BA50_ADR);
	}

	I2C_RepeatRead(I2C0, &Read_Addr, 1, Read_Data, Read_Size);
}

