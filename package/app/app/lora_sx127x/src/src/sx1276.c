#include "sx1276.h"
#include "sx1276_hal.h"

SX1276_t SX1276;
uint8_t sx1276_rx_buf[256];
/**延时函数**/
/**SX1278多字节，连续写**/
void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
	uint8_t write_addr = addr | 0x80;
	uint8_t write_buf[size+1];
	uint8_t read_buf[size+1];

	memset(write_buf,0,size+1);
	memset(read_buf,0,size+1);
	write_buf[0] = write_addr;
	memcpy(&write_buf[1], buffer, size);

	spi_write_read_dis(write_buf, read_buf, size+1);
}

/**SX1278多字节，连续读**/
void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
	uint8_t read_addr = addr & 0x7F;
	uint8_t write_buf[size+1];
	uint8_t read_buf[size+1];

	memset(write_buf,0,size+1);
	memset(read_buf,0,size+1);
	write_buf[0] = read_addr;

	spi_write_read_dis(write_buf, read_buf, size+1);

	memcpy(buffer, &read_buf[1],  size);
}
/**SX1278单字节写**/
void SX1276Write( uint8_t addr, uint8_t data )
{
    SX1276WriteBuffer( addr, &data, 1 );
}
/**SX1278单字节读**/
uint8_t SX1276Read( uint8_t addr )
{
    uint8_t data;
	
    SX1276ReadBuffer(addr, &data, 1);
	
    return data;
}
/**SX1278初始化**/
uint8_t SX1276Init(void)
{
	uint8_t i,def_val;
	RadioRegisters_t RadioRegsInit[] = RADIO_INIT_REGISTERS_VALUE;

	SPI_INIT();
	MX_SX1276_GPIO_INIT();
	MX_SX1276_RESET();
	SX1276ReadBuffer(0x42,&def_val,1);
	if(def_val == 0x12)///version
	{
		DEBUG_PRINTF("SX1276Init:OK:%#x\n", def_val);
		SX1276SetOpMode( RF_OPMODE_SLEEP );
		for( i = 0; i < sizeof( RadioRegsInit ) / sizeof( RadioRegisters_t ); i++ )
		{
			SX1276SetModem( RadioRegsInit[i].Modem );
			SX1276Write( RadioRegsInit[i].Addr, RadioRegsInit[i].value );
		}
		SX1276SetModem( MODEM_LORA );
		return true;
	}
	else
	{
		DEBUG_PRINTF("SX1276Init:Failed:%#x\n", def_val);
	}
    return false;
}

uint8_t SX1276DeInit(void)
{
	MX_SX1276_GPIO_DE_INIT();
	SPI_DEINIT();

	return true;
}

/**设置SX1278调制模式**/
void SX1276SetModem( RadioModems_t modem )
{
    SX1276.Modem = modem;
	SX1276SetOpMode( RF_OPMODE_SLEEP );
	SX1276Write( REG_OPMODE, ( SX1276Read( REG_OPMODE ) & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_ON );
	SX1276Write( REG_DIOMAPPING1, 0x00 );        
	SX1276Write( REG_DIOMAPPING2, 0x00 );
}
/**设置SX1278模式**/
void SX1276SetOpMode( uint8_t opMode )
{
    SX1276Write( REG_OPMODE, ( SX1276Read( REG_OPMODE ) & RF_OPMODE_MASK ) | opMode );
}
/**设置SX1278发射或接收频点**/
void SX1276SetChannel( uint32_t freq )
{
    SX1276.LoRa.Channel = freq;
    freq = ( uint32_t )( ( double )freq / ( double )FREQ_STEP );
    SX1276Write( REG_FRFMSB, ( uint8_t )( ( freq >> 16 ) & 0xFF ) );
    SX1276Write( REG_FRFMID, ( uint8_t )( ( freq >> 8 ) & 0xFF ) );
    SX1276Write( REG_FRFLSB, ( uint8_t )(   freq & 0xFF ) );
}
/**查询SX1278信道，是否空闲**/
uint8_t SX1276IsChannelFree( RadioModems_t modem, uint32_t freq, int16_t rssiThresh )
{
    int16_t rssi = 0;
    SX1276SetModem( modem );
    SX1276SetChannel( freq );
    SX1276SetOpMode( RF_OPMODE_RECEIVER );
    SX1276DelayMs(1);
    rssi = SX1276ReadRssi( modem );
    SX1276SetSleep( );
    if( rssi > rssiThresh )
    {
        return false;
    }
    return true;
}
/**利用SX1278 RSSI值，生成随机数**/
uint16_t SX1276Random( void )
{
    uint8_t i;
    uint16_t rnd = 0;
    SX1276SetModem( MODEM_LORA );
    SX1276Write( REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
                  RFLR_IRQFLAGS_RXDONE |
                  RFLR_IRQFLAGS_PAYLOADCRCERROR |
                  RFLR_IRQFLAGS_VALIDHEADER |
                  RFLR_IRQFLAGS_TXDONE |
                  RFLR_IRQFLAGS_CADDONE |
                  RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                  RFLR_IRQFLAGS_CADDETECTED );
    SX1276SetOpMode( RF_OPMODE_RECEIVER );
    for( i = 0; i < 16; i++ )
    {
        SX1276DelayMs(1);
        rnd |= ( ( uint32_t )SX1276Read( REG_LR_RSSIWIDEBAND ) & 0x01 ) << i;
    }
    SX1276SetSleep( );
    return rnd;
}
/**SX1278接收设置**/
void SX1276SetRxConfig( RadioModems_t modem, uint32_t bandwidth,
                         uint32_t datarate, uint8_t coderate,
                         uint32_t bandwidthAfc, uint16_t preambleLen,
                         uint16_t symbTimeout, uint8_t fixLen,
                         uint8_t payloadLen,
                         uint8_t crcOn, uint8_t freqHopOn, uint8_t hopPeriod,
                         uint8_t iqInverted, uint8_t rxContinuous )
{
    uint8_t Sf;
    SX1276SetModem( modem );
	bandwidth = 7;
	SX1276.LoRa.Bandwidth = bandwidth;
	Sf = SF_12 - datarate;
	SX1276.LoRa.Coderate = coderate;
	SX1276.LoRa.PreambleLen = preambleLen;
	SX1276.LoRa.FixLen = fixLen;
	SX1276.LoRa.PayloadLen = payloadLen;
	SX1276.LoRa.CrcOn = crcOn;
	SX1276.LoRa.FreqHopOn = freqHopOn;
	SX1276.LoRa.HopPeriod = hopPeriod;
	SX1276.LoRa.IqInverted = iqInverted;
	SX1276.LoRa.RxContinuous = rxContinuous;
	if( Sf > 12 )
	{
		Sf = 12;
	}
	else if( Sf < 6 )
	{
		Sf = 6;
	}
	if( ( ( bandwidth == 7 ) && ( ( Sf == 11 ) || ( Sf == 12 ) ) ) ||
		( ( bandwidth == 8 ) && ( Sf == 12 ) ) )
	{
		SX1276.LoRa.LowDatarateOptimize = 0x01;
	}
	else
	{
		SX1276.LoRa.LowDatarateOptimize = 0x00;
	}
	SX1276Write( REG_LR_MODEMCONFIG1,
				 ( SX1276Read( REG_LR_MODEMCONFIG1 ) &
				   RFLR_MODEMCONFIG1_BW_MASK &
				   RFLR_MODEMCONFIG1_CODINGRATE_MASK &
				   RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) |
				   ( bandwidth << 4 ) | ( coderate << 1 ) |
				   fixLen );
	SX1276Write( REG_LR_MODEMCONFIG2,
				 ( SX1276Read( REG_LR_MODEMCONFIG2 ) &
				   RFLR_MODEMCONFIG2_SF_MASK &
				   RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK &
				   RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) |
				   ( Sf << 4 ) | ( crcOn << 2 ) |
				   ( ( symbTimeout >> 8 ) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) );
	SX1276Write( REG_LR_MODEMCONFIG3,
				 ( SX1276Read( REG_LR_MODEMCONFIG3 ) &
				   RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK ) |
				   ( SX1276.LoRa.LowDatarateOptimize << 3 ) );
	SX1276Write( REG_LR_SYMBTIMEOUTLSB, ( uint8_t )( symbTimeout & 0xFF ) );
	SX1276Write( REG_LR_PREAMBLEMSB, ( uint8_t )( ( preambleLen >> 8 ) & 0xFF ) );
	SX1276Write( REG_LR_PREAMBLELSB, ( uint8_t )( preambleLen & 0xFF ) );
	if( fixLen == 1 )
	{
		SX1276Write( REG_LR_PAYLOADLENGTH, payloadLen );
	}
	if( SX1276.LoRa.FreqHopOn == true )
	{
		SX1276Write( REG_LR_PLLHOP, ( SX1276Read( REG_LR_PLLHOP ) & RFLR_PLLHOP_FASTHOP_MASK ) | RFLR_PLLHOP_FASTHOP_ON );
		SX1276Write( REG_LR_HOPPERIOD, SX1276.LoRa.HopPeriod );
	}
	if( ( bandwidth == 9 ) && ( SX1276.LoRa.Channel > RF_MID_BAND_THRESH ))
	{
		SX1276Write( REG_LR_TEST36, 0x02 );
		SX1276Write( REG_LR_TEST3A, 0x64 );
	}
	else if( bandwidth == 9 )
	{
		SX1276Write( REG_LR_TEST36, 0x02 );
		SX1276Write( REG_LR_TEST3A, 0x7F );
	}
	else
	{
		SX1276Write( REG_LR_TEST36, 0x03 );
	}
	if( Sf == 6 )
	{
		SX1276Write( REG_LR_DETECTOPTIMIZE,( SX1276Read( REG_LR_DETECTOPTIMIZE ) &  RFLR_DETECTIONOPTIMIZE_MASK ) | RFLR_DETECTIONOPTIMIZE_SF6 );
		SX1276Write( REG_LR_DETECTIONTHRESHOLD, RFLR_DETECTIONTHRESH_SF6 );
	}
	else
	{
		SX1276Write( REG_LR_DETECTOPTIMIZE,( SX1276Read( REG_LR_DETECTOPTIMIZE ) & RFLR_DETECTIONOPTIMIZE_MASK ) |  RFLR_DETECTIONOPTIMIZE_SF7_TO_SF12 );
		SX1276Write( REG_LR_DETECTIONTHRESHOLD, RFLR_DETECTIONTHRESH_SF7_TO_SF12 );
	}
}
/**SX1278接收函数**/
void SX1276SetRx(void)
{
    uint8_t rxContinuous = false;

	if( SX1276.LoRa.IqInverted == true )
	{
		SX1276Write( REG_LR_INVERTIQ, ( ( SX1276Read( REG_LR_INVERTIQ ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK ) | RFLR_INVERTIQ_RX_ON | RFLR_INVERTIQ_TX_OFF ) );
		SX1276Write( REG_LR_INVERTIQ2, RFLR_INVERTIQ2_ON );
	}
	else
	{
		SX1276Write( REG_LR_INVERTIQ, ( ( SX1276Read( REG_LR_INVERTIQ ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK ) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_OFF ) );
		SX1276Write( REG_LR_INVERTIQ2, RFLR_INVERTIQ2_OFF );
	}
	if( SX1276.LoRa.Bandwidth < 9 )
	{
		SX1276Write( REG_LR_DETECTOPTIMIZE, SX1276Read( REG_LR_DETECTOPTIMIZE ) & 0x7F );
		SX1276Write( REG_LR_TEST30, 0x00 );
		switch( SX1276.LoRa.Bandwidth )
		{
		case 0:                     SX1276Write( REG_LR_TEST2F, 0x48 );
			SX1276SetChannel(SX1276.LoRa.Channel + (uint32_t)(7.81e3 ));
			break;
		case 1:                     SX1276Write( REG_LR_TEST2F, 0x44 );
			SX1276SetChannel(SX1276.LoRa.Channel + (uint32_t)(10.42e3 ));
			break;
		case 2:                     SX1276Write( REG_LR_TEST2F, 0x44 );
			SX1276SetChannel(SX1276.LoRa.Channel + (uint32_t)(15.62e3 ));
			break;
		case 3:                     SX1276Write( REG_LR_TEST2F, 0x44 );
			SX1276SetChannel(SX1276.LoRa.Channel + (uint32_t)(20.83e3 ));
			break;
		case 4:                     SX1276Write( REG_LR_TEST2F, 0x44 );
			SX1276SetChannel(SX1276.LoRa.Channel + (uint32_t)(31.25e3 ));
			break;
		case 5:                     SX1276Write( REG_LR_TEST2F, 0x44 );
			SX1276SetChannel(SX1276.LoRa.Channel + (uint32_t)(41.67e3 ));
			break;
		case 6:                     SX1276Write( REG_LR_TEST2F, 0x40 );
			break;
		case 7:                     SX1276Write( REG_LR_TEST2F, 0x40 );
			break;
		case 8:                     SX1276Write( REG_LR_TEST2F, 0x40 );
			break;
		}
	}
	else
	{
		SX1276Write( REG_LR_DETECTOPTIMIZE, SX1276Read( REG_LR_DETECTOPTIMIZE ) | 0x80 );
	}
	rxContinuous = SX1276.LoRa.RxContinuous;
	if( SX1276.LoRa.FreqHopOn == true )
	{
		SX1276Write( REG_LR_IRQFLAGSMASK,                                                                                                                                                       RFLR_IRQFLAGS_VALIDHEADER |
					 RFLR_IRQFLAGS_TXDONE |
					 RFLR_IRQFLAGS_CADDONE |
					 RFLR_IRQFLAGS_CADDETECTED );
		SX1276Write( REG_DIOMAPPING1, ( SX1276Read( REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK & RFLR_DIOMAPPING1_DIO2_MASK  ) | RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO2_00 );
		SX1276Write( REG_DIOMAPPING2, ( SX1276Read( REG_DIOMAPPING2 ) & RFLR_DIOMAPPING2_DIO4_MASK ) | RFLR_DIOMAPPING2_DIO4_10 );
	}
	else
	{
		SX1276Write( REG_LR_IRQFLAGSMASK,                                                                                                                                                       RFLR_IRQFLAGS_VALIDHEADER |
					 RFLR_IRQFLAGS_TXDONE |
					 RFLR_IRQFLAGS_CADDONE |
					 RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
					 RFLR_IRQFLAGS_CADDETECTED );
		SX1276Write( REG_DIOMAPPING1, ( SX1276Read( REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK ) | RFLR_DIOMAPPING1_DIO0_00 );
		SX1276Write( REG_DIOMAPPING2, ( SX1276Read( REG_DIOMAPPING2 ) & RFLR_DIOMAPPING2_DIO4_MASK ) | RFLR_DIOMAPPING2_DIO4_10 );
	}
	SX1276Write( REG_LR_FIFORXBASEADDR, 0 );
	SX1276Write( REG_LR_FIFOADDRPTR, 0 );

    SX1276.State = RF_RX_RUNNING;
    if( rxContinuous == true )
    {
        SX1276SetOpMode( RFLR_OPMODE_RECEIVER );
    }
    else
    {
        SX1276SetOpMode( RFLR_OPMODE_RECEIVER_SINGLE );
    }
}
/**SX1278发射设置**/
void SX1276SetTxConfig( RadioModems_t modem, uint8_t power, 
						uint32_t fdev, uint32_t bandwidth, 
                        uint32_t datarate,uint8_t coderate, 
						uint16_t preambleLen, uint8_t fixLen, 
                        uint8_t crcOn, uint8_t freqHopOn, 
						uint8_t hopPeriod, uint8_t iqInverted, 
					    uint32_t timeout )
{
    uint8_t paConfig = 0;
    uint8_t paDac = 0,Sf;
    SX1276SetModem( modem );
    paConfig = SX1276Read( REG_PACONFIG );
    paDac = SX1276Read( REG_PADAC );
    paConfig = ( paConfig & RF_PACONFIG_PASELECT_MASK ) | RF_PACONFIG_PASELECT_PABOOST;
    paConfig = ( paConfig & RF_PACONFIG_MAX_POWER_MASK ) | 0x70;
    if( ( paConfig & RF_PACONFIG_PASELECT_PABOOST ) == RF_PACONFIG_PASELECT_PABOOST )
    {
        if( power > 17 )
        {
            paDac = ( paDac & RF_PADAC_20DBM_MASK ) | RF_PADAC_20DBM_ON;
        }
        else
        {
            paDac = ( paDac & RF_PADAC_20DBM_MASK ) | RF_PADAC_20DBM_OFF;
        }
        if( ( paDac & RF_PADAC_20DBM_ON ) == RF_PADAC_20DBM_ON )
        {
            if( power < 5 )
            {
                power = 5;
            }
            if( power > 20 )
            {
                power = 20;
            }
            paConfig = ( paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 5 ) & 0x0F );
        }
        else
        {
            if( power < 2 )
            {
                power = 2;
            }
            if( power > 17 )
            {
                power = 17;
            }
            paConfig = ( paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 2 ) & 0x0F );
        }
    }
    else
    {
        if( power > 14 )
        {
            power = 14;
        }
        paConfig = ( paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power + 1 ) & 0x0F );
    }
    SX1276Write( REG_PACONFIG, paConfig );
    SX1276Write( REG_PADAC, paDac );

	SX1276.LoRa.Power = power;
	if( bandwidth > 2 )
	{
		bandwidth = 0;
	}
	bandwidth += 7;
	SX1276.LoRa.Bandwidth = bandwidth;
	Sf             = SF_12 - datarate;
	SX1276.LoRa.Sf = Sf;
	SX1276.LoRa.Coderate = coderate;
	SX1276.LoRa.PreambleLen = preambleLen;
	SX1276.LoRa.FixLen = fixLen;
	SX1276.LoRa.FreqHopOn = freqHopOn;
	SX1276.LoRa.HopPeriod = hopPeriod;
	SX1276.LoRa.CrcOn = crcOn;
	SX1276.LoRa.IqInverted = iqInverted;
	SX1276.LoRa.TxTimeout = timeout;
	if( Sf > 12 )
	{
		Sf = 12;
	}
	else if( Sf < 6 )
	{
		Sf = 6;
	}
	if( ( ( bandwidth == 7 ) && ( ( Sf == 11 ) || ( Sf == 12 ) ) ) ||
		( ( bandwidth == 8 ) && ( Sf == 12 ) ) )
	{
		SX1276.LoRa.LowDatarateOptimize = 0x01;
	}
	else
	{
		SX1276.LoRa.LowDatarateOptimize = 0x00;
	}
	if( SX1276.LoRa.FreqHopOn == true )
	{
		SX1276Write( REG_LR_PLLHOP, ( SX1276Read( REG_LR_PLLHOP ) & RFLR_PLLHOP_FASTHOP_MASK ) | RFLR_PLLHOP_FASTHOP_ON );
		SX1276Write( REG_LR_HOPPERIOD, SX1276.LoRa.HopPeriod );
	}
	SX1276Write( REG_LR_MODEMCONFIG1,
				 ( SX1276Read( REG_LR_MODEMCONFIG1 ) &
				   RFLR_MODEMCONFIG1_BW_MASK &
				   RFLR_MODEMCONFIG1_CODINGRATE_MASK &
				   RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) |
				   ( bandwidth << 4 ) | ( coderate << 1 ) |
				   fixLen );
	SX1276Write( REG_LR_MODEMCONFIG2,
				 ( SX1276Read( REG_LR_MODEMCONFIG2 ) &
				   RFLR_MODEMCONFIG2_SF_MASK &
				   RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK ) |
				   ( Sf << 4 ) | ( crcOn << 2 ) );
	SX1276Write( REG_LR_MODEMCONFIG3,
				 ( SX1276Read( REG_LR_MODEMCONFIG3 ) &
				   RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK ) |
				   ( SX1276.LoRa.LowDatarateOptimize << 3 ) );
	SX1276Write( REG_LR_PREAMBLEMSB, ( preambleLen >> 8 ) & 0x00FF );
	SX1276Write( REG_LR_PREAMBLELSB, preambleLen & 0xFF );
	if( Sf == 6 )
	{
		SX1276Write( REG_LR_DETECTOPTIMIZE,
					 ( SX1276Read( REG_LR_DETECTOPTIMIZE ) &
					   RFLR_DETECTIONOPTIMIZE_MASK ) |
					   RFLR_DETECTIONOPTIMIZE_SF6 );
		SX1276Write( REG_LR_DETECTIONTHRESHOLD,
					 RFLR_DETECTIONTHRESH_SF6 );
	}
	else
	{
		SX1276Write( REG_LR_DETECTOPTIMIZE,
					 ( SX1276Read( REG_LR_DETECTOPTIMIZE ) &
					 RFLR_DETECTIONOPTIMIZE_MASK ) |
					 RFLR_DETECTIONOPTIMIZE_SF7_TO_SF12 );
		SX1276Write( REG_LR_DETECTIONTHRESHOLD,
					 RFLR_DETECTIONTHRESH_SF7_TO_SF12 );
	}
}
/**SX1278发射函数**/
void SX1276Send( uint8_t *buffer, uint8_t size )
{
	if( SX1276.LoRa.IqInverted == true )
	{
		SX1276Write( REG_LR_INVERTIQ, ( ( SX1276Read( REG_LR_INVERTIQ ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK ) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_ON ) );
		SX1276Write( REG_LR_INVERTIQ2, RFLR_INVERTIQ2_ON );
	}
	else
	{
		SX1276Write( REG_LR_INVERTIQ, ( ( SX1276Read( REG_LR_INVERTIQ ) & RFLR_INVERTIQ_TX_MASK & RFLR_INVERTIQ_RX_MASK ) | RFLR_INVERTIQ_RX_OFF | RFLR_INVERTIQ_TX_OFF ) );
		SX1276Write( REG_LR_INVERTIQ2, RFLR_INVERTIQ2_OFF );
	}
	SX1276.Size = size;
	SX1276Write( REG_LR_PAYLOADLENGTH, size );
	SX1276Write( REG_LR_FIFOTXBASEADDR, 0 );
	SX1276Write( REG_LR_FIFOADDRPTR, 0 );
	if( ( SX1276Read( REG_OPMODE ) & ~RF_OPMODE_MASK ) == RF_OPMODE_SLEEP )
	{
		SX1276SetStby( );
		SX1276DelayMs(1);
	}
	
    SX1276WriteBuffer( 0, buffer, size );
	
	SX1276Write( REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
									  RFLR_IRQFLAGS_RXDONE |
									  RFLR_IRQFLAGS_PAYLOADCRCERROR |
									  RFLR_IRQFLAGS_VALIDHEADER |
									  RFLR_IRQFLAGS_CADDONE |
									  RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
									  RFLR_IRQFLAGS_CADDETECTED );
	SX1276Write( REG_DIOMAPPING1, ( SX1276Read( REG_DIOMAPPING1 ) & RFLR_DIOMAPPING1_DIO0_MASK ) | RFLR_DIOMAPPING1_DIO0_01 );
	SX1276Write( REG_DIOMAPPING2, ( SX1276Read( REG_DIOMAPPING2 ) & RFLR_DIOMAPPING2_DIO4_MASK ) | RFLR_DIOMAPPING2_DIO4_00 );
    SX1276.State = RF_TX_RUNNING;
    SX1276SetOpMode( RF_OPMODE_TRANSMITTER );   
}
/**SX1278低功耗设置**/
void SX1276SetSleep( void )
{
    SX1276SetOpMode( RF_OPMODE_SLEEP );
}
void SX1276SetStby( void )
{
    SX1276SetOpMode( RF_OPMODE_STANDBY );
}
/**读当前接收包的信号强度**/
int16_t SX1276ReadRssi( RadioModems_t modem )
{
    int16_t rssi = 0;
	rssi = RSSI_OFFSET_LF + SX1276Read( REG_LR_RSSIVALUE );
    return rssi;
}
/**设置接收或发射，最大字节数**/
void SX1276SetMaxPayloadLength( RadioModems_t modem, uint8_t max )
{
    SX1276SetModem( modem );
    SX1276Write( REG_LR_PAYLOADMAXLENGTH, max );
}
/**SX1278模块接收到数据，MCU读取其FIFO并打印，并解析RSSI,SNR**/
void SX1276ReadRxPkt(void)
{
	uint8_t irqFlags = 0;
	int16_t rssi,snr = 0;
	uint8_t i,fifo_size;
	
	char 	str_char[100] = {0};
	uint8_t str_len = 0;
	
	SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE );
	irqFlags = SX1276Read( REG_LR_IRQFLAGS );
	if( ( irqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR_MASK ) == RFLR_IRQFLAGS_PAYLOADCRCERROR )
	{
		SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_PAYLOADCRCERROR );
		DEBUG_PRINTF("crc bad\n");
	}
	else
	{
		DEBUG_PRINTF("crc ok\n");
		///灯闪烁一次
		//HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
		//HAL_Delay( 1000 );
		//HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);
	}
	SX1276.SnrValue = SX1276Read( REG_LR_PKTSNRVALUE );
	if( SX1276.SnrValue & 0x80 ) 	
	{
		snr = ( ( ~SX1276.SnrValue + 1 ) & 0xFF ) >> 2;
		snr = -snr;
	}
	else
	{
		snr = ( SX1276.SnrValue & 0xFF ) >> 2;
	}
	rssi = SX1276Read( REG_LR_PKTRSSIVALUE );
	if( snr < 0 )
	{
		SX1276.RssiValue = RSSI_OFFSET_LF + rssi + ( rssi >> 4 ) + snr;
	}
	else
	{
		SX1276.RssiValue = RSSI_OFFSET_LF + rssi + ( rssi >> 4 );
	}

	fifo_size = SX1276Read( REG_LR_RXNBBYTES );
	SX1276ReadBuffer(0,sx1276_rx_buf,fifo_size);
	

	for(i=0;i<fifo_size;i++)
	{
		str_len = sprintf(str_char,"%02X ",sx1276_rx_buf[i]);
		DEBUG_PRINTF("sx1276 rx fifo:%s\n", (uint8_t*)&str_char);
	}

	
	str_len = sprintf(str_char,"rssi = %d,snr = %d\r\n", SX1276.RssiValue,snr);
	DEBUG_PRINTF("%s\n", (uint8_t*)&str_char);

}

/**引脚中断，根据状态，调用不同的事件函数**/
void SX1276OnDio0Irq( void )
{
    if(SX1276.State == RF_RX_RUNNING)
    {
		LoraRxDoneCallback();
	}
    else if(SX1276.State == RF_TX_RUNNING)
	{
		SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE );
		LoraTxDoneCallback( );
	}
}

void SX1276OnDio1Irq( void )
{
    if( SX1276.State == RF_RX_RUNNING)
    {
		SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXTIMEOUT );
		LoraRxTimeoutCallback( );
    }
}
