// 
//	2019.02.13	SENSOR_PAH8001_INTERVAL 1000    // 1 sec
//	2019.02.25	Send [inter-beat interval (IBI)] OR [R-R interval] in Hex format
//				SENSOR_PAH8001_INTERVAL 500    // 500 msec
//				* PIC Note:2019.03.12	In [case AN0_UNDER_RANGE_Event:] reset AGC_MCP4011_Gain to MIDGAIN=32
//	2019.03.13	_myHR_OUT_of_RANGE disable CalculateHeartRate().
#define VERSION_MAJOR            1
#define VERSION_MINOR            2
#define VERSION_REVISION         19314
#define VERSION_BUILD            12638
#define NUM4STR(a,b,c,d)         #a "." #b "." #c "." #d
#define VERSIONBUILDSTR(a,b,c,d) NUM4STR(a,b,c,d)
#define FILE_VERSION_STR         VERSIONBUILDSTR(VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION,VERSION_BUILD)
#define SRC_TIME                 "2016/06/30 11:04:19"
#define BUILDING_TIME            "2016/06/30 11:17:29"
