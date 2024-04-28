#include "App_CanFD.h"

/*Initialise CANFD*/
void App_CanFDInit(void)
{
	CANFD_init();
}

uint8_t DataReq[8];


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