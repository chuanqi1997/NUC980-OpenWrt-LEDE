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
#include <poll.h>

#include "nuc980_gpio.h"
#include "si446x_spi.h"
#include "si446x.h"
#include "board.h"
#include "gpio.h"


#define SI446X_RESET_PORT    NUC980_PC5
#define SI446X_INT_PORT      NUC980_PC4



static pthread_t si446x_thread;
static int si446x_int_poll(unsigned int gpio);


void * si446x_irq_thread()
{
	while(true)
	{
		DEBUG_PRINTF("%s\n", __FUNCTION__);
		si446x_int_poll(SI446X_INT_PORT);
	}
}

/*
=================================================================================
SPI_ExchangeByte( );
Function : Exchange a byte via the SPI bus
INTPUT   : input, The input byte
OUTPUT   : The output byte from SPI bus
=================================================================================
*/
INT8U SPI_ExchangeByte( INT8U input )
{
	return SI446x_SPI_ReadWriteByte(input);
}


/*
=================================================================================
SPI_Initial( );
Function : Initialize the SPI bus
INTPUT   : None
OUTPUT   : None
=================================================================================
*/
void SPI_Initial( void )
{
    SI446x_SPI_Init();
}

/*
=================================================================================
SPI_DeInitial( );
Function : Initialize the SPI bus
INTPUT   : None
OUTPUT   : None
=================================================================================
*/
void SPI_DeInitial( void )
{
    SI446x_SPI_DeInit();
}

/*
=================================================================================
nIRQ_EXTIX_Init( );
Function : Initialize the nIRQ PIN
INTPUT   : None
OUTPUT   : None
=================================================================================
*/
void nIRQ_EXTIX_Init(void)
{
	if (gpio_is_requested(SI446X_INT_PORT) != 1) 
	{
		gpio_request(SI446X_INT_PORT);
		gpio_direction_input(SI446X_INT_PORT);
		//rising ±íÊŸÒýœÅÎªÖÐ¶ÏÊäÈë£¬ÉÏÉýÑØŽ¥·¢
		//falling ±íÊŸÒýœÅÎªÖÐ¶ÏÊäÈë£¬ÏÂœµÑØŽ¥·¢
		//both ±íÊŸÒýœÅÎªÖÐ¶ÏÊäÈë£¬±ßÑØŽ¥·¢
		gpio_direction_edge(SI446X_INT_PORT, "falling");
	}
	/*ŽŽœšÏß³Ì*/
	if( pthread_create(&si446x_thread, NULL, si446x_irq_thread, NULL) != 0)
	{
		DEBUG_PRINTF("si446x_irq_thread creat failed!/n");
	}
	DEBUG_PRINTF("%s\n", __FUNCTION__);
}

/*
=================================================================================
nIRQ_EXTIX_DeInit( );
Function : DeInitialize the nIRQ PIN
INTPUT   : None
OUTPUT   : None
=================================================================================
*/
void nIRQ_EXTIX_DeInit(void)
{
	gpio_free(SI446X_INT_PORT);
	DEBUG_PRINTF("%s\n", __FUNCTION__);
}


/*
=================================================================================
GPIO_Initial( );
Function : Initialize the other GPIOs of the board
INTPUT   : None
OUTPUT   : None
=================================================================================
*/
void GPIO_Initial( void )
{
	DEBUG_PRINTF("%s\n", __FUNCTION__);
	if (gpio_is_requested(SI446X_RESET_PORT) != 1) 
	{
		gpio_request(SI446X_RESET_PORT);
		gpio_direction_output(SI446X_RESET_PORT,  1);
	}
}


/*
=================================================================================
SI_SDN_HIGH( );
Function : set the sdn pin high level
INTPUT   : None
OUTPUT   : None
=================================================================================
*/
void SI_SDN_HIGH( void )
{
	DEBUG_PRINTF("%s\n", __FUNCTION__);
	gpio_set_value(SI446X_RESET_PORT, 1);
}

/*
=================================================================================
SI_SDN_LOW( );
Function : set the sdn pin low level
INTPUT   : None
OUTPUT   : None
=================================================================================
*/
void SI_SDN_LOW( void )
{
	DEBUG_PRINTF("%s\n", __FUNCTION__);
	gpio_set_value(SI446X_RESET_PORT, 0);

}
/*
=================================================================================
GPIO_DeInitial( );
Function : Initialize the other GPIOs of the board
INTPUT   : None
OUTPUT   : None
=================================================================================
*/
void GPIO_DeInitial( void )
{
	gpio_free(SI446X_RESET_PORT);
	DEBUG_PRINTF("%s\n", __FUNCTION__);

}

#define PACKAGE_LENGTH 64
static u8 si446x_cmd_buf[PACKAGE_LENGTH];

/*
=================================================================================
SI446x_Initial( );
Function : Initialize the SI446x module
INTPUT   : None
OUTPUT   : None
DATA     : 2018-5-29 18:59:32
CALL     : board.c  rt_hw_board_init 
=================================================================================
*/
void SI446x_Initial( void )
{
	
	SPI_Initial();// SI446X SPI ³õÊŒ»¯ 
	GPIO_Initial();//SI446x  GPIO ³õÊŒ»¯
	SI446X_RESET( );        //SI446X Ä£¿éžŽÎ»
	SI446X_CONFIG_INIT( );  //SI446X Ä£¿é³õÊŒ»¯ÅäÖÃº¯Êý
	
	SI446X_INT_STATUS( si446x_cmd_buf );   	//Çå³ýÖÐ¶Ï×ŽÌ¬  
	DEBUG_PRINTF("%s\n", __FUNCTION__);
	for(int i=0; i<9; i++)
	{
		DEBUG_PRINTF(" %#x ", si446x_cmd_buf[i]);
	}
	DEBUG_PRINTF("\n");
	//SI446X_EN_INT
	//open the si446x receive mode 2018-6-17 00:40:29
	SI446X_START_RX( 0, 0, PACKET_LENGTH,0,0,3 );
	nIRQ_EXTIX_Init();//SI446x
}

/*
=================================================================================
SI446x_DeInitial( );
Function : Initialize the SI446x module
INTPUT   : None
OUTPUT   : None
DATA     : 2020-3-23 00:31:39
CALL     : board.c 
=================================================================================
*/
void SI446x_DeInitial( void )
{
	GPIO_DeInitial();
	SPI_DeInitial();
	nIRQ_EXTIX_DeInit();
}

static void si446x_send_frame(uint8_t *pBuf ,uint8_t size, uint8_t channel)
{
	uint8_t SI4463_BUF[64] = {0};
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
		usleep(1000 * 10);
	}while( !( SI4463_BUF[3] & ( 1<<5 ))); //等待发射完成（发射中断产生）

	SI446X_CONFIG_INIT();
	SI446X_START_RX(channel, 0, PACKET_LENGTH, 0, 0, 3);  // 进入接收模式  
	SI446X_GPIO_CONFIG( 0, 0, 0x21, 0x20, 0, 0, 0 );//接收
	DEBUG_PRINTF("SI446X send ok!\r\n");
}
void SI446x_GPIO_EXTI_Callback(void)
{
	uint8_t length = 0;
	uint8_t read_buf[64] = {0};
	SI446X_INT_STATUS( read_buf );    //读状态寄存器
	if( read_buf[3] & ( 1<<4 ) )     //接收中断产生 收到有效数据包
	{
		length = SI446X_READ_PACKET(read_buf);//获取接收到的字节数
		if(length > 64 || length == 0)
			DEBUG_PRINTF("SI446X_READ_PACKET:error!");
		else
		{
			for(int i=0; i<length; i++)
				DEBUG_PRINTF("%c ", read_buf[i]);
		
		}
		DEBUG_PRINTF("\r\nSI446X_READ_PACKET:%d %s\n",length, read_buf);

		//si446x_send_frame(read_buf, length, 0);

		SI446X_CONFIG_INIT();
		SI446X_START_RX(0, 0, PACKET_LENGTH, 0, 0, 3);  // 进入接收模式  
		SI446X_GPIO_CONFIG( 0, 0, 0x21, 0x20, 0, 0, 0 );//接收
	}
	else if( read_buf[3] & ( 1<<5 ) )
	{
		DEBUG_PRINTF("SI446X_SEND_PACKET:OK!\n");
		SI446X_CONFIG_INIT();
		SI446X_START_RX(0, 0, PACKET_LENGTH, 0, 0, 3);  // 进入接收模式  
		SI446X_GPIO_CONFIG( 0, 0, 0x21, 0x20, 0, 0, 0 );//接收
	}
}


static int si446x_int_poll(unsigned int gpio)
{
	char pathname[255];
	char buffer;
	snprintf(pathname, sizeof(pathname), GPIO_VALUE, gpio);
    int fd=open(pathname,O_RDONLY);
    if(fd<0)
    {
        perror("open failed!\n");  
        return -1;
    }
    struct pollfd fds[1];
    fds[0].fd=fd;
    fds[0].events=POLLPRI;
    while(1)
    {
        if(poll(fds,1,0)==-1)
        {
            perror("poll failed!\n");
            return -1;
        }
        if(fds[0].revents & POLLPRI)
        {
            if(lseek(fd,0,SEEK_SET)==-1)
            {
                perror("lseek failed!\n");
                return -1;
            }
			char buffer[16];
			int len;
            if((len=read(fd,buffer,sizeof(buffer)))==-1)
            {
                perror("read failed!\n");
                return -1;
            }
            buffer[len]=0;
			DEBUG_PRINTF("%s:%s\n", __FUNCTION__, buffer);
			if(buffer[0] == '0')
			{
				SI446x_GPIO_EXTI_Callback();
			}
		}
    }
}

/*
=================================================================================
------------------------------------End of FILE----------------------------------
=================================================================================
*/
