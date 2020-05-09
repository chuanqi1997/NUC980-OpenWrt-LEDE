#ifndef _BOARD_H_
#define _BOARD_H_

#include "sys.h"
#include <stdint.h>
#include <stdbool.h>

typedef  uint8_t U8;
typedef  unsigned char INT8U;
typedef  unsigned short INT16U;
typedef  unsigned int INT32U;



//#define SI446x_TEST


/*Exchange a byte via the SPI bus*/
INT8U SPI_ExchangeByte( INT8U input );
/*Initialize the SI446x module */
void SI446x_Initial( void );
/*Initialize the SPI bus*/
void SPI_Initial( void );
/*Initialize the other GPIOs of the board*/
void GPIO_Initial( void ); 
void SI446x_DeInitial( void );
void SI_SDN_HIGH(void);
void SI_SDN_LOW( void );

#define DEBUG_PRINTF printf
#endif //_BOARD_H_
/*
=================================================================================
------------------------------------End of FILE----------------------------------
=================================================================================
*/
