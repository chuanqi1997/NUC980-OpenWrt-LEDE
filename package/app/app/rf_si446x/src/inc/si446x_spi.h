#ifndef __SI446X_SPI_H__
#define __SI446X_SPI_H__



void SI446x_SPI_Init(void);  					
uint8_t SI446x_SPI_ReadWriteByte(uint8_t TxData);	 
void SI446x_SPI_SetSpeed(uint8_t SPI_BaudRatePrescaler);	
void SI446x_SPI_WriteByte(uint8_t TxData);
uint8_t SI446x_SPI_ReadByte(void);
void SI446x_SPI_DeInit(void);

void SI446x_SPI_ReadRegs(uint8_t * read, int cnt);
void SI446x_SPI_WriteRegs(uint8_t * write, int cnt);

void SI446x_SPI_WriteReadRegs(uint8_t * write, uint8_t * read, int cnt);


#endif

