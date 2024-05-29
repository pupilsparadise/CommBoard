#ifndef APP_MASTERSLAVECOM_H
#define APP_MASTERSLAVECOM_H

#include "r_cg_macrodriver.h"
#include <stdint.h>
#include "App_Main.h"

#define CAN_DEBUG			1U

#define FRAME_LEN			64U 				//CAN single frame length

#define SLAVE_ID_LEN			11U				//total size of Slave id
#define SLAVE_ID_POS			0U				//slave id start index
#define CMD_CODE_POS			(SLAVE_ID_POS + SLAVE_ID_LEN) 	//command code start index
#define PAYLOAD_POS			(CMD_CODE_POS + 1) 		//payload length start index		
#define MAX_SLAVE			10U //Maximum slaves connected to master
#define MAX_SLAVE_BYTES		256U //maximum bytes recieved from each Slave

// List of Commands for communication between Communication brd and Signal brd
typedef enum
{
	MasterGetSlaveStatus_e = 0x51, //this command is used to fetch the status of the signal brd for presence of data
	MasterGetSlaveData_e   = 0x52, //this command is used to get the data(time+voltage+current)	
}MasterCmdCode_ten;

//Error related in processing the frame and state
typedef enum
{
	CanTxFail_e = 0, //failure in CAN tx
	TimeOut_e,		 //no data rx within speicified time
	SlaveIdMismatch_e, //Slave id mismatch
	Busy_e,				//expecting frame/ processing next data
	NoPayload,          //No payload available from slave 
	FrameMismatch_e,	//Expected frame and rx frame mismatched	 
	NoError_e			//everything working as expected	
}CommEvent_ten;

//Master comunication state related variables
typedef enum
{
	MasterCmdState_e,
	MasterRxState_e,
}ComState_ten;

//Number of slaves connected to master 
typedef enum
{
	Slave1_e = 0,
	Slave2_e,
	Slave3_e,
	Slave4_e,
	Slave5_e,
	Slave6_e,
	Slave7_e,
	Slave8_e,
	Slave9_e,
	Slave10_e,
	MaxSlave_e
}ComSlave_ten;

//Slave connection status
typedef enum
{
	Slave_DeActive_e = 0,	//Slave is not connected to Master
	Slave_Active_e			//Slave is connected to master
}ComSlaveStatus_e;

typedef enum
{
    SlvaeScanPending, //still slave scanning is pending
    SlaveScanDone    // all slave scan done  
}SlaveScan;
//stores the Master communication state related data
typedef struct
{
	ComState_ten State_en;
	CommEvent_ten CommStatus_en;
}MasterStateMachine_tst;


//stores the Slave id information
typedef struct
{
	ComSlave_ten SlaveNo; 					//slave number
	char 		 SlaveId[12];  					//unique slave id
	char 		 SlaveBufData[MAX_SLAVE_BYTES]; 	//stores the data rx from slave
	uint8_t 	 SlaveIdx;							//slave data index
	uint8_t		 SlavePayloadLen;			//payload len recieved from Slave
	ComSlaveStatus_e SlavConState_en; 		//slave connection status with master
}App_SlaveID;

typedef struct
{
	char 				SlaveAddr[11];	//11 bytes slave address
	MasterCmdCode_ten 	CmdCode_en;	//Cmd code to slave
	uint16_t 			Data;		//Data to send/recieved
	uint16_t 			FrameCount; //stores the number for CAN frame(each frame is 64byte) received
}App_MasterFrame_tst;

typedef struct
{
	MasterCmdCode_ten CurrentState_en; //stores current state
	MasterCmdCode_ten PrevState_en;	   //stores prev state	
	volatile uint8_t CmdSendFlag;	   //flag to make sure command is sent once	
	volatile uint8_t SendTrials;	   //no of trials taken to send the frame
	volatile uint8_t ComSlaveId;	   //Slave id
	volatile uint16_t TimeOut;	   //make sure to have responded data within the Timeout
	volatile uint16_t ExpectedFrameCnt;//Stores the expected frame count from slave 
	
}App_CommSM_st;

#pragma pack
typedef struct
{
	char 		SlaveAddr[11];	//11 bytes slave address
	uint16_t 	PayloadLen;	//Payload length
}App_SlaveResponseFrame_tst;

//Function Prototype
CommEvent_ten App_MasterSendCmd(MasterCmdCode_ten CmdCode_en, uint8_t SlaveId ,uint16_t PayloadData);
CommEvent_ten App_ProcessResponseFrame(uint8_t SlaveId, App_SlaveResponseFrame_tst *Response_st);

//Communication Statemachine
SlaveScan App_MainState(void);

void App_ComStateMachine(MasterStateMachine_tst *ComState_ptr, MasterCmdCode_ten ComCommand_en, uint8_t SlaveNo, uint16_t PayloadData);

void App_StateMachineInit(void);
#endif //APP_MASTERSLAVECOM_H