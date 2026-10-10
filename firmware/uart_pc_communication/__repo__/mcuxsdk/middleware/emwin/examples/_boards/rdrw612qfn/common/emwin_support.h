/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EMWIN_SUPPORT_H_
#define _EMWIN_SUPPORT_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LCD_WIDTH             320
#define LCD_HEIGHT            240
#define LCD_FB_BYTE_PER_PIXEL 2
#define LCD_BUF_SIZE          (LCD_WIDTH * LCD_HEIGHT)

#define DEMO_LCDIC         LCDIC
#define DEMO_DMA           DMA0
#define DEMO_LCD_TX_DMA_CH 0

#define BOARD_TOUCH_I2C     Driver_I2C2
#define BOARD_TOUCH_I2C_IRQ FLEXCOMM2_IRQn

#define DISPLAY_DRIVER   GUIDRV_LIN_16
#define COLOR_CONVERSION GUICC_M565

#define APP_MEM_WRITE_CMD 0x2CU

#define GUI_BUFFERS  2
#define GUI_NUMBYTES 0x8000 /*! Amount of memory assigned to the emWin library */

extern int BOARD_Touch_Poll(void);

#endif
