#include "OtdGsm_JSONFrame.h"
#include "App_MasterSlaveCom.h"
#include "OtdCircBuff_App.h"
#include "OtdGsm_TcpApp.h"
#include <stdio.h>
#include <string.h>

extern uint8_t finished;
extern App_SlaveResponseFrame_tst SlaveResponse_st;

extern char SlaveBuf[512];
extern uint16_t SlaveBuf_idx;

void arrayToHexString(unsigned char *arr, int size, char *str) 
{
    int idx = 0;
    int i;
    // Iterate through the array
    for (i = 0; i < size; i++) 
    {
        // Format each value as a hexadecimal string and concatenate them together
        idx += sprintf(str + idx, "%02X", arr[i]);
    }
    // Null-terminate the string
    str[idx] = '\0';
}
void copySubstring(char *dest, const char *src, int start, int length) 
{
    int i;
    // Move src pointer to the starting position
    src += start;

    // Copy characters from src to dest up to 'length' characters
    for (i = 0; i < length && *src != '\0'; i++) {
        *dest = *src;
        dest++;
        src++;
    }
    // Null-terminate the destination string
    *dest = '\0';
}
/*
//Prepare frame and set flag to publish in server
void OtdJsonFrame_PrepareFrame(void)
{
	Json_Frame_tst JsonFrame_st;
	char jsonString[1500];
	char myString[514];
	uint16_t len;
	uint8_t hour,second;
	
	//copy Device id
	copySubstring(&JsonFrame_st.DevId,&SlaveResponse_st.SlaveAddr,0,DEVID_LEN);
	
	//copy Model number
	copySubstring(&JsonFrame_st.Model,&SlaveResponse_st.SlaveAddr,8,MODEL_LEN);
	
	//pop time from Buffer
	hour   = SlaveBuf[0];
	second = SlaveBuf[1];
	
	//copy Time 
	len = sprintf(JsonFrame_st.Time, "%d:%d",hour,second);
	JsonFrame_st.Time[len] = '\0';//update NULL character at end of string
	
	//copy len 
	len = sprintf(JsonFrame_st.Len, "%d",SlaveResponse_st.PayloadLen);
	JsonFrame_st.Len[len] = '\0';//update NULL character at end of string
	
	arrayToHexString(&SlaveBuf[2],sizeof(SlaveBuf)/sizeof(SlaveBuf[0]),myString);//SlaveBuf[2] as data byte start from 
	
	//copy Payload length
	len = sprintf(jsonString,"{DEV_ID: %s,\r\nMODEL: %s,\r\nTIME: %s,\r\nLEN: %s,\r\nDATA: %s}",JsonFrame_st.DevId,JsonFrame_st.Model,JsonFrame_st.Time,JsonFrame_st.Len,myString);
	
	SlaveBuf_idx = 0;
	
	OtdGsmTcpApp_ServerPrepareData(jsonString,len);
		
}*/
	uint8_t hour,second;
	char myString[514];
	char jsonString[1500];
void OtdJsonFrame_PrepareFrame(App_SlaveID *PrepareFrame, uint8_t FrameSlaveNo)
{
	Json_Frame_tst JsonFrame_st;
	//char jsonString[1500];
	//char myString[514];
	uint16_t len;
	//uint8_t hour,second;

	//copy Device id
	copySubstring(&JsonFrame_st.DevId,PrepareFrame[FrameSlaveNo].SlaveId,0,DEVID_LEN);
	
	//copy Model number
	copySubstring(&JsonFrame_st.Model,PrepareFrame[FrameSlaveNo].SlaveId,8,MODEL_LEN);

	//pop time from Buffer
	hour   = PrepareFrame[FrameSlaveNo].SlaveBufData[0];
	second = PrepareFrame[FrameSlaveNo].SlaveBufData[1];

	//copy Time 
	len = sprintf(JsonFrame_st.Time, "%d:%d",hour,second);
	JsonFrame_st.Time[len] = '\0';//update NULL character at end of string
	
	//copy len 
	len = sprintf(JsonFrame_st.Len, "%d",PrepareFrame[FrameSlaveNo].SlavePayloadLen);
	JsonFrame_st.Len[len] = '\0';//update NULL character at end of string


	arrayToHexString(&PrepareFrame[FrameSlaveNo].SlaveBufData[2],sizeof(PrepareFrame[FrameSlaveNo].SlaveBufData)/sizeof(PrepareFrame[FrameSlaveNo].SlaveBufData[0]),myString);//SlaveBuf[2] as data byte start from

	//copy Payload length
	len = sprintf(jsonString,"{DEV_ID: %s,\r\nMODEL: %s,\r\nTIME: %s,\r\nLEN: %s,\r\nDATA: %s}",JsonFrame_st.DevId,JsonFrame_st.Model,JsonFrame_st.Time,JsonFrame_st.Len,myString);

	OtdGsmTcpApp_ServerPrepareData(jsonString,len);
}