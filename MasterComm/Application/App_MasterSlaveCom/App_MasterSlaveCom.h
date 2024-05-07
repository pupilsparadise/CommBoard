#ifndef APP_MASTERSLAVECOM_H
#define APP_MASTERSLAVECOM_H

#include "r_cg_macrodriver.h"

#define SLAVE_1				0U				//SLAVE1 , id shall be configured in structure "ConfigSlaveid"

#define FRAME_LEN			64U 				//CAN single frame length

#define SLAVE_ID_LEN			11U				//total size of Slave id
#define SLAVE_ID_POS			0U				//slave id start index
#define CMD_CODE_POS			(SLAVE_ID_POS + SLAVE_ID_LEN) 	//command code start index
#define PAYLOAD_POS			(CMD_CODE_POS + 1) 		//payload length start index		

// List of Commands for communication between Communication brd and Signal brd
typedef enum
{
	MasterGetSlaveStatus_e = 0x51, //this command is used to fetch the status of the signal brd for presence of data
	MasterGetSlaveData_e   = 0x52, //this command is used to get the data(time+voltage+current)	
}MasterCmdCode_ten;

//Error related in processing the frame and state
typedef enum
{
	CanTxFail_e = 0,
	TimeOut_e,
	SlaveIdMismatch_e,
	Busy_e,
	NoError_e,
}CommEvent_ten;

//Master comunication state related variables
typedef enum
{
	MasterIdleState_e = 0,
	MasterCmdState_e,
	MasterRxState_e,
	MasterSwitchState_e,
}ComState_ten;

//stores the Master communication state related data
typedef struct
{
	ComState_ten State_en;
	CommEvent_ten CommStatus_en;
}MasterStateMachine_tst;



typedef struct
{
	uint8_t SlaveNo;
	char 	SlaveId[11];//unique slave id
}App_SlaveID;

typedef struct
{
	char 				SlaveAddr[11];	//11 bytes slave address
	MasterCmdCode_ten 	CmdCode_en;	//Cmd code to slave
	uint16_t 			Data;		//Data to send/recieved
	uint16_t 			FrameCount; //stores the number for CAN frame(each frame is 64byte) received
}App_MasterFrame_tst;

#pragma pack
typedef struct
{
	char 		SlaveAddr[11];	//11 bytes slave address
	uint16_t 	PayloadLen;	//Payload length
}App_SlaveResponseFrame_tst;

//Function Prototype
CommEvent_ten App_MasterSendCmd(MasterCmdCode_ten CmdCode_en, uint8_t SlaveId ,uint16_t PayloadData);
CommEvent_ten App_ProcessResponseFrame(uint8_t SlaveId, uint16_t *SlavePayload);

//Communication Statemachine
void App_MainState(void);

#endif //APP_MASTERSLAVECOM_H