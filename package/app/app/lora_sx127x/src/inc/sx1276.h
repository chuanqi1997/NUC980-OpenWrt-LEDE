#ifndef __SX1276_H__
#define __SX1276_H__

	#include "sx1276_regs_fsk.h"
	#include "sx1276_regs_lora.h"
	#include "radio.h"
	#include <stdint.h>
	#include <stdbool.h>
	#define RSSI_OFFSET_LF      -164
	#define RADIO_OSC_STARTUP                           2 ///2
	#define RADIO_SLEEP_TO_RX                           3 
	#define RADIO_WAKEUP_TIME                           ( RADIO_OSC_STARTUP + RADIO_SLEEP_TO_RX )
	#define RADIO_INIT_REGISTERS_VALUE                \
	{                                                 \
		{ MODEM_FSK , REG_LNA                , 0x23 },\
		{ MODEM_FSK , REG_RXCONFIG           , 0x1E },\
		{ MODEM_FSK , REG_RSSICONFIG         , 0xD2 },\
		{ MODEM_FSK , REG_AFCFEI             , 0x01 },\
		{ MODEM_FSK , REG_PREAMBLEDETECT     , 0xAA },\
		{ MODEM_FSK , REG_OSC                , 0x07 },\
		{ MODEM_FSK , REG_SYNCCONFIG         , 0x12 },\
		{ MODEM_FSK , REG_SYNCVALUE1         , 0xC1 },\
		{ MODEM_FSK , REG_SYNCVALUE2         , 0x94 },\
		{ MODEM_FSK , REG_SYNCVALUE3         , 0xC1 },\
		{ MODEM_FSK , REG_PACKETCONFIG1      , 0xD8 },\
		{ MODEM_FSK , REG_FIFOTHRESH         , 0x8F },\
		{ MODEM_FSK , REG_IMAGECAL           , 0x02 },\
		{ MODEM_FSK , REG_DIOMAPPING1        , 0x00 },\
		{ MODEM_FSK , REG_DIOMAPPING2        , 0x30 },\
		{ MODEM_LORA, REG_LR_PAYLOADMAXLENGTH, 0x40 },\
	}                                                 \

	#define RF_MID_BAND_THRESH                          525000000
	#define XTAL_FREQ                                   32000000
	#define FREQ_STEP                                   61.03515625
	#define RX_BUFFER_SIZE                              250

	#define SF_12                                       12

	typedef struct
	{
		RadioModems_t 	Modem;
		uint8_t       	Addr;
		uint8_t       	value;
	}RadioRegisters_t;

	typedef struct
	{
		int8_t   	Power;
		uint32_t 	Fdev;
		uint32_t 	Bandwidth;
		uint32_t 	BandwidthAfc;
		uint32_t 	Datarate;
		uint16_t 	PreambleLen;
		uint8_t     FixLen;
		uint8_t  	PayloadLen;
		uint8_t     CrcOn;
		uint8_t     IqInverted;
		uint8_t     RxContinuous;
		uint32_t 	TxTimeout;
	}RadioFskSettings_t;
	typedef struct
	{
		uint8_t  	PreambleDetected;
		uint8_t  	SyncWordDetected;
		int8_t   	RssiValue;
		int32_t  	AfcValue;
		uint8_t  	RxGain;
		uint16_t 	Size;
		uint16_t 	NbBytes;
		uint8_t  	FifoThresh;
		uint8_t  	ChunkSize;
	}RadioFskPacketHandler_t;
	typedef struct
	{
		uint32_t    	Channel;
		uint32_t 		Bandwidth;
		uint8_t   		Power;
		uint8_t         Sf;
		uint8_t     	LowDatarateOptimize;
		uint8_t  		Coderate;
		uint16_t 		PreambleLen;
		uint8_t    		FixLen;
		uint8_t  		PayloadLen;
		uint8_t     	CrcOn;
		uint8_t     	FreqHopOn;
		uint8_t  		HopPeriod;
		uint8_t     	IqInverted;
		uint8_t     	RxContinuous;
		uint32_t 		TxTimeout;
	}LoRaSettings_t;

	typedef struct
	{
		RadioModems_t 	Modem;
		RadioState_t    State;
		LoRaSettings_t  LoRa;
		int16_t 		SnrValue;
		int16_t 		RssiValue;
		uint8_t 		Size;
	}SX1276_t;

	uint8_t 	SX1276Init(void);
	uint8_t 	SX1276DeInit(void);
	void 		SX1276SetModem( RadioModems_t modem );
	void 		SX1276SetChannel( uint32_t freq );
	uint8_t 	SX1276IsChannelFree( RadioModems_t modem, uint32_t freq, int16_t rssiThresh );
	uint16_t 	SX1276Random( void );
	void 		SX1276SetRxConfig( 	RadioModems_t modem, 
									uint32_t bandwidth,
									uint32_t datarate, 
									uint8_t coderate,
									uint32_t bandwidthAfc, 
									uint16_t preambleLen,
									uint16_t symbTimeout, 
									uint8_t fixLen,
									uint8_t payloadLen,
									uint8_t crcOn, 
									uint8_t FreqHopOn, 
									uint8_t HopPeriod,
									uint8_t iqInverted, 
									uint8_t rxContinuous );
	void 		SX1276SetTxConfig( 	RadioModems_t modem, 
									uint8_t power, 
									uint32_t fdev, 
									uint32_t bandwidth, 
									uint32_t datarate,
									uint8_t coderate, 
									uint16_t preambleLen,
									uint8_t fixLen, 
									uint8_t crcOn, 
									uint8_t FreqHopOn,
									uint8_t HopPeriod, 
									uint8_t iqInverted, 
									uint32_t timeout );
	void 		SX1276Send( uint8_t *buffer, uint8_t size );
	void 		SX1276SetSleep( void );
	void 		SX1276SetStby( void );
	void 		SX1276SetRx(void);
	int16_t 	SX1276ReadRssi( RadioModems_t modem );
	void 		SX1276Write( uint8_t addr, uint8_t data );
	uint8_t 	SX1276Read( uint8_t addr );
	void 		SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size );
	void 		SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size );
	void 	    SX1276ReadRxPkt(void);
	void 		SX1276SetMaxPayloadLength( RadioModems_t modem, uint8_t max );
	void 		SX1276SetOpMode( uint8_t opMode );
	void 		SX1276SetTx(void);
	void 		SX1276OnDio0Irq(void);
	void 		SX1276OnDio1Irq(void);
#endif 
