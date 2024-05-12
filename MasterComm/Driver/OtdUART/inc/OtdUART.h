/***********************************************************************************************************************
* File Name    : OtdUART.h
* Version      : 
* Device(s)    : R5F104ML
* Tool-Chain   : CCRL
* Description  : This file implements device driver for UART module.
* Creation Date: 15-08-2023
***********************************************************************************************************************/

#ifndef OTDUART_H
#define OTDUART_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_sau.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define RX_BUFFER_LENGTH 512
/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum
{
	OTD_UART_STATUS_PASS,
	OTD_UART_STATUS_FAIL
}Otd_Uart_Status;

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void OtdUart_Init(void);
//GSM service UART
void OtdUart_GsmSend(const __far uint8_t *buf, uint16_t len);
void OtdUart_Recieve(uint8_t *buf, uint16_t len);
void OtdUart_GsmCallBackRecieve(void);

//Debug service UART
void OtdUart_DebugSend(volatile const char *s);


uint16_t OtdDelay_GetTicks(void);
void OtdUart_IsDataAvailable(void);

#endif