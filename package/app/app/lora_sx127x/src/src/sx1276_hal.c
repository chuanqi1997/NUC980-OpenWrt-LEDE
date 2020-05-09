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
#include "sx1276_hal.h"
#include "sx1276.h"
#include "gpio.h"


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


#define NUC980_PB13	(0x20 + 13)
#define NUC980_PB8	(0x20 + 8)
#define SX127X_RESET_PORT    NUC980_PB13
#define SX127X_INT_PORT      NUC980_PB8


static const char *device = "/dev/spidev2.0";
static uint32_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 10000000;
static uint16_t delay = 0;
static 	int fd = -1;


static pthread_t sx1276_thread;

static int sx1276_int_poll(unsigned int gpio);

LORA_STATUS_T lora_status;

extern void SX1276OnDio0Irq( void );

void * sx1276_irq_thread()
{
	while(true)
	{
		DEBUG_PRINTF("%s\n", __FUNCTION__);
		sx1276_int_poll(SX127X_INT_PORT);
	}
}


static void pabort(const char *s)
{
	perror(s);
	abort();
}

/**SX1278多字节，连续读写**/
void spi_write_read_dis( uint8_t *tx, uint8_t *rx, uint8_t len )
{
	int ret;
	//DEBUG_PRINTF("spi_write_read_dis len:%d \n", len);
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	if (mode & SPI_TX_QUAD)
		tr.tx_nbits = 4;
	else if (mode & SPI_TX_DUAL)
		tr.tx_nbits = 2;
	if (mode & SPI_RX_QUAD)
		tr.rx_nbits = 4;
	else if (mode & SPI_RX_DUAL)
		tr.rx_nbits = 2;
	if (!(mode & SPI_LOOP)) {
		if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
			tr.rx_buf = 0;
		else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
			tr.tx_buf = 0;
	}

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");
}

static void spi_transfer(int fd, uint8_t const *tx, uint8_t const *rx, size_t len)
{
	int ret;
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	if (mode & SPI_TX_QUAD)
		tr.tx_nbits = 4;
	else if (mode & SPI_TX_DUAL)
		tr.tx_nbits = 2;
	if (mode & SPI_RX_QUAD)
		tr.rx_nbits = 4;
	else if (mode & SPI_RX_DUAL)
		tr.rx_nbits = 2;
	if (!(mode & SPI_LOOP)) {
		if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
			tr.rx_buf = 0;
		else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
			tr.tx_buf = 0;
	}

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");
}

static int sx1276_int_poll(unsigned int gpio)
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
            DEBUG_PRINTF("%s",buffer);
			if(buffer[0] == '1')
			{
				SX1276OnDio0Irq();
			}
        }
    }
}


/*
usleep(n) //n微秒
Sleep（n）//n毫秒
sleep（n）//n秒
*/

void SX1276DelayMs(int i)
{
	usleep(i * 1000);
}

void MX_SX1276_GPIO_INIT(void)
{
	DEBUG_PRINTF("%s\n", __FUNCTION__);
	if (gpio_is_requested(SX127X_RESET_PORT) != 1) 
	{
		gpio_request(SX127X_RESET_PORT);
		gpio_direction_output(SX127X_RESET_PORT,  1);
	}

	if (gpio_is_requested(SX127X_INT_PORT) != 1) 
	{
		gpio_request(SX127X_INT_PORT);
		gpio_direction_input(SX127X_INT_PORT);
		gpio_direction_edge(SX127X_INT_PORT, "rising");
	}
	/*创建线程*/
	if( pthread_create(&sx1276_thread, NULL, sx1276_irq_thread, NULL) != 0)
	{
		DEBUG_PRINTF("sx1276_irq_thread creat failed!/n");
	}
	if(sx1276_thread)
	{        
		//pthread_join(sx1276_thread,NULL); NOT need block
	}
}

void MX_SX1276_GPIO_DE_INIT(void)
{
	gpio_free(SX127X_RESET_PORT);
	gpio_free(SX127X_INT_PORT);
	DEBUG_PRINTF("%s\n", __FUNCTION__);

}



void SPI_INIT(void)
{
	int ret = 0;
	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE32, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	DEBUG_PRINTF("spi mode: 0x%x\n", mode);
	DEBUG_PRINTF("bits per word: %d\n", bits);
	DEBUG_PRINTF("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
	DEBUG_PRINTF("SPI_INIT OK\n");
	
}
void SPI_DEINIT(void)
{
	close(fd);
	DEBUG_PRINTF("%s\n", __FUNCTION__);
} 




/*SX1276 SPI */
uint8_t MX_SX1276_SPI_WRITE_READ(uint8_t data_in)
{

	uint8_t data_out = 0xFF;
	spi_transfer(fd, &data_in, &data_out, 1);
	//static void spi_transfer_single(int fd, uint8_t tx, uint8_t  rx)
	//spi_transfer_single(fd, data_in, &data_out);
	DEBUG_PRINTF("MX_SX1276_SPI_WRITE_READ: in:%x out:%x \n", data_in, data_out);

	return( data_out );
}

void MX_SX1276_RESET_ON(void)
{
	gpio_set_value(SX127X_RESET_PORT, 0);
}
void MX_SX1276_RESET_OFF(void)
{
	gpio_set_value(SX127X_RESET_PORT, 1);
}

void MX_SX1276_RESET(void)
{
	DEBUG_PRINTF("%s\n", __FUNCTION__);
	MX_SX1276_RESET_ON();
	SX1276DelayMs(10);
	MX_SX1276_RESET_OFF();
}


void MX_SX1276_CS_LOW(void)
{

}
void MX_SX1276_CS_HIGH(void)
{

}

void LoraTxDoneCallback(void)
{
    DEBUG_PRINTF("LoraTxDoneCallback\r\n");
	lora_status.tx_done_flag 		= true;
}

void LoraRxTimeoutCallback(void)
{	
    DEBUG_PRINTF("LoraRxTimeoutCallback\r\n");
	lora_status.rx_timeout_flag 	= true;
}

void LoraRxDoneCallback(void)
{
    DEBUG_PRINTF("LoraRxDoneCallback\r\n");
	lora_status.rx_done_flag  		= true;
}




   
void StartRx(uint32_t rx_freq, uint8_t dr)
{
    uint16_t symb_timeout = 10;  

    lora_status.rx_done_flag = false;

    SX1276SetChannel( rx_freq );
    SX1276SetMaxPayloadLength( MODEM_LORA,255);
    SX1276SetRxConfig( 	MODEM_LORA, 
                0, 
                dr, 
                1, 
                0, 
                8, 
                symb_timeout, 
                false, 
                0, 
                false, 
                0, 
                0, 
                false, ///iq not invert,
                true );///rx continue mode
    SX1276SetRx();	
}



void StartSingleRx(uint32_t rx_freq,uint8_t dr, uint16_t time_out)
{
       
//    DEBUG_PRINTF("%s tx_freq: %d dr: %d time_out: %d\r\n", __FUNCTION__, rx_freq, dr, time_out);
    lora_status.rx_done_flag = false;
    SX1276SetChannel( rx_freq );
    SX1276SetMaxPayloadLength( MODEM_LORA,255);
    SX1276SetRxConfig( 	MODEM_LORA, 
                0, 
                dr, 
                1, 
                0, 
                8, 
                time_out, 
                false, 
                0, 
                false, 
                0, 
                0, 
                false, ///iq not invert,
                false );///rx Single mode

    SX1276SetRx();	
}





void SetTxPacket(uint32_t tx_freq,uint8_t tx_pwr,uint8_t tx_dr, uint8_t*buf, uint8_t len)
{
//    DEBUG_PRINTF("%s tx_freq: %d tx_pwr: %d tx_dr: %d len: %d\r\n", __FUNCTION__, tx_freq, tx_pwr, tx_dr, len);

    lora_status.tx_done_flag = false;
    SX1276SetMaxPayloadLength(MODEM_LORA, len);
    SX1276SetTxConfig(MODEM_LORA,tx_pwr, 
    0,0,
    tx_dr, 1, 
    8, false,    
    true,  0, 
    0, false, 
    RF_SEND_TIME_OUT);

    SX1276SetChannel( tx_freq );

  
/*
  	do
    	{
        Device_State.Device_Current_Channel_RSSI = SX1276ReadChannelRssi( MODEM_LORA, tx_freq);
        DEBUG_PRINTF("Device_Current_Channel_RSSI:%d\r\n", Device_State.Device_Current_Channel_RSSI);
        DEBUG_PRINTF("Device_Channel_Used_RSSI:%d\r\n", Device_State.Device_RF_Parameter.Device_Channel_Used_RSSI);
        if( Device_State.Device_Current_Channel_RSSI < Device_State.Device_RF_Parameter.Device_Channel_Used_RSSI)
        {  
            break;
        }
        else
        {
            srand( SX1276Random() );
            uint32_t time = rand() %100;//Ëæ»úÑÓÊ± 0-100ms
            SX1276DelayMs(time); 
        }
    }while( Device_State.Device_Current_Channel_RSSI > Device_State.Device_RF_Parameter.Device_Channel_Used_RSSI);
*/

//    while(SX1276IsChannelFree( MODEM_LORA, tx_freq, Device_State.Device_Channel_Used_RSSI) == false)
//    {  
//        srand( SX1276Random() );
//        uint32_t time = rand() %100;//Ëæ»úÑÓÊ± 0-100ms
//        HAL_Delay(time);
//    } 
    ///Æô¶¯·¢Éä  
    SX1276Send(buf, len);                            
}


