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

#include "si446x.h"
#include "board.h"

uint8_t SI4463_BUF[64];    

static void pabort(const char *s)
{
	perror(s);
	abort();
}

void Stop(int signo) 
{
	SI446x_DeInitial();
	printf("SIGINT! STOP!\n");
	_exit(0);
}

void si446x_send_frame(uint8_t *pBuf ,uint8_t size, uint8_t channel)
{
	DEBUG_PRINTF("%s size:%d\r\n", __FUNCTION__, size);
	
	SI446X_GPIO_CONFIG( 0, 0, 0x20, 0x21, 0, 0, 0 );//发送 201.10.26
	   
	SI446X_SEND_PACKET( pBuf, size, channel, 0 ); //发射数据包
	do
	{           
		SI446X_INT_STATUS( SI4463_BUF );          //读状态寄存器
		for(int i=0; i<4; i++)
		{
			DEBUG_PRINTF(" %#x ",SI4463_BUF[i] );
		}
		DEBUG_PRINTF("\r\n");
		usleep(1000 * 100);
	}while( !( SI4463_BUF[3] & ( 1<<5 ) ) ); //等待发射完成（发射中断产生）
	SI446X_CONFIG_INIT();
	SI446X_START_RX(channel, 0, PACKET_LENGTH, 0, 0, 3);  // 进入接收模式  
	SI446X_GPIO_CONFIG( 0, 0, 0x21, 0x20, 0, 0, 0 );//接收
	DEBUG_PRINTF("SI446X send ok!\r\n");
}

#define SI446x_TEST
#define SI446X_RX 
int main(int argc, char *argv[])
{
	const uint8_t * send = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	signal(SIGINT, Stop); 
	SI446x_Initial();
	//SI446X_START_RX( 0, 0, PACKET_LENGTH, 0, 0, 3 );

	#ifdef SI446x_TEST 
	DEBUG_PRINTF("\r\nSI446x_TEST\r\n");
	//SI446X_SEND_PACKET( send, 10,0, 0 ); //发射数据包

	#else
	/* SI446x start rx */
	SI446X_START_RX( 0, 0, PACKET_LENGTH, 0, 0, 3 );
	#endif

	while(true)
	{
		printf("run..\n");
		#ifdef SI446x_TEST 
		DEBUG_PRINTF("\r\nSI446x_TEST\r\n");
		//si446x_send_frame(send, strlen(send), 0);
		SI446X_SEND_PACKET( send, strlen(send),0, 0 ); //发射数据包
		#else
		/* SI446x start rx */
		//SI446X_START_RX( 0, 0, PACKET_LENGTH, 0, 0, 3 );
		#endif
/*
		#ifndef SI446X_RX

		#else
			SI446X_CONFIG_INIT();
			SI446X_START_RX(0, 0, PACKET_LENGTH, 0, 0, 3);  // 进入接收模式  
			SI446X_GPIO_CONFIG( 0, 0, 0x21, 0x20, 0, 0, 0 );//接收
		#endif

*/
	
		sleep(1);
	}
	
	return 0;
}
