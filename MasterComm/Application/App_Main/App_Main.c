#include "App_Main.h"
#include "r_cg_macrodriver.h"
#include "r_cg_sau.h"
#include "r_cg_intp.h"
#include "App_CanFD.h"
#include "App_MasterSlaveCom.h"
#include "OtdUART.h"
#include "OtdCircBuff_App.h"
#include "OtdMUX.h"
#include "OtdDelay.h"
#include "OtdGsm_App.h"
#include "OtdGsm_TcpApp.h"

#include <string.h>

extern uint8_t frame_rx_flag;
uint16_t mypayload;

char data[] = {25,35,45,46};


void App_CanFDCyclic(void)
{
	//Can_Frame_Prepare();
	//App_CanReceiveCheck();
	/*if(frame_rx_flag)
	{
		App_ProcessResponseFrame(SLAVE_1,&mypayload);
		frame_rx_flag = 0;
	}*/
	//App_MasterSendCmd(MASTER_GET_SLAVE_STATUS,SLAVE_1,0xCAFE);
	
	//App_MainState();
	OtdGsmApp_MainStateMachine(); //GSM Main state machine
	//OtdUart_DebugSend("Jai Sri Ram");
	//OtdDelay_ms(1000);
	
	//Can_Frame_Prepare();
}
void App_Cyclic(void)
{
	OtdGsmTcpApp_ServerPrepareData(data,strlen(data));
	while(1)
	{
		App_CanFDCyclic();
	}
}
void App_StartupInit(void)
{
	R_CSI30_Start();
	R_INTC0_Start();
	OtdDelay_Start();
	App_CanFDInit();
	OtdUart_Init();
	OtdCircBuffApp_Init();
	OtdMux_SelectMux(L510_Uart_e);//Select GSM UART
	
	OtdGsmApp_GsmStateInit();//Initialise the GSM init state
}

void App_Main(void)
{
	App_StartupInit();
	App_Cyclic();
}

