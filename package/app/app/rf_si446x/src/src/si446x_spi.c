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

#include "si446x_spi.h"
#include "si446x.h"




#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


static const char *device = "/dev/spidev2.1";
static uint32_t mode = 0;
static uint8_t bits = 8;
static uint32_t speed = 1000000 * 1;
static uint16_t delay = 0;
static 	int fd = -1;

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


//static function interface define
static uint8_t SPI1_ReadWriteByte(uint8_t TxData);

static void SPI_Init(void);

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */  
static void SPI_DeInit(void)
{
	close(fd);
	DEBUG_PRINTF("%s\n", __FUNCTION__);
}

    /**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */  
static void SPI_Init(void)
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
	DEBUG_PRINTF("spi device: %s\n", device);
	DEBUG_PRINTF("spi mode: 0x%x\n", mode);
	DEBUG_PRINTF("bits per word: %d\n", bits);
	DEBUG_PRINTF("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
	DEBUG_PRINTF("SPI_INIT OK\n");
	
}



//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
static uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{
/**SX1278多字节，连续读写**/
//void spi_write_read_dis( uint8_t *tx, uint8_t *rx, uint8_t len )
	uint8_t Rxdata ;
	spi_write_read_dis(&TxData, &Rxdata, 1);
	return Rxdata;          		    //返回收到的数据			
}


/*
SI 4463 初始化 接口 2018-5-29 09:27:23 
*/
void SI446x_SPI_Init(void)
{
	SPI_Init();
}

void SI446x_SPI_DeInit(void)
{
	SPI_DeInit();
}

/*
2018-5-29 09:27:37
SI4463 SPI1 读写 函数
*/
uint8_t SI446x_SPI_ReadWriteByte(uint8_t TxData)
{
	return SPI1_ReadWriteByte(TxData);
}

void SI446x_SPI_WriteByte(uint8_t TxData)
{
	//HAL_SPI_Transmit(&hspi1, &TxData, 1, 1000);
}

void SI446x_SPI_ReadRegs(uint8_t * read, int cnt)
{
//void spi_write_read_dis( uint8_t *tx, uint8_t *rx, uint8_t len )
	int tx[cnt];
	for(int i=0; i<cnt; i++)
		tx[i] = 0xFF;
	spi_write_read_dis(tx, read, cnt);
}

void SI446x_SPI_WriteRegs(uint8_t * write, int cnt)
{
//void spi_write_read_dis( uint8_t *tx, uint8_t *rx, uint8_t len )
	int rx[cnt];
	for(int i=0; i<cnt; i++)
		rx[i] = 0;
	spi_write_read_dis(write, rx, cnt);
}

void SI446x_SPI_WriteReadRegs(uint8_t * write, uint8_t * read, int cnt)
{
	spi_write_read_dis(write, read, cnt);
}




uint8_t SI446x_SPI_ReadByte(void)
{
	uint8_t Rxdata = 0;
	//HAL_SPI_Receive(&hspi1, &Rxdata, 1, 1000);
	return Rxdata;
}


