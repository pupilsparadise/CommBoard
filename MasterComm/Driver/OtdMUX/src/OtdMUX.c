#include "OtdMUX.h"
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"

#define MUX_A(state)			(P5_bit.no3 = state)
#define MUX_B(state)			(P5_bit.no4 = state)

/****************************************************************************/
// Function table of mux part no. SN74LV4052APWR
//		INPUTS
//--------------------------------------------------------
//	B		A		CHANNELS
//---------------------------------------------------------	
//	0		0		Com_Uart_e
//	0		1		Uart_e
//	1		0		Lora_e
//	1		1		L510_Uart_e
/****************************************************************************/
//Select particular Channels
void OtdMux_SelectMux(OtdMux_Select Channel)
{
	switch(Channel)
	{
		case Com_Uart_e:
		
				MUX_A(0);
				MUX_B(0);
				break;
				
		case Uart_e:
				MUX_A(0);
				MUX_B(1);		
				break;	
				
		case Lora_e:
				MUX_A(1);
				MUX_B(0);		
				break;		
				
		case L510_Uart_e:
		
				MUX_A(1);
				MUX_B(1);		
				break;
				
		default:
				//do nothing
				break;
	}
}