#ifndef __SX1276_HAL_H__
#define __SX1276_HAL_H__


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define DEBUG_PRINTF printf

/****************************************默认射频参数定义****************************************/
#define MANAGE_CHANNEL   0//管理信道
#define RF_RX_WINDOWS_MS     800 //定义接收窗口的大小  单位MS
#define RF_SEND_TIME_OUT     400//发送超时时间MS
#define RF_TX_PWR     20             ///发射功率,取值0 - 20
#define RF_TX_RX_DR         5              ///发射或接收速率，取值0 - 5,分别为SF12 - SF7 
#define RF_CHANNEL_USED_RSSI   -70  //如果当前信道的RSSI的值大于这个值则认为信道被占用
/****************************************默认射频参数定义****************************************/


typedef struct
{
    uint8_t tx_done_flag;
    uint8_t rx_done_flag;
    uint8_t rx_timeout_flag;
}LORA_STATUS_T;

void spi_write_read_dis( uint8_t *tx, uint8_t *rx, uint8_t len );
void		SPI_INIT(void);
void        SPI_DEINIT(void);
void 		SX1276DelayMs(int i);
uint8_t 	MX_SX1276_SPI_WRITE_READ(uint8_t data_in);
void 		MX_SPI_DEINIT(void);
void    	MX_SPI_INIT(void);
void 		MX_SX1276_RESET(void);
void 		MX_SX1276_CS_LOW(void);
void 		MX_SX1276_CS_HIGH(void);
void 		MX_SX1276_GPIO_INIT(void);
void MX_SX1276_RESET_ON(void);
void MX_SX1276_RESET_OFF(void);
void MX_SX1276_RESET_DEINIT(void);
void MX_SX1276_RESET_INIT(void);
void MX_SX1276_GPIO_DE_INIT(void);


void StartSingleRx(uint32_t rx_freq,uint8_t dr, uint16_t time_out);
void SetTxPacket(uint32_t tx_freq,uint8_t tx_pwr,uint8_t tx_dr, uint8_t*buf, uint8_t len);
void StartRx(uint32_t rx_freq,uint8_t dr);
void 		LoraTxDoneCallback(void);
void 		LoraRxTimeoutCallback(void);
void 		LoraRxDoneCallback(void);


extern LORA_STATUS_T				lora_status;






#endif


