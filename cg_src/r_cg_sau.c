/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2015, 2021 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_sau.c
* Version      : Code Generator for RL78/I1C V1.01.07.02 [08 Nov 2021]
* Device(s)    : R5F10NPJ
* Tool-Chain   : CCRL
* Description  : This file implements device driver for SAU module.
* Creation Date: 10-05-2024
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_sau.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
volatile uint8_t * gp_uart1_tx_address;        /* uart1 transmit buffer address */
volatile uint16_t  g_uart1_tx_count;           /* uart1 transmit data number */
volatile uint8_t * gp_uart1_rx_address;        /* uart1 receive buffer address */
volatile uint16_t  g_uart1_rx_count;           /* uart1 receive data number */
volatile uint16_t  g_uart1_rx_length;          /* uart1 receive data length */
volatile uint8_t * gp_uart2_tx_address;        /* uart2 transmit buffer address */
volatile uint16_t  g_uart2_tx_count;           /* uart2 transmit data number */
volatile uint8_t * gp_uart2_rx_address;        /* uart2 receive buffer address */
volatile uint16_t  g_uart2_rx_count;           /* uart2 receive data number */
volatile uint16_t  g_uart2_rx_length;          /* uart2 receive data length */
volatile uint8_t * gp_csi30_rx_address;        /* csi30 receive buffer address */
volatile uint16_t  g_csi30_rx_length;          /* csi30 receive data length */
volatile uint16_t  g_csi30_rx_count;           /* csi30 receive data count */
volatile uint8_t * gp_csi30_tx_address;        /* csi30 send buffer address */
volatile uint16_t  g_csi30_send_length;        /* csi30 send data length */
volatile uint16_t  g_csi30_tx_count;           /* csi30 send data count */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_SAU0_Create
* Description  : This function initializes the SAU0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SAU0_Create(void)
{
    SAU0EN = 1U;    /* enables input clock supply */
    NOP();
    NOP();
    NOP();
    NOP();
    SPS0 = _0040_SAU_CK01_fCLK_4 | _0004_SAU_CK00_fCLK_4;
    R_UART1_Create();
}
/***********************************************************************************************************************
* Function Name: R_UART1_Create
* Description  : This function initializes the UART1 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART1_Create(void)
{
    ST0 |= _0008_SAUm_CH3_STOP_TRG_ON | _0004_SAUm_CH2_STOP_TRG_ON;
    STMK1 = 1U;     /* disable INTST1 interrupt */
    STIF1 = 0U;     /* clear INTST1 interrupt flag */
    SRMK1 = 1U;     /* disable INTSR1 interrupt */
    SRIF1 = 0U;     /* clear INTSR1 interrupt flag */
    SREMK1 = 1U;    /* disable INTSRE1 interrupt */
    SREIF1 = 0U;    /* clear INTSRE1 interrupt flag */
    /* Set INTSR1 low priority */
    SRPR11 = 1U;
    SRPR01 = 1U;
    /* Set INTST1 low priority */
    STPR11 = 1U;
    STPR01 = 1U;
    SMR02 = _0020_SMR02_DEFAULT_VALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS | 
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR02 = _0004_SCR02_DEFAULT_VALUE | _8000_SAU_TRANSMISSION | _0000_SAU_TIMING_1 | _0000_SAU_INTSRE_MASK | 
            _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0003_SAU_LENGTH_8;
    SDR02 = _9A00_SAU0_CH2_BAUDRATE_DIVISOR;
    NFEN0 |= _04_SAU_RXD1_FILTER_ON;
    SIR03 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;
    SMR03 = _0020_SMR03_DEFAULT_VALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS | 
            _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL | _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR03 = _0004_SCR03_DEFAULT_VALUE | _4000_SAU_RECEPTION | _0000_SAU_TIMING_1 | _0000_SAU_INTSRE_MASK | 
            _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0003_SAU_LENGTH_8;
    SDR03 = _9A00_SAU0_CH3_BAUDRATE_DIVISOR;
    SO0 |= _0004_SAUm_CH2_DATA_OUTPUT_1;
    SOL0 &= (uint16_t)~_0004_SAUm_CHANNEL2_INVERTED;
    SOE0 |= _0004_SAUm_CH2_OUTPUT_ENABLE;
    /* Set RxD1 pin */
    PM0 |= 0x08U;
    /* Set TxD1 pin */
    P0 |= 0x10U;
    PM0 &= 0xEFU;
}
/***********************************************************************************************************************
* Function Name: R_UART1_Start
* Description  : This function starts the UART1 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART1_Start(void)
{
    SO0 |= _0004_SAUm_CH2_DATA_OUTPUT_1;
    SOE0 |= _0004_SAUm_CH2_OUTPUT_ENABLE;
    SS0 |= _0008_SAUm_CH3_START_TRG_ON | _0004_SAUm_CH2_START_TRG_ON;
    STIF1 = 0U;     /* clear INTST1 interrupt flag */
    SRIF1 = 0U;     /* clear INTSR1 interrupt flag */
    STMK1 = 0U;     /* enable INTST1 interrupt */
    SRMK1 = 0U;     /* enable INTSR1 interrupt */
}
/***********************************************************************************************************************
* Function Name: R_UART1_Stop
* Description  : This function stops the UART1 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART1_Stop(void)
{
    STMK1 = 1U;     /* disable INTST1 interrupt */
    SRMK1 = 1U;     /* disable INTSR1 interrupt */
    ST0 |= _0008_SAUm_CH3_STOP_TRG_ON | _0004_SAUm_CH2_STOP_TRG_ON;
    SOE0 &= (uint16_t)~_0004_SAUm_CH2_OUTPUT_ENABLE;
    STIF1 = 0U;     /* clear INTST1 interrupt flag */
    SRIF1 = 0U;     /* clear INTSR1 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_UART1_Receive
* Description  : This function receives UART1 data.
* Arguments    : rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART1_Receive(uint8_t * const rx_buf, uint16_t rx_num)
{
    MD_STATUS status = MD_OK;

    if (rx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_uart1_rx_count = 0U;
        g_uart1_rx_length = rx_num;
        gp_uart1_rx_address = rx_buf;
    }

    return (status);
}
/***********************************************************************************************************************
* Function Name: R_UART1_Send
* Description  : This function sends UART1 data.
* Arguments    : tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART1_Send(uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (tx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        gp_uart1_tx_address = tx_buf;
        g_uart1_tx_count = tx_num;
        STMK1 = 1U;    /* disable INTST1 interrupt */
        TXD1 = *gp_uart1_tx_address;
        gp_uart1_tx_address++;
        g_uart1_tx_count--;
        STMK1 = 0U;    /* enable INTST1 interrupt */
    }

    return (status);
}
/***********************************************************************************************************************
* Function Name: R_SAU1_Create
* Description  : This function initializes the SAU1 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SAU1_Create(void)
{
    SAU1EN = 1U;    /* enables input clock supply */
    NOP();
    NOP();
    NOP();
    NOP();
    SPS1 = _0000_SAU_CK01_fCLK_0 | _0004_SAU_CK00_fCLK_4;
    R_UART2_Create();
    R_CSI30_Create();
}
/***********************************************************************************************************************
* Function Name: R_UART2_Create
* Description  : This function initializes the UART2 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART2_Create(void)
{
    ST1 |= _0002_SAUm_CH1_STOP_TRG_ON | _0001_SAUm_CH0_STOP_TRG_ON;
    STMK2 = 1U;     /* disable INTST2 interrupt */
    STIF2 = 0U;     /* clear INTST2 interrupt flag */
    SRMK2 = 1U;     /* disable INTSR2 interrupt */
    SRIF2 = 0U;     /* clear INTSR2 interrupt flag */
    SREMK2 = 1U;    /* disable INTSRE2 interrupt */
    SREIF2 = 0U;    /* clear INTSRE2 interrupt flag */
    /* Set INTSR2 low priority */
    SRPR12 = 1U;
    SRPR02 = 1U;
    /* Set INTST2 low priority */
    STPR12 = 1U;
    STPR02 = 1U;
    SMR10 = _0020_SMR10_DEFAULT_VALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS | 
            _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR10 = _0004_SCR10_DEFAULT_VALUE | _8000_SAU_TRANSMISSION | _0000_SAU_TIMING_1 | _0000_SAU_INTSRE_MASK | 
            _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0003_SAU_LENGTH_8;
    SDR10 = _9A00_SAU1_CH0_BAUDRATE_DIVISOR;
    NFEN0 |= _10_SAU_RXD2_FILTER_ON;
    SIR11 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;
    SMR11 = _0020_SMR11_DEFAULT_VALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS | 
            _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL | _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
    SCR11 = _0004_SCR11_DEFAULT_VALUE | _4000_SAU_RECEPTION | _0000_SAU_TIMING_1 | _0000_SAU_INTSRE_MASK | 
            _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0003_SAU_LENGTH_8;
    SDR11 = _9A00_SAU1_CH1_BAUDRATE_DIVISOR;
    SO1 |= _0001_SAUm_CH0_DATA_OUTPUT_1;
    SOL1 &= (uint16_t)~_0001_SAUm_CHANNEL0_INVERTED;
    SOE1 |= _0001_SAUm_CH0_OUTPUT_ENABLE;
    /* Set RxD2 pin */
    PFSEG4 &= 0xDFU;
    PM5 |= 0x20U;
    /* Set TxD2 pin */
    PFSEG4 &= 0xBFU;
    P5 |= 0x40U;
    PM5 &= 0xBFU;
}
/***********************************************************************************************************************
* Function Name: R_UART2_Start
* Description  : This function starts the UART2 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART2_Start(void)
{
    SO1 |= _0001_SAUm_CH0_DATA_OUTPUT_1;
    SOE1 |= _0001_SAUm_CH0_OUTPUT_ENABLE;
    SS1 |= _0002_SAUm_CH1_START_TRG_ON | _0001_SAUm_CH0_START_TRG_ON;
    STIF2 = 0U;     /* clear INTST2 interrupt flag */
    SRIF2 = 0U;     /* clear INTSR2 interrupt flag */
    STMK2 = 0U;     /* enable INTST2 interrupt */
    SRMK2 = 0U;     /* enable INTSR2 interrupt */
}
/***********************************************************************************************************************
* Function Name: R_UART2_Stop
* Description  : This function stops the UART2 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART2_Stop(void)
{
    STMK2 = 1U;     /* disable INTST2 interrupt */
    SRMK2 = 1U;     /* disable INTSR2 interrupt */
    ST1 |= _0002_SAUm_CH1_STOP_TRG_ON | _0001_SAUm_CH0_STOP_TRG_ON;
    SOE1 &= (uint16_t)~_0001_SAUm_CH0_OUTPUT_ENABLE;
    STIF2 = 0U;     /* clear INTST2 interrupt flag */
    SRIF2 = 0U;     /* clear INTSR2 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_UART2_Receive
* Description  : This function receives UART2 data.
* Arguments    : rx_buf -
*                    receive buffer pointer
*                rx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART2_Receive(uint8_t * const rx_buf, uint16_t rx_num)
{
    MD_STATUS status = MD_OK;

    if (rx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_uart2_rx_count = 0U;
        g_uart2_rx_length = rx_num;
        gp_uart2_rx_address = rx_buf;
    }

    return (status);
}
/***********************************************************************************************************************
* Function Name: R_UART2_Send
* Description  : This function sends UART2 data.
* Arguments    : tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART2_Send(uint8_t * const tx_buf, uint16_t tx_num)
{
    MD_STATUS status = MD_OK;

    if (tx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        gp_uart2_tx_address = tx_buf;
        g_uart2_tx_count = tx_num;
        STMK2 = 1U;    /* disable INTST2 interrupt */
        TXD2 = *gp_uart2_tx_address;
        gp_uart2_tx_address++;
        g_uart2_tx_count--;
        STMK2 = 0U;    /* enable INTST2 interrupt */
    }

    return (status);
}
/***********************************************************************************************************************
* Function Name: R_CSI30_Create
* Description  : This function initializes the CSI30 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CSI30_Create(void)
{
    ST1 |= _0004_SAUm_CH2_STOP_TRG_ON;
    CSIMK30 = 1U;   /* disable INTCSI30 interrupt */
    CSIIF30 = 0U;   /* clear INTCSI30 interrupt flag */
    /* Set INTCSI30 low priority */
    CSIPR130 = 1U;
    CSIPR030 = 1U;
    SIR12 = _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;
    SMR12 = _0020_SMR12_DEFAULT_VALUE | _8000_SAU_CLOCK_SELECT_CK01 | _0000_SAU_CLOCK_MODE_CKS | 
            _0000_SAU_TRIGGER_SOFTWARE | _0000_SAU_MODE_CSI | _0000_SAU_TRANSFER_END;
    SCR12 = _0004_SCR12_DEFAULT_VALUE | _C000_SAU_RECEPTION_TRANSMISSION | _0000_SAU_TIMING_1 | 
            _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0000_SAU_MSB | _0000_SAU_STOP_NONE | 
            _0003_SAU_LENGTH_8;
    SDR12 = _0400_SAU1_CH2_BAUDRATE_DIVISOR;
    SO1 |= _0400_SAUm_CH2_CLOCK_OUTPUT_1;
    SO1 &= (uint16_t)~(_0004_SAUm_CH2_DATA_OUTPUT_1);
    SOE1 |= _0004_SAUm_CH2_OUTPUT_ENABLE;
    /* Set SI30 pin */
    PFSEG5 &= 0xFEU;
    PM8 |= 0x10U;
    /* Set SO30 pin */
    PFSEG5 &= 0xFDU;
    P8 |= 0x20U;
    PM8 &= 0xDFU;
    /* Set SCK30 pin */
    PFSEG4 &= 0x7FU;
    P5 |= 0x80U;
    PM5 &= 0x7FU;
}
/***********************************************************************************************************************
* Function Name: R_CSI30_Start
* Description  : This function starts the CSI30 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CSI30_Start(void)
{
    SO1 |= _0400_SAUm_CH2_CLOCK_OUTPUT_1;
    SO1 &= (uint16_t)~(_0004_SAUm_CH2_DATA_OUTPUT_1);
    SOE1 |= _0004_SAUm_CH2_OUTPUT_ENABLE;
    SS1 |= _0004_SAUm_CH2_START_TRG_ON;
    CSIIF30 = 0U;   /* clear INTCSI30 interrupt flag */
    CSIMK30 = 0U;   /* enable INTCSI30 interrupt */
}
/***********************************************************************************************************************
* Function Name: R_CSI30_Stop
* Description  : This function stops the CSI30 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CSI30_Stop(void)
{
    CSIMK30 = 1U;   /* disable INTCSI30 interrupt */
    ST1 |= _0004_SAUm_CH2_STOP_TRG_ON;
    SOE1 &= (uint16_t)~_0004_SAUm_CH2_OUTPUT_ENABLE;
    CSIIF30 = 0U;   /* clear INTCSI30 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_CSI30_Send_Receive
* Description  : This function sends and receives CSI30 data.
* Arguments    : tx_buf -
*                    transfer buffer pointer
*                tx_num -
*                    buffer size
*                rx_buf -
*                    receive buffer pointer
* Return Value : status -
*                    MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_CSI30_Send_Receive(uint8_t * const tx_buf, uint16_t tx_num, uint8_t * const rx_buf)
{
    MD_STATUS status = MD_OK;

    if (tx_num < 1U)
    {
        status = MD_ARGERROR;
    }
    else
    {
        g_csi30_tx_count = tx_num;        /* send data count */
        gp_csi30_tx_address = tx_buf;     /* send buffer pointer */
        gp_csi30_rx_address = rx_buf;     /* receive buffer pointer */
        CSIMK30 = 1U;                     /* disable INTCSI30 interrupt */

        if (0U != gp_csi30_tx_address)
        {
            SIO30 = *gp_csi30_tx_address;    /* started by writing data to SDR[7:0] */
            gp_csi30_tx_address++;
        }
        else
        {
            SIO30 = 0xFFU;
        }

        g_csi30_tx_count--;
        CSIMK30 = 0U;                     /* enable INTCSI30 interrupt */
    }

    return (status);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
