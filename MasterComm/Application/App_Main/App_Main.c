#include "App_Main.h"
#include "r_cg_macrodriver.h"
#include "r_cg_sau.h"
#include "r_cg_intp.h"
#include "App_CanFD.h"
#include "App_MasterSlaveCom.h"

extern uint8_t frame_rx_flag;
uint16_t mypayload;
void App_CanFDCyclic(void)
{
	//Can_Frame_Prepare();
	//App_CanReciveCheck();
	/*if(frame_rx_flag)
	{
		App_ProcessResponseFrame(SLAVE_1,&mypayload);
		frame_rx_flag = 0;
	}*/
	//App_MasterSendCmd(MASTER_GET_SLAVE_STATUS,SLAVE_1,0xCAFE);
	App_MainState();
	//Can_Frame_Prepare();
}
void App_Cyclic(void)
{
	while(1)
	{
		App_CanFDCyclic();
	}
}
void App_StartupInit(void)
{
	R_CSI30_Start();
	R_INTC0_Start();
	App_CanFDInit();
}

void App_Main(void)
{
	App_StartupInit();
	App_Cyclic();
}

