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

#include "fsl_i2c.h"
#include "fsl_lcdic.h"
#include "fsl_lcdic_dma.h"
#include "fsl_debug_console.h"
#include "fsl_ili9341.h"
#include "fsl_ft6x06.h"
#include "fsl_i2c_cmsis.h"
#include "fsl_inputmux.h"

#ifndef GUI_MEMORY_ADDR
AT_NONCACHEABLE_SECTION_ALIGN(static uint32_t s_gui_memory[(GUI_NUMBYTES)], 4); /* needs to be word aligned */
#define GUI_MEMORY_ADDR ((uint32_t)s_gui_memory)
#endif

/*******************************************************************************
 * Implementation of PortAPI for emWin LCD driver
 ******************************************************************************/
static lcdic_dma_handle_t s_lcdDmaHandle;
static dma_handle_t s_lcdDmaTxHandle;
static volatile int32_t s_LCDpendingBuffer = -1;
AT_NONCACHEABLE_SECTION_ALIGN(static uint8_t s_frameBuffer[2][LCD_BUF_SIZE * LCD_FB_BYTE_PER_PIXEL], 4);
AT_NONCACHEABLE_SECTION_ALIGN(static dma_descriptor_t s_dmaDesc[2], 16);
#define VRAM_ADDR ((uint32_t)&s_frameBuffer[0])
#define VRAM_SIZE (LCD_BUF_SIZE * LCD_FB_BYTE_PER_PIXEL)

/* Get I2C2_CLK_Freq.  */
uint32_t I2C2_GetFreq(void)
{
    return CLOCK_GetFlexCommClkFreq(2);
}

static void DEMO_LcdDoneCallback(LCDIC_Type *base, lcdic_dma_handle_t *handle, status_t status, void *userData)
{
    if (s_LCDpendingBuffer >= 0)
    {
        /* Send a confirmation that the given buffer is visible */
        GUI_MULTIBUF_Confirm(s_LCDpendingBuffer);
        s_LCDpendingBuffer = -1;
    }
}
static void DEMO_LcdWriteCmdData(uint8_t cmd, const uint8_t *data, uint32_t dataLen)
{
    lcdic_tx_xfer_t xfer;

    if (0U == dataLen)
    {
        LCDIC_SendCommandBlocking(DEMO_LCDIC, cmd);
    }
    else
    {
        xfer.cmd            = cmd;
        xfer.teSyncMode     = kLCDIC_TeNoSync;
        xfer.trxTimeoutMode = kLCDIC_ShortTimeout;
        xfer.dataFormat     = kLCDIC_DataFormatByte;
        xfer.dataLen        = dataLen;
        xfer.txData         = data;

        LCDIC_SendDataArrayBlocking(DEMO_LCDIC, &xfer);
    }
}

void DEMO_PanelSelectRegion(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY)
{
    uint8_t data[4];
    /*Column addresses*/
    data[0] = (startX >> 8) & 0xFF;
    data[1] = startX & 0xFF;
    data[2] = (endX >> 8) & 0xFF;
    data[3] = endX & 0xFF;
    DEMO_LcdWriteCmdData(ILI9341_CMD_COLADDR, data, 4u);

    /*Page addresses*/
    data[0] = (startY >> 8) & 0xFF;
    data[1] = startY & 0xFF;
    data[2] = (endY >> 8) & 0xFF;
    data[3] = endY & 0xFF;
    DEMO_LcdWriteCmdData(ILI9341_CMD_PAGEADDR, data, 4u);
}

static void DEMO_FlushDisplay(uint32_t send_size, uint8_t *data)
{
    lcdic_xfer_t xfer;

    DEMO_PanelSelectRegion(0, 0, LCD_WIDTH, LCD_HEIGHT);

    xfer.mode                  = kLCDIC_XferSendDataArray;
    xfer.txXfer.cmd            = APP_MEM_WRITE_CMD;
    xfer.txXfer.teSyncMode     = kLCDIC_TeNoSync;
    xfer.txXfer.trxTimeoutMode = kLCDIC_ShortTimeout;
    xfer.txXfer.dataFormat     = kLCDIC_DataFormatHalfWord;
    xfer.txXfer.dataLen        = send_size;
    xfer.txXfer.txData         = data;

    s_LCDpendingBuffer = -1;

    LCDIC_TransferDMA(DEMO_LCDIC, &s_lcdDmaHandle, &xfer);
}

static void BOARD_LCD_InterfaceInit(void)
{
    status_t status;
    lcdic_config_t config;

    LCDIC_GetDefaultConfig(&config);
    config.mode        = kLCDIC_4WireSPI;
    config.endian      = kLCDIC_BigEndian;
    config.txThreshold = kLCDIC_TxThreshold3Word;
    config.timerRatio0 = 8;
    config.timerRatio1 = 9;
    config.spiCtrlFlags =
        kLCDIC_SPI_MsbFirst | kLCDIC_SPI_ClkActiveLow | kLCDIC_SPI_ClkPhaseSecondEdge | kLCDIC_SPI_DcCmdLow;
    config.cmdShortTimeout_Timer0 = 0U; /* disable */
    config.cmdLongTimeout_Timer1  = 0U; /* disable */

    status = LCDIC_Init(DEMO_LCDIC, &config);
    if (kStatus_Success != status)
    {
        PRINTF("LCDIC init failed\r\n");
        for (;;)
            ;
    }

    /* The NVIC IRQ is pending during init, clear it. */
    NVIC_ClearPendingIRQ(LCDIC_IRQn);

    NVIC_SetPriority(LCDIC_IRQn, 3);
    NVIC_SetPriority(DMA0_IRQn, 3);

    INPUTMUX_AttachSignal(INPUTMUX, DEMO_LCD_TX_DMA_CH, kINPUTMUX_LcdTxRegToDmaSingleToDma0);
    INPUTMUX_EnableSignal(INPUTMUX, kINPUTMUX_Dmac0InputTriggerLcdTxRegToDmaSingleEna, true);

    /* Configure the DMA channel,priority and handle. */
    DMA_CreateHandle(&s_lcdDmaTxHandle, DEMO_DMA, DEMO_LCD_TX_DMA_CH);

    status = LCDIC_TransferCreateHandleDMA(DEMO_LCDIC, &s_lcdDmaHandle, DEMO_LcdDoneCallback, NULL, &s_lcdDmaTxHandle,
                                           NULL, s_dmaDesc);

    if (kStatus_Success != status)
    {
        PRINTF("LCDIC handle creation failed\r\n");
        for (;;)
            ;
    }

    FT9341_Init1(DEMO_LcdWriteCmdData);
    DEMO_LcdWriteCmdData(ILI9341_CMD_MAC, (const uint8_t[]){0x28U}, 1u);
}

/*******************************************************************************
 * Implementation of communication with the touch controller
 ******************************************************************************/
static ft6x06_handle_t touch_handle;

static void I2C_MasterSignalEvent(uint32_t event)
{
    /* Notify touch driver about status of the IO operation */
    FT6X06_EventHandler(&touch_handle, event);
}

static void BOARD_Touch_InterfaceInit(void)
{
    NVIC_SetPriority(BOARD_TOUCH_I2C_IRQ, 0);

    /*Init I2C */
    BOARD_TOUCH_I2C.Initialize(I2C_MasterSignalEvent);
    BOARD_TOUCH_I2C.PowerControl(ARM_POWER_FULL);
    BOARD_TOUCH_I2C.Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD);
}

void BOARD_Touch_InterfaceDeinit(void)
{
    BOARD_TOUCH_I2C.PowerControl(ARM_POWER_OFF);
    BOARD_TOUCH_I2C.Uninitialize();
}

int BOARD_Touch_Poll(void)
{
    touch_event_t touch_event;
    int touch_x;
    int touch_y;
    GUI_PID_STATE pid_state;

    if (kStatus_Success != FT6X06_GetSingleTouch(&touch_handle, &touch_event, &touch_x, &touch_y))
    {
        return 0;
    }
    else if (touch_event != kTouch_Reserved)
    {
        pid_state.x       = LCD_WIDTH - touch_y;
        pid_state.y       = touch_x;
        pid_state.Pressed = ((touch_event == kTouch_Down) || (touch_event == kTouch_Contact));
        pid_state.Layer   = 0;
        GUI_TOUCH_StoreStateEx(&pid_state);
        return 1;
    }
    return 0;
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

    BOARD_Touch_InterfaceInit();
    FT6X06_Init(&touch_handle, &BOARD_TOUCH_I2C);
}

int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void *p)
{
    uint32_t addr;
    int result = 0;
    LCD_X_SHOWBUFFER_INFO *pData;
    switch (Cmd)
    {
        case LCD_X_INITCONTROLLER:
            BOARD_LCD_InterfaceInit();
            GUI_X_Delay(50); /* settle down delay after reset */
            break;
        case LCD_X_SHOWBUFFER:
        {
            pData = (LCD_X_SHOWBUFFER_INFO *)p;
            /* Calculate address of the given buffer */
            addr = VRAM_ADDR + VRAM_SIZE * pData->Index;
            DEMO_FlushDisplay(LCD_BUF_SIZE * LCD_FB_BYTE_PER_PIXEL, (uint8_t *)addr);
            /* Remember buffer index to be used by ISR */
            s_LCDpendingBuffer = pData->Index;
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
