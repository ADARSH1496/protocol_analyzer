/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _EMWIN_SUPPORT_H_
#define _EMWIN_SUPPORT_H_

#include "display_support.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LCD_WIDTH             DEMO_PANEL_WIDTH
#define LCD_HEIGHT            DEMO_PANEL_HEIGHT
#define LCD_FB_BYTE_PER_PIXEL DEMO_BUFFER_BYTE_PER_PIXEL
#define LCD_BUF_SIZE          (LCD_WIDTH * LCD_HEIGHT)

#define DISPLAY_DRIVER   GUIDRV_LIN_16
#define COLOR_CONVERSION GUICC_565

#define GUI_BUFFERS  2
#define GUI_NUMBYTES 0x8000 /*! Amount of memory assigned to the emWin library */

extern int BOARD_Touch_Poll(void);

#endif
