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
* File Name    : r_cg_intp.c
* Version      : Code Generator for RL78/I1C V1.01.07.02 [08 Nov 2021]
* Device(s)    : R5F10NPJ
* Tool-Chain   : CCRL
* Description  : This file implements device driver for INTP module.
* Creation Date: 10-05-2024
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_intp.h"
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
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_INTC_Create
* Description  : This function initializes INTP module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_INTC_Create(void)
{
    PMK0 = 1U;      /* disable INTP0 interrupt */
    PIF0 = 0U;      /* clear INTP0 interrupt flag */
    PMK1 = 1U;      /* disable INTP1 interrupt */
    PIF1 = 0U;      /* clear INTP1 interrupt flag */
    PMK2 = 1U;      /* disable INTP2 interrupt */
    PIF2 = 0U;      /* clear INTP2 interrupt flag */
    PMK3 = 1U;      /* disable INTP3 interrupt */
    PIF3 = 0U;      /* clear INTP3 interrupt flag */
    PMK4 = 1U;      /* disable INTP4 interrupt */
    PIF4 = 0U;      /* clear INTP4 interrupt flag */
    PMK5 = 1U;      /* disable INTP5 interrupt */
    PIF5 = 0U;      /* clear INTP5 interrupt flag */
    PMK6 = 1U;      /* disable INTP6 interrupt */
    PIF6 = 0U;      /* clear INTP6 interrupt flag */
    PMK7 = 1U;      /* disable INTP7 interrupt */
    PIF7 = 0U;      /* clear INTP7 interrupt flag */
    RTCIMK0 = 1U;   /* disable INTRTCIC0 interrupt */
    RTCIIF0 = 0U;   /* clear INTRTCIC0 interrupt flag */
    RTCIMK1 = 1U;   /* disable INTRTCIC1 interrupt */
    RTCIIF1 = 0U;   /* clear INTRTCIC1 interrupt flag */
    RTCIMK2 = 1U;   /* disable INTRTCIC2 interrupt */
    RTCIIF2 = 0U;   /* clear INTRTCIC2 interrupt flag */
    /* Set INTP0 level 1 priority */
    PPR10 = 0U;
    PPR00 = 1U;
    EGN0 = _01_INTP0_EDGE_FALLING_BOTH;
    EGP0 = _00_INTP0_EDGE_FALLING_DISABLE;
    /* Set INTP0 pin */
    ISCLCD |= 0x02U;
}

/***********************************************************************************************************************
* Function Name: R_INTC0_Start
* Description  : This function clears INTP0 interrupt flag and enables interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_INTC0_Start(void)
{
    PIF0 = 0U;      /* clear INTP0 interrupt flag */
    PMK0 = 0U;      /* enable INTP0 interrupt */
}
/***********************************************************************************************************************
* Function Name: R_INTC0_Stop
* Description  : This function disables INTP0 interrupt and clears interrupt flag.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_INTC0_Stop(void)
{
    PMK0 = 1U;      /* disable INTP0 interrupt */
    PIF0 = 0U;      /* clear INTP0 interrupt flag */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
