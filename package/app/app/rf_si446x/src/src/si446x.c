#include <stdio.h>	
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <pthread.h>


#include "si446x.h"
#include "radio_config_Si4463_2kbps.h"
#include "board.h"
const static INT8U config_table[] = RADIO_CONFIGURATION_DATA_ARRAY;

/*read a array of command response*/
void SI446X_READ_RESPONSE( INT8U *buffer, INT8U size );

/*wait the device ready to response a command*/
void SI446X_WAIT_CTS( void );

/*write data to TX fifo*/
void SI446X_W_TX_FIFO( INT8U *txbuffer, INT8U size );


static void  delay_ms(int i);
static void  delay_us(int i);

static void  delay_ms(int i)
{    	
	usleep(i * 1000);
}
static void  delay_us(int i)
{
	usleep(i);
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
/*
        SPI_ExchangeByte( READ_CMD_BUFF );
        cts = SPI_ExchangeByte( 0xFF );
		SI446x_SPI_ReadRegs(buffer, size);
		//void SI446x_SPI_WriteReadRegs(uint8_t * write, uint8_t * read, int cnt);
*/
		INT8U write[2] = {READ_CMD_BUFF, 0xFF};
		INT8U read[2] = {0};
		SI446x_SPI_WriteReadRegs(write, read, 2);
		cts = read[1];
		//DEBUG_PRINTF("%s cts:%#x\n", __FUNCTION__, cts);
    }while( cts != 0xFF );
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
	SI446x_SPI_WriteRegs(cmd, cmdsize);
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
		//void SI446x_SPI_WriteReadRegs(uint8_t * write, uint8_t * read, int cnt);
	int send_size = size+1;
	INT8U write_buf[send_size];
	INT8U read_buf[send_size];

	memset(write_buf,0xFF,send_size);
	memset(read_buf,0,send_size);

	write_buf[0] = READ_CMD_BUFF;
/*
	SPI_ExchangeByte( READ_CMD_BUFF );
	SI446x_SPI_ReadRegs(buffer, size);
*/
	SI446x_SPI_WriteReadRegs(write_buf, read_buf, send_size);
	memcpy(buffer, &read_buf[1],  size);
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

	INT8U write[2] = {READ_RX_FIFO, 0xFF};
	INT8U read[2] = {0};
	SI446x_SPI_WriteReadRegs(write, read, 2);

	#if PACKET_LENGTH == 0
		length = read[1];
	#else
		length = PACKET_LENGTH;
	#endif

    i = length;

	int send_size = length+2;

	INT8U write_buf[send_size];
	INT8U read_buf[send_size];

	memset(write_buf,0xFF,send_size);
	memset(read_buf,0,send_size);

	write_buf[0] = READ_RX_FIFO;

	SI446x_SPI_WriteReadRegs(write_buf, read_buf, send_size);
	memcpy(buffer, &read_buf[1], length);

    return i;
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

/*
    ////SI_CSN_LOW( );start
    SPI_ExchangeByte( WRITE_TX_FIFO );
#if PACKET_LENGTH == 0
    tx_len ++;
    SPI_ExchangeByte( size );
#endif
    while( size -- )    { SPI_ExchangeByte( *txbuffer++ ); }
    ////SI_CSN_HIGH( );//end

*/

	#if PACKET_LENGTH == 0
		tx_len ++;
		int send_size = size+2;
		INT8U write_buf[send_size];
		INT8U read_buf[send_size];

		memset(write_buf,0xFF,send_size);
		memset(read_buf,0,send_size);
		write_buf[0] = WRITE_TX_FIFO;
		write_buf[1] = size;

		memcpy(&write_buf[2], txbuffer, size);

		SI446x_SPI_WriteReadRegs(write_buf, read_buf, send_size);
	#else
		int send_size = length+1;
		INT8U write_buf[send_size];
		INT8U read_buf[send_size];

		memset(write_buf,0xFF,send_size);
		memset(read_buf,0,send_size);
		write_buf[0] = WRITE_TX_FIFO;

		memcpy(&write_buf[1], txbuffer, size);
		SI446x_SPI_WriteReadRegs(write_buf, read_buf, send_size);
	#endif




    cmd[0] = START_TX;
    cmd[1] = channel;
    cmd[2] = condition;
    cmd[3] = 0;
    cmd[4] = tx_len;
    SI446X_CMD( cmd, 5 );
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
SI446X_NOP( );
Function : NO Operation command
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
INT8U SI446X_NOP( void )
{
    INT8U cts;
    //SI_CSN_LOW( );
    cts = SPI_ExchangeByte( NOP );
    //SI_CSN_HIGH( );
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

	SI_SDN_HIGH( );//si446x power down
	delay_ms(10);// delay for a moment
	SI_SDN_LOW( );//power on
	
	//SI_CSN_HIGH( );// nSEL
	delay_ms(100);
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
        SI446X_CMD( (u8*)(config_table + j), i );
        j += i;
    }
#if PACKET_LENGTH > 0           //fixed packet length
    SI446X_SET_PROPERTY_1( PKT_FIELD_1_LENGTH_7_0, PACKET_LENGTH );
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
    SI446X_SET_PROPERTY_1( PKT_FIELD_2_LENGTH_7_0, 0x40 );//FIFO SIZE 64 Byte
    SI446X_SET_PROPERTY_1( PKT_FIELD_2_CONFIG, 0x00 );
    SI446X_SET_PROPERTY_1( PKT_FIELD_2_CRC_CONFIG, 0x00 );
#endif //PACKET_LENGTH

	  SI446X_SET_PROPERTY_1(PA_PWR_LVL,0x7f);//2019.04.21
    //重要： 4463的GDO2，GDO3控制射频开关，  0X20 ,0X21 
    //发射时必须： GDO2=1，GDO3=0
    //接收时必须： GDO2=0，GDO3=1
	SI446X_GPIO_CONFIG( 0, 0, 0x21, 0x20, 0, 0, 0 );//接收
   // SI446X_GPIO_CONFIG( 0, 0, 0x20, 0x21, 0, 0, 0 );//重要
   // SI446X_GPIO_CONFIG(35, 24, 33|0x40, 32|0x40, 0, 0, 0);  
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
    //SI_CSN_LOW( );
    SPI_ExchangeByte( WRITE_TX_FIFO );
    while( size -- )    { SPI_ExchangeByte( *txbuffer++ ); }
    //SI_CSN_HIGH( );
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
	
	#ifdef SI446x_TEST
		DEBUG_PRINTF("SI446X_GPIO_CONFIG():");
		DEBUG_PRINTF(" %#x %#x %#x %#x %#x %#x %#x %#x \r\n", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4],cmd[5], cmd[6], cmd[7]);
	#endif
	
	
}

/*
=================================================================================
Get_RSSIValue( );
Function : configuration the device
INTPUT   : NONE
OUTPUT   : unsigned char
=================================================================================
*/
void Get_RSSIValue(u8 *buffer, u8 size )
{
	u8 cmd= GET_MODEM_STATUS;
    SI446X_CMD(&cmd, 1);
    SI446X_READ_RESPONSE(buffer, size);
}

//\C9\E8\D6\C3SI4463\B5ķ\A2\C9书\C2\CA	 TxPower\B7\B6Χ\D4\DA0\B5\BD127֮\BC\E4
void SI446X_SetTxPower(u8 TxPower) 
{
	u8 cmd[8];
    cmd[0] = SET_PROPERTY;
    cmd[1] = 0x22;
    cmd[2] = 0x04;
    cmd[3] = 0x00;
    cmd[4] = 0x08;
	cmd[5] = TxPower;
	cmd[6] = 0x00;
	cmd[7] = 0x3D;
    SI446X_CMD( cmd, 8);
}

//\B3\CC\D0\F2Ĭ\C8\CF\C5\E4\D6õ\C4\CA\C750Kbps, \BF\C9\D2\D4ͨ\B9\FD\D5\E2\B8\F6\BA\AF\CA\FD\B8\FC\B8Ŀ\D5\D6д\AB\CA\E4\CB\D9\C2\CA
// \CB\D9\C2\CA50K, DataRate, \B7ֱ\F0\CAǣ\BA0x0F  0x42  0x40
// \CB\D9\C2\CA100K, DataRate, \B7ֱ\F0\CAǣ\BA0x1E  0x84  0x80
// \CB\D9\C2\CA250K, DataRate, \B7ֱ\F0\CAǣ\BA0x26  0x25  0xA0
// \CB\D9\C2\CA500K, DataRate, \B7ֱ\F0\CAǣ\BA0x4C  0x4B  0x40
void SI446X_Set_DataRate(u8 *DataRate)  //\C9\E8\D6\C3SI4463\BF\D5\D6\D0\CE\DE\CFߴ\AB\CA\E4\CB\D9\C2ʵĺ\AF\CA\FD
{
	u8 cmd[10];
    cmd[0] = SET_PROPERTY;
	cmd[1] = 0x20;
    cmd[2] = 0x06;
    cmd[3] = 0x00;

	cmd[4] = 0x03;
    cmd[5] = 0x00;
    cmd[6] = 0x07;

	cmd[7] = *DataRate;
	cmd[8] = *(DataRate+1);
	cmd[9] = *(DataRate+1);
	SI446X_CMD( cmd, 10);
}

INT8U SI446X_GET_DEVICE_STATE(void)
{
   INT8U cmd[3];
   cmd[0] = REQUEST_DEVICE_STATE;
   SI446X_CMD(cmd, 1);
   SI446X_READ_RESPONSE(cmd, 3);
   return cmd[1] & 0x0F;
}
/*===========================================================================
SI446X_SET_POWER();
Function : Set the PA Power of the device
INTPUT   : Power_Level,  0- 7F
============================================================================*/
void SI446X_SET_POWER(INT8U Power_Level)
{
    DEBUG_PRINTF("%s :%d\r\n", __FUNCTION__, Power_Level);
    SI446X_SET_PROPERTY_1(PA_PWR_LVL, Power_Level);
}


/*===========================================================================
SI446X_SET_SYNC();
Function : Set the SYNC bytes of the device
INTPUT   : sync, the SYNC word
============================================================================*/
void SI446X_SET_SYNC(INT32U sync)
{
    SI446X_SET_PROPERTY_1(SYNC_BITS_31_24, sync>>24);
    SI446X_SET_PROPERTY_1(SYNC_BITS_23_16, sync>>16);
    SI446X_SET_PROPERTY_1(SYNC_BITS_15_8,  sync>>8);
    SI446X_SET_PROPERTY_1(SYNC_BITS_7_0,   sync   );
}
/*===========================================================================
SI446X_CHIP_STATUS();
Function : Get the chip status
INTPUT   : buffer_8, a buffer to store the read date
============================================================================*/
void SI446X_CHIP_STATUS(INT8U* buffer_8)
{
    INT8U cmd[2];
    cmd[0] = GET_CHIP_STATUS;
    cmd[1] = 0xFF;
    SI446X_CMD(cmd, 8);
    SI446X_READ_RESPONSE(buffer_8, 4);
}

/*===========================================================================
SI446X_SET_MODEM();
Function : set the rf modem mod
INTPUT   : MODEM_TYPE
============================================================================*/
void SI446X_SET_MODEM(INT8U MODEM_TYPE)
{
    uint8_t Modem_Type =  SI446X_GET_PROPERTY_1(MODEM_MOD_TYPE);
    DEBUG_PRINTF("%s Modem_Type Read:%d\r\n", Modem_Type);
//#define SI446X_PROP_MODEM_MOD_TYPE_MOD_TYPE_ENUM_CW 0
//#define SI446X_PROP_MODEM_MOD_TYPE_MOD_TYPE_ENUM_OOK 1
//#define SI446X_PROP_MODEM_MOD_TYPE_MOD_TYPE_ENUM_2FSK 2
//#define SI446X_PROP_MODEM_MOD_TYPE_MOD_TYPE_ENUM_2GFSK 3
//#define SI446X_PROP_MODEM_MOD_TYPE_MOD_TYPE_ENUM_4FSK 4
//#define SI446X_PROP_MODEM_MOD_TYPE_MOD_TYPE_ENUM_4GFSK 5
    Modem_Type &= ~0x07;
    Modem_Type |= MODEM_TYPE;
    SI446X_SET_PROPERTY_1(MODEM_MOD_TYPE, Modem_Type);
    
    Modem_Type =  SI446X_GET_PROPERTY_1(MODEM_MOD_TYPE);
    DEBUG_PRINTF("%s Modem_Type Set then Read:%d\r\n", Modem_Type);
}

/*===========================================================================
SI446X_SET_FREQ();
Function : set the rf freq
INTPUT   : 
============================================================================*/
void SI446X_SET_FREQ(uint32_t Freq)
{
    //FREQ_CONTROL_INTE
    //FREQ_CONTROL_FRAC2
    //FREQ_CONTROL_FRAC1
    //FREQ_CONTROL_FRAC0
}

 


/*
=================================================================================
------------------------------------End of FILE----------------------------------
=================================================================================
*/
