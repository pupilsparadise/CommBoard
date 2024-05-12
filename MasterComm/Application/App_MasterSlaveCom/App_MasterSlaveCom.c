#include "stdio.h"
#include "string.h"
#include "App_MasterSlaveCom.h"
#include "App_CanFD.h"
#include <stdint.h>

extern App_CanFlags_tst CanFlagState_st;//Flags related to CAN tx and rx
extern uint8_t frame_rx_flag;

//structure to hold the command frame data for sending to slave
App_MasterFrame_tst MasterCommandFrame_st;

//structure to hold the rx frame data responded from slave
App_MasterFrame_tst MasterRxFrame_st;

uint16_t App_SlavePayload;

//Buffer for CAN Tx
uint8_t App_CanTxFrame[FRAME_LEN];

//Buffer for CAN Rx
uint8_t App_CanRxFrame[FRAME_LEN];

ComState_ten MasterComState_en;

//Structure maitains the communication state and error state
MasterStateMachine_tst CommMainState_st = {	MasterCmdState_e,//default com state
										NoError_e	//default error state
};


//Config the slave address
App_SlaveID ConfigSlaveid[1] = {SLAVE_1, "12345678SIG"};

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

/*!
* @brief Process the CAN Rx frame from slave
* @param[in] 
* @param[in] 
* @return 
*/
CommEvent_ten App_ProcessResponseFrame(uint8_t SlaveId, uint16_t *SlavePayload)
{
	void* RxResponse_ptr;
	uint8_t SlaveidCheck;
	char str[SLAVE_ID_LEN];

	CommEvent_ten Status = NoError_e;
	
	//Check if requested Slave has responded, if not then discard the frame and resend cmd
	RxResponse_ptr = (void*)&App_CanRxFrame[0];
	
	//copy slave id
	memcpy(str,((App_SlaveResponseFrame_tst*)RxResponse_ptr)->SlaveAddr, 11);
	//if slave is same then return is 0
	SlaveidCheck = strcmp(str,ConfigSlaveid[SlaveId].SlaveId);
	
	//Process the rx frame
	if(SlaveidCheck == 0)
	{
		*SlavePayload = ((App_SlaveResponseFrame_tst*)RxResponse_ptr)->PayloadLen;
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
* @brief Process the Command state
* @param[in] 
* @param[in] 
* @return 
*/
CommEvent_ten App_CommandStateHandler(MasterCmdCode_ten ComCommand_en, uint8_t SlaveNo , uint16_t PayloadData)
{
	CommEvent_ten CmdEvent_e = NoError_e;
	
	switch(ComCommand_en)
	{
		case MasterGetSlaveStatus_e:
									CmdEvent_e = App_MasterSendCmd(ComCommand_en,SlaveNo,PayloadData);
									break;

		case MasterGetSlaveData_e:
									CmdEvent_e = App_MasterSendCmd(ComCommand_en,SlaveNo,PayloadData);
									break;	
		default:
									break;

	}

	return CmdEvent_e;
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
/*!
* @brief Process the Recieve state
* @param[in] 
* @param[in] 
* @return 
*/
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
	
}

/*!
* @brief Process the Error state
* @param[in] 
* @param[in] 
* @return 
*/
CommEvent_ten App_EventStateHandler(void)
{
	switch (CommMainState_st.CommStatus_en)
	{
		case CanTxFail_e:
							//TODO: ...
							break;
		case TimeOut_e:
							//TODO: ...
							break;
		case SlaveIdMismatch_e:
		 					//change to cmd state for sending command for another Slave id
							CommMainState_st.State_en = MasterCmdState_e;
							break;
		case Busy_e:
							//Expected frame has not recieved from slave
							//keep the state to Rx state
							CommMainState_st.State_en = MasterRxState_e;
							break;		
		case NoError_e:
							//do nothing here, it is handled in each state						
							break;																											
	
	}

}

/*!
* @brief Process the Idle state
* @param[in] 
* @param[in] 
* @return 
*/
CommEvent_ten App_IdleStateHandler(MasterStateMachine_tst *ComState_ptr)
{

}

void App_ComStateMachine(MasterStateMachine_tst *ComState_ptr, MasterCmdCode_ten ComCommand_en, uint8_t SlaveNo, uint16_t PayloadData)
{ 
	CommEvent_ten Status;

	switch(ComState_ptr->State_en)
	{
		case MasterIdleState_e://Idle state

					//Status = App_IdleStateHandler(ComState_ptr);
					break;

		case MasterCmdState_e://State to send command frame to slave

					CommMainState_st.CommStatus_en = App_CommandStateHandler(ComCommand_en,SlaveNo,PayloadData);

					if(CommMainState_st.CommStatus_en == NoError_e)
					{
						//Command frame send successfully
						CommMainState_st.State_en = MasterRxState_e;
					}
					else 
					{
						//let masterswitch state decide the next state
						CommMainState_st.State_en = MasterSwitchState_e;
					}
					break;

		case MasterRxState_e: //State to recieve frame from slave

					CommMainState_st.CommStatus_en = App_RxStateHandler(ComCommand_en,SlaveNo);

					if(CommMainState_st.CommStatus_en == NoError_e)
					{
						CommMainState_st.State_en = MasterIdleState_e;
					}
					else 
					{
						//let masterswitch state decide the next state
						CommMainState_st.State_en = MasterSwitchState_e;
					}

					break;

		case MasterSwitchState_e: //Switches the states depending on Event
					App_EventStateHandler();
					break;					
					
	}
}

//Function takes care the of two states
//1. Get data from Slave
//2. Send data to server via GSM
void App_MainState(void)
{
	App_ComStateMachine(&CommMainState_st,MasterGetSlaveStatus_e,SLAVE_1,0xCAFE);
}
