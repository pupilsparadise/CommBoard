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
* Creation Date: 29-04-2024
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
volatile uint8_t * gp_csi30_rx_address;        /* csi30 receive buffer address */
volatile uint16_t  g_csi30_rx_length;          /* csi30 receive data length */
volatile uint16_t  g_csi30_rx_count;           /* csi30 receive data count */
volatile uint8_t * gp_csi30_tx_address;        /* csi30 send buffer address */
volatile uint16_t  g_csi30_send_length;        /* csi30 send data length */
volatile uint16_t  g_csi30_tx_count;           /* csi30 send data count */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

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
    SPS1 = _0000_SAU_CK01_fCLK_0 | _0000_SAU_CK00_fCLK_0;
    R_CSI30_Create();
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
    SMR12 = _0020_SMR12_DEFAULT_VALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS | 
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
