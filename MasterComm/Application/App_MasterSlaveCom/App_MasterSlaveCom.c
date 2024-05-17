#include "stdio.h"
#include "string.h"
#include "App_MasterSlaveCom.h"
#include "App_CanFD.h"
#include "OtdUART.h"
#include <stdint.h>

extern App_CanFlags_tst CanFlagState_st;//Flags related to CAN tx and rx
extern uint8_t frame_rx_flag;

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

//Structure maitains the communication state and error state
MasterStateMachine_tst CommMainState_st = {	MasterCmdState_e,//default com state
										NoError_e	//default error state
};


//Config the slave address
App_SlaveID ConfigSlaveid[1] = {SLAVE_1, "00000001SIG"};

typedef CommEvent_ten (*RxCallBack)(uint8_t SlaveId, uint16_t *);

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

// Function to compare two strings
int compareStrings(const char *str1, const char *str2) {
    while (*str1 && *str2 && *str1 == *str2) {
        str1++;
        str2++;
    }

    // Strings are equal if both have reached the end simultaneously
    if (*str1 == '\0' && *str2 == '\0')
        return 0;
    else
        return (*str1 - *str2); // Return the difference if strings are not equal
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
	memset(Response_st->SlaveAddr, 0 , sizeof(Response_st->SlaveAddr)); 
	
	memcpy(Response_st->SlaveAddr,((App_SlaveResponseFrame_tst*)RxResponse_ptr)->SlaveAddr, 12);
	//if slave is same then return is 0
	//SlaveidCheck = strcmp(Response_st->SlaveAddr,ConfigSlaveid[SlaveId].SlaveId);
	SlaveidCheck = compareStrings(Response_st->SlaveAddr,ConfigSlaveid[SlaveId].SlaveId);
	
	//Process the rx frame
	if(SlaveidCheck == 0)
	{
		Response_st->PayloadLen = ((App_SlaveResponseFrame_tst*)RxResponse_ptr)->PayloadLen;
		
		Response_st->PayloadLen = (Response_st->PayloadLen <<8)|(Response_st->PayloadLen >>8);
		
		Status = NoError_e;
	}
	else if(SlaveidCheck != 0)
	{
		Status = SlaveIdMismatch_e;
	}
	else 
	{
		//do nothing	
	}

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
	CanRxTimeout = 2000; //timeout 2s
	
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

#define COM_TIMEOUT	2000
#define COM_TRIALS	10

App_CommSM_st App_ComCommandStateMachine;


void App_StateMachineInit(void)
{
	App_ComCommandStateMachine.CurrentState_en = MasterGetSlaveStatus_e;
	App_ComCommandStateMachine.PrevState_en = MasterGetSlaveStatus_e;
	App_ComCommandStateMachine.SendTrials = COM_TRIALS;
	App_ComCommandStateMachine.TimeOut = COM_TIMEOUT;
	App_ComCommandStateMachine.ComSlaveId = SLAVE_1;
	App_ComCommandStateMachine.CmdSendFlag = 0;
	App_ComCommandStateMachine.ExpectedFrameCnt = 0;
	
}

CommEvent_ten App_ResponseComSM(MasterCmdCode_ten ComRxState, uint8_t CmdSlaveID)
{
	CommEvent_ten RxEvent_e = NoError_e;
	
	switch(ComRxState)
	{
		case MasterGetSlaveStatus_e:
							RxEvent_e = App_RxStateHandler();
							
							if(RxEvent_e == NoError_e)
							{

								RxEvent_e = App_ProcessResponseFrame(CmdSlaveID,&SlaveResponse_st);
								
								#if CAN_DEBUG
								OtdUart_DebugSend("CAN_DEBUG >> Rx Frame for 0x51 command=====\n");
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
								RxEvent_e = NoError_e;//need to update the frame structure;
								App_ComCommandStateMachine.ExpectedFrameCnt--;//update the frame count
								
								#if CAN_DEBUG
								OtdUart_DebugSend("CAN_DEBUG >> Rx Frame for 0x52 command=====\n");
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
							break;	
	}
	
	return RxEvent_e;
}

CommEvent_ten App_CommandComSM(void)
{
	CommEvent_ten CmdEvent_e = NoError_e;
	
	switch(App_ComCommandStateMachine.CurrentState_en)
	{
		case MasterGetSlaveStatus_e:
							if(!App_ComCommandStateMachine.CmdSendFlag)
							{
								CmdEvent_e = App_MasterSendCmd(App_ComCommandStateMachine.CurrentState_en,
								                               App_ComCommandStateMachine.ComSlaveId,
								                               0xCAFE);
							
								#if CAN_DEBUG
								OtdUart_DebugSend("CAN_DEBUG >> Command 0x51 sent=====\n");
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
								OtdUart_DebugSend("CAN_DEBUG >> Command 0x52 sent=====\n");
								#endif		
								
								App_ComCommandStateMachine.CmdSendFlag = 1;
							}
	
							
							break;	
		default:
							break;

	}
	
	
	CmdEvent_e = App_ResponseComSM(App_ComCommandStateMachine.CurrentState_en , App_ComCommandStateMachine.ComSlaveId);
	
	if(CmdEvent_e == NoError_e)
	{
		if(App_ComCommandStateMachine.CurrentState_en == MasterGetSlaveStatus_e)
		{
			//slave has some data then fetch it
			if(SlaveResponse_st.PayloadLen > 0)
			{
				App_ComCommandStateMachine.ExpectedFrameCnt = App_ExpectedRxFrame(SlaveResponse_st.PayloadLen);
				MasterRxFrame_st.FrameCount = App_ComCommandStateMachine.ExpectedFrameCnt;
				App_ComCommandStateMachine.CurrentState_en = MasterGetSlaveData_e;
			}
			else
			{
				//slave has no data
				//TODO: update the next slave id and try for next slave 
				App_ComCommandStateMachine.CurrentState_en = MasterGetSlaveStatus_e;
			
			}
		}
		else if(App_ComCommandStateMachine.CurrentState_en == MasterGetSlaveData_e)
		{
			if(App_ComCommandStateMachine.ExpectedFrameCnt > 0)
			{
				App_ComCommandStateMachine.CurrentState_en = MasterGetSlaveData_e;
			}
			else
			{
				//slave has no data
				//TODO: update the next slave id and try for next slave 
				App_ComCommandStateMachine.CurrentState_en = MasterGetSlaveStatus_e;
				
				//TODO: remove this, next slave id has to be updated
				App_ComCommandStateMachine.ComSlaveId = SLAVE_1;
			}
		}
		else
		{
				//do nothing
		}
		
		App_ComCommandStateMachine.CmdSendFlag = 0;
		
	}
	
}

void App_MainState(void)
{
	App_CommandComSM();
}

