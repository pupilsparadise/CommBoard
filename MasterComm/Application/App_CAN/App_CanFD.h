#ifndef APP_CANFD_H
#define APP_CANFD_H

#include "r_cg_macrodriver.h"
#include "CAN.h"

typedef struct
{
	uint8_t	RxInt;	//CAN Rx interrupt 
	uint8_t TxInt;  //CAN Tx interrupt 
}App_CanFlags_tst;

typedef struct
{
	uint8_t CanRxBuffer[64];
	uint8_t CanRxLen;
}App_CanRecieve_tst;


void App_CanFDInit(void);
void App_CanFDSend(const uint8_t *buf, uint8_t len, unsigned long id, uint8_t ext, uint8_t rtr, bool wait_sent);
void Can_Frame_Prepare(void);
void App_CanReciveCheck(void);

#endif //APP_CANFD_H