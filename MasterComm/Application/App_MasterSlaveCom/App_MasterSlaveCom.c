#include "stdio.h"
#include "string.h"
#include "App_MasterSlaveCom.h"
#include "App_CanFD.h"
#include "OtdCircBuff_App.h"
#include "OtdUART.h"
#include <stdint.h>


uint8_t finished = 0;

extern App_CanFlags_tst CanFlagState_st;//Flags related to CAN tx and rx
extern uint8_t frame_rx_flag;

extern uint8_t waiting_for_data;

//structure to hold the command frame data for sending to slave
App_MasterFrame_tst MasterCommandFrame_st;

//structure to hold the rx frame data responded from slave
App_MasterFrame_tst MasterRxFrame_st;

App_SlaveResponseFrame_tst SlaveResponse_st;

uint16_t App_SlavePayload;

//Buffer for CAN Tx
uint8_t App_CanTxFrame[FRAME_LEN];

//Buffer for CAN Rx
uint8_t App_CanRxFrame[FRAME_LEN];

ComState_ten MasterComState_en;

volatile uint16_t CanRxTimeout;

//uint8_t SlavePayloadData[512];
#define COM_TIMEOUT	2000
#define COM_TRIALS	10

App_CommSM_st App_ComCommandStateMachine;

uint16_t ExpectedFrame = 0;

//Structure maitains the communication state and error state
MasterStateMachine_tst CommMainState_st = {	MasterCmdState_e,//default com state
										NoError_e	//default error state
};


//Config the slave address
App_SlaveID ConfigSlaveid[MaxSlave_e] = 
{					Slave1_e, "00000001SIG",{0},0,0,Slave_Active_e,
					Slave2_e, "00000001SIG",{0},0,0,Slave_Active_e,
					Slave3_e, "00000001SIG",{0},0,0,Slave_Active_e,	
					Slave4_e, "00000001SIG",{0},0,0,Slave_Active_e,
					Slave5_e, "00000001SIG",{0},0,0,Slave_Active_e,
					Slave6_e, "00000001SIG",{0},0,0,Slave_Active_e,
					Slave7_e, "00000001SIG",{0},0,0,Slave_Active_e,
					Slave8_e, "00000001SIG",{0},0,0,Slave_Active_e,
					Slave9_e, "00000001SIG",{0},0,0,Slave_Active_e,
					Slave10_e, "00000001SIG",{0},0,0,Slave_Active_e										 
};

//Global buffer to store Slave data
char SlaveBuf[512];
uint16_t SlaveBuf_idx = 0;

void App_StoreBytes(uint8_t* buf,App_SlaveID *Resp_ptr,uint8_t numBytes)
{
	uint8_t idx;
	uint8_t slave_no;

	slave_no = App_ComCommandStateMachine.ComSlaveId;

	for (idx= 0; idx < numBytes; idx++) 
	{
		//SlaveBuf[SlaveBuf_idx] = buf[idx];
		Resp_ptr[slave_no].SlaveBufData[Resp_ptr[slave_no].SlaveIdx++] = buf[idx];
	}
	
}
/*!
* @brief update the command frame in CAN Tx and send to slave.
* @param[in] 
* @param[in] 
* @return 
*/
void App_ProcessMasterCmdFrame(App_MasterFrame_tst *CmdFrame_st)
{
	uint8_t FrameIndex;
	
	//update Slave id
	for(FrameIndex = 0;FrameIndex < SLAVE_ID_LEN; FrameIndex++)
	{
		App_CanTxFrame[FrameIndex] = CmdFrame_st->SlaveAddr[FrameIndex];
	}
	
	//Update command
	App_CanTxFrame[CMD_CODE_POS] = CmdFrame_st->CmdCode_en;
	
	//Update payload len
	
	App_CanTxFrame[PAYLOAD_POS] 	= (uint8_t)(CmdFrame_st->Data);//lsb
	App_CanTxFrame[PAYLOAD_POS + 1] = (uint8_t)(CmdFrame_st->Data >> 8);//msb
	
	//Send CAN frame
	App_CanFDSend(App_CanTxFrame,64,CAN_ID,STANDARD_FRAME,0,1);
	//App_CanFDSend(&App_CanTxFrame[8],8,CAN_ID,STANDARD_FRAME,0,1); //TODO: this has to be change for 64 bytes format for slave communication
}

/*!
* @brief Prepare the CAN tx frame and send.
* @param[in] 
* @param[in] 
* @return 
*/
CommEvent_ten App_MasterSendCmd(MasterCmdCode_ten Command_en, uint8_t SlaveId ,uint16_t PayloadData)
{
	uint8_t index;
	CommEvent_ten Events_e = NoError_e;
	
	MasterCommandFrame_st.CmdCode_en = Command_en;
	
	for(index = 0; index < SLAVE_ID_LEN ;index++)
	{
		MasterCommandFrame_st.SlaveAddr[index] = ConfigSlaveid[SlaveId].SlaveId[index];
	}
	
	MasterCommandFrame_st.Data = PayloadData;
	
	App_ProcessMasterCmdFrame(&MasterCommandFrame_st);

	return Events_e;
	
}

int areEqual(char arr1[], char arr2[], int len)
{
	int i;
    for ( i = 0; i < len; i++) {
        if (arr1[i] != arr2[i]) {
            return 0; // Return false if any character is different
        }
    }
    return 1; // Return true if all characters are the same
}

/*!
* @brief Process the CAN Rx frame from slave
* @param[in] 
* @param[in] 
* @return 
*/
CommEvent_ten App_ProcessResponseFrame(uint8_t SlaveId, App_SlaveResponseFrame_tst *Response_st)
{
	void* RxResponse_ptr;
	uint8_t SlaveidCheck;
	
	char str[SLAVE_ID_LEN];

	CommEvent_ten Status = NoError_e;
	
	//Check if requested Slave has responded, if not then discard the frame and resend cmd
	RxResponse_ptr = (void*)&App_CanRxFrame[0];
	
	//copy slave id
	//memcpy(str,((App_SlaveResponseFrame_tst*)RxResponse_ptr)->SlaveAddr, 11);
	memset(Response_st->SlaveAddr, '\0' , sizeof(Response_st->SlaveAddr)); 
	
	//memcpy(Response_st->SlaveAddr,((App_SlaveResponseFrame_tst*)RxResponse_ptr)->SlaveAddr, 11);
	memcpy(Response_st->SlaveAddr,App_CanRxFrame, 11);
	
	//if slave is same then return is 0
	//SlaveidCheck = strcmp(Response_st->SlaveAddr,ConfigSlaveid[SlaveId].SlaveId);
	//SlaveidCheck = compareStrings(Response_st->SlaveAddr,ConfigSlaveid[SlaveId].SlaveId);
	//SlaveidCheck = strstr(ConfigSlaveid[SlaveId].SlaveId,Response_st->SlaveAddr);
	
	SlaveidCheck = areEqual(Response_st->SlaveAddr,ConfigSlaveid[SlaveId].SlaveId,11);
	//Process the rx frame
	if(SlaveidCheck)
	{
		Response_st->PayloadLen = ((App_SlaveResponseFrame_tst*)RxResponse_ptr)->PayloadLen;
		
		//Response_st->PayloadLen = (Response_st->PayloadLen <<8)|(Response_st->PayloadLen >>8);
		
		Status = NoError_e;
	}
	else /*if(SlaveidCheck != 0)*/
	{
		Status = SlaveIdMismatch_e;
	}
	/*else 
	{
		//do nothing	
	}*/

	return Status;

}

/*!
* @brief Calculate the expected CAN frame using the Payload lenght
* @param[in] 
* @param[in] 
* @return 
*/
uint16_t App_ExpectedRxFrame(uint16_t TotalLen)
{
	uint16_t expectedframe;

	if(TotalLen !=0xCAFE)
	{
		expectedframe = (TotalLen / 64);
		if((TotalLen % 64) !=0)
		{
			expectedframe++;
		}
	}
	else 
	{
		expectedframe = 1;
	}

	return expectedframe;
	
}

CommEvent_ten App_RxStateHandler(void)
{
	CommEvent_ten Status = NoError_e;
	CanRxTimeout = 5000; //timeout 2s
	
	do
	{
		// check any rx frame and store
		App_CanReceiveCheck();

	}while((CanRxTimeout<=0)&&(frame_rx_flag));
	
	if(frame_rx_flag)
	{
		Status = NoError_e;
	}
	else
	{
		Status = TimeOut_e;
	}
	
	frame_rx_flag = 0;

	
	return Status;
}

/*!
* @brief Process the Recieve state
* @param[in] 
* @param[in] 
* @return 
*/
/*
CommEvent_ten App_RxStateHandler(MasterCmdCode_ten ComCommand_en, uint8_t SlaveNo)
{
	uint16_t FrameCount;
	CommEvent_ten Status = NoError_e;
	MasterRxFrame_st.CmdCode_en = ComCommand_en;
	MasterRxFrame_st.FrameCount = 0;//reset the frame count

	//get the expected frame from slave
	FrameCount = App_ExpectedRxFrame(MasterCommandFrame_st.Data);
	
	do
	{
		// check any rx frame and store
		App_CanReceiveCheck();
		
		if(frame_rx_flag)
		{
			MasterRxFrame_st.FrameCount++;
			frame_rx_flag = 0;
		}

	}while(MasterRxFrame_st.FrameCount < FrameCount);

	return Status;
	
}*/
//check if the rx frame and calculated frame are same
CommEvent_ten App_ValidateSlaveRxFrame(uint16_t ExpFrame,uint8_t FrameIdx)
{
	
	if((FrameIdx % 64) == 0)//should be multiple of 64 as CAN FD is configured to recieve 64 bytes in each frame
	{
		if((FrameIdx / 64) == ExpFrame)
		{
			return NoError_e;
		}
		else 
		{
			return FrameMismatch_e;
		}
	}
	else 
	{
		return FrameMismatch_e;
	}


}
void App_StateMachineInit(void)
{
	App_ComCommandStateMachine.CurrentState_en = MasterGetSlaveStatus_e;
	App_ComCommandStateMachine.PrevState_en = MasterGetSlaveStatus_e;
	App_ComCommandStateMachine.SendTrials = COM_TRIALS;
	App_ComCommandStateMachine.TimeOut = COM_TIMEOUT;
//	App_ComCommandStateMachine.ComSlaveId = Slave1_e;
	App_ComCommandStateMachine.CmdSendFlag = 0;
	App_ComCommandStateMachine.ExpectedFrameCnt = 0;
	
}

CommEvent_ten App_ResponseComSM(MasterCmdCode_ten ComRxState, uint8_t CmdSlaveID , App_SlaveID *Rsp_ptr)
{
	#if CAN_DEBUG
	char dbg_str[100];
	#endif
	static uint16_t SlaveIdx = 0;
	
	CommEvent_ten RxEvent_e = NoError_e;
	
	switch(ComRxState)
	{
		case MasterGetSlaveStatus_e:
							RxEvent_e = App_RxStateHandler();
							
							if(RxEvent_e == NoError_e)
							{

								RxEvent_e = App_ProcessResponseFrame(CmdSlaveID,&SlaveResponse_st);

								Rsp_ptr[CmdSlaveID].SlavePayloadLen = SlaveResponse_st.PayloadLen; //store the total payload len expected from slave
								
								/*#if CAN_DEBUG
								OtdUart_DebugSend("CAN_DEBUG >> Rx Frame for 0x51 command=====\n");
								#endif*/

								#if CAN_DEBUG
								sprintf(dbg_str,"CAN_DEBUG >> ==========Rx Frame for 0x51 command for Slaveid :%d\n",App_ComCommandStateMachine.ComSlaveId);
								OtdUart_DebugSend(dbg_str);
								#endif								
							}
							else
							{
								#if CAN_DEBUG
								OtdUart_DebugSend("CAN_DEBUG >> Error in 0x51 command=====\n");
								#endif							
							}
							

							
							break;

		case MasterGetSlaveData_e:
							RxEvent_e = App_RxStateHandler();
							
							if(RxEvent_e == NoError_e)
							{
								App_StoreBytes(&App_CanRxFrame,&ConfigSlaveid,FRAME_LEN);//need to check
								
								RxEvent_e = NoError_e;//need to update the frame structure;
								App_ComCommandStateMachine.ExpectedFrameCnt--;//update the frame count
								
								/*#if CAN_DEBUG
								OtdUart_DebugSend("CAN_DEBUG >> Rx Frame for 0x52 command=====\n");
								#endif*/

								#if CAN_DEBUG
								sprintf(dbg_str,"CAN_DEBUG >> ==========Rx Frame for 0x52 command for Slaveid :%d\n",App_ComCommandStateMachine.ComSlaveId);
								OtdUart_DebugSend(dbg_str);
								#endif									
							}
							else
							{
								#if CAN_DEBUG
								OtdUart_DebugSend("CAN_DEBUG >> Error in 0x52 command=====\n");
								#endif							
							}							
							
							break;	
		default:
								#if CAN_DEBUG
								OtdUart_DebugSend("CAN_DEBUG >> No command rx=====\n");
								#endif			
							break;	
	}
	
	return RxEvent_e;
}

CommEvent_ten CmdEvent_e = NoError_e;
CommEvent_ten App_CommandComSM(void)
{
	//CommEvent_ten CmdEvent_e = NoError_e;
	#if CAN_DEBUG
	char dbg_str[100];
	#endif
	
	switch(App_ComCommandStateMachine.CurrentState_en)
	{
		case MasterGetSlaveStatus_e:
							if(!App_ComCommandStateMachine.CmdSendFlag)
							{
								CmdEvent_e = App_MasterSendCmd(App_ComCommandStateMachine.CurrentState_en,
								                               App_ComCommandStateMachine.ComSlaveId,
								                               0xCAFE);
							
								#if CAN_DEBUG
								sprintf(dbg_str,"CAN_DEBUG >> ==========Command 0x51 sent for Slaveid :%d\n",App_ComCommandStateMachine.ComSlaveId);
								OtdUart_DebugSend(dbg_str);
								#endif
								
								App_ComCommandStateMachine.CmdSendFlag = 1;
							}

							
							break;

		case MasterGetSlaveData_e:
							if(!App_ComCommandStateMachine.CmdSendFlag)
							{
								if(SlaveResponse_st.PayloadLen > 0)
								{
									
									CmdEvent_e = App_MasterSendCmd(App_ComCommandStateMachine.CurrentState_en,
												       App_ComCommandStateMachine.ComSlaveId,
												       SlaveResponse_st.PayloadLen);								
								}
								else
								{
									#if CAN_DEBUG
									OtdUart_DebugSend("CAN_DEBUG >> Slave is less than 0=====\n");
									#endif	
								}

								
								#if CAN_DEBUG
								sprintf(dbg_str,"CAN_DEBUG >> ==========Command 0x52 sent for Slaveid :%d\n",App_ComCommandStateMachine.ComSlaveId);
								OtdUart_DebugSend(dbg_str);
								#endif	
								
								App_ComCommandStateMachine.CmdSendFlag = 1;
							}
	
							
							break;	
		default:
		
								#if CAN_DEBUG
								OtdUart_DebugSend("CAN_DEBUG >> No command rx=====\n");
								#endif	
							break;

	}
	
	
	CmdEvent_e = App_ResponseComSM(App_ComCommandStateMachine.CurrentState_en , App_ComCommandStateMachine.ComSlaveId,&ConfigSlaveid);
	
	if(CmdEvent_e == NoError_e)
	{
		if(App_ComCommandStateMachine.CurrentState_en == MasterGetSlaveStatus_e)//Cmd Frame id 0x51
		{
			//slave has some data then fetch it
			if(SlaveResponse_st.PayloadLen > 0)
			{
				App_ComCommandStateMachine.ExpectedFrameCnt = App_ExpectedRxFrame(SlaveResponse_st.PayloadLen);
				ExpectedFrame = App_ComCommandStateMachine.ExpectedFrameCnt;
				//MasterRxFrame_st.FrameCount = App_ComCommandStateMachine.ExpectedFrameCnt;
				App_ComCommandStateMachine.CurrentState_en = MasterGetSlaveData_e;

				#if CAN_DEBUG
				sprintf(dbg_str,"===============================================================================\n");
				OtdUart_DebugSend(dbg_str);
				sprintf(dbg_str,"CAN_DEBUG >> ======Slave id==%d Expected Frame Count===%d with Payload===%d\n",App_ComCommandStateMachine.ComSlaveId,MasterRxFrame_st.FrameCount,SlaveResponse_st.PayloadLen);
				OtdUart_DebugSend(dbg_str);
				sprintf(dbg_str,"===============================================================================\n");
				OtdUart_DebugSend(dbg_str);				
				#endif		

				CmdEvent_e = Busy_e;//as payload is present in slave
			}
			else
			{
				//slave has no data
				//TODO: update the next slave id and try for next slave 
				App_ComCommandStateMachine.CurrentState_en = MasterGetSlaveStatus_e;

				CmdEvent_e = NoPayload;//as no data is present in slave
				//indicate to scan next slave
			
			}
		}
		else if(App_ComCommandStateMachine.CurrentState_en == MasterGetSlaveData_e)//Cmd Frame id 0x52
		{
			if(App_ComCommandStateMachine.ExpectedFrameCnt > 0)
			{
				App_ComCommandStateMachine.CurrentState_en = MasterGetSlaveData_e;
				CmdEvent_e = Busy_e;//as payload is present in slave
			}
			else
			{
				//check if all the frame has recieved properly
				if(App_ValidateSlaveRxFrame(ExpectedFrame,ConfigSlaveid[App_ComCommandStateMachine.ComSlaveId].SlaveIdx) == NoError_e)
				{
					ConfigSlaveid[App_ComCommandStateMachine.ComSlaveId].SlaveIdx = 0;//reset as all the expected frame has recieved
				}
				//slave has no data
				//TODO: update the next slave id and try for next slave 
				App_ComCommandStateMachine.CurrentState_en = MasterGetSlaveStatus_e;
				
				//TODO: remove this, next slave id has to be updated
				
				
				finished = 1;
				waiting_for_data = 0;

				CmdEvent_e = NoPayload;//as no more data is present in slave
			}
		}
		else
		{
			#if CAN_DEBUG
			OtdUart_DebugSend("CAN_DEBUG >> ===========Wrong command recieved =\n");
			#endif	
		}
		
		App_ComCommandStateMachine.CmdSendFlag = 0; //reset command flag
		
	}
	else if(CmdEvent_e == TimeOut_e)
	{
		App_ComCommandStateMachine.CmdSendFlag = 0;
	}
	else
	{
		//do nothing
	}

	return CmdEvent_e;
	
}

SlaveScan App_MainState(void)
{

	ComSlave_ten SlaveNo;
	CommEvent_ten StatusEvent_en;
	//initially retrive the slave id from EEPROM
	
	//#if CAN_DEBUG
	OtdUart_DebugSend("CAN_DEBUG >> ==============Slave reading started \n");
	//#endif	

	for(SlaveNo = 0; SlaveNo < MaxSlave_e; )
	{
		App_ComCommandStateMachine.ComSlaveId = SlaveNo;

		StatusEvent_en = App_CommandComSM();

		if((StatusEvent_en == TimeOut_e) || (StatusEvent_en == NoPayload))
		{
			SlaveNo++;//scan next slave
			App_StateMachineInit(); //reset the flags
		}


	}

	#if CAN_DEBUG
	OtdUart_DebugSend("CAN_DEBUG >> ===========ALL slave Scan finished \n");
	#endif

	return SlaveScanDone;
}

