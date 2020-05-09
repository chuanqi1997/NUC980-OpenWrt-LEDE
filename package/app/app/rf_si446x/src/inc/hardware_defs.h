/*! @file hardware_defs.h
 * @brief This file contains hardware specific definitions defined by the platform.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#ifndef HARDWARE_DEFS_H
#define HARDWARE_DEFS_H

/*-------------------------------------------------------------*/
/*						      Global definitions				                 */
/*-------------------------------------------------------------*/

#if (defined SILABS_PLATFORM_RFSTICK)

/*!* LED state Type definition */
typedef enum
{
  ILLUMINATE,
  EXTINGUISH
} eLEDStates;

#define M_P0_UART_TX 0x10
#define M_P0_UART_RX 0x20

#define M_P0_I2C_SCL 0x40
#define M_P0_I2C_SDA 0x80

#define M_P0_RF_GPIO0 0x01
#define M_P0_RF_GPIO1 0x02
#define M_P0_RF_GPIO2 0x04
#define M_P0_RF_GPIO3 0x08

#define M_P1_SPI1_SCK  0x01
#define M_P1_SPI1_MISO 0x02
#define M_P1_SPI1_MOSI 0x04

#define M_P1_RF_NSEL  0x08
#define M_P1_RF_NIRQ  0x10
#define M_P1_RF_PWRDN 0x20

#define M_P0_PB1  0x01
#define M_P0_PB2  0x02
#define M_P0_PB3  0x04
#define M_P0_PB4  0x08

#define M_P2_LED1 0x01
#define M_P2_LED2 0x02
#define M_P2_LED3 0x04
#define M_P2_LED4 0x08

#define M_P2_SW1_A 0x20
#define M_P2_SW1_B 0x40
#define M_P1_SW1_C 0x80
#define M_P1_SW1_D 0x40

#define M_P2_BZ1  0x10
#define M_P2_C2D  0x80

#define NUMOF_PUSHBUTTONS 0x04

/* Definition of pins that are used by basic HWs, be aware not to have same*/
#define M_LED1 M_P2_LED1
#define M_LED2 M_P2_LED2
#define M_LED3 M_P2_LED3
#define M_LED4 M_P2_LED4

#define M_BZ1 M_P2_BZ1

#define B_P0_UART_TX 4
#define B_P0_UART_RX 5

#define B_P0_RF_GPIO0 0
#define B_P0_RF_GPIO1 1
#define B_P0_RF_GPIO2 2
#define B_P0_RF_GPIO3 3

#define B_P1_SPI1_SCK  0
#define B_P1_SPI1_MISO 1
#define B_P1_SPI1_MOSI 2

#define B_P1_RF_NSEL  3
#define B_P1_RF_NIRQ  4
#define B_P1_RF_PWRDN 5

#define B_P0_PB1  0
#define B_P0_PB2  1
#define B_P0_PB3  2
#define B_P0_PB4  3
#define B_P2_LED1 0
#define B_P2_LED2 1
#define B_P2_LED3 2
#define B_P2_LED4 3

#define B_P2_SW1_A 5
#define B_P2_SW1_B 6
#define B_P1_SW1_C 7
#define B_P1_SW1_D 6

#define B_P2_BZ1  4
#define B_P2_C2D  7

SBIT(RF_GPIO0,   SFR_P0, B_P0_RF_GPIO0);
SBIT(RF_GPIO1,   SFR_P0, B_P0_RF_GPIO1);
SBIT(RF_GPIO2,   SFR_P0, B_P0_RF_GPIO2);
SBIT(RF_GPIO3,   SFR_P0, B_P0_RF_GPIO3);

SBIT(SPI1_SCK,   SFR_P1, B_P1_SPI1_SCK);
SBIT(SPI1_MISO,  SFR_P1, B_P1_SPI1_MISO);
SBIT(SPI1_MOSI,  SFR_P1, B_P1_SPI1_MOSI);

SBIT(RF_NSEL,  SFR_P1, B_P1_RF_NSEL);
SBIT(RF_IRQ,   SFR_P1, B_P1_RF_NIRQ);
SBIT(RF_NIRQ,  SFR_P1, B_P1_RF_NIRQ);
SBIT(RF_PWRDN, SFR_P1, B_P1_RF_PWRDN);

SBIT(PB1,   SFR_P0, B_P0_PB1);
SBIT(PB2,   SFR_P0, B_P0_PB2);
SBIT(PB3,   SFR_P0, B_P0_PB3);
SBIT(PB4,   SFR_P0, B_P0_PB4);
SBIT(LED1,  SFR_P2, B_P2_LED1);
SBIT(LED2,  SFR_P2, B_P2_LED2);
SBIT(LED3,  SFR_P2, B_P2_LED3);
SBIT(LED4,  SFR_P2, B_P2_LED4);

SBIT(SW1_A, SFR_P2, B_P2_SW1_A);
SBIT(SW1_B, SFR_P2, B_P2_SW1_B);
SBIT(SW1_C, SFR_P1, B_P1_SW1_C);
SBIT(SW1_D, SFR_P1, B_P1_SW1_D);

SBIT(BZ1,   SFR_P2, B_P2_BZ1);

#elif (defined SILABS_PLATFORM_LCDBB)

/*!* LED state Type definition */
typedef enum
{
  ILLUMINATE,
  EXTINGUISH
} eLEDStates;

/*! Hardware related definitions */
#define HW_NUMOF_LEDS             4u
#define NUMOF_PUSHBUTTONS         4u

/* UART related port mapping */
#define B_P0_UART_TX 4
#define B_P0_UART_RX 5

#define M_P0_UART_TX (1 << B_P0_UART_TX )
#define M_P0_UART_RX (1 << B_P0_UART_RX )

/*
 * TODO: Check HW what about SPI0 ?
 */
#if 0
/* SPI0 related port mapping */
#define B_P1_SPI0_SCK   5u
#define B_P1_SPI0_MISO  6u
#define B_P1_SPI0_MOSI  7u

#define M_P1_SPI0_SCK   (1 << B_P1_SPI0_SCK )
#define M_P1_SPI0_MISO  (1 << B_P1_SPI0_MISO)
#define M_P1_SPI0_MOSI  (1 << B_P1_SPI0_MOSI)

#define MASK_SPI0_MOSI     M_P1_SPI0_MOSI
#define PIN_SPI0_MOSI      B_P1_SPI0_MOSI
#define PORT_SPI0_MOSI                 P1
#define PXMDOUT_SPI0_MOSI         P1MDOUT
#define PXSKIP_SPI0_MOSI           P1SKIP

#define MASK_SPI0_MISO     M_P1_SPI0_MISO
#define PIN_SPI0_MISO      B_P1_SPI0_MISO
#define PORT_SPI0_MISO                 P1
#define PXMDOUT_SPI0_MISO         P1MDOUT
#define PXSKIP_SPI0_MISO           P1SKIP

#define MASK_SPI0_SCK       M_P1_SPI0_SCK
#define PIN_SPI0_SCK        B_P1_SPI0_SCK
#define PORT_SPI0_SCK                  P1
#define PXMDOUT_SPI0_SCK          P1MDOUT
#define PXSKIP_SPI0_SCK            P1SKIP
#else
#define SILABS_PLATFORM_DISABLE_SPI0
#endif

/* SPI1 related port mapping */
#define B_P1_SPI1_SCK   0u
#define B_P1_SPI1_MISO  1u
#define B_P1_SPI1_MOSI  2u

#define M_P1_SPI1_SCK   (1 << B_P1_SPI1_SCK )
#define M_P1_SPI1_MISO  (1 << B_P1_SPI1_MISO)
#define M_P1_SPI1_MOSI  (1 << B_P1_SPI1_MOSI)

#define MASK_SPI1_MOSI     M_P1_SPI1_MOSI
#define PIN_SPI1_MOSI      B_P1_SPI1_MOSI
#define PORT_SPI1_MOSI                 P1
#define PXMDOUT_SPI1_MOSI         P1MDOUT
#define PXSKIP_SPI1_MOSI           P1SKIP

#define MASK_SPI1_MISO     M_P1_SPI1_MISO
#define PIN_SPI1_MISO      B_P1_SPI1_MISO
#define PORT_SPI1_MISO                 P1
#define PXMDOUT_SPI1_MISO         P1MDOUT
#define PXSKIP_SPI1_MISO           P1SKIP

#define MASK_SPI1_SCK       M_P1_SPI1_SCK
#define PIN_SPI1_SCK        B_P1_SPI1_SCK
#define PORT_SPI1_SCK                  P1
#define PXMDOUT_SPI1_SCK          P1MDOUT
#define PXSKIP_SPI1_SCK            P1SKIP

/* LCD related port mapping */
#define B_P1_LCD_NSEL   6u
#define B_P1_LCD_A0     7u

#define M_P1_LCD_NSEL   (1 << B_P1_LCD_NSEL)
#define M_P1_LCD_A0     (1 << B_P1_LCD_A0)

/* SMBus related port mapping */
#define B_P0_I2C_SDA    6u
#define B_P0_I2C_SCL    7u

#define M_P0_I2C_SCL    (1 << B_P0_I2C_SDA)
#define M_P0_I2C_SDA    (1 << B_P0_I2C_SCL)

/* Push-Button related port mapping */
#define B_P0_PB1        0u
#define B_P0_PB2        1u
#define B_P0_PB3        2u
#define B_P0_PB4        3u

/* LED related port mapping */
#define B_P2_LED1       0u
#define B_P2_LED2       1u
#define B_P2_LED3       2u
#define B_P2_LED4       3u

#define M_P2_LED1       (1 << B_P2_LED1)
#define M_P2_LED2       (1 << B_P2_LED2)
#define M_P2_LED3       (1 << B_P2_LED3)
#define M_P2_LED4       (1 << B_P2_LED4)

#define M_LED1          M_P2_LED1
#define M_LED2          M_P2_LED2
#define M_LED3          M_P2_LED3
#define M_LED4          M_P2_LED4

/* Buzzer related port mapping */
#define B_P2_BZ1        4u

#define M_P2_BZ1        (1 << B_P2_BZ1)

#define M_BZ1           M_P2_BZ1

/* RF related port mapping */
#define B_P1_RF_NSEL    3u
#define B_P1_RF_NIRQ    4u
#define B_P1_RF_PWRDN   5u

#define M_P1_RF_NSEL    (1 << B_P1_RF_NSEL)
#define M_P1_RF_NIRQ    (1 << B_P1_RF_NIRQ)
#define M_P1_RF_PWRDN   (1 << B_P1_RF_PWRDN)

#define B_P0_RF_GPIO0   0u
#define B_P0_RF_GPIO1   1u
#define B_P0_RF_GPIO2   2u
#define B_P0_RF_GPIO3   3u

#define M_P0_RF_GPIO0   (1 << B_P0_RF_GPIO0)
#define M_P0_RF_GPIO1   (1 << B_P0_RF_GPIO1)
#define M_P0_RF_GPIO2   (1 << B_P0_RF_GPIO2)
#define M_P0_RF_GPIO3   (1 << B_P0_RF_GPIO3)


#define PIN_MCU_MOSI      PIN_SPI1_MOSI
#define PORT_MCU_MOSI     PORT_SPI1_MOSI
#define PXMDOUT_MCU_MOSI  PXMDOUT_SPI1_MOSI

#define PIN_MCU_MISO      PIN_SPI1_MISO
#define PORT_MCU_MISO     PORT_SPI1_MISO
#define PXMDOUT_MCU_MISO  PXMDOUT_SPI1_MISO

#define PIN_MCU_SCK       PIN_SPI1_SCK
#define PORT_MCU_SCK      PORT_SPI1_SCK
#define PXMDOUT_MCU_SCK   PXMDOUT_SPI1_SCK

#define PIN_MCU_NSEL        B_P1_RF_NSEL
#define PORT_MCU_NSEL                 P1
#define PXMDOUT_MCU_NSEL         P1MDOUT

#define PIN_MCU_SDA         B_P0_I2C_SDA
#define PORT_MCU_SDA                  P0
#define PXMDOUT_MCU_SDA          P0MDOUT

#define PIN_MCU_SCL         B_P0_I2C_SCL
#define PORT_MCU_SCL                  P0
#define PXMDOUT_MCU_SCL          P0MDOUT

#define PIN_MCU_GPIO0      B_P0_RF_GPIO0
#define PORT_MCU_GPIO0                P0
#define PXMDOUT_MCU_GPIO0        P0MDOUT

#define PIN_MCU_GPIO1      B_P0_RF_GPIO1
#define PORT_MCU_GPIO1                P0
#define PXMDOUT_MCU_GPIO1        P0MDOUT

#define PIN_MCU_GPIO2      B_P0_RF_GPIO2
#define PORT_MCU_GPIO2                P0
#define PXMDOUT_MCU_GPIO2        P0MDOUT

#define PIN_MCU_GPIO3      B_P0_RF_GPIO3
#define PORT_MCU_GPIO3                P0
#define PXMDOUT_MCU_GPIO3        P0MDOUT

#define PIN_MCU_SDN        B_P1_RF_PWRDN
#define PORT_MCU_SDN                  P1
#define PXMDOUT_MCU_SDN          P1MDOUT

#define PIN_MCU_NIRQ        B_P1_RF_NIRQ
#define PORT_MCU_NIRQ                 P1
#define PXMDOUT_MCU_NIRQ         P1MDOUT


SBIT(RF_GPIO0,  SFR_P0, B_P0_RF_GPIO0);
SBIT(RF_GPIO1,  SFR_P0, B_P0_RF_GPIO1);
SBIT(RF_GPIO2,  SFR_P0, B_P0_RF_GPIO2);
SBIT(RF_GPIO3,  SFR_P0, B_P0_RF_GPIO3);

/* SMBus */
SBIT(MCU_SDA,   SFR_P0, B_P0_I2C_SDA);
SBIT(MCU_SCL,   SFR_P0, B_P0_I2C_SCL);

/* LEDs */
SBIT(LED1,      SFR_P2, B_P2_LED1);
SBIT(LED2,      SFR_P2, B_P2_LED2);
SBIT(LED3,      SFR_P2, B_P2_LED3);
SBIT(LED4,      SFR_P2, B_P2_LED4);

#if 0
/* MCU SPI0 */
SBIT(SPI0_SCK,  SFR_P1, B_P1_SPI1_SCK);
SBIT(SPI0_MISO, SFR_P1, B_P1_SPI1_MISO);
SBIT(SPI0_MOSI, SFR_P1, B_P1_SPI1_MOSI);
#endif

/* MCU SPI1 */
SBIT(SPI1_SCK,  SFR_P1, B_P1_SPI1_SCK);
SBIT(SPI1_MISO, SFR_P1, B_P1_SPI1_MISO);
SBIT(SPI1_MOSI, SFR_P1, B_P1_SPI1_MOSI);

/* RF related */
SBIT(RF_NSEL,   SFR_P1, B_P1_RF_NSEL);
SBIT(RF_IRQ,    SFR_P1, B_P1_RF_NIRQ);
SBIT(RF_NIRQ,   SFR_P1, B_P1_RF_NIRQ);
SBIT(RF_PWRDN,  SFR_P1, B_P1_RF_PWRDN);

/* LCD related */
SBIT(LCD_NSEL,  SFR_P1, B_P1_LCD_NSEL);
SBIT(LCD_A0,    SFR_P1, B_P1_LCD_A0);

SBIT(PB1,       SFR_P0, B_P0_PB1);
SBIT(PB2,       SFR_P0, B_P0_PB2);
SBIT(PB3,       SFR_P0, B_P0_PB3);
SBIT(PB4,       SFR_P0, B_P0_PB4);

/* Buzzer */
SBIT(BZ1,       SFR_P2, B_P2_BZ1);

#elif (defined SILABS_PLATFORM_WMB)

/*!* LED state Type definition */
typedef enum
{
  ILLUMINATE,
  EXTINGUISH
} eLEDStates;

/*! Hardware related definitions */
#if (defined SILABS_PLATFORM_WMB912)
#define HW_NUMOF_LEDS             1u
#define NUMOF_PUSHBUTTONS         1u
#else
#define HW_NUMOF_LEDS             4u
#define NUMOF_PUSHBUTTONS         4u
#endif

#define M_P0_UART_TX 0x10
#define M_P0_UART_RX 0x20

#if (defined SILABS_PLATFORM_WMB912)
#define B_P0_SPI0_SCK   6u
//#define B_P1_SPI0_MISO  6u
#define B_P0_SPI0_MOSI  3u

#define M_P0_SPI0_SCK   (1 << B_P0_SPI0_SCK )
//#define M_P1_SPI0_MISO  (1 << B_P1_SPI0_MISO)
#define M_P0_SPI0_MOSI  (1 << B_P0_SPI0_MOSI)

#define MASK_SPI0_MOSI     M_P0_SPI0_MOSI
#define PIN_SPI0_MOSI      B_P0_SPI0_MOSI
#define PORT_SPI0_MOSI                 P0
#define PXMDOUT_SPI0_MOSI         P0MDOUT
#define PXSKIP_SPI0_MOSI           P0SKIP

#if 0
#define MASK_SPI0_MISO     M_P1_SPI0_MISO
#define PIN_SPI0_MISO      B_P1_SPI0_MISO
#define PORT_SPI0_MISO                 P1
#define PXMDOUT_SPI0_MISO         P1MDOUT
#define PXSKIP_SPI0_MISO           P1SKIP
#endif

#define MASK_SPI0_SCK       M_P0_SPI0_SCK
#define PIN_SPI0_SCK        B_P0_SPI0_SCK
#define PORT_SPI0_SCK                  P0
#define PXMDOUT_SPI0_SCK          P0MDOUT
#define PXSKIP_SPI0_SCK            P0SKIP
#else
#define B_P1_SPI0_SCK   5u
#define B_P1_SPI0_MISO  6u
#define B_P1_SPI0_MOSI  7u

#define M_P1_SPI0_SCK   (1 << B_P1_SPI0_SCK )
#define M_P1_SPI0_MISO  (1 << B_P1_SPI0_MISO)
#define M_P1_SPI0_MOSI  (1 << B_P1_SPI0_MOSI)

#define MASK_SPI0_MOSI     M_P1_SPI0_MOSI
#define PIN_SPI0_MOSI      B_P1_SPI0_MOSI
#define PORT_SPI0_MOSI                 P1
#define PXMDOUT_SPI0_MOSI         P1MDOUT
#define PXSKIP_SPI0_MOSI           P1SKIP

#define MASK_SPI0_MISO     M_P1_SPI0_MISO
#define PIN_SPI0_MISO      B_P1_SPI0_MISO
#define PORT_SPI0_MISO                 P1
#define PXMDOUT_SPI0_MISO         P1MDOUT
#define PXSKIP_SPI0_MISO           P1SKIP

#define MASK_SPI0_SCK       M_P1_SPI0_SCK
#define PIN_SPI0_SCK        B_P1_SPI0_SCK
#define PORT_SPI0_SCK                  P1
#define PXMDOUT_SPI0_SCK          P1MDOUT
#define PXSKIP_SPI0_SCK            P1SKIP
#endif

#define B_P1_SPI1_SCK   0u
#define B_P1_SPI1_MISO  1u
#define B_P1_SPI1_MOSI  2u

#define M_P1_SPI1_SCK   (1 << B_P1_SPI1_SCK )
#define M_P1_SPI1_MISO  (1 << B_P1_SPI1_MISO)
#define M_P1_SPI1_MOSI  (1 << B_P1_SPI1_MOSI)

#define MASK_SPI1_MOSI     M_P1_SPI1_MOSI
#define PIN_SPI1_MOSI      B_P1_SPI1_MOSI
#define PORT_SPI1_MOSI                 P1
#define PXMDOUT_SPI1_MOSI         P1MDOUT
#define PXSKIP_SPI1_MOSI           P1SKIP

#define MASK_SPI1_MISO     M_P1_SPI1_MISO
#define PIN_SPI1_MISO      B_P1_SPI1_MISO
#define PORT_SPI1_MISO                 P1
#define PXMDOUT_SPI1_MISO         P1MDOUT
#define PXSKIP_SPI1_MISO           P1SKIP

#define MASK_SPI1_SCK       M_P1_SPI1_SCK
#define PIN_SPI1_SCK        B_P1_SPI1_SCK
#define PORT_SPI1_SCK                  P1
#define PXMDOUT_SPI1_SCK          P1MDOUT
#define PXSKIP_SPI1_SCK            P1SKIP

#if (defined SILABS_PLATFORM_WMB912)
  #define B_P0_LCD_NSEL   2u

  #if ( (defined SILABS_WMCU_SI106x) || (defined SILABS_WMCU_SI108x) )
    #define B_P1_LCD_A0     6u
  #else
    #define B_P0_LCD_A0     7u
  #endif
#else
#define B_P2_LCD_NSEL   0u
#define B_P0_LCD_A0     7u
#endif

#if (defined SILABS_PLATFORM_WMB912)
  #define M_P0_LCD_NSEL   (1 << B_P0_LCD_NSEL)

  #if ( (defined SILABS_WMCU_SI106x) || (defined SILABS_WMCU_SI108x) )
    #define M_P1_LCD_A0     (1 << B_P1_LCD_A0)
  #else
    #define M_P0_LCD_A0     (1 << B_P0_LCD_A0)
  #endif
#else
#define M_P2_LCD_NSEL   (1 << B_P2_LCD_NSEL)
#define M_P0_LCD_A0     (1 << B_P0_LCD_A0)
#endif

#if (defined SILABS_PLATFORM_WMB912)
#define B_P1_I2C_SDA    4u
#define B_P1_I2C_SCL    5u

#define M_P1_I2C_SCL    (1 << B_P1_I2C_SDA)
#define M_P1_I2C_SDA    (1 << B_P1_I2C_SCL)
#else
#define B_P2_I2C_SDA    1u
#define B_P2_I2C_SCL    2u

#define M_P2_I2C_SCL    (1 << B_P2_I2C_SDA)
#define M_P2_I2C_SDA    (1 << B_P2_I2C_SCL)
#endif

#define B_P0_PB1        0u
#if !(defined SILABS_PLATFORM_WMB912)
#define B_P0_PB2        6u
#define B_P0_PB3        2u
#define B_P0_PB4        3u
#endif

#define B_P0_LED1       0u
#if !(defined SILABS_PLATFORM_WMB912)
#define B_P0_LED2       6u
#define B_P0_LED3       2u
#define B_P0_LED4       3u
#endif

#define M_P0_LED1       (1 << B_P0_LED1)
#if !(defined SILABS_PLATFORM_WMB912)
#define M_P0_LED2       (1 << B_P0_LED2)
#define M_P0_LED3       (1 << B_P0_LED3)
#define M_P0_LED4       (1 << B_P0_LED4)
#endif

#define M_LED1          M_P0_LED1
#if !(defined SILABS_PLATFORM_WMB912)
#define M_LED2          M_P0_LED2
#define M_LED3          M_P0_LED3
#define M_LED4          M_P0_LED4

#define B_P2_BZ1        6u
#endif

#if (defined SILABS_PLATFORM_WMB912)
  #define B_P1_RF_NSEL    3u

  #if ( (defined SILABS_WMCU_SI106x) || (defined SILABS_WMCU_SI108x) )
    #define B_P0_RF_PWRDN   7u
  #else
    #define B_P1_RF_PWRDN   6u
  #endif
#else
  #define B_P1_RF_NSEL    4u
  #define B_P2_RF_PWRDN   3u
#endif

#define B_P0_RF_NIRQ    1u

#define M_P1_RF_NSEL    (1 << B_P1_RF_NSEL)
#if (defined SILABS_PLATFORM_WMB912)
  #if ( (defined SILABS_WMCU_SI106x) || (defined SILABS_WMCU_SI108x) )
    #define M_P0_RF_PWRDN   (1 << B_P0_RF_PWRDN)
  #else
    #define M_P1_RF_PWRDN   (1 << B_P1_RF_PWRDN)
  #endif
#else
#define M_P2_RF_PWRDN   (1 << B_P2_RF_PWRDN)
#endif
#define M_P0_RF_NIRQ    (1 << B_P0_RF_NIRQ)

/* In WMB RF_GPIO0 is not connected to the MCU by default,
 * the input pin is interchangeable with the buzzer.
 * It is defined here for consistency with DCP_Module.
 */
#if (defined SILABS_PLATFORM_WMB912)
#define B_P0_RF_GPIO0   0u
#define B_P0_RF_GPIO1   6u
#define B_P0_RF_GPIO2   2u
#define B_P0_RF_GPIO3   3u

#define M_P0_RF_GPIO0   (1 << B_P0_RF_GPIO0)
#define M_P0_RF_GPIO1   (1 << B_P0_RF_GPIO1)
#define M_P0_RF_GPIO2   (1 << B_P0_RF_GPIO2)
#define M_P0_RF_GPIO3   (1 << B_P0_RF_GPIO3)
#else
#define B_P2_RF_GPIO0   6u
#define B_P1_RF_GPIO1   3u
#define B_P2_RF_GPIO2   5u
#define B_P2_RF_GPIO3   4u

#define M_P2_RF_GPIO0   (1 << B_P2_RF_GPIO0)
#define M_P1_RF_GPIO1   (1 << B_P1_RF_GPIO1)
#define M_P2_RF_GPIO2   (1 << B_P2_RF_GPIO2)
#define M_P2_RF_GPIO3   (1 << B_P2_RF_GPIO3)
#endif

#define PIN_MCU_MOSI      PIN_SPI1_MOSI
#define PORT_MCU_MOSI     PORT_SPI1_MOSI
#define PXMDOUT_MCU_MOSI  PXMDOUT_SPI1_MOSI

#define PIN_MCU_MISO      PIN_SPI1_MISO
#define PORT_MCU_MISO     PORT_SPI1_MISO
#define PXMDOUT_MCU_MISO  PXMDOUT_SPI1_MISO

#define PIN_MCU_SCK       PIN_SPI1_SCK
#define PORT_MCU_SCK      PORT_SPI1_SCK
#define PXMDOUT_MCU_SCK   PXMDOUT_SPI1_SCK

#define PIN_MCU_NSEL        B_P1_RF_NSEL
#define PORT_MCU_NSEL                 P1
#define PXMDOUT_MCU_NSEL         P1MDOUT

#if (defined SILABS_PLATFORM_WMB912)
#define PIN_MCU_SDA         B_P1_I2C_SDA
#define PORT_MCU_SDA                  P1
#define PXMDOUT_MCU_SDA          P1MDOUT

#define PIN_MCU_SCL         B_P1_I2C_SCL
#define PORT_MCU_SCL                  P1
#define PXMDOUT_MCU_SCL          P1MDOUT
#else
#define PIN_MCU_SDA         B_P2_I2C_SDA
#define PORT_MCU_SDA                  P2
#define PXMDOUT_MCU_SDA          P2MDOUT

#define PIN_MCU_SCL         B_P2_I2C_SCL
#define PORT_MCU_SCL                  P2
#define PXMDOUT_MCU_SCL          P2MDOUT
#endif

/* In WMB RF_GPIO0 is not connected to the MCU by default,
 * the input pin is interchangeable with the buzzer.
 * It is defined here for consistency with DCP_Module.
 */
#if (defined SILABS_PLATFORM_WMB912)
#define PIN_MCU_GPIO0      B_P0_RF_GPIO0
#define PORT_MCU_GPIO0                P0
#define PXMDOUT_MCU_GPIO0        P0MDOUT

#define PIN_MCU_GPIO1      B_P0_RF_GPIO1
#define PORT_MCU_GPIO1                P0
#define PXMDOUT_MCU_GPIO1        P0MDOUT

#define PIN_MCU_GPIO2      B_P0_RF_GPIO0
#define PORT_MCU_GPIO2                P0
#define PXMDOUT_MCU_GPIO2        P0MDOUT

#define PIN_MCU_GPIO3      B_P0_RF_GPIO3
#define PORT_MCU_GPIO3                P0
#define PXMDOUT_MCU_GPIO3        P0MDOUT

#if ( (defined SILABS_WMCU_SI106x) || (defined SILABS_WMCU_SI108x) )
  #define PIN_MCU_SDN        B_P0_RF_PWRDN
  #define PORT_MCU_SDN                  P0
  #define PXMDOUT_MCU_SDN          P0MDOUT
#else
  #define PIN_MCU_SDN        B_P1_RF_PWRDN
  #define PORT_MCU_SDN                  P1
  #define PXMDOUT_MCU_SDN          P1MDOUT
#endif

#else
#define PIN_MCU_GPIO0      B_P2_RF_GPIO0
#define PORT_MCU_GPIO0                P2
#define PXMDOUT_MCU_GPIO0        P2MDOUT

#define PIN_MCU_GPIO1      B_P1_RF_GPIO1
#define PORT_MCU_GPIO1                P1
#define PXMDOUT_MCU_GPIO1        P1MDOUT

#define PIN_MCU_GPIO2      B_P2_RF_GPIO2
#define PORT_MCU_GPIO2                P2
#define PXMDOUT_MCU_GPIO2        P2MDOUT

#define PIN_MCU_GPIO3      B_P2_RF_GPIO3
#define PORT_MCU_GPIO3                P2
#define PXMDOUT_MCU_GPIO3        P2MDOUT

#define PIN_MCU_SDN        B_P2_RF_PWRDN
#define PORT_MCU_SDN                  P2
#define PXMDOUT_MCU_SDN          P2MDOUT
#endif

#define PIN_MCU_NIRQ        B_P0_RF_NIRQ
#define PORT_MCU_NIRQ                 P0
#define PXMDOUT_MCU_NIRQ         P0MDOUT


#if (defined SILABS_PLATFORM_WMB912)
//SBIT(RF_GPIO0,  SFR_P0, B_P0_RF_GPIO0);
SBIT(RF_GPIO1,  SFR_P0, B_P0_RF_GPIO1);
SBIT(RF_GPIO2,  SFR_P0, B_P0_RF_GPIO2);
SBIT(RF_GPIO3,  SFR_P0, B_P0_RF_GPIO3);
#else
//SBIT(RF_GPIO0,  SFR_P2, B_P2_RF_GPIO0);
SBIT(RF_GPIO1,  SFR_P1, B_P1_RF_GPIO1);
SBIT(RF_GPIO2,  SFR_P2, B_P2_RF_GPIO2);
SBIT(RF_GPIO3,  SFR_P2, B_P2_RF_GPIO3);
#endif

/* SMBus */
#if (defined SILABS_PLATFORM_WMB912)
SBIT(MCU_SDA,   SFR_P1, B_P1_I2C_SDA);
SBIT(MCU_SCL,   SFR_P1, B_P1_I2C_SCL);
#else
SBIT(MCU_SDA,   SFR_P2, B_P2_I2C_SDA);
SBIT(MCU_SCL,   SFR_P2, B_P2_I2C_SCL);
#endif

/* LEDs */
SBIT(LED1,      SFR_P0, B_P0_LED1);
#if !(defined SILABS_PLATFORM_WMB912)
SBIT(LED2,      SFR_P0, B_P0_LED2);
SBIT(LED3,      SFR_P0, B_P0_LED3);
SBIT(LED4,      SFR_P0, B_P0_LED4);
#endif

/* MCU SPI0 */
#if (defined SILABS_PLATFORM_WMB912)
SBIT(SPI0_SCK,  SFR_P0, B_P0_SPI0_SCK);
//SBIT(SPI0_MISO, SFR_P1, B_P1_SPI1_MISO);
SBIT(SPI0_MOSI, SFR_P0, B_P0_SPI0_MOSI);
#else
SBIT(SPI0_SCK,  SFR_P1, B_P1_SPI1_SCK);
SBIT(SPI0_MISO, SFR_P1, B_P1_SPI1_MISO);
SBIT(SPI0_MOSI, SFR_P1, B_P1_SPI1_MOSI);
#endif

/* MCU SPI1 */
SBIT(SPI1_SCK,  SFR_P1, B_P1_SPI1_SCK);
SBIT(SPI1_MISO, SFR_P1, B_P1_SPI1_MISO);
SBIT(SPI1_MOSI, SFR_P1, B_P1_SPI1_MOSI);

/* RF related */
#if (defined SILABS_PLATFORM_WMB912)
  #if ( (defined SILABS_WMCU_SI106x) || (defined SILABS_WMCU_SI108x) )
    SBIT(RF_PWRDN,  SFR_P0, B_P0_RF_PWRDN);
  #else
    SBIT(RF_PWRDN,  SFR_P1, B_P1_RF_PWRDN);
  #endif
#else
SBIT(RF_PWRDN,  SFR_P2, B_P2_RF_PWRDN);
#endif
SBIT(RF_NSEL,   SFR_P1, B_P1_RF_NSEL);
SBIT(RF_IRQ,    SFR_P0, B_P0_RF_NIRQ);
SBIT(RF_NIRQ,   SFR_P0, B_P0_RF_NIRQ);

/* LCD related */
#if (defined SILABS_PLATFORM_WMB912)
  SBIT(LCD_NSEL,  SFR_P0, B_P0_LCD_NSEL);

  #if ( (defined SILABS_WMCU_SI106x) || (defined SILABS_WMCU_SI108x) )
    SBIT(LCD_A0,    SFR_P1, B_P1_LCD_A0);
  #else
    SBIT(LCD_A0,    SFR_P0, B_P0_LCD_A0);
  #endif
#else
SBIT(LCD_NSEL,  SFR_P2, B_P2_LCD_NSEL);
SBIT(LCD_A0,    SFR_P0, B_P0_LCD_A0);
#endif

SBIT(PB1,       SFR_P0, B_P0_PB1);
#if !(defined SILABS_PLATFORM_WMB912)
SBIT(PB2,       SFR_P0, B_P0_PB2);
SBIT(PB3,       SFR_P0, B_P0_PB3);
SBIT(PB4,       SFR_P0, B_P0_PB4);

/* Buzzer */
SBIT(BZ1,       SFR_P2, B_P2_BZ1);
#endif

#else
//#error Other platforms are not supported yet!
#endif

#endif //HARDWARE_DEFS_H
