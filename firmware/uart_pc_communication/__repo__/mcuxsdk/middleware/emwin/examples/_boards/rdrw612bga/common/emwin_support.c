/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "GUI.h"
#include "WM.h"
#include "GUIDRV_Lin.h"
#include "emwin_support.h"
#include "fsl_video_common.h"

#include "fsl_debug_console.h"

#ifndef GUI_MEMORY_ADDR
AT_NONCACHEABLE_SECTION_ALIGN(static uint32_t s_gui_memory[(GUI_NUMBYTES)], 4); /* needs to be word aligned */
#define GUI_MEMORY_ADDR ((uint32_t)s_gui_memory)
#endif

/*******************************************************************************
 * Implementation of PortAPI for emWin LCD driver
 ******************************************************************************/
static volatile int32_t s_LCDpendingBuffer = -1;
AT_NONCACHEABLE_SECTION_ALIGN(static uint8_t s_frameBuffer[2][LCD_BUF_SIZE * LCD_FB_BYTE_PER_PIXEL], 4);
static dc_fb_info_t s_fbInfo;

#define VRAM_ADDR ((uint32_t)&s_frameBuffer[0])
#define VRAM_SIZE (LCD_BUF_SIZE * LCD_FB_BYTE_PER_PIXEL)

static void DEMO_BufferSwitchOffCallback(void *param, void *inactiveFrameBuffer)
{
    if (s_LCDpendingBuffer >= 0)
    {
        /* Send a confirmation that the given buffer is visible */
        GUI_MULTIBUF_Confirm(s_LCDpendingBuffer);
        s_LCDpendingBuffer = -1;
    }
}

/*******************************************************************************
 * Implementation of communication with the touch controller
 ******************************************************************************/

int BOARD_Touch_Poll(void)
{
    int ret;
    static int touch_x;
    static int touch_y;
    static bool isPressedPreviously = false;
    GUI_PID_STATE pid_state;
    status_t status;

    pid_state.Layer = 0;
    status = BOARD_GetTouchPanelPoint(&touch_x, &touch_y);

    if (kStatus_Success == status)
    {
        pid_state.Pressed   = true;
        isPressedPreviously = true;
        ret = 1;
    }
    else
    {
        if (isPressedPreviously)
        {
            pid_state.Pressed   = false;
            isPressedPreviously = false;
            ret                 = 1;
        }
        else
        {
            ret = 0;
        }
    }

    if (ret == 1)
    {
        pid_state.x = touch_x;
        pid_state.y = touch_y;
        GUI_TOUCH_StoreStateEx(&pid_state);
    }

    return ret;
}

/*******************************************************************************
 * Application implemented functions required by emWin library
 ******************************************************************************/
void LCD_X_Config(void)
{
    GUI_MULTIBUF_Config(GUI_BUFFERS);
    GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
    LCD_SetSizeEx(0, LCD_WIDTH, LCD_HEIGHT);
    LCD_SetVSizeEx(0, LCD_WIDTH, LCD_HEIGHT);
    LCD_SetVRAMAddrEx(0, (void *)VRAM_ADDR);

    BOARD_PrepareDisplayController();
    BOARD_PrepareTouchPanel();
    BOARD_InitTouchPanel();
}

int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void *p)
{
    uint32_t addr;
    int result = 0;
    LCD_X_SHOWBUFFER_INFO *pData;
    switch (Cmd)
    {
        case LCD_X_INITCONTROLLER:
            g_dc.ops->init(&g_dc);
            g_dc.ops->getLayerDefaultConfig(&g_dc, 0, &s_fbInfo);
            s_fbInfo.pixelFormat = kVIDEO_PixelFormatRGB565;
            s_fbInfo.width       = DEMO_BUFFER_WIDTH;
            s_fbInfo.height      = DEMO_BUFFER_HEIGHT;
            s_fbInfo.startX      = DEMO_BUFFER_START_X;
            s_fbInfo.startY      = DEMO_BUFFER_START_Y;
            s_fbInfo.strideBytes = DEMO_BUFFER_WIDTH * DEMO_BUFFER_BYTE_PER_PIXEL;

            g_dc.ops->setLayerConfig(&g_dc, 0, &s_fbInfo);
            g_dc.ops->setCallback(&g_dc, 0, DEMO_BufferSwitchOffCallback, NULL);
            g_dc.ops->enableLayer(&g_dc, 0);

            GUI_X_Delay(50); /* settle down delay after reset */
            break;

        case LCD_X_SHOWBUFFER:
        {
            pData = (LCD_X_SHOWBUFFER_INFO *)p;

            /* Calculate address of the given buffer */
            addr = VRAM_ADDR + VRAM_SIZE * pData->Index;

            s_LCDpendingBuffer = pData->Index;
            g_dc.ops->setFrameBuffer(&g_dc, 0, (void *)addr);

            /* Remember buffer index to be used by ISR */
            while (s_LCDpendingBuffer >= 0)
                ;
            return 0;
        }
        default:
            result = -1;
            break;
    }

    return result;
}

void GUI_X_Config(void)
{
    /* Assign work memory area to emWin */
    GUI_ALLOC_AssignMemory((void *)GUI_MEMORY_ADDR, GUI_NUMBYTES);

    /* Select default font */
    GUI_SetDefaultFont(GUI_FONT_6X8);
}

void GUI_X_Init(void)
{
}

/* Dummy RTOS stub required by emWin */
void GUI_X_InitOS(void)
{
}

/* Dummy RTOS stub required by emWin */
void GUI_X_Lock(void)
{
}

/* Dummy RTOS stub required by emWin */
void GUI_X_Unlock(void)
{
}

/* Dummy RTOS stub required by emWin */
U32 GUI_X_GetTaskId(void)
{
    return 0;
}

void GUI_X_ExecIdle(void)
{
}

GUI_TIMER_TIME GUI_X_GetTime(void)
{
    return 0;
}

void GUI_X_Delay(int Period)
{
    volatile int per;
    volatile int i;
    for (per = Period; per > 0; per--)
    {
        for (i = 15000; i > 0; i--)
            ;
    }
}
