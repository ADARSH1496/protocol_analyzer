/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EMWIN_SUPPORT_H_
#define _EMWIN_SUPPORT_H_

#include <stdbool.h>

/* Macros for the LCD controller. */
#define BOARD_SSD1963_XTAL_FREQ     10000000U
#define BOARD_SSD1963_PCLK_FREQ     30000000U
#define BOARD_SSD1963_HSW           48U
#define BOARD_SSD1963_HFP           40U
#define BOARD_SSD1963_HBP           0U
#define BOARD_SSD1963_VSW           3U
#define BOARD_SSD1963_VFP           13U
#define BOARD_SSD1963_VBP           18U
#define BOARD_SSD1963_POLARITY_FLAG 0U
#define BOARD_SSD1963_RST_GPIO      GPIO4
#define BOARD_SSD1963_RST_PIN       7
#define BOARD_SSD1963_TE_GPIO       GPIO0
#define BOARD_SSD1963_TE_PIN        13
#define BOARD_SSD1963_CS_GPIO       GPIO0
#define BOARD_SSD1963_CS_PIN        12
#define BOARD_SSD1963_RS_GPIO       GPIO0
#define BOARD_SSD1963_RS_PIN        7

/* Macros for FlexIO interfacing the LCD */
#define BOARD_FLEXIO              FLEXIO0
#define BOARD_FLEXIO_CLOCK_FREQ   CLOCK_GetFlexioClkFreq()
#define BOARD_FLEXIO_BAUDRATE_BPS 160000000U

/* Macros for FlexIO shifter, timer, and pins. */
#define BOARD_FLEXIO_WR_PIN           1
#define BOARD_FLEXIO_RD_PIN           0
#define BOARD_FLEXIO_DATA_PIN_START   16
#define BOARD_FLEXIO_TX_START_SHIFTER 0
#define BOARD_FLEXIO_RX_START_SHIFTER 0
#define BOARD_FLEXIO_TX_END_SHIFTER   7
#define BOARD_FLEXIO_RX_END_SHIFTER   7
#define BOARD_FLEXIO_TIMER            0

/* Macros for the touch touch controller. */
#define BOARD_TOUCH_I2C            LPI2C2
#define BOARD_TOUCH_I2C_CLOCK_FREQ CLOCK_GetLPFlexCommClkFreq(2u)
#define BOARD_TOUCH_I2C_BAUDRATE   100000U

#define BOARD_LCD_READABLE 1

#define LCD_WIDTH  800
#define LCD_HEIGHT 480

/* Define scale factors */
#define GUI_SCALE_FACTOR   2
#define GUI_SCALE_FACTOR_X 2.5
#define GUI_SCALE_FACTOR_Y 2

/* Use larger fonts */
#define GUI_NORMAL_FONT (&GUI_Font24_ASCII)
#define GUI_LARGE_FONT  (&GUI_Font32B_ASCII)

#define GUI_NUMBYTES 0x8000 /*! Amount of memory assigned to the emWin library */

void BOARD_SetCSPin(bool set);
void BOARD_SetRSPin(bool set);

extern int BOARD_Touch_Poll(void);

#endif
