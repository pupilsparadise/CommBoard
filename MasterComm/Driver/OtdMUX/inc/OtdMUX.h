#ifndef OTDMUX_H
#define OTDMUX_H

//Output selection of Mux
typedef enum
{
	Com_Uart_e,
	Uart_e,
	Lora_e,
	L510_Uart_e
}OtdMux_Select;


void OtdMux_SelectMux(OtdMux_Select Channel);

#endif