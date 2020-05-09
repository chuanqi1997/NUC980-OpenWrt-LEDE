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
#include <stdbool.h>
#include <pthread.h>
#include <sys/wait.h>
#include "sx1276.h"
#include "sx1276_hal.h"

static void pabort(const char *s)
{
	perror(s);
	abort();
}

void Stop(int signo) 
{
	printf("SIGINT! STOP!\n");
	SX1276DeInit();
	_exit(0);
}

extern LORA_STATUS_T				lora_status;




/**
Lora发送模块，参数设置
freq = 470~510MHz,频点
pwr  = 0~20,数值越大，发射功率越大
dr   = 0,1,2,3,4,5，对应着SF12,11,10,9,8,7，SF7。其中SF7，速率最快

发送流程，函数调用说明：
1)准备发送的数据；
2)设置最大发送字节数；
3)设置LoRa发送参数；
4)准备发送的频点；
5)开启发送；
6)等待发送完成，引脚中断MCU；
7)发送完成；
8)重复过程1~7。
**/
void StackTxParaSet(RadioModems_t modem, uint32_t tx_freq,uint8_t tx_pwr,uint8_t tx_dr)
{
    uint8_t sx1276_pkt_buf[256];
	uint8_t sx1276_pkt_size;

	lora_status.tx_done_flag = false;
	
	sx1276_pkt_size = strlen("LoRa Tx Packet.");
	memcpy(sx1276_pkt_buf,"LoRa Tx Packet.",sx1276_pkt_size);

	SX1276SetMaxPayloadLength(modem, sx1276_pkt_size);
	SX1276SetTxConfig(modem,
					  tx_pwr, 
					  0, 
					  0, 
					  tx_dr, 
					  1, 
					  8, 
					  false, 
					  true, 
					  0, 
					  0, 
					  false, 
					  3000);
	///设置频点				  
	SX1276SetChannel( tx_freq );
	///启动发射
	SX1276Send(sx1276_pkt_buf, sx1276_pkt_size);
	while(!lora_status.tx_done_flag)
	{
		usleep(100);
	}
}
/**
Lora接收模块，参数设置
freq = 470~510MHz,频点
dr   = 0,1,2,3,4,5，对应着SF12,11,10,9,8,7。其中SF7，速率最快

发送流程，函数调用说明：
1)设置接收频点；
2)设置接收最大字节数；
3)设置LoRa接收参数；
4)开启接收，并等待接收到数据包；
5)接收完成，引脚中断MCU；
6)读取数据包并解析；
7)设置模块休眠；
8)重复过程1~7。
**/
void LoraRxParaSet( RadioModems_t modem, uint32_t rx_freq,uint8_t dr)
{
	uint16_t symb_timeout = 10;  

	lora_status.rx_done_flag = false;
	///设置频点
	SX1276SetChannel( rx_freq );
	SX1276SetMaxPayloadLength( modem,255);

	/*
	void SX1276SetRxConfig( RadioModems_t modem, uint32_t bandwidth,
                         uint32_t datarate, uint8_t coderate,
                         uint32_t bandwidthAfc, uint16_t preambleLen,
                         uint16_t symbTimeout, uint8_t fixLen,
                         uint8_t payloadLen,
                         uint8_t crcOn, uint8_t freqHopOn, uint8_t hopPeriod,
                         uint8_t iqInverted, uint8_t rxContinuous )
	*/
	SX1276SetRxConfig( 	modem, //modem
						0, //bandwidth
						dr, //datarate
						1, //coderate
						0, //bandwidthAfc
						8, //preambleLen
						symb_timeout, //symbTimeout
						false, //fixLen
						0, //payloadLen
						false, //crcOn
						0, //freqHopOn
						0, //hopPeriod
						false, ///iq not invert,
						true );///rx continue mode
	///开启接收
	SX1276SetRx();	
	while(!lora_status.rx_done_flag);
	
	SX1276ReadRxPkt();
	SX1276SetSleep();
}


#define RX_MODULE
#define FREQ_OF_TEST  433000000      ///发射频点,取值470000000 - 510000000
#define PWR_OF_TEST   20             ///发射功率,取值0 - 20
#define DR_OF_TEST    0              ///发射或接收速率，取值0 - 5,分别为SF12 - SF7

int main(int argc, char *argv[])
{
	signal(SIGINT, Stop); 

	if( SX1276Init() == false)
	{
		pabort("SX1276Init Failed!");
	}
	while(true)
	{
		#ifdef RX_MODULE
		printf("rx pkt,wait...\n");
		LoraRxParaSet(MODEM_FSK, FREQ_OF_TEST,DR_OF_TEST);
		usleep(1000);
		#else
		printf("tx a pkt...\n");
		StackTxParaSet(MODEM_FSK, FREQ_OF_TEST,PWR_OF_TEST,DR_OF_TEST);
		sleep(1);
		#endif
	}

	return 0;
}
