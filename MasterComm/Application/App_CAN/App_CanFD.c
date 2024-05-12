#include "App_CanFD.h"
#include "App_MasterSlaveCom.h"


App_CanFlags_tst CanFlagState_st;//Flags related to CAN tx and rx
App_CanRecieve_tst CanRx_st; //Structure stores CAN rx frame and Length

uint8_t frame_rx_flag = 0;

extern uint8_t App_CanRxFrame[FRAME_LEN];
uint8_t index = 0;

uint8_t DataReq[8];

/*Initialise CANFD*/
void App_CanFDInit(void)
{
	CANFD_init();
}

/*Check and store CAN rx frame*/
void App_CanReceiveCheck(void)
{
	uint8_t volatile rx_idx = 0;
	
	if(CanFlagState_st.RxInt)
	{
		mcp2518fd_readMsgBufID(&CanRx_st.CanRxLen, CanRx_st.CanRxBuffer);
		CanFlagState_st.RxInt = 0;
		DRV_CANFDSPI_ModuleEventClear(CAN_ALL_EVENTS);
		
		
		for(rx_idx=0; rx_idx<CanRx_st.CanRxLen; rx_idx++)
		{
			App_CanRxFrame[rx_idx + index] = CanRx_st.CanRxBuffer[rx_idx];
		}

		/*index += CanRx_st.CanRxLen;
		
		if(index == 16)
		{
			index = 0;
			frame_rx_flag = 1;
		}*/
	}
}


/*CanFD Frame send*/
void App_CanFDSend(const uint8_t *buf, uint8_t len, unsigned long id, uint8_t ext, uint8_t rtr, bool wait_sent)
{
	MCP2518FD_SendMsg(buf, len, id, ext, rtr, wait_sent);
}

//uint8_t DataReq[64];
void Can_Frame_Prepare(void)
{
uint8_t i;
   /* DataReq[0] = 'C';
    DataReq[1] = 'O';
    DataReq[2] = 'M';
    DataReq[3] = 0xAA;
    DataReq[4] = 0xBB;
    DataReq[5] = 0x00;
    DataReq[6] = 0x0D;
    DataReq[7] = 0x0A;
     Can_TXD(CAN_ID, STANDARD_FRAME, 8, DataReq);
     App_CanFDSend(DataReq,8,CAN_ID,STANDARD_FRAME,0,1);*/
    
    
    //testing my code
    DataReq[0] = 0xAA;
    for(i=1;i<64;i++)
    {
    	DataReq[i] = 0xBE;
    }
    Can_TXD(CAN_ID, STANDARD_FRAME, 64, DataReq);
}