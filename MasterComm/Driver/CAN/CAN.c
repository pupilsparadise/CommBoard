#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "r_cg_sau.h"
#include <stdio.h>
#include "CAN.h"
 
// Control Register Reset Values up to FIFOs
const static uint32_t canControlResetValues[] = {
    /* Address 0x000 to 0x00C */
    0x04980760, 0x003E0F0F, 0x000E0303, 0x00021000,
    /* Address 0x010 to 0x01C */
    0x00000000, 0x00000000, 0x40400040, 0x00000000,
    /* Address 0x020 to 0x02C */
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    /* Address 0x030 to 0x03C */
    0x00000000, 0x00200000, 0x00000000, 0x00000000,
    /* Address 0x040 to 0x04C */
    0x00000400, 0x00000000, 0x00000000, 0x00000000
};
// FIFO Register Reset Values
const static uint32_t canFifoResetValues[] = {
    0x00600400, 0x00000000, 0x00000000
};
uint8_t spiTransmitBuffer[SPI_DEFAULT_BUFFER_LENGTH + 4];
uint8_t spiReceiveBuffer[SPI_DEFAULT_BUFFER_LENGTH + 4];
CAN_CONFIG 			    config;
CAN_TX_FIFO_CONFIG 		txConfig;
CAN_RX_FIFO_CONFIG 		rxConfig;
REG_CiFLTOBJ 			fObj;
REG_CiMASK 			    mObj;
CAN_TX_MSGOBJ 			txObj;
CAN_OPERATION_MODE      mcpMode = CAN_CLASSIC_MODE; // Current controller mode
uint8_t Send = 0;
uint8_t Receive = 0;

static CAN_RX_MSGOBJ rxObj;
static CAN_TX_FIFO_EVENT txFlags;
static uint8_t txd[MAX_DATA_BYTES];
static CAN_RX_FIFO_EVENT rxFlags;
static uint8_t rxd[MAX_DATA_BYTES];
uint8_t __flgFDF = 1;                      // 1: FD, 0: 2.0
uint8_t ext_flg = 0;; // identifier xxxID
uint8_t rtr_ = 0;
unsigned long can_id = 0; // can id

void CANFD_init(void)
{
    int i = 0;
    /*Reset device*/
    DRV_CANFDSPI_Reset();
    /*Enable ECC and initialize RAM*/
    DRV_CANFDSPI_EccEnable();
    DRV_CANFDSPI_RamInit(0xff);
    /*Configure device*/
    DRV_CANFDSPI_ConfigureObjectReset(&config); 
    config.IsoCrcEnable 	= 1;
    config.StoreInTEF 		= 0;
    DRV_CANFDSPI_Configure(&config);
    /*Setup TX FIFO*/
    DRV_CANFDSPI_TransmitChannelConfigureObjectReset(&txConfig);
    txConfig.FifoSize 		= 7;
    txConfig.PayLoadSize 	= CAN_PLSIZE_64;
    txConfig.TxPriority 	= 1;
    DRV_CANFDSPI_TransmitChannelConfigure(APP_TX_FIFO, &txConfig);
    /*Setup RX FIFO*/
    DRV_CANFDSPI_ReceiveChannelConfigureObjectReset(&rxConfig);
    rxConfig.FifoSize		 = 15;
    rxConfig.PayLoadSize 	 = CAN_PLSIZE_64;
    DRV_CANFDSPI_ReceiveChannelConfigure(APP_RX_FIFO, &rxConfig);
    /*Setup RX Filter*/
    //fObj.word = 0;
    //fObj.bF.SID = 0x300;
    //fObj.bF.EXIDE = 0;
    //fObj.bF.EID = 0x00;
    //DRV_CANFDSPI_FilterObjectConfigure(CAN_FILTER0, &fObj.bF);
    

    /*Setup RX Mask*/
    //mObj.word = 0;
    //mObj.bF.MSID = 0x7F8;
    //mObj.bF.MIDE = 1; // Only allow standard IDs
   // mObj.bF.MEID = 0x0;
    //DRV_CANFDSPI_FilterMaskConfigure(CAN_FILTER0, &mObj.bF);

    for(i=0; i<32; i++)
       CANFDSPI_FilterDisable((CAN_FILTER)i);          // disable all filter

    // Link FIFO and Filter
    DRV_CANFDSPI_FilterToFifoLink(CAN_FILTER0, APP_RX_FIFO, true);
    // Setup Bit Time
    DRV_CANFDSPI_BitTimeConfigure(CAN_500K_2M, CAN_SSP_MODE_AUTO, CAN_SYSCLK_20M);
    // Setup Transmit and Receive Interrupts
    DRV_CANFDSPI_GpioModeConfigure(GPIO_MODE_INT, GPIO_MODE_INT);
    #ifdef APP_USE_TX_INT
        DRV_CANFDSPI_TransmitChannelEventEnable(APP_TX_FIFO, CAN_TX_FIFO_NOT_FULL_EVENT);
    #endif
    DRV_CANFDSPI_ReceiveChannelEventEnable(APP_RX_FIFO, CAN_RX_FIFO_NOT_EMPTY_EVENT);
    DRV_CANFDSPI_ModuleEventEnable((CAN_MODULE_EVENT)(CAN_TX_EVENT | CAN_RX_EVENT));
    /*if (CAN_CLASSIC_MODE != CAN_SLEEP_MODE) { // if going to sleep, the value stored in opMode is not
        // changed so that we can return to it later
        mcpMode = (CAN_OPERATION_MODE)opMode;
    }*/
    DRV_CANFDSPI_OperationModeSelect(CAN_CLASSIC_MODE);
    
    //DRV_CANFDSPI_OperationModeSelect(CAN_EXTERNAL_LOOPBACK_MODE); 
}


int8_t DRV_CANFDSPI_ConfigureObjectReset(CAN_CONFIG* config)
{
    REG_CiCON ciCon = {0};
    ciCon.word 					            = canControlResetValues[cREGADDR_CiCON / 4];
    config->DNetFilterCount 			    = ciCon.bF.DNetFilterCount;
    config->IsoCrcEnable 			        = ciCon.bF.IsoCrcEnable;
    config->ProtocolExpectionEventDisable 	= ciCon.bF.ProtocolExceptionEventDisable;
    config->WakeUpFilterEnable 			    = ciCon.bF.WakeUpFilterEnable;
    config->WakeUpFilterTime 			    = ciCon.bF.WakeUpFilterTime;
    config->BitRateSwitchDisable 		    = ciCon.bF.BitRateSwitchDisable;
    config->RestrictReTxAttempts 		    = ciCon.bF.RestrictReTxAttempts;
    config->EsiInGatewayMode 			    = ciCon.bF.EsiInGatewayMode;
    config->SystemErrorToListenOnly 	    = ciCon.bF.SystemErrorToListenOnly;
    config->StoreInTEF 				        = ciCon.bF.StoreInTEF;
    config->TXQEnable 				        = ciCon.bF.TXQEnable;
    config->TxBandWidthSharing 			    = ciCon.bF.TxBandWidthSharing;
    return 0;
}

// Section: Configuration
int8_t DRV_CANFDSPI_Configure(CAN_CONFIG* config)
{
    REG_CiCON ciCon = {0};
    int8_t spiTransferError = 0;
    
    ciCon.word 					= canControlResetValues[cREGADDR_CiCON / 4];
    ciCon.bF.DNetFilterCount 			= config->DNetFilterCount;
    ciCon.bF.IsoCrcEnable 			= config->IsoCrcEnable;
    ciCon.bF.ProtocolExceptionEventDisable 	= config->ProtocolExpectionEventDisable;
    ciCon.bF.WakeUpFilterEnable 		= config->WakeUpFilterEnable;
    ciCon.bF.WakeUpFilterTime 			= config->WakeUpFilterTime;
    ciCon.bF.BitRateSwitchDisable 		= config->BitRateSwitchDisable;
    ciCon.bF.RestrictReTxAttempts 		= config->RestrictReTxAttempts;
    ciCon.bF.EsiInGatewayMode 			= config->EsiInGatewayMode;
    ciCon.bF.SystemErrorToListenOnly 		= config->SystemErrorToListenOnly;
    ciCon.bF.StoreInTEF 			= config->StoreInTEF;
    ciCon.bF.TXQEnable 				= config->TXQEnable;
    ciCon.bF.TxBandWidthSharing 		= config->TxBandWidthSharing;
    spiTransferError = DRV_CANFDSPI_WriteWord(cREGADDR_CiCON, ciCon.word);
    if (spiTransferError) {
        return -1;
    }
    return spiTransferError;
}

int8_t DRV_CANFDSPI_TransmitChannelConfigureObjectReset(CAN_TX_FIFO_CONFIG* config)
{
    REG_CiFIFOCON ciFifoCon = {0};
    ciFifoCon.word 		= canFifoResetValues[0];	// 10010010100101010000
    config->RTREnable 		= ciFifoCon.txBF.RTREnable;
    config->TxPriority 		= ciFifoCon.txBF.TxPriority;
    config->TxAttempts 		= ciFifoCon.txBF.TxAttempts;
    config->FifoSize 		= ciFifoCon.txBF.FifoSize;
    config->PayLoadSize 	= ciFifoCon.txBF.PayLoadSize;
    return 0;
}

int8_t CANFDSPI_FilterDisable(CAN_FILTER filter)
{
    uint16_t a;
    REG_CiFLTCON_BYTE fCtrl;
    int8_t spiTransferError = 0;

    // Read
    a = cREGADDR_CiFLTCON + filter;

    // mcp2518fd_ReadByte(uint16_t address, uint8_t *rxd)
    spiTransferError = DRV_CANFDSPI_ReadByte(a, &fCtrl.byte);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    fCtrl.bF.Enable = 0;

    // mcp2518fd_WriteByte(uint16_t address, uint8_t txd)
    spiTransferError = DRV_CANFDSPI_WriteByte(a, fCtrl.byte);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TransmitChannelEventEnable(CAN_FIFO_CHANNEL channel,CAN_TX_FIFO_EVENT flags)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;
    REG_CiFIFOCON ciFifoCon;
    // Read Interrupt Enables
    a = cREGADDR_CiFIFOCON + (channel * CiFIFO_OFFSET);
    ciFifoCon.word = 0;
    spiTransferError = DRV_CANFDSPI_ReadByte(a, &ciFifoCon.byte[0]);
    if (spiTransferError) {
        return -1;
    }
    // Modify
    ciFifoCon.byte[0] |= (flags & CAN_TX_FIFO_ALL_EVENTS);
    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(a, ciFifoCon.byte[0]);
    if (spiTransferError) {
        return -2;
    }
    return spiTransferError;
}

int8_t DRV_CANFDSPI_ReceiveChannelConfigureObjectReset(CAN_RX_FIFO_CONFIG* config)
{
    REG_CiFIFOCON ciFifoCon = {0};
    ciFifoCon.word = canFifoResetValues[0];

    config->FifoSize = ciFifoCon.rxBF.FifoSize;
    config->PayLoadSize = ciFifoCon.rxBF.PayLoadSize;
    config->RxTimeStampEnable = ciFifoCon.rxBF.RxTimeStampEnable;

    return 0;
}

int8_t DRV_CANFDSPI_ReceiveChannelEventEnable(CAN_FIFO_CHANNEL channel,CAN_RX_FIFO_EVENT flags)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;
    REG_CiFIFOCON ciFifoCon = {0};
    
    if (channel == CAN_TXQUEUE_CH0) return -100;
    // Read Interrupt Enables
    a = cREGADDR_CiFIFOCON + (channel * CiFIFO_OFFSET);
    ciFifoCon.word = 0;
    spiTransferError = DRV_CANFDSPI_ReadByte(a, &ciFifoCon.byte[0]);
    if (spiTransferError) {
        return -1;
    }
    // Modify
    ciFifoCon.byte[0] |= (flags & CAN_RX_FIFO_ALL_EVENTS);
    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(a, ciFifoCon.byte[0]);
    if (spiTransferError) {
        return -2;
    }
    return spiTransferError;
}

int8_t DRV_CANFDSPI_BitTimeConfigureNominal40MHz(CAN_BITTIME_SETUP bitTime)
{
    int8_t spiTransferError = 0;
    REG_CiNBTCFG ciNbtcfg;
    ciNbtcfg.word = canControlResetValues[cREGADDR_CiNBTCFG / 4];
    // Arbitration Bit rate
    switch (bitTime) {
            // All 500K
        case CAN_500K_1M:
        case CAN_500K_2M:
        case CAN_500K_3M:
        case CAN_500K_4M:
        case CAN_500K_5M:
        case CAN_500K_6M7:
        case CAN_500K_8M:
        case CAN_500K_10M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 62;
            ciNbtcfg.bF.TSEG2 = 15;
            ciNbtcfg.bF.SJW = 15;
            break;
            // All 250K
        case CAN_250K_500K:
        case CAN_250K_833K:
        case CAN_250K_1M:
        case CAN_250K_1M5:
        case CAN_250K_2M:
        case CAN_250K_3M:
        case CAN_250K_4M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 126;
            ciNbtcfg.bF.TSEG2 = 31;
            ciNbtcfg.bF.SJW = 31;
            break;
        case CAN_1000K_4M:
        case CAN_1000K_8M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 30;
            ciNbtcfg.bF.TSEG2 = 7;
            ciNbtcfg.bF.SJW = 7;
            break;
        case CAN_125K_500K:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 254;
            ciNbtcfg.bF.TSEG2 = 63;
            ciNbtcfg.bF.SJW = 63;
            break;
        default:
            return -1;
    }
    // Write Bit time registers
    spiTransferError = DRV_CANFDSPI_WriteWord(cREGADDR_CiNBTCFG, ciNbtcfg.word);
    return spiTransferError;
}

int8_t DRV_CANFDSPI_BitTimeConfigureData40MHz(CAN_BITTIME_SETUP bitTime,CAN_SSP_MODE sspMode)
{
    int8_t spiTransferError = 0;
    uint32_t tdcValue = 0;
    REG_CiDBTCFG ciDbtcfg;
    REG_CiTDC ciTdc;
    //    sspMode;
    ciDbtcfg.word = canControlResetValues[cREGADDR_CiDBTCFG / 4];
    ciTdc.word = 0;
    // Configure Bit time and sample point
    ciTdc.bF.TDCMode = CAN_SSP_MODE_AUTO;
    tdcValue = 0;
    // Data Bit rate and SSP
    switch (bitTime) {
        case CAN_500K_1M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 30;
            ciDbtcfg.bF.TSEG2 = 7;
            ciDbtcfg.bF.SJW = 7;
            // SSP
            ciTdc.bF.TDCOffset = 31;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_2M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 14;
            ciDbtcfg.bF.TSEG2 = 3;
            ciDbtcfg.bF.SJW = 3;
            // SSP
            ciTdc.bF.TDCOffset = 15;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_3M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 8;
            ciDbtcfg.bF.TSEG2 = 2;
            ciDbtcfg.bF.SJW = 2;
            // SSP
            ciTdc.bF.TDCOffset = 9;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_4M:
        case CAN_1000K_4M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 6;
            ciDbtcfg.bF.TSEG2 = 1;
            ciDbtcfg.bF.SJW = 1;
            // SSP
            ciTdc.bF.TDCOffset = 7;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_5M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 4;
            ciDbtcfg.bF.TSEG2 = 1;
            ciDbtcfg.bF.SJW = 1;
            // SSP
            ciTdc.bF.TDCOffset = 5;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_6M7:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 3;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 4;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_8M:
        case CAN_1000K_8M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 2;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 3;
            ciTdc.bF.TDCValue = 1;
            break;
        case CAN_500K_10M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 1;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 2;
            ciTdc.bF.TDCValue = 0;
            break;

        case CAN_250K_500K:
        case CAN_125K_500K:
            ciDbtcfg.bF.BRP = 1;
            ciDbtcfg.bF.TSEG1 = 30;
            ciDbtcfg.bF.TSEG2 = 7;
            ciDbtcfg.bF.SJW = 7;
            // SSP
            ciTdc.bF.TDCOffset = 31;
            ciTdc.bF.TDCValue = tdcValue;
            ciTdc.bF.TDCMode = CAN_SSP_MODE_OFF;
            break;
        case CAN_250K_833K:
            ciDbtcfg.bF.BRP = 1;
            ciDbtcfg.bF.TSEG1 = 17;
            ciDbtcfg.bF.TSEG2 = 4;
            ciDbtcfg.bF.SJW = 4;
            // SSP
            ciTdc.bF.TDCOffset = 18;
            ciTdc.bF.TDCValue = tdcValue;
            ciTdc.bF.TDCMode = CAN_SSP_MODE_OFF;
            break;
        case CAN_250K_1M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 30;
            ciDbtcfg.bF.TSEG2 = 7;
            ciDbtcfg.bF.SJW = 7;
            // SSP
            ciTdc.bF.TDCOffset = 31;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_1M5:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 18;
            ciDbtcfg.bF.TSEG2 = 5;
            ciDbtcfg.bF.SJW = 5;
            // SSP
            ciTdc.bF.TDCOffset = 19;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_2M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 14;
            ciDbtcfg.bF.TSEG2 = 3;
            ciDbtcfg.bF.SJW = 3;
            // SSP
            ciTdc.bF.TDCOffset = 15;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_3M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 8;
            ciDbtcfg.bF.TSEG2 = 2;
            ciDbtcfg.bF.SJW = 2;
            // SSP
            ciTdc.bF.TDCOffset = 9;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_4M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 6;
            ciDbtcfg.bF.TSEG2 = 1;
            ciDbtcfg.bF.SJW = 1;
            // SSP
            ciTdc.bF.TDCOffset = 7;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        default:
            return -1;
    }

    // Write Bit time registers
    spiTransferError = DRV_CANFDSPI_WriteWord(cREGADDR_CiDBTCFG, ciDbtcfg.word);
    if (spiTransferError) {
        return -2;
    }

    // Write Transmitter Delay Compensation
#ifdef REV_A
    ciTdc.bF.TDCOffset = 0;
    ciTdc.bF.TDCValue = 0;
#endif

    spiTransferError = DRV_CANFDSPI_WriteWord(cREGADDR_CiTDC, ciTdc.word);
    if (spiTransferError) {
        return -3;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_BitTimeConfigureNominal20MHz(CAN_BITTIME_SETUP bitTime)
{
    int8_t spiTransferError = 0;
    REG_CiNBTCFG ciNbtcfg;
    ciNbtcfg.word = canControlResetValues[cREGADDR_CiNBTCFG / 4];
    // Arbitration Bit rate
    switch (bitTime) {
            // All 500K
        case CAN_500K_1M:
        case CAN_500K_2M:
        case CAN_500K_4M:
        case CAN_500K_5M:
        case CAN_500K_6M7:
        case CAN_500K_8M:
        case CAN_500K_10M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 30;
            ciNbtcfg.bF.TSEG2 = 7;
            ciNbtcfg.bF.SJW = 7;
            break;

            // All 250K
        case CAN_250K_500K:
        case CAN_250K_833K:
        case CAN_250K_1M:
        case CAN_250K_1M5:
        case CAN_250K_2M:
        case CAN_250K_3M:
        case CAN_250K_4M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 62;
            ciNbtcfg.bF.TSEG2 = 15;
            ciNbtcfg.bF.SJW = 15;
            break;

        case CAN_1000K_4M:
        case CAN_1000K_8M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 14;
            ciNbtcfg.bF.TSEG2 = 3;
            ciNbtcfg.bF.SJW = 3;
            break;

        case CAN_125K_500K:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 126;
            ciNbtcfg.bF.TSEG2 = 31;
            ciNbtcfg.bF.SJW = 31;
            break;

        default:
            return -1;
    }
    // Write Bit time registers
    spiTransferError = DRV_CANFDSPI_WriteWord(cREGADDR_CiNBTCFG, ciNbtcfg.word);
    if (spiTransferError) {
        return -2;
    }
    return spiTransferError;
}

int8_t DRV_CANFDSPI_BitTimeConfigureData20MHz(CAN_BITTIME_SETUP bitTime,CAN_SSP_MODE sspMode)
{
    int8_t spiTransferError = 0;
    uint32_t tdcValue = 0;
    REG_CiDBTCFG ciDbtcfg;
    REG_CiTDC ciTdc;
    //    sspMode;
    ciDbtcfg.word = canControlResetValues[cREGADDR_CiDBTCFG / 4];
    ciTdc.word = 0;
    // Configure Bit time and sample point
    ciTdc.bF.TDCMode = CAN_SSP_MODE_AUTO;
    tdcValue = 0;

    // Data Bit rate and SSP
    switch (bitTime) {
        case CAN_500K_1M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 14;
            ciDbtcfg.bF.TSEG2 = 3;
            ciDbtcfg.bF.SJW = 3;
            // SSP
            ciTdc.bF.TDCOffset = 15;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_2M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 6;
            ciDbtcfg.bF.TSEG2 = 1;
            ciDbtcfg.bF.SJW = 1;
            // SSP
            ciTdc.bF.TDCOffset = 7;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_4M:
        case CAN_1000K_4M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 2;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 3;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_5M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 1;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 2;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_6M7:
        case CAN_500K_8M:
        case CAN_500K_10M:
        case CAN_1000K_8M:
            //qDebug("Data Bitrate not feasible with this clock!");
            return -1;

        case CAN_250K_500K:
        case CAN_125K_500K:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 30;
            ciDbtcfg.bF.TSEG2 = 7;
            ciDbtcfg.bF.SJW = 7;
            // SSP
            ciTdc.bF.TDCOffset = 31;
            ciTdc.bF.TDCValue = tdcValue;
            ciTdc.bF.TDCMode = CAN_SSP_MODE_OFF;
            break;
        case CAN_250K_833K:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 17;
            ciDbtcfg.bF.TSEG2 = 4;
            ciDbtcfg.bF.SJW = 4;
            // SSP
            ciTdc.bF.TDCOffset = 18;
            ciTdc.bF.TDCValue = tdcValue;
            ciTdc.bF.TDCMode = CAN_SSP_MODE_OFF;
            break;
        case CAN_250K_1M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 14;
            ciDbtcfg.bF.TSEG2 = 3;
            ciDbtcfg.bF.SJW = 3;
            // SSP
            ciTdc.bF.TDCOffset = 15;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_1M5:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 8;
            ciDbtcfg.bF.TSEG2 = 2;
            ciDbtcfg.bF.SJW = 2;
            // SSP
            ciTdc.bF.TDCOffset = 9;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_2M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 6;
            ciDbtcfg.bF.TSEG2 = 1;
            ciDbtcfg.bF.SJW = 1;
            // SSP
            ciTdc.bF.TDCOffset = 7;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_3M:
            //qDebug("Data Bitrate not feasible with this clock!");
            return -1;
        case CAN_250K_4M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 2;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 3;
            ciTdc.bF.TDCValue = tdcValue;
            break;

        default:
            return -1;
    }

    // Write Bit time registers
    spiTransferError = DRV_CANFDSPI_WriteWord(cREGADDR_CiDBTCFG, ciDbtcfg.word);
    if (spiTransferError) {
        return -2;
    }

    // Write Transmitter Delay Compensation
#ifdef REV_A
    ciTdc.bF.TDCOffset = 0;
    ciTdc.bF.TDCValue = 0;
#endif

    spiTransferError = DRV_CANFDSPI_WriteWord(cREGADDR_CiTDC, ciTdc.word);
    if (spiTransferError) {
        return -3;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_BitTimeConfigureNominal10MHz(CAN_BITTIME_SETUP bitTime)
{
    int8_t spiTransferError = 0;
    REG_CiNBTCFG ciNbtcfg;

    ciNbtcfg.word = canControlResetValues[cREGADDR_CiNBTCFG / 4];

    // Arbitration Bit rate
    switch (bitTime) {
            // All 500K
        case CAN_500K_1M:
        case CAN_500K_2M:
        case CAN_500K_4M:
        case CAN_500K_5M:
        case CAN_500K_6M7:
        case CAN_500K_8M:
        case CAN_500K_10M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 14;
            ciNbtcfg.bF.TSEG2 = 3;
            ciNbtcfg.bF.SJW = 3;
            break;

            // All 250K
        case CAN_250K_500K:
        case CAN_250K_833K:
        case CAN_250K_1M:
        case CAN_250K_1M5:
        case CAN_250K_2M:
        case CAN_250K_3M:
        case CAN_250K_4M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 30;
            ciNbtcfg.bF.TSEG2 = 7;
            ciNbtcfg.bF.SJW = 7;
            break;

        case CAN_1000K_4M:
        case CAN_1000K_8M:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 7;
            ciNbtcfg.bF.TSEG2 = 2;
            ciNbtcfg.bF.SJW = 2;
            break;

        case CAN_125K_500K:
            ciNbtcfg.bF.BRP = 0;
            ciNbtcfg.bF.TSEG1 = 62;
            ciNbtcfg.bF.TSEG2 = 15;
            ciNbtcfg.bF.SJW = 15;
            break;

        default:
            return -1;
    }

    // Write Bit time registers
    spiTransferError = DRV_CANFDSPI_WriteWord(cREGADDR_CiNBTCFG, ciNbtcfg.word);
    if (spiTransferError) {
        return -2;
    }
    return spiTransferError;
}

int8_t DRV_CANFDSPI_BitTimeConfigureData10MHz(CAN_BITTIME_SETUP bitTime,CAN_SSP_MODE sspMode)
{
    int8_t spiTransferError = 0;
    uint32_t tdcValue = 0;
    REG_CiDBTCFG ciDbtcfg;
    REG_CiTDC ciTdc;
    //    sspMode;

    ciDbtcfg.word = canControlResetValues[cREGADDR_CiDBTCFG / 4];
    ciTdc.word = 0;

    // Configure Bit time and sample point
    ciTdc.bF.TDCMode = CAN_SSP_MODE_AUTO;
    tdcValue = 0;

    // Data Bit rate and SSP
    switch (bitTime) {
        case CAN_500K_1M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 6;
            ciDbtcfg.bF.TSEG2 = 1;
            ciDbtcfg.bF.SJW = 1;
            // SSP
            ciTdc.bF.TDCOffset = 7;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_2M:
            // Data BR
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 2;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 3;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_500K_4M:
        case CAN_500K_5M:
        case CAN_500K_6M7:
        case CAN_500K_8M:
        case CAN_500K_10M:
        case CAN_1000K_4M:
        case CAN_1000K_8M:
            //qDebug("Data Bitrate not feasible with this clock!");
            return -1;

        case CAN_250K_500K:
        case CAN_125K_500K:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 14;
            ciDbtcfg.bF.TSEG2 = 3;
            ciDbtcfg.bF.SJW = 3;
            // SSP
            ciTdc.bF.TDCOffset = 15;
            ciTdc.bF.TDCValue = tdcValue;
            ciTdc.bF.TDCMode = CAN_SSP_MODE_OFF;
            break;
        case CAN_250K_833K:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 7;
            ciDbtcfg.bF.TSEG2 = 2;
            ciDbtcfg.bF.SJW = 2;
            // SSP
            ciTdc.bF.TDCOffset = 8;
            ciTdc.bF.TDCValue = tdcValue;
            ciTdc.bF.TDCMode = CAN_SSP_MODE_OFF;
            break;
        case CAN_250K_1M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 6;
            ciDbtcfg.bF.TSEG2 = 1;
            ciDbtcfg.bF.SJW = 1;
            // SSP
            ciTdc.bF.TDCOffset = 7;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_1M5:
            //qDebug("Data Bitrate not feasible with this clock!");
            return -1;
        case CAN_250K_2M:
            ciDbtcfg.bF.BRP = 0;
            ciDbtcfg.bF.TSEG1 = 2;
            ciDbtcfg.bF.TSEG2 = 0;
            ciDbtcfg.bF.SJW = 0;
            // SSP
            ciTdc.bF.TDCOffset = 3;
            ciTdc.bF.TDCValue = tdcValue;
            break;
        case CAN_250K_3M:
        case CAN_250K_4M:
            //qDebug("Data Bitrate not feasible with this clock!");
            return -1;
        default:
            return -1;
    }

    // Write Bit time registers
    spiTransferError = DRV_CANFDSPI_WriteWord(cREGADDR_CiDBTCFG, ciDbtcfg.word);
    if (spiTransferError) {
        return -2;
    }

    // Write Transmitter Delay Compensation
#ifdef REV_A
    ciTdc.bF.TDCOffset = 0;
    ciTdc.bF.TDCValue = 0;
#endif

    spiTransferError = DRV_CANFDSPI_WriteWord(cREGADDR_CiTDC, ciTdc.word);
    if (spiTransferError) {
        return -3;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_BitTimeConfigure(CAN_BITTIME_SETUP bitTime,CAN_SSP_MODE sspMode,CAN_SYSCLK_SPEED clk)
{
    int8_t spiTransferError = 0;

    // Decode clk
    switch (clk) {
        case CAN_SYSCLK_40M:
            spiTransferError = DRV_CANFDSPI_BitTimeConfigureNominal40MHz(bitTime);
            if (spiTransferError) return spiTransferError;

            spiTransferError = DRV_CANFDSPI_BitTimeConfigureData40MHz(bitTime, sspMode);
            break;
        case CAN_SYSCLK_20M:
            spiTransferError = DRV_CANFDSPI_BitTimeConfigureNominal20MHz(bitTime);
            if (spiTransferError) return spiTransferError;

            spiTransferError = DRV_CANFDSPI_BitTimeConfigureData20MHz(bitTime, sspMode);
            break;
        case CAN_SYSCLK_10M:
            spiTransferError = DRV_CANFDSPI_BitTimeConfigureNominal10MHz(bitTime);
            if (spiTransferError) return spiTransferError;

            spiTransferError = DRV_CANFDSPI_BitTimeConfigureData10MHz(bitTime, sspMode);
            break;
        default:
            spiTransferError = -1;
            break;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_Reset(void)
{
    int8_t spiTransferError = 0;
    uint16_t spiTransmitSize = 2;
    // Compose command
    spiTransmitBuffer[0] = (uint8_t) (cINSTRUCTION_RESET << 4);
    spiTransmitBuffer[1] = 0;
    CAN_CS_Clear();
    Send = 0;
    R_CSI30_Send_Receive(spiTransmitBuffer, spiTransmitSize, spiTransmitBuffer);
    while(!Send);
    CAN_CS_Set();
    return spiTransferError;
}

// Section: Operating mode
int8_t DRV_CANFDSPI_OperationModeSelect(CAN_OPERATION_MODE opMode)
{
    uint8_t d = 0;
    int8_t spiTransferError = 0;

    // Read
    spiTransferError = DRV_CANFDSPI_ReadByte(cREGADDR_CiCON + 3, &d);
    if (spiTransferError) {
        return -1;
    }
    // Modify
    d &= ~0x07;
    d |= opMode;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(cREGADDR_CiCON + 3, d);
    if (spiTransferError) {
        return -2;
    }
    return spiTransferError;
}

int8_t DRV_CANFDSPI_ModuleEventEnable(CAN_MODULE_EVENT flags)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;
    REG_CiINTENABLE intEnables;
    // Read Interrupt Enables
    a = cREGADDR_CiINTENABLE;
    intEnables.word = 0;
    spiTransferError = DRV_CANFDSPI_ReadHalfWord(a, &intEnables.word);
    if (spiTransferError) {
        return -1;
    }
    // Modify
    intEnables.word |= (flags & CAN_ALL_EVENTS);

    // Write
    spiTransferError = DRV_CANFDSPI_WriteHalfWord(a, intEnables.word);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_WriteHalfWord(uint16_t address, uint16_t txd)
{
    uint8_t i;
    uint16_t spiTransmitSize = 4;
    int8_t spiTransferError = 0;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_WRITE << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);

    // Split word into 2 bytes and add them to buffer
    for (i = 0; i < 2; i++) {
        spiTransmitBuffer[i + 2] = (uint8_t) ((txd >> (i * 8)) & 0xFF);
    }
    CAN_CS_Clear();
    Send = 0;
    spiTransferError = R_CSI30_Send_Receive(spiTransmitBuffer, spiTransmitSize, spiTransmitBuffer);
    while(!Send);
    CAN_CS_Set();
    return (spiTransferError);
}

int8_t DRV_CANFDSPI_ReadHalfWord(uint16_t address, uint16_t *rxd)
{
    uint8_t i;
    uint32_t x;
    uint16_t spiTransmitSize = 2;
    uint16_t spiReceiveSize = 2;
    int8_t spiTransferError = 0;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_READ << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);

    CAN_CS_Clear();
    Send = 0;
    spiTransferError = R_CSI30_Send_Receive(spiTransmitBuffer, spiTransmitSize, spiTransmitBuffer);
    while(!Send);
    Receive = 0;
    spiTransferError &= R_CSI30_Send_Receive(spiReceiveBuffer, spiReceiveSize, spiReceiveBuffer);
    while(!Receive);
    CAN_CS_Set();
        
    // Update data
    *rxd = 0;
    for (i = 0; i < 2; i++) {
        x = (uint32_t) spiReceiveBuffer[i];
        *rxd += x << (i * 8);
    }

    return (spiTransferError);
}

int8_t DRV_CANFDSPI_GpioModeConfigure(GPIO_PIN_MODE gpio0, GPIO_PIN_MODE gpio1)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;
    REG_IOCON iocon;
    // Read
    a = cREGADDR_IOCON + 3;
    iocon.word = 0;

    spiTransferError = DRV_CANFDSPI_ReadByte(a, &iocon.byte[3]);
    if (spiTransferError) {
        return -1;
    }

    // Modify
    iocon.bF.PinMode0 = gpio0;
    iocon.bF.PinMode1 = gpio1;

    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(a, iocon.byte[3]);
    if (spiTransferError) {
        return -2;
    }

    return spiTransferError;
}

// Section: ECC
int8_t DRV_CANFDSPI_EccEnable(void)
{
    int8_t spiTransferError = 0;
    uint8_t d = 0;
    // Read
    spiTransferError = DRV_CANFDSPI_ReadByte(cREGADDR_ECCCON, &d);
    if (spiTransferError) {
	__nop();
        return -1;
    }
    // Modify
    d |= 0x01;
    // Write
    spiTransferError = DRV_CANFDSPI_WriteByte(cREGADDR_ECCCON, d);
    if (spiTransferError) {
	    __nop();
        return -2;
    }
    __nop();
    return 0;
}

int8_t DRV_CANFDSPI_RamInit(uint8_t d)
{
    uint8_t txd[SPI_DEFAULT_BUFFER_LENGTH];
    uint32_t k;
    int8_t spiTransferError = 0;
    uint16_t a;

    // Prepare data
    for (k = 0; k < SPI_DEFAULT_BUFFER_LENGTH; k++) {
        txd[k] = d;
    }

    a = cRAMADDR_START;

    for (k = 0; k < (cRAM_SIZE / SPI_DEFAULT_BUFFER_LENGTH); k++) {
        spiTransferError = DRV_CANFDSPI_WriteByteArray(a, txd, SPI_DEFAULT_BUFFER_LENGTH);
        if (spiTransferError) {
            return -1;
        }
        a += SPI_DEFAULT_BUFFER_LENGTH;
    }

    return spiTransferError;
}

// Section: CAN Receive
int8_t DRV_CANFDSPI_FilterObjectConfigure(CAN_FILTER filter, CAN_FILTEROBJ_ID* id)
{
    uint16_t a;
    REG_CiFLTOBJ fObj;
    int8_t spiTransferError = 0;
    // Setup
    fObj.word = 0;
    fObj.bF = *id;
    a = cREGADDR_CiFLTOBJ + (filter * CiFILTER_OFFSET);
    spiTransferError = DRV_CANFDSPI_WriteWord(a, fObj.word);
    return spiTransferError;
}

int8_t DRV_CANFDSPI_FilterMaskConfigure(CAN_FILTER filter, CAN_MASKOBJ_ID* mask)
{
    uint16_t a;
    REG_CiMASK mObj;
    int8_t spiTransferError = 0;
    // Setup
    mObj.word = 0;
    mObj.bF = *mask;
    a = cREGADDR_CiMASK + (filter * CiFILTER_OFFSET);
    spiTransferError = DRV_CANFDSPI_WriteWord(a, mObj.word);
    return spiTransferError;
}

// Section: SPI Access Functions
int8_t DRV_CANFDSPI_ReadByte(uint16_t address, uint8_t *rxd)
{
    uint16_t spiTransmitSize = 2;
    uint16_t spiReceiveSize = 1;
    int8_t spiTransferError = 0;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_READ << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);
    spiTransmitBuffer[2] = 0;
    CAN_CS_Clear();
    Send = 0;
    spiTransferError = R_CSI30_Send_Receive(spiTransmitBuffer, spiTransmitSize, spiTransmitBuffer);
    while(!Send);
    Receive = 0;
    spiTransferError &= R_CSI30_Send_Receive(spiReceiveBuffer, spiReceiveSize, spiReceiveBuffer);
    while(!Receive);
    CAN_CS_Set();
    // Update data
    *rxd = spiReceiveBuffer[0];
    return (spiTransferError);
}


int8_t DRV_CANFDSPI_ReceiveChannelConfigure(CAN_FIFO_CHANNEL channel,CAN_RX_FIFO_CONFIG* config)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;
    REG_CiFIFOCON ciFifoCon = {0};
    if (channel == CAN_TXQUEUE_CH0) {
        return -100;
    }
    // Setup FIFO
    ciFifoCon.word 			= canFifoResetValues[0];
    ciFifoCon.rxBF.TxEnable 		= 0;
    ciFifoCon.rxBF.FifoSize 		= config->FifoSize;
    ciFifoCon.rxBF.PayLoadSize 		= config->PayLoadSize;
    ciFifoCon.rxBF.RxTimeStampEnable 	= config->RxTimeStampEnable;

    a = cREGADDR_CiFIFOCON + (channel * CiFIFO_OFFSET);
    spiTransferError = DRV_CANFDSPI_WriteWord(a, ciFifoCon.word);
    return spiTransferError;
}

int8_t DRV_CANFDSPI_FilterToFifoLink(CAN_FILTER filter,CAN_FIFO_CHANNEL channel, bool enable)
{
    uint16_t a;
    REG_CiFLTCON_BYTE fCtrl;
    int8_t spiTransferError = 0;

    // Enable
    if (enable) {
        fCtrl.bF.Enable = 1;
    } else {
        fCtrl.bF.Enable = 0;
    }

    // Link
    fCtrl.bF.BufferPointer = channel;
    a = cREGADDR_CiFLTCON + filter;

    spiTransferError = DRV_CANFDSPI_WriteByte(a, fCtrl.byte);

    return spiTransferError;
}


int8_t DRV_CANFDSPI_WriteByte(uint16_t address, uint8_t txd)
{
    uint16_t spiTransmitSize = 3;
    int8_t spiTransferError = 0;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_WRITE << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);
    spiTransmitBuffer[2] = txd;
    CAN_CS_Clear();
    Send = 0;
    spiTransferError = R_CSI30_Send_Receive(spiTransmitBuffer, spiTransmitSize, spiTransmitBuffer);
    while(!Send);
    CAN_CS_Set();
    return spiTransferError;
}


int8_t DRV_CANFDSPI_ReadByteArray(uint16_t address, uint8_t *rxd,uint16_t nBytes) 
{
    uint16_t i;
    uint16_t spiTransmitSize = 2;
    uint16_t spiReceiveSize = nBytes; 
    int8_t spiTransferError = 0; 

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_READ << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);

    CAN_CS_Clear();
    Send = 0;
    spiTransferError = R_CSI30_Send_Receive(spiTransmitBuffer, spiTransmitSize, spiTransmitBuffer);
    while(!Send);
    Receive = 0;
    spiTransferError &= R_CSI30_Send_Receive(spiReceiveBuffer, spiReceiveSize, spiReceiveBuffer);
    while(!Receive);
    CAN_CS_Set();
    // Update data
    for (i = 0; i < nBytes; i++) 
    {
        rxd[i] = spiReceiveBuffer[i];
    }
    return (spiTransferError);
}



int8_t DRV_CANFDSPI_WriteByteArray(uint16_t address, uint8_t *txd, uint16_t nBytes)
{
    uint16_t i;
    uint16_t spiTransmitSize = nBytes + 2;
    int8_t spiTransferError = 0;    
    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_WRITE << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);
    // Add data
    for (i = 2; i < spiTransmitSize; i++) {
        spiTransmitBuffer[i] = txd[i - 2];
    }
    CAN_CS_Clear();
    Send = 0;
    spiTransferError = R_CSI30_Send_Receive(spiTransmitBuffer, spiTransmitSize, spiTransmitBuffer);
    while(!Send);
    CAN_CS_Set();
    return (spiTransferError);
}

int8_t DRV_CANFDSPI_WriteWord(uint16_t address, uint32_t txd)
{
    uint8_t i;
    uint16_t spiTransmitSize = 6;
    int8_t spiTransferError = 0;
    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_WRITE << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);
    // Split word into 4 bytes and add them to buffer
    for (i = 0; i < 4; i++) {
        spiTransmitBuffer[i + 2] = (uint8_t) ((txd >> (i * 8)) & 0xFF);
    }

    CAN_CS_Clear();
    Send = 0;
    spiTransferError = R_CSI30_Send_Receive(spiTransmitBuffer, spiTransmitSize, spiTransmitBuffer);
    while(!Send);
    CAN_CS_Set();
    return (spiTransferError);
}

// Section: CAN Transmit
int8_t DRV_CANFDSPI_TransmitChannelConfigure(CAN_FIFO_CHANNEL channel,CAN_TX_FIFO_CONFIG* config)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Setup FIFO
    REG_CiFIFOCON ciFifoCon = {0};
    ciFifoCon.word = canFifoResetValues[0];

    ciFifoCon.txBF.TxEnable = 1;
    ciFifoCon.txBF.FifoSize = config->FifoSize;
    ciFifoCon.txBF.PayLoadSize = config->PayLoadSize;
    ciFifoCon.txBF.TxAttempts = config->TxAttempts;
    ciFifoCon.txBF.TxPriority = config->TxPriority;
    ciFifoCon.txBF.RTREnable = config->RTREnable;

    a = cREGADDR_CiFIFOCON + (channel * CiFIFO_OFFSET);

    spiTransferError = DRV_CANFDSPI_WriteWord(a, ciFifoCon.word);

    return spiTransferError;
}

uint32_t DRV_CANFDSPI_DlcToDataBytes(CAN_DLC dlc)
{
    uint32_t dataBytesInObject = 0;

    if (dlc < CAN_DLC_12) {
        dataBytesInObject = dlc;
    } else {
        switch (dlc) {
            case CAN_DLC_12:
                dataBytesInObject = 12;
                break;
            case CAN_DLC_16:
                dataBytesInObject = 16;
                break;
            case CAN_DLC_20:
                dataBytesInObject = 20;
                break;
            case CAN_DLC_24:
                dataBytesInObject = 24;
                break;
            case CAN_DLC_32:
                dataBytesInObject = 32;
                break;
            case CAN_DLC_48:
                dataBytesInObject = 48;
                break;
            case CAN_DLC_64:
                dataBytesInObject = 64;
                break;
            default:
                break;
        }
    }

    return dataBytesInObject;
}

// Section: Transmit FIFO Events
int8_t DRV_CANFDSPI_TransmitChannelEventGet(CAN_FIFO_CHANNEL channel, CAN_TX_FIFO_EVENT* flags)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;
    // Read Interrupt flags
    REG_CiFIFOSTA ciFifoSta;
    ciFifoSta.word = 0;
    a = cREGADDR_CiFIFOSTA + (channel * CiFIFO_OFFSET);
    spiTransferError = DRV_CANFDSPI_ReadByte(a, &ciFifoSta.byte[0]);
    if (spiTransferError) {
        return -1;
    }
    // Update data
    *flags = (CAN_TX_FIFO_EVENT) (ciFifoSta.byte[0] & CAN_TX_FIFO_ALL_EVENTS);
    return spiTransferError;
}

int8_t DRV_CANFDSPI_ReadWord(uint16_t address, uint32_t *rxd)
{
    uint8_t i;
    uint32_t x;
    uint16_t spiTransmitSize = 2;
    uint16_t spiReceiveSize = 4;
    int8_t spiTransferSuccess = 0;

    // Compose command
    spiTransmitBuffer[0] = (uint8_t) ((cINSTRUCTION_READ << 4) + ((address >> 8) & 0xF));
    spiTransmitBuffer[1] = (uint8_t) (address & 0xFF);

    CAN_CS_Clear();
    Receive = 0;
    spiTransferSuccess = R_CSI30_Send_Receive(spiTransmitBuffer, spiTransmitSize, spiTransmitBuffer);
    while(!Receive);
    Receive = 0;
    spiTransferSuccess &= R_CSI30_Send_Receive(spiReceiveBuffer, spiReceiveSize, spiReceiveBuffer);
    while(!Receive);
    CAN_CS_Set();
    if (!spiTransferSuccess) {
        return (!spiTransferSuccess);
    }

    // Update data
    *rxd = 0;
    for (i = 0; i < 4; i++) {
        x = (uint32_t) spiReceiveBuffer[i];
        *rxd += x << (i * 8);
    }

    return (!spiTransferSuccess);
}

int8_t DRV_CANFDSPI_ErrorCountStateGet(uint8_t* tec, uint8_t* rec,CAN_ERROR_STATE* flags)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;
    REG_CiTREC ciTrec;
    // Read Error
    a = cREGADDR_CiTREC;
    ciTrec.word = 0;
    spiTransferError = DRV_CANFDSPI_ReadWord(a, &ciTrec.word);
    if (spiTransferError) {
        return -1;
    }
    // Update data
    *tec = ciTrec.byte[1];
    *rec = ciTrec.byte[0];
    *flags = (CAN_ERROR_STATE) (ciTrec.byte[2] & CAN_ERROR_ALL);

    return spiTransferError;
}

int8_t DRV_CANFDSPI_ReadWordArray(uint16_t address,uint32_t *rxd, uint16_t nWords)
{
    uint16_t i, j, n;
    REG_t w;

    uint16_t spiTransmitSize = 2;
    uint16_t spiReceiveSize = nWords * 4;
    int8_t spiTransferError = 0;     

    // Compose command
    spiTransmitBuffer[0] = (cINSTRUCTION_READ << 4) + ((address >> 8) & 0xF);
    spiTransmitBuffer[1] = address & 0xFF;

   /* for (i = 2; i < spiTransferSize; i++) {
        spiTransmitBuffer[i] = 0;
    }*/

    CAN_CS_Clear();
    Send = 0;
    spiTransferError = R_CSI30_Send_Receive(spiTransmitBuffer, spiTransmitSize, spiTransmitBuffer);
    while(!Send);
    Receive = 0;
    spiTransferError &= R_CSI30_Send_Receive(spiReceiveBuffer, spiReceiveSize, spiReceiveBuffer);
    while(!Receive);
    CAN_CS_Set();
        
    // Convert Byte array to Word array
    n = 0;
    for (i = 0; i < nWords; i++) {
        w.word = 0;
        for (j = 0; j < 4; j++, n++) {
            w.byte[j] = spiReceiveBuffer[n];
        }
        rxd[i] = w.word;
    }
    return (spiTransferError);
}

int8_t DRV_CANFDSPI_TransmitChannelUpdate(CAN_FIFO_CHANNEL channel, bool flush)
{
    uint16_t a;
    REG_CiFIFOCON ciFifoCon;
    int8_t spiTransferError = 0;

    // Set UINC
    a = cREGADDR_CiFIFOCON + (channel * CiFIFO_OFFSET) + 1; // Byte that contains FRESET
    ciFifoCon.word = 0;
    ciFifoCon.txBF.UINC = 1;

    // Set TXREQ
    if (flush) {
        ciFifoCon.txBF.TxRequest = 1;
    }

    spiTransferError = DRV_CANFDSPI_WriteByte(a, ciFifoCon.byte[1]);
    if (spiTransferError) {
        return -1;
    }

    return spiTransferError;
}

int8_t DRV_CANFDSPI_TransmitChannelLoad(CAN_FIFO_CHANNEL channel,CAN_TX_MSGOBJ* txObj,uint8_t *txd, uint32_t txdNumBytes, bool flush)
{
    uint16_t a;
    uint32_t fifoReg[3] = {0};
    uint32_t dataBytesInObject;
    REG_CiFIFOCON ciFifoCon;
    REG_CiFIFOSTA ciFifoSta;
    REG_CiFIFOUA ciFifoUa;
    int8_t spiTransferError = 0;
    uint8_t txBuffer[76] = {0};
    uint8_t i;
    uint16_t n = 0;
    uint8_t j = 0;
    
    // Get FIFO registers
    a = cREGADDR_CiFIFOCON + (channel * CiFIFO_OFFSET);
    spiTransferError = DRV_CANFDSPI_ReadWordArray(a, fifoReg, 3);
    if (spiTransferError) {
        return -1;
    }
    // Check that it is a transmit buffer
    ciFifoCon.word = fifoReg[0];
    if (!ciFifoCon.txBF.TxEnable) {
        return -2;
    }
    // Check that DLC is big enough for data
    dataBytesInObject = DRV_CANFDSPI_DlcToDataBytes((CAN_DLC) txObj->bF.ctrl.DLC);
    if (dataBytesInObject < txdNumBytes) {
        return -3;
    }
    // Get status
    ciFifoSta.word = fifoReg[1];
    // Get address
    ciFifoUa.word = fifoReg[2];
#ifdef USERADDRESS_TIMES_FOUR
    a = 4 * ciFifoUa.bF.UserAddress;
#else
    a = ciFifoUa.bF.UserAddress;
#endif
    a += cRAMADDR_START;

    txBuffer[0] = txObj->byte[0]; //not using 'for' to reduce no of instructions
    txBuffer[1] = txObj->byte[1];
    txBuffer[2] = txObj->byte[2];
    txBuffer[3] = txObj->byte[3];
    txBuffer[4] = txObj->byte[4];
    txBuffer[5] = txObj->byte[5];
    txBuffer[6] = txObj->byte[6];
    txBuffer[7] = txObj->byte[7];
    
    for (i = 0; i < txdNumBytes; i++) {
        txBuffer[i + 8] = txd[i];
    }

    if (txdNumBytes % 4) {
        // Need to add bytes
        n = 4 - (txdNumBytes % 4);    //4
        i = txdNumBytes + 8;		//72

        for (j = 0; j < n; j++) {
            txBuffer[i + 8 + j] = 0;
        }
    }

    spiTransferError = DRV_CANFDSPI_WriteByteArray(a, txBuffer, txdNumBytes + 8 + n);
    if (spiTransferError) {
        return -4;
    }

    // Set UINC and TXREQ
    spiTransferError = DRV_CANFDSPI_TransmitChannelUpdate(channel, flush);
    if (spiTransferError) {
        return -5;
    }

    return spiTransferError;
}



int8_t mcp2518fd_TransmitChannelEventGet(CAN_FIFO_CHANNEL channel, CAN_TX_FIFO_EVENT *flags) {
    int8_t spiTransferError = 0;
    uint16_t a = 0;
    // Read Interrupt flags
    REG_CiFIFOSTA ciFifoSta;
    ciFifoSta.word = 0;
    a = cREGADDR_CiFIFOSTA + (channel * CiFIFO_OFFSET);
    spiTransferError = DRV_CANFDSPI_ReadByte(a, &ciFifoSta.byte[0]);
    if (spiTransferError) {
        return -1;
    }
    // Update data
    *flags = (CAN_TX_FIFO_EVENT)(ciFifoSta.byte[0] & CAN_TX_FIFO_ALL_EVENTS);
    return spiTransferError;
}


void mcp2518fd_TransmitMessageQueue(void) 
{
    uint8_t attempts = MAX_TXQUEUE_ATTEMPTS;
    CAN_ERROR_STATE errorFlags;
    uint8_t tec, rec;
    uint8_t n;

    // Check if FIFO is not full
    do {
        mcp2518fd_TransmitChannelEventGet(APP_TX_FIFO, &txFlags);
        if (attempts == 0) {
            __nop(); __nop();
            DRV_CANFDSPI_ErrorCountStateGet(&tec, &rec, &errorFlags);
            return;
        }
        attempts--;
    } while (!(txFlags & CAN_TX_FIFO_NOT_FULL_EVENT));

    // Load message and transmit
    n = DRV_CANFDSPI_DlcToDataBytes((CAN_DLC)txObj.bF.ctrl.DLC);
    DRV_CANFDSPI_TransmitChannelLoad(APP_TX_FIFO, &txObj, txd, n, true);
}

int8_t DRV_CANFDSPI_TransmitChannelEventGets(CAN_FIFO_CHANNEL channel, CAN_TX_FIFO_EVENT *flags)
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;

    // Read Interrupt flags
    REG_CiFIFOSTA ciFifoSta;
    ciFifoSta.word = 0;
    a = cREGADDR_CiFIFOSTA + (channel * CiFIFO_OFFSET);

    spiTransferError = DRV_CANFDSPI_ReadByte(a, &ciFifoSta.byte[0]);
    if (spiTransferError) {
        return -1;
    }

    // Update data
    *flags = (CAN_TX_FIFO_EVENT)(ciFifoSta.byte[0] & CAN_TX_FIFO_ALL_EVENTS);

    return spiTransferError;
}

int8_t MCP2518FD_ReceiveChannelEventGet(CAN_FIFO_CHANNEL channel, CAN_RX_FIFO_EVENT *flags) 
{
    int8_t spiTransferError = 0;
    uint16_t a = 0;
    REG_CiFIFOSTA ciFifoSta = {0};

    if (channel == CAN_TXQUEUE_CH0)
    return -100;

    // Read Interrupt flags
    ciFifoSta.word = 0;
    a = cREGADDR_CiFIFOSTA + (channel * CiFIFO_OFFSET);

    spiTransferError = DRV_CANFDSPI_ReadByte(a, &ciFifoSta.byte[0]);
    if (spiTransferError) {
        return -1;
    }

    // Update data
    *flags = (CAN_RX_FIFO_EVENT)(ciFifoSta.byte[0] & CAN_RX_FIFO_ALL_EVENTS);
    return spiTransferError;
}

uint8_t readRxTxStatus(void) 
{
    uint8_t ret;
    MCP2518FD_ReceiveChannelEventGet(APP_RX_FIFO, &rxFlags);
    ret = (uint8_t)rxFlags;
    return ret;
}

uint8_t mcp2518fd_readMsgBufID(uint8_t *len, uint8_t *buf) 
{
    uint8_t n;
    int i;
    mcp2518fd_ReceiveMessageGet(APP_RX_FIFO, &rxObj, rxd, MAX_DATA_BYTES);
    ext_flg = rxObj.bF.ctrl.IDE;
    //can_id = ext_flg? (rxObj.bF.id.EID | (rxObj.bF.id.SID << 18))   //:  rxObj.bF.id.SID;
    can_id = ext_flg? (rxObj.bF.id.EID | ((uint32_t) rxObj.bF.id.SID << 18)) : rxObj.bF.id.SID;
    rtr_ = rxObj.bF.ctrl.RTR;
    n = DRV_CANFDSPI_DlcToDataBytes((CAN_DLC)rxObj.bF.ctrl.DLC);
    if (len) {
        *len = n;
    }
    for (i = 0; i < n; i++) {
        buf[i] = rxd[i];
    }

    return 0;
}


int8_t mcp2518fd_ReceiveChannelUpdate(CAN_FIFO_CHANNEL channel) 
{
    uint16_t a = 0;
    REG_CiFIFOCON ciFifoCon;
    int8_t spiTransferError = 0;
    ciFifoCon.word = 0;
    // Set UINC
    a = cREGADDR_CiFIFOCON + (channel * CiFIFO_OFFSET) +   1; // Byte that contains FRESET
    ciFifoCon.rxBF.UINC = 1;
    // Write byte
    spiTransferError = DRV_CANFDSPI_WriteByte(a, ciFifoCon.byte[1]);
    return spiTransferError;
}

int8_t mcp2518fd_ReceiveMessageGet(CAN_FIFO_CHANNEL channel,CAN_RX_MSGOBJ *rxObj,uint8_t *rxd, uint8_t nBytes) 
{
    uint8_t n = 0;
    uint8_t i = 0;
    uint16_t a;
    uint32_t fifoReg[3];
    REG_CiFIFOCON ciFifoCon;
    REG_CiFIFOUA ciFifoUa;
    int8_t spiTransferError = 0;
    uint8_t ba[MAX_MSG_SIZE];
    REG_t myReg;
    // Get FIFO registers
    a = cREGADDR_CiFIFOCON + (channel * CiFIFO_OFFSET);

    spiTransferError = DRV_CANFDSPI_ReadWordArray(a, fifoReg, 3);
    if (spiTransferError) {
        return -1;
    }

    // Check that it is a receive buffer
    ciFifoCon.word = fifoReg[0];
    ciFifoCon.txBF.TxEnable = 0;
    if (ciFifoCon.txBF.TxEnable) {
        return -2;
    }

    // Get address
    ciFifoUa.word = fifoReg[2];
#ifdef USERADDRESS_TIMES_FOUR
    a = 4 * ciFifoUa.bF.UserAddress;
#else
    a = ciFifoUa.bF.UserAddress;
#endif
    a += cRAMADDR_START;

    // Number of bytes to read
    n = nBytes + 8; // Add 8 header bytes

    if (ciFifoCon.rxBF.RxTimeStampEnable) {
        n += 4; // Add 4 time stamp bytes
    }

    // Make sure we read a multiple of 4 bytes from RAM
    if (n % 4) {
        n = n + 4 - (n % 4);
    }

    // Read rxObj using one access
    if (n > MAX_MSG_SIZE) {
        n = MAX_MSG_SIZE;
    }
    spiTransferError = DRV_CANFDSPI_ReadByteArray(a, ba, n);
    if (spiTransferError) {
        return -3;
    }
    // Assign message header
    
    myReg.byte[0] = ba[0];
    myReg.byte[1] = ba[1];
    myReg.byte[2] = ba[2];
    myReg.byte[3] = ba[3];
    rxObj->word[0] = myReg.word;

    myReg.byte[0] = ba[4];
    myReg.byte[1] = ba[5];
    myReg.byte[2] = ba[6];
    myReg.byte[3] = ba[7];
    rxObj->word[1] = myReg.word;

    if (ciFifoCon.rxBF.RxTimeStampEnable) {
        myReg.byte[0] = ba[8];
        myReg.byte[1] = ba[9];
        myReg.byte[2] = ba[10];
        myReg.byte[3] = ba[11];
        rxObj->word[2] = myReg.word;

        // Assign message data
        for (i = 0; i < nBytes; i++) {
            rxd[i] = ba[i + 12];
        }
    } else {
        rxObj->word[2] = 0;

        // Assign message data
        for (i = 0; i < nBytes; i++) {
            rxd[i] = ba[i + 8];
        }
    }
    // UINC channel
    spiTransferError = mcp2518fd_ReceiveChannelUpdate(channel);
    if (spiTransferError) {
        return -4;
    }
    return spiTransferError;
}

uint8_t readMsgBufID(uint8_t status, unsigned long *id, uint8_t *ext, uint8_t *rtr, uint8_t *len, uint8_t *buf) 
{
    uint8_t r = 0;
    r = mcp2518fd_readMsgBufID(len, buf);
    if (id)
    	*id  = can_id;
    if (ext)
    	*ext = ext_flg;
    if (rtr)
    	*rtr = rtr_;
    return r;
}

uint8_t readMsgBuf(uint8_t *len, uint8_t *buf) 
{
    return readMsgBufID(readRxTxStatus(), &can_id, &ext_flg, &rtr_, len, buf);
}

uint8_t CAN_Send(unsigned long id, uint8_t ext, uint8_t rtr, uint8_t len, const uint8_t *buf, bool wait_sent)
{
	wait_sent = true;
	return MCP2518FD_SendMsg(buf, len, id, ext, rtr, wait_sent);
}

uint8_t MCP2518FD_SendMsg(const uint8_t *buf, uint8_t len, unsigned long id, uint8_t ext, uint8_t rtr, bool wait_sent) 
{
    uint8_t n;
    int i;
    uint8_t spiTransferError = 0;
    // Configure message data
    txObj.word[0] = 0;
    txObj.word[1] = 0;

    txObj.bF.ctrl.RTR = !!rtr;
    if (rtr && len > CAN_DLC_8) {
        len = CAN_DLC_8;
    }
    txObj.bF.ctrl.DLC = len;

    txObj.bF.ctrl.IDE = !!ext;
    if (ext) {
        txObj.bF.id.SID = (id >> 18) & 0x7FF;
        txObj.bF.id.EID = id & 0x3FFFF;
    } else {
        txObj.bF.id.SID = id;
    }

    txObj.bF.ctrl.BRS = true;

    //txObj.bF.ctrl.FDF = (len > 8);
    txObj.bF.ctrl.FDF = __flgFDF;
    //if(__flgFDF)
        
    
    n = DRV_CANFDSPI_DlcToDataBytes((CAN_DLC)txObj.bF.ctrl.DLC);
    // Prepare data
    for (i = 0; i < n; i++) {
        txd[i] = buf[i];
    }

    mcp2518fd_TransmitMessageQueue();
    return spiTransferError;
}

unsigned long getCanId(void)    { return can_id; }

uint8_t isRemoteRequest(void)   { return rtr_; }

uint8_t isExtendedFrame(void)   { return ext_flg;}

/*
void OtdUart_DebugSend_CAN(volatile const char *s)
{
	//TODO: check the altenative for sprintf
	const uint8_t debug_data[1024];
	sprintf(debug_data,"%s",s);
	
	uartFlg.dbg_wifi_tx = True;
    	R_UART1_Send(debug_data, strlen(debug_data));
    	while(uartFlg.dbg_wifi_tx);
	
}*/