#ifndef CAN_H
#define CAN_H


#define CAN_ID				1
#define STANDARD_FRAME			0
#define EXTENDED_FRAME			1


#define SPI_DEFAULT_BUFFER_LENGTH       96
#define MAX_DATA_BYTES 			64



#define MAX_TXQUEUE_ATTEMPTS 50

/*SPI Instruction set*/
#define cINSTRUCTION_RESET		0x00
#define cINSTRUCTION_READ		0x03
#define cINSTRUCTION_WRITE		0x02


#define cREGADDR_OSC        		0xE00
#define cREGADDR_IOCON      		0xE04
#define cREGADDR_CRC    		0xE08
#define cREGADDR_ECCCON  		0xE0C
#define cREGADDR_ECCSTA  		0xE10

/*RAM Addresses*/
#define cRAMADDR_START  		0x400
#define cRAM_SIZE       		2048

#define MAX_MSG_SIZE			76

#define CAN_CS_Clear()			{P5_bit.no1 = 0;}
#define CAN_CS_Set()			{P5_bit.no1 = 1;}

/* Register Addresses */

/* CAN FD Controller */
#define cREGADDR_CiCON  		0x000
#define cREGADDR_CiNBTCFG		0x004
#define cREGADDR_CiDBTCFG		0x008
#define cREGADDR_CiTDC  		0x00C

#define cREGADDR_CiTBC      		0x010
#define cREGADDR_CiTSCON   		0x014
#define cREGADDR_CiVEC      		0x018
#define cREGADDR_CiINT      		0x01C
#define cREGADDR_CiINTFLAG      	cREGADDR_CiINT
#define cREGADDR_CiINTENABLE    	(cREGADDR_CiINT+2)

#define cREGADDR_CiRXIF     		0x020
#define cREGADDR_CiTXIF     		0x024
#define cREGADDR_CiRXOVIF   		0x028
#define cREGADDR_CiTXATIF   		0x02C

#define cREGADDR_CiTXREQ    		0x030
#define cREGADDR_CiTREC     		0x034
#define cREGADDR_CiBDIAG0   		0x038
#define cREGADDR_CiBDIAG1   		0x03C

#define cREGADDR_CiTEFCON   		0x040
#define cREGADDR_CiTEFSTA   		0x044
#define cREGADDR_CiTEFUA    		0x048
#define cREGADDR_CiFIFOBA   		0x04C

#define cREGADDR_CiFIFOCON  		0x050
#define cREGADDR_CiFIFOSTA  		0x054
#define cREGADDR_CiFIFOUA   		0x058
#define CiFIFO_OFFSET       		(3*4)

#define cREGADDR_CiTXQCON  		0x050
#define cREGADDR_CiTXQSTA  		0x054
#define cREGADDR_CiTXQUA   		0x058

#define APP_TX_FIFO 			CAN_FIFO_CH2
#define APP_RX_FIFO 			CAN_FIFO_CH1

// The filters start right after the FIFO control/status registers
#define cREGADDR_CiFLTCON   		(cREGADDR_CiFIFOCON+(CiFIFO_OFFSET*CAN_FIFO_TOTAL_CHANNELS))
#define cREGADDR_CiFLTOBJ   		(cREGADDR_CiFLTCON+CAN_FIFO_TOTAL_CHANNELS)
#define cREGADDR_CiMASK     		(cREGADDR_CiFLTOBJ+4)

#define CiFILTER_OFFSET     		(2*4)


extern uint8_t Send;
extern uint8_t Receive;

//! CAN Control Register
typedef union _REG_CiCON {
    struct {
        uint32_t DNetFilterCount : 5;
        uint32_t IsoCrcEnable : 1;
        uint32_t ProtocolExceptionEventDisable : 1;
        uint32_t unimplemented1 : 1;
        uint32_t WakeUpFilterEnable : 1;
        uint32_t WakeUpFilterTime : 2;
        uint32_t unimplemented2 : 1;
        uint32_t BitRateSwitchDisable : 1;
        uint32_t unimplemented3 : 3;
        uint32_t RestrictReTxAttempts : 1;
        uint32_t EsiInGatewayMode : 1;
        uint32_t SystemErrorToListenOnly : 1;
        uint32_t StoreInTEF : 1;
        uint32_t TXQEnable : 1;
        uint32_t OpMode : 3;
        uint32_t RequestOpMode : 3;
        uint32_t AbortAllTx : 1;
        uint32_t TxBandWidthSharing : 4;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiCON;

//! CAN Configure
typedef struct _CAN_CONFIG {
    uint32_t DNetFilterCount : 5;
    uint32_t IsoCrcEnable : 1;
    uint32_t ProtocolExpectionEventDisable : 1;
    uint32_t WakeUpFilterEnable : 1;
    uint32_t WakeUpFilterTime : 2;
    uint32_t BitRateSwitchDisable : 1;
    uint32_t RestrictReTxAttempts : 1;
    uint32_t EsiInGatewayMode : 1;
    uint32_t SystemErrorToListenOnly : 1;
    uint32_t StoreInTEF : 1;
    uint32_t TXQEnable : 1;
    uint32_t TxBandWidthSharing : 4;
} CAN_CONFIG;

//! CAN Transmit Channel Configure
typedef struct _CAN_TX_FIFO_CONFIG {
    uint32_t RTREnable : 1;
    uint32_t TxPriority : 5;
    uint32_t TxAttempts : 2;
    uint32_t FifoSize : 5;
    uint32_t PayLoadSize : 3;
} CAN_TX_FIFO_CONFIG;

//! CAN Receive Channel Configure

typedef struct _CAN_RX_FIFO_CONFIG {
    uint32_t RxTimeStampEnable : 1;
    uint32_t FifoSize : 5;
    uint32_t PayLoadSize : 3;
} CAN_RX_FIFO_CONFIG;


typedef union _REG_CiFIFOCON {
    // Receive FIFO
    struct {
        uint32_t RxNotEmptyIE : 1;
        uint32_t RxHalfFullIE : 1;
        uint32_t RxFullIE : 1;
        uint32_t RxOverFlowIE : 1;
        uint32_t unimplemented1 : 1;
        uint32_t RxTimeStampEnable : 1;
        uint32_t unimplemented2 : 1;
        uint32_t TxEnable : 1;
        uint32_t UINC : 1;
        uint32_t unimplemented3 : 1;
        uint32_t FRESET : 1;
        uint32_t unimplemented4 : 13;
        uint32_t FifoSize : 5;
        uint32_t PayLoadSize : 3;
    } rxBF;
    // Transmit FIFO
    struct {
        uint32_t TxNotFullIE : 1;
        uint32_t TxHalfFullIE : 1;
        uint32_t TxEmptyIE : 1;
        uint32_t unimplemented1 : 1;
        uint32_t TxAttemptIE : 1;
        uint32_t unimplemented2 : 1;
        uint32_t RTREnable : 1;
        uint32_t TxEnable : 1;
        uint32_t UINC : 1;
        uint32_t TxRequest : 1;
        uint32_t FRESET : 1;
        uint32_t unimplemented3 : 5;
        uint32_t TxPriority : 5;
        uint32_t TxAttempts : 2;
        uint32_t unimplemented4 : 1;
        uint32_t FifoSize : 5;
        uint32_t PayLoadSize : 3;
    } txBF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiFIFOCON;

//! CAN Filter Object ID
typedef struct _CAN_FILTEROBJ_ID {
    uint32_t SID : 11;
    uint32_t EID : 18;
    uint32_t SID11 : 1;
    uint32_t EXIDE : 1;
    uint32_t unimplemented1 : 1;
} CAN_FILTEROBJ_ID;

//! Filter Object Register
typedef union _REG_CiFLTOBJ {
    CAN_FILTEROBJ_ID bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiFLTOBJ;

//! CAN Mask Object ID
typedef struct _CAN_MASKOBJ_ID {
    uint32_t MSID : 11;
    uint32_t MEID : 18;
    uint32_t MSID11 : 1;
    uint32_t MIDE : 1;
    uint32_t unimplemented1 : 1;
} CAN_MASKOBJ_ID;
//! Mask Object Register
typedef union _REG_CiMASK {
    CAN_MASKOBJ_ID bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiMASK;

//! Filter Control Register
typedef union _REG_CiFLTCON_BYTE {

    struct {
        uint32_t BufferPointer : 5;
        uint32_t unimplemented1 : 2;
        uint32_t Enable : 1;
    } bF;
    uint8_t byte;
} REG_CiFLTCON_BYTE;

//! Nominal Bit Time Configuration Register
typedef union _REG_CiNBTCFG {

    struct {
        uint32_t SJW : 7;
        uint32_t unimplemented1 : 1;
        uint32_t TSEG2 : 7;
        uint32_t unimplemented2 : 1;
        uint32_t TSEG1 : 8;
        uint32_t BRP : 8;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiNBTCFG;

//! Data Bit Time Configuration Register

typedef union _REG_CiDBTCFG {

    struct {
        uint32_t SJW : 4;
        uint32_t unimplemented1 : 4;
        uint32_t TSEG2 : 4;
        uint32_t unimplemented2 : 4;
        uint32_t TSEG1 : 5;
        uint32_t unimplemented3 : 3;
        uint32_t BRP : 8;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiDBTCFG;

//! Transmitter Delay Compensation Register
typedef union _REG_CiTDC {

    struct {
        uint32_t TDCValue : 6;
        uint32_t unimplemented1 : 2;
        uint32_t TDCOffset : 7;
        uint32_t unimplemented2 : 1;
        uint32_t TDCMode : 2;
        uint32_t unimplemented3 : 6;
        uint32_t SID11Enable : 1;
        uint32_t EdgeFilterEnable : 1;
        uint32_t unimplemented4 : 6;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiTDC;

//! I/O Control Register
typedef union _REG_IOCON {
    struct {
        uint32_t TRIS0 : 1;
        uint32_t TRIS1 : 1;
        uint32_t unimplemented1 : 2;
        uint32_t ClearAutoSleepOnMatch : 1;
        uint32_t AutoSleepEnable : 1;
        uint32_t XcrSTBYEnable : 1;
        uint32_t unimplemented2 : 1;
        uint32_t LAT0 : 1;
        uint32_t LAT1 : 1;
        uint32_t unimplemented3 : 5;
        uint32_t HVDETSEL : 1;
        uint32_t GPIO0 : 1;
        uint32_t GPIO1 : 1;
        uint32_t unimplemented4 : 6;
        uint32_t PinMode0 : 1;
        uint32_t PinMode1 : 1;
        uint32_t unimplemented5 : 2;
        uint32_t TXCANOpenDrain : 1;
        uint32_t SOFOutputEnable : 1;
        uint32_t INTPinOpenDrain : 1;
        uint32_t unimplemented6 : 1;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_IOCON;

typedef struct _CAN_TX_MSGOBJ_CTRL {
    uint32_t DLC : 4;
    uint32_t IDE : 1;
    uint32_t RTR : 1;
    uint32_t BRS : 1;
    uint32_t FDF : 1;
    uint32_t ESI : 1;
#ifdef MCP2517FD
    uint32_t SEQ : 7;
    uint32_t unimplemented1 : 16;
#else
    uint32_t SEQ : 23;
#endif
} CAN_TX_MSGOBJ_CTRL;
//! CAN Message Object ID
typedef struct _CAN_MSGOBJ_ID {
    uint32_t SID : 11;
    uint32_t EID : 18;
    uint32_t SID11 : 1;
    uint32_t unimplemented1 : 2;
} CAN_MSGOBJ_ID;

//! CAN Message Time Stamp
typedef uint32_t CAN_MSG_TIMESTAMP;
//! CAN TX Message Object
typedef union _CAN_TX_MSGOBJ {
    struct {
        CAN_MSGOBJ_ID id;
        CAN_TX_MSGOBJ_CTRL ctrl;
        CAN_MSG_TIMESTAMP timeStamp;
    } bF;
    uint32_t word[3];
    uint8_t byte[12];
} CAN_TX_MSGOBJ;

//! FIFO Status Register

typedef union _REG_CiFIFOSTA {
    // Receive FIFO

    struct {
        uint32_t RxNotEmptyIF : 1;
        uint32_t RxHalfFullIF : 1;
        uint32_t RxFullIF : 1;
        uint32_t RxOverFlowIF : 1;
        uint32_t unimplemented1 : 4;
        uint32_t FifoIndex : 5;
        uint32_t unimplemented2 : 19;
    } rxBF;
    // Transmit FIFO
    struct {
        uint32_t TxNotFullIF : 1;
        uint32_t TxHalfFullIF : 1;
        uint32_t TxEmptyIF : 1;
        uint32_t unimplemented1 : 1;
        uint32_t TxAttemptIF : 1;
        uint32_t TxError : 1;
        uint32_t TxLostArbitration : 1;
        uint32_t TxAborted : 1;
        uint32_t FifoIndex : 5;
        uint32_t unimplemented2 : 19;
    } txBF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiFIFOSTA;

//! CAN RX Message Object Control
typedef struct _CAN_RX_MSGOBJ_CTRL {
  uint32_t DLC : 4;
  uint32_t IDE : 1;
  uint32_t RTR : 1;
  uint32_t BRS : 1;
  uint32_t FDF : 1;
  uint32_t ESI : 1;
  uint32_t unimplemented1 : 2;
  uint32_t FilterHit : 5;
  uint32_t unimplemented2 : 16;
} CAN_RX_MSGOBJ_CTRL;

//! CAN RX Message Object
typedef union _CAN_RX_MSGOBJ {

  struct {
    CAN_MSGOBJ_ID id;
    CAN_RX_MSGOBJ_CTRL ctrl;
    CAN_MSG_TIMESTAMP timeStamp;
  } bF;
  uint32_t word[3];
  uint8_t byte[12];
} CAN_RX_MSGOBJ;

//! CAN Data Length Code
typedef enum {
    CAN_DLC_0 = 0,
    CAN_DLC_1,
    CAN_DLC_2,
    CAN_DLC_3,
    CAN_DLC_4,
    CAN_DLC_5,
    CAN_DLC_6,
    CAN_DLC_7,
    CAN_DLC_8,
    CAN_DLC_12,
    CAN_DLC_16,
    CAN_DLC_20,
    CAN_DLC_24,
    CAN_DLC_32,
    CAN_DLC_48,
    CAN_DLC_64
} CAN_DLC;

//! FIFO Payload Size
typedef enum {
    CAN_PLSIZE_8 = 0,
    CAN_PLSIZE_12,
    CAN_PLSIZE_16,
    CAN_PLSIZE_20,
    CAN_PLSIZE_24,
    CAN_PLSIZE_32,
    CAN_PLSIZE_48,
    CAN_PLSIZE_64
} CAN_FIFO_PLSIZE;

//! CAN FIFO Channels
typedef enum {
    CAN_FIFO_CH0 = 0,   // CAN_TXQUEUE_CH0
    CAN_FIFO_CH1,
    CAN_FIFO_CH2,
    CAN_FIFO_CH3,
    CAN_FIFO_CH4,
    CAN_FIFO_CH5,
    CAN_FIFO_CH6,
    CAN_FIFO_CH7,
    CAN_FIFO_CH8,
    CAN_FIFO_CH9,
    CAN_FIFO_CH10,
    CAN_FIFO_CH11,
    CAN_FIFO_CH12,
    CAN_FIFO_CH13,
    CAN_FIFO_CH14,
    CAN_FIFO_CH15,
    CAN_FIFO_CH16,
    CAN_FIFO_CH17,
    CAN_FIFO_CH18,
    CAN_FIFO_CH19,
    CAN_FIFO_CH20,
    CAN_FIFO_CH21,
    CAN_FIFO_CH22,
    CAN_FIFO_CH23,
    CAN_FIFO_CH24,
    CAN_FIFO_CH25,
    CAN_FIFO_CH26,
    CAN_FIFO_CH27,
    CAN_FIFO_CH28,
    CAN_FIFO_CH29,
    CAN_FIFO_CH30,
    CAN_FIFO_CH31,
    CAN_FIFO_TOTAL_CHANNELS
} CAN_FIFO_CHANNEL;

#define CAN_TXQUEUE_CH0 CAN_FIFO_CH0

//! CAN Filter Channels
typedef enum {
    CAN_FILTER0 = 0,
    CAN_FILTER1,
    CAN_FILTER2,
    CAN_FILTER3,
    CAN_FILTER4,
    CAN_FILTER5,
    CAN_FILTER6,
    CAN_FILTER7,
    CAN_FILTER8,
    CAN_FILTER9,
    CAN_FILTER10,
    CAN_FILTER11,
    CAN_FILTER12,
    CAN_FILTER13,
    CAN_FILTER14,
    CAN_FILTER15,
    CAN_FILTER16,
    CAN_FILTER17,
    CAN_FILTER18,
    CAN_FILTER19,
    CAN_FILTER20,
    CAN_FILTER21,
    CAN_FILTER22,
    CAN_FILTER23,
    CAN_FILTER24,
    CAN_FILTER25,
    CAN_FILTER26,
    CAN_FILTER27,
    CAN_FILTER28,
    CAN_FILTER29,
    CAN_FILTER30,
    CAN_FILTER31,
    CAN_FILTER_TOTAL,
} CAN_FILTER;

//! CAN Bit Time Setup: Arbitration/Data Bit Phase
typedef enum {
    CAN_500K_1M = 0, // 0x00
    CAN_500K_2M, // 0x01
    CAN_500K_3M,
    CAN_500K_4M,
    CAN_500K_5M, // 0x04
    CAN_500K_6M7,
    CAN_500K_8M, // 0x06
    CAN_500K_10M,
    CAN_250K_500K, // 0x08
    CAN_250K_833K,
    CAN_250K_1M,
    CAN_250K_1M5,
    CAN_250K_2M,
    CAN_250K_3M,
    CAN_250K_4M,
    CAN_1000K_4M, // 0x0f
    CAN_1000K_8M,
    CAN_125K_500K // 0x11
} CAN_BITTIME_SETUP;

//! Secondary Sample Point Mode
typedef enum {
    CAN_SSP_MODE_OFF = 0,
    CAN_SSP_MODE_MANUAL,
    CAN_SSP_MODE_AUTO
} CAN_SSP_MODE;

//! System Clock Selection

typedef enum {
    CAN_SYSCLK_NO = 0,
    CAN_SYSCLK_16M,
    CAN_SYSCLK_8M,
    CAN_SYSCLK_40M = CAN_SYSCLK_16M,
    CAN_SYSCLK_20M,
    CAN_SYSCLK_10M
} CAN_SYSCLK_SPEED;

//! GPIO Pin Modes
typedef enum {
    GPIO_MODE_INT = 0,
    GPIO_MODE_GPIO
} GPIO_PIN_MODE;

//! GPIO Pin Directions
typedef enum {
    GPIO_OUTPUT = 0,
    GPIO_INPUT
} GPIO_PIN_DIRECTION;

//! Interrupt Enables
typedef struct _CAN_INT_ENABLES {
    uint32_t TXIE : 1;
    uint32_t RXIE : 1;
    uint32_t TBCIE : 1;
    uint32_t MODIE : 1;
    uint32_t TEFIE : 1;
    uint32_t unimplemented2 : 3;

    uint32_t ECCIE : 1;
    uint32_t SPICRCIE : 1;
    uint32_t TXATIE : 1;
    uint32_t RXOVIE : 1;
    uint32_t SERRIE : 1;
    uint32_t CERRIE : 1;
    uint32_t WAKIE : 1;
    uint32_t IVMIE : 1;
} CAN_INT_ENABLES;

//! Interrupt Enable Register
typedef union _REG_CiINTENABLE {
    CAN_INT_ENABLES IE;
    uint16_t word;
    uint8_t byte[2];
} REG_CiINTENABLE;

typedef union _REG_t {
    uint8_t byte[4];
    uint32_t word;
} REG_t;

//! CAN TX FIFO Event (Interrupts)
typedef enum {
    CAN_TX_FIFO_NO_EVENT = 0,
    CAN_TX_FIFO_ALL_EVENTS = 0x17,
    CAN_TX_FIFO_NOT_FULL_EVENT = 0x01,
    CAN_TX_FIFO_HALF_FULL_EVENT = 0x02,
    CAN_TX_FIFO_EMPTY_EVENT = 0x04,
    CAN_TX_FIFO_ATTEMPTS_EXHAUSTED_EVENT = 0x10
} CAN_TX_FIFO_EVENT;

//! CAN RX FIFO Event (Interrupts)
typedef enum {
    CAN_RX_FIFO_NO_EVENT = 0,
    CAN_RX_FIFO_ALL_EVENTS = 0x0F,
    CAN_RX_FIFO_NOT_EMPTY_EVENT = 0x01,
    CAN_RX_FIFO_HALF_FULL_EVENT = 0x02,
    CAN_RX_FIFO_FULL_EVENT = 0x04,
    CAN_RX_FIFO_OVERFLOW_EVENT = 0x08
} CAN_RX_FIFO_EVENT;

//! CAN Operation Modes
typedef enum {
    CAN_NORMAL_MODE = 0x00,
    CAN_SLEEP_MODE = 0x01,
    CAN_INTERNAL_LOOPBACK_MODE = 0x02,
    CAN_LISTEN_ONLY_MODE = 0x03,
    CAN_CONFIGURATION_MODE = 0x04,
    CAN_EXTERNAL_LOOPBACK_MODE = 0x05,
    CAN_CLASSIC_MODE = 0x06,
    CAN_RESTRICTED_MODE = 0x07,
    CAN_INVALID_MODE = 0xFF
} CAN_OPERATION_MODE;

//! CAN Module Event (Interrupts)
typedef enum {
    CAN_NO_EVENT = 0,
    CAN_ALL_EVENTS = 0xFF1F,
    CAN_TX_EVENT = 0x0001,
    CAN_RX_EVENT = 0x0002,
    CAN_TIME_BASE_COUNTER_EVENT = 0x0004,
    CAN_OPERATION_MODE_CHANGE_EVENT = 0x0008,
    CAN_TEF_EVENT = 0x0010,

    CAN_RAM_ECC_EVENT = 0x0100,
    CAN_SPI_CRC_EVENT = 0x0200,
    CAN_TX_ATTEMPTS_EVENT = 0x0400,
    CAN_RX_OVERFLOW_EVENT = 0x0800,
    CAN_SYSTEM_ERROR_EVENT = 0x1000,
    CAN_BUS_ERROR_EVENT = 0x2000,
    CAN_BUS_WAKEUP_EVENT = 0x4000,
    CAN_RX_INVALID_MESSAGE_EVENT = 0x8000
} CAN_MODULE_EVENT;

typedef enum {
    CAN_ERROR_FREE_STATE = 0,
    CAN_ERROR_ALL = 0x3F,
    CAN_TX_RX_WARNING_STATE = 0x01,
    CAN_RX_WARNING_STATE = 0x02,
    CAN_TX_WARNING_STATE = 0x04,
    CAN_RX_BUS_PASSIVE_STATE = 0x08,
    CAN_TX_BUS_PASSIVE_STATE = 0x10,
    CAN_TX_BUS_OFF_STATE = 0x20
} CAN_ERROR_STATE;

//! Interrupt Flags
typedef struct _CAN_INT_FLAGS {
    uint32_t TXIF : 1;
    uint32_t RXIF : 1;
    uint32_t TBCIF : 1;
    uint32_t MODIF : 1;
    uint32_t TEFIF : 1;
    uint32_t unimplemented1 : 3;

    uint32_t ECCIF : 1;
    uint32_t SPICRCIF : 1;
    uint32_t TXATIF : 1;
    uint32_t RXOVIF : 1;
    uint32_t SERRIF : 1;
    uint32_t CERRIF : 1;
    uint32_t WAKIF : 1;
    uint32_t IVMIF : 1;
} CAN_INT_FLAGS;

//! Interrupt Flag Register
typedef union _REG_CiINTFLAG {
    CAN_INT_FLAGS IF;
    uint16_t word;
    uint8_t byte[2];
} REG_CiINTFLAG;

//! Transmit/Receive Error Count Register
typedef union _REG_CiTREC {

    struct {
        uint32_t RxErrorCount : 8;
        uint32_t TxErrorCount : 8;
        uint32_t ErrorStateWarning : 1;
        uint32_t RxErrorStateWarning : 1;
        uint32_t TxErrorStateWarning : 1;
        uint32_t RxErrorStatePassive : 1;
        uint32_t TxErrorStatePassive : 1;
        uint32_t TxErrorStateBusOff : 1;
        uint32_t unimplemented1 : 10;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiTREC;

//! FIFO User Address Register
typedef union _REG_CiFIFOUA {

    struct {
        uint32_t UserAddress : 12;
        uint32_t unimplemented1 : 20;
    } bF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiFIFOUA;

typedef enum
{
    false = 0,
    true = 1,
}bool;

void CANFD_init(void);
int8_t DRV_CANFDSPI_ConfigureObjectReset(CAN_CONFIG* config);
int8_t DRV_CANFDSPI_Configure(CAN_CONFIG* config);
int8_t DRV_CANFDSPI_TransmitChannelConfigureObjectReset(CAN_TX_FIFO_CONFIG* config);
int8_t DRV_CANFDSPI_ReceiveChannelConfigureObjectReset(CAN_RX_FIFO_CONFIG* config);
int8_t DRV_CANFDSPI_ReceiveChannelEventEnable(CAN_FIFO_CHANNEL channel,CAN_RX_FIFO_EVENT flags);
int8_t DRV_CANFDSPI_TransmitChannelEventEnable(CAN_FIFO_CHANNEL channel,CAN_TX_FIFO_EVENT flags);
int8_t DRV_CANFDSPI_BitTimeConfigureNominal40MHz(CAN_BITTIME_SETUP bitTime);
int8_t DRV_CANFDSPI_BitTimeConfigure(CAN_BITTIME_SETUP bitTime,CAN_SSP_MODE sspMode,CAN_SYSCLK_SPEED clk);
int8_t DRV_CANFDSPI_Reset(void);
int8_t DRV_CANFDSPI_OperationModeSelect(CAN_OPERATION_MODE opMode);
int8_t DRV_CANFDSPI_ModuleEventEnable(CAN_MODULE_EVENT flags);
int8_t DRV_CANFDSPI_ReadHalfWord(uint16_t address, uint16_t *rxd);
int8_t DRV_CANFDSPI_WriteHalfWord(uint16_t address, uint16_t txd);
int8_t DRV_CANFDSPI_GpioModeConfigure(GPIO_PIN_MODE gpio0, GPIO_PIN_MODE gpio1);
uint32_t DRV_CANFDSPI_DlcToDataBytes(CAN_DLC dlc);
int8_t DRV_CANFDSPI_EccEnable(void);
int8_t DRV_CANFDSPI_RamInit(uint8_t d);
int8_t DRV_CANFDSPI_FilterObjectConfigure(CAN_FILTER filter, CAN_FILTEROBJ_ID* id);
int8_t DRV_CANFDSPI_FilterMaskConfigure(CAN_FILTER filter, CAN_MASKOBJ_ID* mask);
int8_t DRV_CANFDSPI_ReadByte(uint16_t address, uint8_t *rxd);
int8_t DRV_CANFDSPI_ReceiveChannelConfigure(CAN_FIFO_CHANNEL channel,CAN_RX_FIFO_CONFIG* config);
int8_t DRV_CANFDSPI_WriteByte(uint16_t address, uint8_t txd);

int8_t DRV_CANFDSPI_ReadByteArray(uint16_t address, uint8_t *rxd, uint16_t nBytes);
int8_t DRV_CANFDSPI_WriteByteArray(uint16_t address, uint8_t *txd, uint16_t nBytes);
int8_t DRV_CANFDSPI_WriteWord(uint16_t address, uint32_t txd);
int8_t DRV_CANFDSPI_TransmitChannelConfigure(CAN_FIFO_CHANNEL channel,CAN_TX_FIFO_CONFIG* config);
int8_t DRV_CANFDSPI_FilterToFifoLink(CAN_FILTER filter,CAN_FIFO_CHANNEL channel, bool enable);
int8_t DRV_CANFDSPI_BitTimeConfigureData40MHz(CAN_BITTIME_SETUP bitTime,CAN_SSP_MODE sspMode);
int8_t DRV_CANFDSPI_BitTimeConfigureNominal20MHz(CAN_BITTIME_SETUP bitTime);
int8_t DRV_CANFDSPI_BitTimeConfigureData20MHz(CAN_BITTIME_SETUP bitTime,CAN_SSP_MODE sspMode);
int8_t DRV_CANFDSPI_BitTimeConfigureNominal10MHz(CAN_BITTIME_SETUP bitTime);
int8_t DRV_CANFDSPI_BitTimeConfigureData10MHz(CAN_BITTIME_SETUP bitTime,CAN_SSP_MODE sspMode);
int8_t DRV_CANFDSPI_TransmitChannelEventGet(CAN_FIFO_CHANNEL channel, CAN_TX_FIFO_EVENT* flags);
int8_t DRV_CANFDSPI_ReadWord(uint16_t address, uint32_t *rxd);
int8_t DRV_CANFDSPI_ErrorCountStateGet(uint8_t* tec, uint8_t* rec,CAN_ERROR_STATE* flags);
int8_t DRV_CANFDSPI_TransmitChannelLoad(CAN_FIFO_CHANNEL channel,CAN_TX_MSGOBJ* txObj,uint8_t *txd, uint32_t txdNumBytes, bool flush);
int8_t DRV_CANFDSPI_ReadWordArray(uint16_t address,uint32_t *rxd, uint16_t nWords);
int8_t DRV_CANFDSPI_TransmitChannelUpdate(CAN_FIFO_CHANNEL channel, bool flush);
int8_t mcp2518fd_TransmitChannelEventGet(CAN_FIFO_CHANNEL channel, CAN_TX_FIFO_EVENT *flags);
void mcp2518fd_TransmitMessageQueue(void) ;
int8_t CANFDSPI_FilterDisable(CAN_FILTER filter);
int8_t DRV_CANFDSPI_TransmitChannelEventGets(CAN_FIFO_CHANNEL channel, CAN_TX_FIFO_EVENT *flags);
int8_t MCP2518FD_ReceiveChannelEventGet(CAN_FIFO_CHANNEL channel, CAN_RX_FIFO_EVENT *flags);
uint8_t readRxTxStatus(void) ;
uint8_t readMsgBuf(uint8_t *len, uint8_t *buf); 
int8_t mcp2518fd_ReceiveChannelUpdate(CAN_FIFO_CHANNEL channel);
uint8_t mcp2518fd_readMsgBufID(uint8_t *len, uint8_t *buf);
uint8_t readMsgBuf(uint8_t *len, uint8_t *buf); 
uint8_t readMsgBufID(uint8_t status, unsigned long *id, uint8_t *ext, uint8_t *rtr,uint8_t *len, uint8_t *buf); 
uint8_t MCP2518FD_SendMsg(const uint8_t *buf, uint8_t len, unsigned long id, uint8_t ext, uint8_t rtr, bool wait_sent);
uint8_t CAN_Send(unsigned long id, uint8_t ext, uint8_t rtr, uint8_t len, const uint8_t *buf, bool wait_sent);
int8_t mcp2518fd_ReceiveMessageGet(CAN_FIFO_CHANNEL channel,CAN_RX_MSGOBJ *rxObj,uint8_t *rxd, uint8_t nBytes); 
__inline uint8_t Can_TXD(unsigned long id, uint8_t ext, uint8_t len, uint8_t *buf)
{
	return CAN_Send(id, ext, 0, len, buf, true);
}
unsigned long getCanId(void);
uint8_t isRemoteRequest(void);
uint8_t isExtendedFrame(void);

void OtdUart_DebugSend_CAN(volatile const char *s);
int8_t DRV_CANFDSPI_ModuleEventClear(CAN_MODULE_EVENT flags);
#endif