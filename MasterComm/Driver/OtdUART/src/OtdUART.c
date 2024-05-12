/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "OtdUART.h"
#include "OtdCircBuff_App.h"
//#include "OtdGsm_App.h"
#include <stdio.h>
#include <string.h>

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
#define GSM_UART_INIT()		R_UART2_Start()
#define DEBUG_UART_INIT()	R_UART1_Start()

#define GSM_UART_STOP()		R_UART2_Stop()
#define DEBUG_UART_STOP()	R_UART1_Stop()

//Rx related service for GSM
volatile uint8_t GsmRxData = 0;
extern volatile uint16_t RxIndex;
extern volatile char Gsm_RxBuffer[RX_BUFFER_LENGTH];
//Tx related flag for GSM UART
uint8_t gsm_tx_pending = 0;

//Tx related flag for Debug UART
uint8_t debug_tx_pending = 0;


/** @brief Uart initialisation
 *  @param void
 *  @return Void.
 */
void OtdUart_Init(void)
{
	GSM_UART_INIT();
	DEBUG_UART_INIT();
	OtdUart_Recieve((uint8_t * __near)&GsmRxData,1);	
}

/** @brief Uart Disable
 *  @param void
 *  @return Void.
 */
void OtdUart_Disable(void)
{
	GSM_UART_STOP();
	DEBUG_UART_STOP();
}

/** @brief Callback from UART transmission completion
 *  @param void
 *  @return Void.
 */
void OtdUart_GsmCallBackSend(void)
{
	gsm_tx_pending = 1;
}

/** @brief Callback from GSM UART recieve
 *  @param void
 *  @return Void.
 */
void OtdUart_GsmCallBackRecieve(void)
{
	//Gsm_RxBuffer[RxIndex++] = GsmRxData;
	OtdCircBuffApp_CircBuffPush(&GsmRxData);
	OtdUart_Recieve((uint8_t * __near)&GsmRxData,1);	
}
void OtdUart_GsmSend(const __far uint8_t *buf, uint16_t len)
{
	R_UART2_Send((uint8_t * __near)buf,len);
}

void OtdUart_Recieve(uint8_t *buf, uint16_t len)
{
	R_UART2_Receive(buf,len);
}
void OtdUart_DebugSend(volatile const char *s)
{
	//TODO: check the altenative for sprintf
	const uint8_t debug_data[128];
	sprintf(debug_data,"%s",s);
	R_UART1_Send((uint8_t * __near)debug_data,strlen(s));	
	while(!debug_tx_pending);
	debug_tx_pending = 0;
}

void OtdUart_IsDataAvailable(void)
{	
	uint16_t rx_data_count;//number of data recieved
	unsigned char rx_data;
	
	rx_data_count = OtdCircBuffApp_IsData();
	
	if(rx_data_count > 0)
	{
		/*data available*/
		/*copy the data from Circular buffer to Global Rx buffer*/
		while(OtdCircBuffApp_BufferPop(&rx_data) == OtdCircBuffApp_Ok)
		{
			Gsm_RxBuffer[RxIndex++] = rx_data;
		}
		
	}
}