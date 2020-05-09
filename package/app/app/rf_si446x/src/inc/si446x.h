#ifndef _SI446X_H_
#define _SI446X_H_

#include "si446x_defs.h" 

/*
=================================================================================
-----------------------------External IMPORT functions---------------------------
=================================================================================
*/

#include "board.h"   //BSP里面包含了Si446X所用的相关函数。


//PA15 --- nSEL
//#define SI_CSN_LOW( )
//#define SI_CSN_HIGH( )

//PA0 --- SDN
//#define SI_SDN_LOW( )
//#define SI_SDN_HIGH( )

/*
=================================================================================
------------------------------INTERNAL EXPORT APIs-------------------------------
=================================================================================
*/

#define  PACKET_LENGTH      0 //0-64, if = 0: variable mode, else: fixed mode



/*
=================================================================================
------------------------------INTERNAL EXPORT APIs-------------------------------
=================================================================================
*/

/*Read the PART_INFO of the device, 8 bytes needed*/
void SI446X_PART_INFO( INT8U *buffer );

/*Read the FUNC_INFO of the device, 7 bytes needed*/
void SI446X_FUNC_INFO( INT8U *buffer );

/*Send a command to the device*/
void SI446X_CMD( INT8U *cmd, INT8U cmdsize );

/*Read the INT status of the device, 9 bytes needed*/
void SI446X_INT_STATUS( INT8U *buffer );

/*Read the PROPERTY of the device*/
void SI446X_GET_PROPERTY_X( SI446X_PROPERTY GROUP_NUM, INT8U NUM_PROPS, INT8U *buffer  );

/*configuration the device*/
void SI446X_CONFIG_INIT( void );

/*reset the SI446x device*/
void SI446X_RESET( void );

/*write data to TX fifo*/
void SI446X_W_TX_FIFO( INT8U *txbuffer, INT8U size );

/*start TX command*/
void SI446X_START_TX( INT8U channel, INT8U condition, INT16U tx_len );

/*read RX fifo*/
INT8U SI446X_READ_PACKET( INT8U *buffer );

/*start RX state*/
void SI446X_START_RX( INT8U channel, INT8U condition, INT16U rx_len,
                      INT8U n_state1, INT8U n_state2, INT8U n_state3 );

/*read packet information*/
void SI446X_PKT_INFO( INT8U *buffer, INT8U FIELD, INT16U length, INT16U diff_len );

/*read fifo information*/
void SI446X_FIFO_INFO( INT8U *buffer );

/*Power up the device*/
void SI446X_POWER_UP( INT32U f_xtal );

/*send a packet*/
void SI446X_SEND_PACKET( INT8U *txbuffer, INT8U size, INT8U channel, INT8U condition );

/*Set the PROPERTY of the device*/
void SI446X_SET_PROPERTY_X( SI446X_PROPERTY GROUP_NUM, INT8U NUM_PROPS, INT8U *PAR_BUFF );

/*config the CRC, PROPERTY 0x1200*/
void SI446X_CRC_CONFIG( INT8U PKT_CRC_CONFIG );

/*Get the PROPERTY of the device, only 1 byte*/
INT8U SI446X_GET_PROPERTY_1( SI446X_PROPERTY GROUP_NUM );

/*Set the PROPERTY of the device, only 1 byte*/
void SI446X_SET_PROPERTY_1( SI446X_PROPERTY GROUP_NUM, INT8U proirity );

/*config the GPIOs, IRQ, SDO*/
void SI446X_GPIO_CONFIG( INT8U G0, INT8U G1, INT8U G2, INT8U G3,
                         INT8U IRQ, INT8U SDO, INT8U GEN_CONFIG );

/*reset the RX FIFO of the device*/
void SI446X_RX_FIFO_RESET( void );

/*reset the TX FIFO of the device*/
void SI446X_TX_FIFO_RESET( void );

uint8_t SI4463_READ_RSSI(void);
void Get_RSSIValue(INT8U *buffer, INT8U size );
void SI446X_SetTxPower(INT8U TxPower);
void SI446X_Set_DataRate(INT8U *DataRate);  //设置SI4463空中无线传输速率的函数
void SI446X_SET_MODEM(INT8U MODEM_TYPE);
void SI446X_SET_FREQ(uint32_t Freq);
void SI446X_SET_POWER(INT8U Power_Level);
/*
=================================================================================
----------------------------PROPERTY fast setting macros-------------------------
=================================================================================
*/
// GOLBAL(0x00)
#define GLOBAL_XO_TUNE( x )                 SI446X_SET_PROPERTY_1( 0x0000, x )
#define GLOBAL_CLK_CFG( x )                 SI446X_SET_PROPERTY_1( 0x0001, x )
#define GLOBAL_LOW_BATT_THRESH( x )         SI446X_SET_PROPERTY_1( 0x0002, x )
#define GLOBAL_CONFIG( x )                  SI446X_SET_PROPERTY_1( 0x0003, x )
#define GLOBAL_WUT_CONFIG( x )              SI446X_SET_PROPERTY_1( 0x0004, x )
#define GLOBAL_WUT_M_15_8( x )              SI446X_SET_PROPERTY_1( 0x0005, x )
#define GLOBAL_WUT_M_7_0( x )               SI446X_SET_PROPERTY_1( 0x0006, x )
#define GLOBAL_WUT_R( x )                   SI446X_SET_PROPERTY_1( 0x0007, x )
#define GLOBAL_WUT_LDC( x )                 SI446X_SET_PROPERTY_1( 0x0008, x )
#define GLOBAL_WUT_CAL( x )                 SI446X_SET_PROPERTY_1( 0x0009, x )

// INT_CTL(0x01)
#define INT_CTL_ENABLE( x )                 SI446X_SET_PROPERTY_1( 0x0100, x )
#define INT_CTL_PH_ENABLE( x )              SI446X_SET_PROPERTY_1( 0x0101, x )
#define INT_CTL_MODEM_ENABLE( x )           SI446X_SET_PROPERTY_1( 0x0102, x )
#define INT_CTL_CHIP_ENABLE( x )            SI446X_SET_PROPERTY_1( 0x0103, x )

//group 0x02, FRR_CTL
#define FRR_CTL_A_MODE( x )                 SI446X_SET_PROPERTY_1( 0x0200, x )
#define FRR_CTL_B_MODE( x )                 SI446X_SET_PROPERTY_1( 0x0201, x )
#define FRR_CTL_C_MODE( x )                 SI446X_SET_PROPERTY_1( 0x0202, x )
#define FRR_CTL_D_MODE( x )                 SI446X_SET_PROPERTY_1( 0x0203, x )

// PREAMBLE (0x10)


#endif //_SI446X_H_

/*
=================================================================================
------------------------------------End of FILE----------------------------------
=================================================================================
*/
