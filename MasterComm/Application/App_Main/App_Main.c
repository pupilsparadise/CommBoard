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
#include "OtdGsm_JSONFrame.h"



#include <string.h>
#include <stdio.h>

extern uint8_t finished;
extern uint8_t frame_rx_flag;
uint16_t mypayload;

extern OtdGsm_TcpApp_TcpHandle_tst TcpHandle_st;//structure to maintain status and capture data for the server
extern App_SlaveID ConfigSlaveid[MaxSlave_e];

extern OtdGsmTcpApp_SubState_tst TcpSubState_st;

char data[9] = "00000001";
char model[4] = "SIG";
char time[6] = "02:56";

extern uint8_t waiting_for_data;

#define COMM_TO_SLAVE 1
#define PUBLISH 2
#define IDLE_SM 3
#define GSM_SM  4

uint8_t state_machine = IDLE_SM;

void App_CanFDCyclic(void)
{
	ComSlave_ten slv_en;
	//Can_Frame_Prepare();
	//App_CanReceiveCheck();
	/*if(frame_rx_flag)
	{
		App_ProcessResponseFrame(Slave1_e,&mypayload);
		frame_rx_flag = 0;
	}*/
	//App_MasterSendCmd(MASTER_GET_SLAVE_STATUS,Slave1_e,0xCAFE);
	
	//App_MainState();
	
	//OtdGsmApp_MainStateMachine(); //GSM Main state machine
	do {

		OtdGsmApp_MainStateMachine(); //GSM Main state machine

	}while(TcpSubState_st.TcpCurrent_en !=TCP_IDLESTATE);

	do {

		//do nothing
	}while (App_MainState() != SlaveScanDone);

	//publish active slave data to server
	for(slv_en = Slave1_e ; slv_en < MaxSlave_e ; slv_en++)
	{
		//prepare data
		OtdJsonFrame_PrepareFrame(&ConfigSlaveid,slv_en);
		
		TcpHandle_st.IsDataPublished = UNPUBLISHED;//Data published to server
		
		while(TcpHandle_st.IsDataPublished!= PUBLISHED)
		{
			OtdGsmApp_MainStateMachine(); //GSM Main state machine
		}
	}
	


	/*switch(state_machine)
	{
		case GSM_SM	  :
					OtdGsmApp_MainStateMachine(); //GSM Main state machine
					if(waiting_for_data)
					{
						state_machine = IDLE_SM;
					}
					break;
		case COMM_TO_SLAVE:
					App_MainState();
					if(!waiting_for_data)
					{
						state_machine = PUBLISH;
					}
					break;
		case PUBLISH:
					OtdJsonFrame_PrepareFrame();
					state_machine = IDLE_SM;
					finished = 0;
					break;
		case IDLE_SM:
					//do nothing
					if(!waiting_for_data)
					{
						state_machine = GSM_SM;
					}
					if(waiting_for_data)
					{
						state_machine = COMM_TO_SLAVE;
					}
					if(finished)
					{
						state_machine = PUBLISH;
						finished = 0;
						waiting_for_data = 0;
					}
					break;
	}*/
	
	/*if(!finished)
	{
		if(TcpHandle_st.IsNetworkConnected && TcpHandle_st.IsServerConnected && waiting_for_data && TcpHandle_st.IsDataPublished)
		{
			App_MainState();
			//OtdGsmTcpApp_ServerPrepareData(data,strlen(data));
			waiting_for_data = 0;
			
		}
	}
	else if(finished)
	{
		OtdJsonFrame_PrepareFrame();
		finished = 0;
		TcpHandle_st.IsDataPublished = UNPUBLISHED;
	}*/

	//OtdUart_DebugSend("Jai Sri Ram");
	//OtdDelay_ms(1000);
	
	//Can_Frame_Prepare();
}
void App_Cyclic(void)
{
	//char myjson[256];
	//sprintf(myjson,"{DEV_ID: %s,\r\nMODEL: %s,\r\nTIME: %s}",data,model,time);
	//OtdGsmTcpApp_ServerPrepareData(myjson,strlen(myjson));
	App_StateMachineInit();
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

