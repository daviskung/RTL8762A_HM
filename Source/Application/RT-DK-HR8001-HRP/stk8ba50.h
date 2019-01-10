#ifndef __STK8BA50_H__
#define __STK8BA50_H__

#include "rtl876x.h"

//--------------------------------------------------------------------------
// I2C Addr
//--------------------------------------------------------------------------
#define STK8BA50_ADR		0x18

//--------------------------------------------------------------------------
// Reg Addr
//--------------------------------------------------------------------------
#define REG_XOUT1			0x02
#define REG_XOUT2			0x03
#define REG_YOUT1			0x04
#define REG_YOUT2			0x05
#define REG_ZOUT1			0x06
#define REG_ZOUT2			0x07
#define REG_INTSTS1			0x09
#define REG_INTSTS2			0x0A
#define REG_EVENTINFO1		0x0B
#define REG_EVENTINFO2		0x0C
#define REG_RANGESEL		0x0F
#define REG_BWSEL			0x10
#define REG_POWMODE			0x11
#define REG_DATASETUP		0x13
#define REG_SWRST			0x14
#define REG_INTEN1			0x16
#define REG_INTEN2			0x17
#define REG_INTMAP1			0x19
#define REG_INTMAP2			0x1A
#define REG_INTMAP3			0x1B
#define REG_DATASRC			0x1E
#define REG_INTCFG1			0x20
#define REG_INTCFG2			0x21
#define REG_LGDLY			0x22
#define REG_LGTHD			0x23
#define REG_HLGCFG			0x24
#define REG_HGDLY			0x25
#define REG_HGTHD			0x26
#define REG_SLOPEDLY		0x27
#define REG_SLOPETHD		0x28
#define REG_TAPTIME			0x2A
#define REG_TAPCFG			0x2B
#define REG_ORIENTCFG		0x2C
#define REG_ORIENTTHETA		0x2D
#define REG_FLATTHETA		0x2E
#define REG_FLATHOLD		0x2F
#define REG_SLFTST			0x32
#define REG_INTFCFG			0x34
#define REG_OFSTCOMP1		0x36
#define REG_OFSTCOMP2		0x37
#define REG_OFSTFILTX		0x38
#define REG_OFSTFILTY		0x39
#define REG_OFSTFILTZ		0x3A
#define REG_OFSTUNFILTX		0x3B
#define REG_OFSTUNFILTY		0x3C
#define REG_OFSTUNFILTZ		0x3D


//--------------------------------------------------------------------------
//  Setting Define
//--------------------------------------------------------------------------
#define RANGE_2G			0x03
#define RANGE_4G			0x05
#define RANGE_8G			0x08
#define RANGE_16G			0x0C

#define BWSEL_4				0x08
#define BWSEL_9				0x09
#define BWSEL_18			0x0A
#define BWSEL_37			0x0B
#define BWSEL_75			0x0C
#define BWSEL_150			0x0D
#define BWSEL_300			0x0E
#define BWSEL_600			0x0F

#define SL_DR_1				0x0C
#define SL_DR_2				0x0E
#define SL_DR_4				0x10
#define SL_DR_6				0x12
#define SL_DR_10			0x14
#define SL_DR_25			0x16
#define SL_DR_50			0x18
#define SL_DR_100			0x1A
#define SL_DR_500			0x1C
#define SL_DR_1000			0x1E

#define NORMAL_PWR			0x00
#define LOW_PWR				0x40
#define SUSPEND				0x80

#define DATA_FILTER			0x00
#define DATA_UNFILTER		0x80
#define DATA_PROTECT		0x00
#define DATA_UNPROTECT		0x40

#define SLP_EN_X			0x01
#define SLP_EN_Y			0x02
#define SLP_EN_Z			0x04
#define DTAP_EN				0x10
#define STAP_EN				0x20
#define ORIENT_EN			0x40
#define FLAT_EN				0x80

#define HG_EN_X				0x01
#define HG_EN_Y				0x02
#define HG_EN_Z				0x04
#define LG_EN				0x08
#define DATA_EN				0x10

#define LG2INT1				0x01
#define HG2INT1				0x02
#define SLP2INT1			0x04
#define DTAP2INT1			0x10
#define STAP2INT1			0x20
#define ORIENT2INT1			0x40
#define FLAT2INT1			0x80

#define DATA2INT1			0x01
#define DATA2INT2			0x80

#define LG2INT1				0x01
#define HG2INT1				0x02
#define SLP2INT1			0x04
#define DTAP2INT1			0x10
#define STAP2INT1			0x20
#define ORIENT2INT1			0x40
#define FLAT2INT1			0x80

#define UNFILTER_LG_SRC		0x01
#define UNFILTER_HG_SRC		0x02
#define UNFILTER_SLP_SRC	0x04
#define UNFILTER_TAP_SRC	0x08
#define UNFILTER_DATA_SRC	0x10
#define ALL_FILTER			0x00

#define INT1_LOW			0x00
#define INT1_HIGH			0x01
#define INT2_LOW			0x00
#define INT2_HIGH			0x04

#define INT_LATCH_OFF		0x00
#define INT_LATCH_250MS		0x01
#define INT_LATCH_500MS		0x02
#define INT_LATCH_1S		0x03
#define INT_LATCH_2S		0x04
#define INT_LATCH_4S		0x05
#define INT_LATCH_8S		0x06
#define INT_LATCH_ON		0x07
#define INT_LATCH_500US		0x0A
#define INT_LATCH_1MS		0x0B
#define INT_LATCH_12MS		0x0C
#define INT_LATCH_25MS		0x0D
#define INT_LATCH_50MS		0x0E

#define INT_LATCH_RST		0x80

#define TAP_WIN_50MS		0x00
#define TAP_WIN_100MS		0x02
#define TAP_WIN_150MS		0x04
#define TAP_WIN_200MS		0x06
#define TAP_WIN_250MS		0x08
#define TAP_WIN_375MS		0x0A
#define TAP_WIN_500MS		0x0C
#define TAP_WIN_700MS		0x0E

#define TAP_SHOCK_50MS		0x00
#define TAP_SHOCK_75MS		0x40

#define TAP_QUIET_30MS		0x00
#define TAP_QUIET_20MS		0x80

#define I2C_WDT_1MS			0x00
#define I2C_WDT_50MS		0x02
#define I2C_WDT_EN			0x04

#define SLOW_X_EN			0x01
#define SLOW_Y_EN			0x02
#define SLOW_Z_EN			0x04

#define CAL_AXIS_X_EN		0x20
#define CAL_AXIS_Y_EN		0x40
#define CAL_AXIS_Z_EN		0x60

#define OFST_RST			0x80

#define CUTOFF_8			0x00
#define CUTOFF_16			0x01

#define OFST_TG_X_0G		0x00
#define OFST_TG_X_PG		0x02
#define OFST_TG_X_NG		0x04

#define OFST_TG_Y_0G		0x00
#define OFST_TG_Y_PG		0x08
#define OFST_TG_Y_NG		0x10

#define OFST_TG_Z_0G		0x00
#define OFST_TG_Z_PG		0x20
#define OFST_TG_Z_NG		0x40

#define TAP_SAMP_2			0x00
#define TAP_SAMP_4			0x40
#define TAP_SAMP_8			0x80
#define TAP_SAMP_16			0xC0


//--------------------------------------------------------------------------
// Prototypes
//--------------------------------------------------------------------------
void STK8BA50_Settings(void);
void STK8BA50_Write(uint8_t Write_Addr, uint8_t Write_Data);
void STK8BA50_Read(uint8_t Read_Addr, uint8_t *Read_Data);
void STK8BA50_Burst_Read(uint8_t Read_Addr, uint8_t *Read_Data, uint8_t Read_Size);


//--------------------------------------------------------------------------
// Extern
//--------------------------------------------------------------------------



#endif
