/*
 * Copyright 2022 NXP.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v15.0
processor: RW612
package_id: RW612ETA2I
mcu_data: ksdk2_0
processor_version: 0.16.7
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

#include "fsl_common.h"
#include "fsl_io_mux.h"
#include "pin_mux.h"

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 *
 * END ****************************************************************************************************************/
void BOARD_InitBootPins(void)
{
    BOARD_InitPins();
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', coreID: cm33, enableClock: 'true'}
- pin_list:
  - {pin_num: E5, peripheral: FLEXCOMM3, signal: USART_RXD, pin_signal: GPIO_24}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
/* Function assigned for the Cortex-M33 */
void BOARD_InitPins(void)
{
    /* Initialize FC3_USART_DATA functionality on pin GPIO_24 (pin E5) */
    IO_MUX_SetPinMux(IO_MUX_FC3_USART_DATA);
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitLcd0Pins:
- options: {callFromInitBoot: 'false', coreID: cm33, enableClock: 'true'}
- pin_list:
  - {pin_num: H2, peripheral: FLEXCOMM2, signal: I2C_SCL, pin_signal: GPIO_17}
  - {pin_num: J3, peripheral: FLEXCOMM2, signal: I2C_SDA, pin_signal: GPIO_16}
  - {pin_num: F12, peripheral: LCDIC, signal: LCD_SPI_DATA, pin_signal: GPIO_46}
  - {pin_num: D14, peripheral: LCDIC, signal: LCD_SPI_SS, pin_signal: GPIO_49}
  - {pin_num: J11, peripheral: LCDIC, signal: LCD_SPI_RESET, pin_signal: GPIO_44}
  - {pin_num: H10, peripheral: LCDIC, signal: LCD_SPI_TE, pin_signal: GPIO_45}
  - {pin_num: E13, peripheral: LCDIC, signal: LCD_SPI_DC, pin_signal: GPIO_47}
  - {pin_num: D13, peripheral: LCDIC, signal: LCD_SPI_SCK, pin_signal: GPIO_48}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitLcd0Pins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
/* Function assigned for the Cortex-M33 */
void BOARD_InitLcd0Pins(void)
{
    /* Initialize FC2_I2C_16_17 functionality on pin GPIO_17, GPIO_16 (pin H2_J3) */
    IO_MUX_SetPinMux(IO_MUX_FC2_I2C_16_17);
    /* Initialize LCD_SPI functionality on pin GPIO_46, GPIO_49, GPIO_44, GPIO_45, GPIO_47, GPIO_48, GPIO_54, GPIO_56,
     * GPIO_57, GPIO_42, GPIO_53, GPIO_43 (pin F12_D14_J11_H10_E13_D13_M10_A14_B13_G13_L11_H12) */
    IO_MUX_SetPinMux(IO_MUX_LCD_SPI);
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitLcd1Pins:
- options: {callFromInitBoot: 'false', coreID: cm33, enableClock: 'true'}
- pin_list:
  - {pin_num: H2, peripheral: FLEXCOMM2, signal: I2C_SCL, pin_signal: GPIO_17}
  - {pin_num: J3, peripheral: FLEXCOMM2, signal: I2C_SDA, pin_signal: GPIO_16}
  - {pin_num: A14, peripheral: LCDIC, signal: LCD_8080_CS, pin_signal: GPIO_56}
  - {pin_num: B13, peripheral: LCDIC, signal: LCD_8080_DC, pin_signal: GPIO_57}
  - {pin_num: G13, peripheral: LCDIC, signal: 'LCD_8080_DATA, 0', pin_signal: GPIO_42}
  - {pin_num: E13, peripheral: LCDIC, signal: 'LCD_8080_DATA, 3', pin_signal: GPIO_47}
  - {pin_num: D13, peripheral: LCDIC, signal: LCD_8080_RD, pin_signal: GPIO_48}
  - {pin_num: L11, peripheral: LCDIC, signal: 'LCD_8080_DATA, 7', pin_signal: GPIO_53}
  - {pin_num: H12, peripheral: LCDIC, signal: 'LCD_8080_DATA, 1', pin_signal: GPIO_43}
  - {pin_num: J11, peripheral: LCDIC, signal: LCD_8080_RESET, pin_signal: GPIO_44}
  - {pin_num: M10, peripheral: LCDIC, signal: LCD_8080_WR, pin_signal: GPIO_54}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitLcd1Pins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
/* Function assigned for the Cortex-M33 */
void BOARD_InitLcd1Pins(void)
{
    /* Initialize FC2_I2C_16_17 functionality on pin GPIO_17, GPIO_16 (pin H2_J3) */
    IO_MUX_SetPinMux(IO_MUX_FC2_I2C_16_17);
    /* Initialize LCD_8080 functionality on pin GPIO_46, GPIO_49, GPIO_44, GPIO_45, GPIO_47, GPIO_48, GPIO_54, GPIO_56,
     * GPIO_57, GPIO_42, GPIO_53, GPIO_43 (pin F12_D14_J11_H10_E13_D13_M10_A14_B13_G13_L11_H12) */
    IO_MUX_SetPinMux(IO_MUX_LCD_8080);
}
/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
