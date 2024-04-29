#include "App_Main.h"
#include "r_cg_macrodriver.h"
#include "r_cg_sau.h"
#include "r_cg_intp.h"
#include "App_CanFD.h"

void App_CanFDCyclic(void)
{
	//Can_Frame_Prepare();
	App_CanReciveCheck();
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