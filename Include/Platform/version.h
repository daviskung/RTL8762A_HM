// 
//	2019.02.13	SENSOR_PAH8001_INTERVAL 1000    // 1 sec
//	2019.02.25	Send [inter-beat interval (IBI)] OR [R-R interval] in Hex format
//				SENSOR_PAH8001_INTERVAL 500    // 500 msec
//				* PIC Note:2019.03.12	In [case AN0_UNDER_RANGE_Event:] reset AGC_MCP4011_Gain to MIDGAIN=32
//	2019.03.13	_myHR_OUT_of_RANGE disable CalculateHeartRate().
//	2019.03.22	mail: Date[21 Mar 2019] send the one data point when you collect an IBI.
//				2019.03.27 test ok
//	2019.04.17	 test after new HD(重灌)
//	2019.05.10	ack 增加 PIC version number 訊息
//	2019.06.06	NSTROBE_LOW_EndSet send out to monitor
//	2019.06.13  增加 "NSTROBE_Rset"
//	2019.07.09	增加 _RR_Interval_pre 檢查 是否合理值
//				modify _RR_Interval Hex format error ('A'~ 'F')

#define VERSION_MAJOR            1
#define VERSION_MINOR            2
#define VERSION_REVISION         19709	// test after new HD
#define VERSION_BUILD            12638
#define NUM4STR(a,b,c,d)         #a "." #b "." #c "." #d
#define VERSIONBUILDSTR(a,b,c,d) NUM4STR(a,b,c,d)
#define FILE_VERSION_STR         VERSIONBUILDSTR(VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION,VERSION_BUILD)
#define SRC_TIME                 "2016/06/30 11:04:19"
#define BUILDING_TIME            "2016/06/30 11:17:29"
