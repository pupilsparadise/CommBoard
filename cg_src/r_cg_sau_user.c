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
* File Name    : r_cg_sau_user.c
* Version      : Code Generator for RL78/I1C V1.01.07.02 [08 Nov 2021]
* Device(s)    : R5F10NPJ
* Tool-Chain   : CCRL
* Description  : This file implements device driver for SAU module.
* Creation Date: 28-04-2024
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
#pragma interrupt r_csi30_interrupt(vect=INTCSI30)
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
extern volatile uint8_t * gp_csi30_rx_address;         /* csi30 receive buffer address */
extern volatile uint16_t  g_csi30_rx_length;           /* csi30 receive data length */
extern volatile uint16_t  g_csi30_rx_count;            /* csi30 receive data count */
extern volatile uint8_t * gp_csi30_tx_address;         /* csi30 send buffer address */
extern volatile uint16_t  g_csi30_send_length;         /* csi30 send data length */
extern volatile uint16_t  g_csi30_tx_count;            /* csi30 send data count */
/* Start user code for global. Do not edit comment generated here */
extern uint8_t Send;
extern uint8_t Receive;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_csi30_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_csi30_interrupt(void)
{
    volatile uint8_t err_type;
    volatile uint8_t sio_dummy;

    err_type = (uint8_t)(SSR12 & _0001_SAU_OVERRUN_ERROR);
    SIR12 = (uint16_t)err_type;

    if (1U == err_type)
    {
        r_csi30_callback_error(err_type);    /* overrun error occurs */
    }
    else
    {
        if (g_csi30_tx_count > 0U)
        {
            if (0U != gp_csi30_rx_address)
            {
                *gp_csi30_rx_address = SIO30;
                gp_csi30_rx_address++;
            }
            else
            {
                sio_dummy = SIO30;
            }

            if (0U != gp_csi30_tx_address)
            {
                SIO30 = *gp_csi30_tx_address;
                gp_csi30_tx_address++;
            }
            else
            {
                SIO30 = 0xFFU;
            }

            g_csi30_tx_count--;
        }
        else 
        {
            if (0U == g_csi30_tx_count)
            {
                if (0U != gp_csi30_rx_address)
                {
                    *gp_csi30_rx_address = SIO30;
                }
                else
                {
                    sio_dummy = SIO30;
                }
            }

            r_csi30_callback_sendend();    /* complete send */
            r_csi30_callback_receiveend();    /* complete receive */
        }
    }
}
/***********************************************************************************************************************
* Function Name: r_csi30_callback_receiveend
* Description  : This function is a callback function when CSI30 finishes reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_csi30_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
    Receive = 1;
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_csi30_callback_error
* Description  : This function is a callback function when CSI30 reception error occurs.
* Arguments    : err_type -
*                    error type value
* Return Value : None
***********************************************************************************************************************/
static void r_csi30_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_csi30_callback_sendend
* Description  : This function is a callback function when CSI30 finishes transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_csi30_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    Send = 1;
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
