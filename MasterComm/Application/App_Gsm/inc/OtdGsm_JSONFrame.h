#ifndef OTDGSMJSONFRAME_H
#define OTDGSMJSONFRAME_H

#include "r_cg_macrodriver.h"
#include "App_MasterSlaveCom.h"

#define SERVER_LEN   320U //server data slave data len

#define NULL_CHAR_LEN	1U
#define DEVID_LEN	8U //max character for Device ID
#define MODEL_LEN	3U //max character for Model ID
#define TIME_LEN	5U //max character for Time (HH:SS)
#define PAYLOAD_LEN	5U //max character to store Payload len


typedef struct
{
	char DevId[DEVID_LEN + NULL_CHAR_LEN];
	char Model[MODEL_LEN + NULL_CHAR_LEN];
	char Time[TIME_LEN + NULL_CHAR_LEN];
	char Len[PAYLOAD_LEN + NULL_CHAR_LEN];
	uint8_t SlaveData[SERVER_LEN];
}Json_Frame_tst;

#pragma pack
typedef struct
{
	volatile char SlaveDevId[8]; //Slave device id	
	volatile char SlaveModel[3]; //Slave model number
}Json_SlaveId;

//void OtdJsonFrame_PrepareFrame(void);
void OtdJsonFrame_PrepareFrame(App_SlaveID *PrepareFrame, uint8_t FrameSlaveNo);

#endif