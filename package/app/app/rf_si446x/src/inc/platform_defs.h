/*! @file platform_defs.h
 * @brief This file contains platform specific definitions.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#ifndef PLATFORM_DEFS_H_
#define PLATFORM_DEFS_H_


/*---------------------------------------------------------------------*/
/*            Platform specific global definitions                     */
/*---------------------------------------------------------------------*/

#ifdef SILABS_PLATFORM_RFSTICK

  #define SILABS_RFSTICK_4455HB
  #define SILABS_MCU_F930

  #ifdef SILABS_RFSTICK_4455HB
    #define SILABS_RADIO_SI4455
    #undef SILABS_RADIO_SI446X
  #else
    #error Check and define the type of your RFstick & radio.
  #endif

  #define SILABS_PLATFORM_COMPONENT_LED     4
  #define SILABS_PLATFORM_COMPONENT_PB      4
  #define SILABS_PLATFORM_COMPONENT_SWITCH  4
  #define SILABS_PLATFORM_COMPONENT_BUZZER  1

#elif (defined SILABS_PLATFORM_WMB930)
  #define SILABS_PLATFORM_WMB
  #define SILABS_RADIO_SI446X
  #undef  SILABS_RADIO_SI4455
  #define SILABS_MCU_F930
  #define SILABS_LCD_DOG_GLCD

  #define SILABS_PLATFORM_COMPONENT_LED     4
  #define SILABS_PLATFORM_COMPONENT_PB      4
//  #define SILABS_PLATFORM_COMPONENT_SWITCH  0
  #define SILABS_PLATFORM_COMPONENT_BUZZER  1
#elif (defined SILABS_PLATFORM_WMB912)
  #define SILABS_PLATFORM_WMB
  #define SILABS_RADIO_SI446X
  #undef  SILABS_RADIO_SI4455
  #define SILABS_IO_WITH_EXTENDER
  #if (defined SILABS_WMCU_SI106x)
  #define SILABS_MCU_F930
  #else
  #define SILABS_MCU_F912
  #endif
  #define SILABS_LCD_DOG_GLCD

  #define SILABS_PLATFORM_COMPONENT_LED     4
  #define SILABS_PLATFORM_COMPONENT_PB      4
//  #define SILABS_PLATFORM_COMPONENT_SWITCH  0
  #define SILABS_PLATFORM_COMPONENT_BUZZER  1
#elif (defined SILABS_PLATFORM_LCDBB)
  #define SILABS_RADIO_SI446X
  #undef  SILABS_RADIO_SI4455
  #define SILABS_MCU_F930
  #define SILABS_LCD_DOG_GLCD

  #define SILABS_PLATFORM_COMPONENT_LED     4
  #define SILABS_PLATFORM_COMPONENT_PB      4
//  #define SILABS_PLATFORM_COMPONENT_SWITCH  0
  #define SILABS_PLATFORM_COMPONENT_BUZZER  1
#else
//  #error Platform is undefined or unidentified platform is used!
#endif

#ifdef SILABS_MCU_F930
  #include "drivers\c8051f930_defs.h"
  #include "drivers\C8051F930_defs_add.h"
#endif

#ifdef SILABS_MCU_F912
  #include "drivers\c8051f912_defs.h"
  #include "drivers\C8051F910_defs_add.h"
#endif

#ifdef SILABS_MCU_F960
  #include "drivers\c8051f960_defs.h"
  #include "drivers\C8051F960_defs_add.h"
#endif

#endif /* PLATFORM_DEFS_H_ */
