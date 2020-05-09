#ifndef __RADIO_H__
#define __RADIO_H__

	typedef enum
	{
		MODEM_FSK = 0,
		MODEM_LORA,
	}RadioModems_t;

	typedef enum
	{
		RF_IDLE = 0,
		RF_RX_RUNNING,
		RF_TX_RUNNING,
	}RadioState_t;

#endif 
