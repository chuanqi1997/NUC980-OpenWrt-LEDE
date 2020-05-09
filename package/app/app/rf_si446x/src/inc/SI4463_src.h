
#include <ws_SPI.h>
#include <SI4463F.h>
#include <si446x_defs.h>
#include <radio_config_Si4463.h>



/*********************************************************************
**  Constant Declaration
*********************************************************************/
#define Uint8  u8
#define Uint16  u16
#define uchar  u8

#define    SI_CSN_LOW()    cbi(PA_ODR,3)
#define    SI_CSN_HIGH()   sbi(PA_ODR,3)

#define    SI_SDN_HIGH()   sbi(PC_ODR,4)
#define    SI_SDN_LOW()    cbi(PC_ODR,4)
#define  RED_LED_ON   cbi(PD_ODR,6)
#define  RED_LED_OFF  sbi(PD_ODR,6)

#define  GREEN_LED_ON   cbi(PD_ODR,5)
#define  GREEN_LED_OFF  sbi(PD_ODR,5)
#define    cCommand_RxFifoRead      0x61
#define    cCommand_TxFifoWrite     0xa0
#define    cCommand_TxFifoFlush     0xe1
#define    cCommand_RxFifoFlush     0xe2

#define    cFIFOlength          6           /* RF的FIFO长度 */

const static INT8U config_table[] = RADIO_CONFIGURATION_DATA_ARRAY;

/*********************************************************************
** Nrf24l01_InterFace
*********************************************************************/
void SI4463_InterFace(void)
{
    //init io pin
		
		
		sbi(PA_DDR,3);//SCS OUTPUT
		sbi(PA_CR1,3);
		cbi(PA_CR2,3);
		
		sbi(PC_DDR,5);//SCK OUTPUT
		sbi(PC_CR1,5);
		cbi(PC_CR2,5);
		
		sbi(PC_DDR,6);//MOSI OUTPUT
		sbi(PC_CR1,6);
		cbi(PC_CR2,6);
		
		cbi(PC_DDR,7);//MISO INPUT
		sbi(PC_CR1,7);
		cbi(PC_CR2,7);
		
		cbi(PB_DDR,5);//GIO1 WTR INPUT
		sbi(PB_CR1,5);
		sbi(PB_CR2,5);
					
		sbi(PC_DDR,4);//CE OUTPUT
	  sbi(PC_CR1,4);
	  cbi(PC_CR2,4);
		
		
		
		//cbi(PD_DDR,6);//key INPUT
	 // sbi(PD_CR1,6);
	 // sbi(PD_CR2,6); //enable EXTI PD6 intrrrupt
	 	sbi(PD_DDR,6);//RED_LED OUTPUT
	  sbi(PD_CR1,6);
	  cbi(PD_CR2,6); 
	
		cbi(PD_DDR,3);//RED_LED OUTPUT
		sbi(PD_CR1,3);
		sbi(PD_CR2,3);
		
		sbi(PD_DDR,4);//YELLOE_LED OUTPUT
	  sbi(PD_CR1,4);
	  cbi(PD_CR2,4);
		
		sbi(PD_DDR,5);//GRE_LED OUTPUT
	  sbi(PD_CR1,5);
	  cbi(PD_CR2,5);
		
		sbi(PC_DDR,3);//HEAT OUTPUT
	  sbi(PC_CR1,3);
	  cbi(PC_CR2,3);
		
		
		
		
    
		
		
		
	  
		
		
		
		
		
		SPI_Init();
		
}

INT8U SPI_ExchangeByte( INT8U input )
{
	
	u8 data;
	
	while(!(SPI_SR & 0x02));
	SPI_DR=input;
	
	while(!(SPI_SR & 0x01));
	data=SPI_DR;
	
	
	return data;
	
}

/*
=================================================================================
SI446X_CMD( );
Function : Send a command to the device
INTPUT   : cmd, the buffer stores the command array
           cmdsize, the size of the command array
OUTPUT   : NONE
=================================================================================
*/
void SI446X_CMD( INT8U *cmd, INT8U cmdsize )
{
    SI446X_WAIT_CTS( );
    SI_CSN_LOW( );
    while( cmdsize -- )
    {
        SPI_ExchangeByte( *cmd++ );
    }
    SI_CSN_HIGH( );
}
/*
=================================================================================
SI446X_POWER_UP( );
Function : Power up the device
INTPUT   : f_xtal, the frequency of the external high-speed crystal
OUTPUT   : NONE
=================================================================================
*/
void SI446X_POWER_UP( INT32U f_xtal )
{
    INT8U cmd[7];
    cmd[0] = POWER_UP;
    cmd[1] = 0x01;
    cmd[2] = 0x00;
    cmd[3] = f_xtal>>24;
    cmd[4] = f_xtal>>16;
    cmd[5] = f_xtal>>8;
    cmd[6] = f_xtal;
    SI446X_CMD( cmd, 7 );
}
/*
=================================================================================
SI446X_READ_RESPONSE( );
Function : read a array of command response
INTPUT   : buffer,  a buffer, stores the data responsed
           size,    How many bytes should be read
OUTPUT   : NONE
=================================================================================
*/
void SI446X_READ_RESPONSE( INT8U *buffer, INT8U size )
{
    SI446X_WAIT_CTS( );
    SI_CSN_LOW( );
	SPI_ExchangeByte( READ_CMD_BUFF );
	while( size -- )
    {
        *buffer++ = SPI_ExchangeByte( 0xFF );
    }
    SI_CSN_HIGH( );

}
/*
=================================================================================
SI446X_WAIT_CTS( );
Function : wait the device ready to response a command
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void SI446X_WAIT_CTS( void )
{
    INT8U cts;
    do
    {
        SI_CSN_LOW( );
        SPI_ExchangeByte( READ_CMD_BUFF );
        cts = SPI_ExchangeByte( 0xFF );
        SI_CSN_HIGH( );
    }while( cts != 0xFF );
}
/*
=================================================================================
SI446X_NOP( );
Function : NO Operation command
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
INT8U SI446X_NOP( void )
{
    INT8U cts;
    SI_CSN_LOW( );
    cts = SPI_ExchangeByte( NOP );
    SI_CSN_HIGH( );
	return cts;
}

/*
=================================================================================
SI446X_PART_INFO( );
Function : Read the PART_INFO of the device, 8 bytes needed
INTPUT   : buffer, the buffer stores the part information
OUTPUT   : NONE
=================================================================================
*/
void SI446X_PART_INFO( INT8U *buffer )
{
    INT8U cmd = PART_INFO;

    SI446X_CMD( &cmd, 1 );
    SI446X_READ_RESPONSE( buffer, 8 );

}
/*
=================================================================================
SI446X_FUNC_INFO( );
Function : Read the FUNC_INFO of the device, 7 bytes needed
INTPUT   : buffer, the buffer stores the FUNC information
OUTPUT   : NONE
=================================================================================
*/
void SI446X_FUNC_INFO( INT8U *buffer )
{
    INT8U cmd = FUNC_INFO;

    SI446X_CMD( &cmd, 1 );
    SI446X_READ_RESPONSE( buffer, 7 );
}
/*
=================================================================================
SI446X_INT_STATUS( );
Function : Read the INT status of the device, 9 bytes needed
INTPUT   : buffer, the buffer stores the int status
OUTPUT   : NONE
=================================================================================
*/
void SI446X_INT_STATUS( INT8U *buffer )
{
    INT8U cmd[4];
    cmd[0] = GET_INT_STATUS;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;

    SI446X_CMD( cmd, 4 );
    SI446X_READ_RESPONSE( buffer, 9 );

}
/*
=================================================================================
SI446X_GET_PROPERTY( );
Function : Read the PROPERTY of the device
INTPUT   : buffer, the buffer stores the PROPERTY value
           GROUP_NUM, the group and number of the parameter
           NUM_GROUP, number of the group
OUTPUT   : NONE
=================================================================================
*/
void SI446X_GET_PROPERTY_X( SI446X_PROPERTY GROUP_NUM, INT8U NUM_PROPS, INT8U *buffer  )
{
    INT8U cmd[4];

    cmd[0] = GET_PROPERTY;
    cmd[1] = GROUP_NUM>>8;
    cmd[2] = NUM_PROPS;
    cmd[3] = GROUP_NUM;

    SI446X_CMD( cmd, 4 );
    SI446X_READ_RESPONSE( buffer, NUM_PROPS + 1 );
}
/*
=================================================================================
SI446X_SET_PROPERTY_X( );
Function : Set the PROPERTY of the device
INTPUT   : GROUP_NUM, the group and the number of the parameter
           NUM_GROUP, number of the group
           PAR_BUFF, buffer stores parameters
OUTPUT   : NONE
=================================================================================
*/
void SI446X_SET_PROPERTY_X( SI446X_PROPERTY GROUP_NUM, INT8U NUM_PROPS, INT8U *PAR_BUFF )
{
    INT8U cmd[20], i = 0;
    if( NUM_PROPS >= 16 )   { return; }
    cmd[i++] = SET_PROPERTY;
    cmd[i++] = GROUP_NUM>>8;
    cmd[i++] = NUM_PROPS;
    cmd[i++] = GROUP_NUM;
    while( NUM_PROPS-- )
    {
        cmd[i++] = *PAR_BUFF++;
    }
    SI446X_CMD( cmd, i );
}
/*
=================================================================================
SI446X_SET_PROPERTY_1( );
Function : Set the PROPERTY of the device, only 1 byte
INTPUT   : GROUP_NUM, the group and number index
           prioriry,  the value to be set
OUTPUT   : NONE
=================================================================================
*/
void SI446X_SET_PROPERTY_1( SI446X_PROPERTY GROUP_NUM, INT8U proirity )
{
    INT8U cmd[5];

    cmd[0] = SET_PROPERTY;
    cmd[1] = GROUP_NUM>>8;
    cmd[2] = 1;
    cmd[3] = GROUP_NUM;
    cmd[4] = proirity;
    SI446X_CMD( cmd, 5 );
}
/*
=================================================================================
SI446X_GET_PROPERTY_1( );
Function : Get the PROPERTY of the device, only 1 byte
INTPUT   : GROUP_NUM, the group and number index
OUTPUT   : the PROPERTY value read from device
=================================================================================
*/
INT8U SI446X_GET_PROPERTY_1( SI446X_PROPERTY GROUP_NUM )
{
    INT8U cmd[4];

    cmd[0] = GET_PROPERTY;
    cmd[1] = GROUP_NUM>>8;
    cmd[2] = 1;
    cmd[3] = GROUP_NUM;
    SI446X_CMD( cmd, 4 );
    SI446X_READ_RESPONSE( cmd, 2 );
    return cmd[1];
}
/*
=================================================================================
SI446X_RESET( );
Function : reset the SI446x device
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void SI446X_RESET( void )
{
    INT16U x = 2550;
    //SI_SDN_HIGH( );
    while( x-- );
    SI_SDN_LOW( );
    SI_CSN_HIGH( );
}
/*
=================================================================================
SI446X_CONFIG_INIT( );
Function : configuration the device
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void SI446X_CONFIG_INIT( void )
{
    INT8U i;
    INT16U j = 0;

    while( ( i = config_table[j] ) != 0 )
    {
        j += 1;
        SI446X_CMD( config_table + j, i );
        j += i;
    }
#if PACKET_LENGTH > 0           //fixed packet length
    SI446X_SET_PROPERTY_1( PKT_FIELD_1_LENGTH_7_0, PACKET_LENGTH );
		 SI446X_SET_PROPERTY_1( PKT_FIELD_1_CRC_CONFIG, 0xA2 );
		  SI446X_SET_PROPERTY_1( PKT_CRC_CONFIG, 0x05 );
		
#else                           //variable packet length
    SI446X_SET_PROPERTY_1( PKT_CONFIG1, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_CRC_CONFIG, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_LEN_FIELD_SOURCE, 0x01 );
    SI446X_SET_PROPERTY_1( PKT_LEN, 0x2A );
    SI446X_SET_PROPERTY_1( PKT_LEN_ADJUST, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_1_LENGTH_12_8, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_1_LENGTH_7_0, 0x01 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_1_CONFIG, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_1_CRC_CONFIG, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_2_LENGTH_12_8, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_2_LENGTH_7_0, 0x10 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_2_CONFIG, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_2_CRC_CONFIG, 0x00 );
#endif //PACKET_LENGTH

    //重要： 4463的GDO2，GDO3控制射频开关，  0X20 ,0X21 
    //发射时必须： GDO2=1，GDO3=0
    //接收时必须： GDO2=0，GDO3=1
    SI446X_GPIO_CONFIG( 0, 0, 0x20, 0x21, 0, 0, 0 );//重要

}
/*
=================================================================================
SI446X_W_TX_FIFO( );
Function : write data to TX fifo
INTPUT   : txbuffer, a buffer stores TX array
           size,  how many bytes should be written
OUTPUT   : NONE
=================================================================================
*/
void SI446X_W_TX_FIFO( INT8U *txbuffer, INT8U size )
{
    SI_CSN_LOW( );
    SPI_ExchangeByte( WRITE_TX_FIFO );
    while( size -- )    { SPI_ExchangeByte( *txbuffer++ ); }
    SI_CSN_HIGH( );
}
/*
=================================================================================
SI446X_SEND_PACKET( );
Function : send a packet
INTPUT   : txbuffer, a buffer stores TX array
           size,  how many bytes should be written
           channel, tx channel
           condition, tx condition
OUTPUT   : NONE
=================================================================================
*/
void SI446X_SEND_PACKET( INT8U *txbuffer, INT8U size, INT8U channel, INT8U condition )
{
    INT8U cmd[5];
    INT8U tx_len = size;

    SI446X_TX_FIFO_RESET( );

    SI_CSN_LOW( );
    SPI_ExchangeByte( WRITE_TX_FIFO );
#if PACKET_LENGTH == 0
    tx_len ++;
    SPI_ExchangeByte( size );
#endif
    while( size -- )    { SPI_ExchangeByte( *txbuffer++ ); }
    SI_CSN_HIGH( );
    cmd[0] = START_TX;
    cmd[1] = channel;
    cmd[2] = condition;
    cmd[3] = 0;
    cmd[4] = tx_len;
    SI446X_CMD( cmd, 5 );
		while((PD_IDR&0x04)==0x04);
		SI_CSN_HIGH( );
    cmd[0] = CHANGE_STATE;
    cmd[1] = 0x03;
    SI446X_CMD( cmd, 2 );

}
/*
=================================================================================
SI446X_START_TX( );
Function : start TX command
INTPUT   : channel, tx channel
           condition, tx condition
           tx_len, how many bytes to be sent
OUTPUT   : NONE
=================================================================================
*/
void SI446X_START_TX( INT8U channel, INT8U condition, INT16U tx_len )
{
    INT8U cmd[5];

    cmd[0] = START_TX;
    cmd[1] = channel;
    cmd[2] = condition;
    cmd[3] = tx_len>>8;
    cmd[4] = tx_len;
    SI446X_CMD( cmd, 5 );
}
/*
=================================================================================
SI446X_READ_PACKET( );
Function : read RX fifo
INTPUT   : buffer, a buffer to store data read
OUTPUT   : received bytes
=================================================================================
*/
INT8U SI446X_READ_PACKET( INT8U *buffer )
{
    INT8U length, i;
    SI446X_WAIT_CTS( );
    SI_CSN_LOW( );

    SPI_ExchangeByte( READ_RX_FIFO );
#if PACKET_LENGTH == 0
    length = SPI_ExchangeByte( 0xFF );
#else
    length = PACKET_LENGTH;
#endif
    i = length;

    while( length -- )
    {
        *buffer++ = SPI_ExchangeByte( 0xFF );
    }
    SI_CSN_HIGH( );
    return i;
}
/*
=================================================================================
SI446X_START_RX( );
Function : start RX state
INTPUT   : channel, receive channel
           condition, receive condition
           rx_len, how many bytes should be read
           n_state1, next state 1
           n_state2, next state 2
           n_state3, next state 3
OUTPUT   : NONE
=================================================================================
*/
void SI446X_START_RX( INT8U channel, INT8U condition, INT16U rx_len,
                      INT8U n_state1, INT8U n_state2, INT8U n_state3 )
{
    INT8U cmd[8];
    SI446X_RX_FIFO_RESET( );
    SI446X_TX_FIFO_RESET( );
    cmd[0] = START_RX;
    cmd[1] = channel;
    cmd[2] = condition;
    cmd[3] = rx_len>>8;
    cmd[4] = rx_len;
    cmd[5] = n_state1;
    cmd[6] = n_state2;
    cmd[7] = n_state3;
    SI446X_CMD( cmd, 8 );
}
/*
=================================================================================
SI446X_RX_FIFO_RESET( );
Function : reset the RX FIFO of the device
INTPUT   : None
OUTPUT   : NONE
=================================================================================
*/
void SI446X_RX_FIFO_RESET( void )
{
    INT8U cmd[2];

    cmd[0] = FIFO_INFO;
    cmd[1] = 0x02;
    SI446X_CMD( cmd, 2 );
}
/*
=================================================================================
SI446X_TX_FIFO_RESET( );
Function : reset the TX FIFO of the device
INTPUT   : None
OUTPUT   : NONE
=================================================================================
*/
void SI446X_TX_FIFO_RESET( void )
{
    INT8U cmd[2];

    cmd[0] = FIFO_INFO;
    cmd[1] = 0x01;
    SI446X_CMD( cmd, 2 );
}
/*
=================================================================================
SI446X_PKT_INFO( );
Function : read packet information
INTPUT   : buffer, stores the read information
           FIELD, feild mask
           length, the packet length
           diff_len, diffrence packet length
OUTPUT   : NONE
=================================================================================
*/
void SI446X_PKT_INFO( INT8U *buffer, INT8U FIELD, INT16U length, INT16U diff_len )
{
    INT8U cmd[6];
    cmd[0] = PACKET_INFO;
    cmd[1] = FIELD;
    cmd[2] = length >> 8;
    cmd[3] = length;
    cmd[4] = diff_len >> 8;
    cmd[5] = diff_len;

    SI446X_CMD( cmd, 6 );
    SI446X_READ_RESPONSE( buffer, 3 );
}
/*
=================================================================================
SI446X_FIFO_INFO( );
Function : read fifo information
INTPUT   : buffer, stores the read information
OUTPUT   : NONE
=================================================================================
*/
void SI446X_FIFO_INFO( INT8U *buffer )
{
    INT8U cmd[2];
    cmd[0] = FIFO_INFO;
    cmd[1] = 0x03;

    SI446X_CMD( cmd, 2 );
    SI446X_READ_RESPONSE( buffer, 3);
}
/*
=================================================================================
SI446X_GPIO_CONFIG( );
Function : config the GPIOs, IRQ, SDO
INTPUT   :
OUTPUT   : NONE
=================================================================================
*/
void SI446X_GPIO_CONFIG( INT8U G0, INT8U G1, INT8U G2, INT8U G3,
                         INT8U IRQ, INT8U SDO, INT8U GEN_CONFIG )
{
    INT8U cmd[10];
    cmd[0] = GPIO_PIN_CFG;
    cmd[1] = G0;
    cmd[2] = G1;
    cmd[3] = G2;
    cmd[4] = G3;
    cmd[5] = IRQ;
    cmd[6] = SDO;
    cmd[7] = GEN_CONFIG;
    SI446X_CMD( cmd, 8 );
    SI446X_READ_RESPONSE( cmd, 8 );
}

/************************************************************************
**  ByteSend
************************************************************************/
void ByteSend(Uint8 src)
{
	 
    Uint8 i;
    		
		while(!(SPI_SR & 0x02));
		SPI_DR=src;
		while(!(SPI_SR & 0x01));
		i==SPI_DR;
		
		
	  
}

/************************************************************************
**  ByteRead
************************************************************************/
Uint8 ByteRead(void)
{
	 
	u8 data;
	while(!(SPI_SR & 0x02));
	SPI_DR=0x55;
	
	while(!(SPI_SR & 0x01));
	data=SPI_DR;
	
	
	return data;
	
}





