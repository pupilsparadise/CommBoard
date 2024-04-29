#include "App_CanFD.h"


App_CanFlags_tst CanFlagState_st;//Flags related to CAN tx and rx
App_CanRecieve_tst CanRx_st; //Structure stores CAN rx frame and Length

uint8_t DataReq[8];

/*Initialise CANFD*/
void App_CanFDInit(void)
{
	CANFD_init();
}

/*Check and store CAN rx frame*/
void App_CanReciveCheck(void)
{
	if(CanFlagState_st.RxInt)
	{
		mcp2518fd_readMsgBufID(&CanRx_st.CanRxLen, CanRx_st.CanRxBuffer);
		CanFlagState_st.RxInt = 0;
		DRV_CANFDSPI_ModuleEventClear(CAN_ALL_EVENTS);
	}
}

/*CanFD Frame send*/
void App_CanFDSend(const uint8_t *buf, uint8_t len, unsigned long id, uint8_t ext, uint8_t rtr, bool wait_sent)
{
	MCP2518FD_SendMsg(buf, len, id, ext, rtr, wait_sent);
}

void Can_Frame_Prepare(void)
{
	uint8_t i;
    DataReq[0] = 'C';
    DataReq[1] = 'O';
    DataReq[2] = 'M';
    DataReq[3] = 0xAA;
    DataReq[4] = 0xBB;
    DataReq[5] = 0x00;
    DataReq[6] = 0x0D;
    DataReq[7] = 0x0A;
     //Can_TXD(CAN_ID, STANDARD_FRAME, 8, DataReq);
     App_CanFDSend(DataReq,8,CAN_ID,STANDARD_FRAME,0,1);
    
    /*
    //testing my code
    DataReq[0] = 0xAA;
    for(i=1;i<64;i++)
    {
    	DataReq[i] = 0xBE;
    }
    Can_TXD(CAN_ID, STANDARD_FRAME, 64, DataReq);*/
}