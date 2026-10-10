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
#include "board.h"

#include "fsl_ctimer.h"
#include "fsl_debug_console.h"
#include "fsl_video_common.h"
#include "fsl_gpio.h"
#include "fsl_lcdc.h"
#include "fsl_i2c.h"
#if (DEMO_PANEL == DEMO_PANEL_RK043FN66HS)
#include "fsl_gt911.h"
#else
#include "fsl_ft5406.h"
#endif

/* Frame end flag. */
static volatile int32_t s_LCDpendingBuffer = -1;

#if (DEMO_PANEL == DEMO_PANEL_RK043FN66HS)
static void BOARD_PullTouchResetPin(bool pullUp);
static void BOARD_ConfigTouchIntPin(gt911_int_pin_mode_t mode);
#endif

/*******************************************************************************
 * Implementation of PortAPI for emWin LCD driver
 ******************************************************************************/
void APP_LCD_IRQHandler(void)
{
    uint32_t intStatus = LCDC_GetEnabledInterruptsPendingStatus(APP_LCD);

    LCDC_ClearInterruptsStatus(APP_LCD, intStatus);

    if (intStatus & kLCDC_VerticalCompareInterrupt)
    {
        if (s_LCDpendingBuffer >= 0)
        {
            /* Send a confirmation that the given buffer is visible */
            GUI_MULTIBUF_Confirm(s_LCDpendingBuffer);
            s_LCDpendingBuffer = -1;
        }
    }
    __DSB();
    SDK_ISR_EXIT_BARRIER;
}

status_t APP_LCDC_Init(void)
{
    /* Initialize the display. */
    lcdc_config_t lcdConfig;

    LCDC_GetDefaultConfig(&lcdConfig);

    lcdConfig.panelClock_Hz  = LCD_PANEL_CLK;
    lcdConfig.ppl            = LCD_PPL;
    lcdConfig.hsw            = LCD_HSW;
    lcdConfig.hfp            = LCD_HFP;
    lcdConfig.hbp            = LCD_HBP;
    lcdConfig.lpp            = LCD_LPP;
    lcdConfig.vsw            = LCD_VSW;
    lcdConfig.vfp            = LCD_VFP;
    lcdConfig.vbp            = LCD_VBP;
    lcdConfig.polarityFlags  = LCD_POL_FLAGS;
    lcdConfig.upperPanelAddr = VRAM_ADDR;
    lcdConfig.bpp            = kLCDC_24BPP;
    lcdConfig.display        = kLCDC_DisplayTFT;
    lcdConfig.swapRedBlue    = true;

    LCDC_Init(APP_LCD, &lcdConfig, LCD_INPUT_CLK_FREQ);

    /* Trigger interrupt at start of every vertical back porch. */
    LCDC_SetVerticalInterruptMode(APP_LCD, kLCDC_StartOfVsync);
    LCDC_EnableInterrupts(APP_LCD, kLCDC_VerticalCompareInterrupt);
    NVIC_EnableIRQ(APP_LCD_IRQn);

    LCDC_Start(APP_LCD);
    LCDC_PowerUp(APP_LCD);

    return kStatus_Success;
}

/*******************************************************************************
 * Implementation of communication with the touch controller
 ******************************************************************************/

/* Touch driver handle. */
#if (DEMO_PANEL == DEMO_PANEL_RK043FN66HS)
static gt911_handle_t s_touchHandle;
static const gt911_config_t s_touchConfig = {
    .I2C_SendFunc     = BOARD_Touch_I2C_Send,
    .I2C_ReceiveFunc  = BOARD_Touch_I2C_Receive,
    .pullResetPinFunc = BOARD_PullTouchResetPin,
    .intPinFunc       = BOARD_ConfigTouchIntPin,
    .timeDelayMsFunc  = VIDEO_DelayMs,
    .touchPointNum    = 1,
    .i2cAddrMode      = kGT911_I2cAddrMode0,
    .intTrigMode      = kGT911_IntRisingEdge,
};

#else
static ft5406_handle_t touch_handle;
#endif

#if (DEMO_PANEL == DEMO_PANEL_RK043FN66HS)
static void BOARD_PullTouchResetPin(bool pullUp)
{
    if (pullUp)
    {
        GPIO_PinWrite(BOARD_TOUCH_RST_GPIO, BOARD_TOUCH_RST_PORT, BOARD_TOUCH_RST_PIN, 1);
    }
    else
    {
        GPIO_PinWrite(BOARD_TOUCH_RST_GPIO, BOARD_TOUCH_RST_PORT, BOARD_TOUCH_RST_PIN, 0);
    }
}

static void BOARD_ConfigTouchIntPin(gt911_int_pin_mode_t mode)
{
    if (mode == kGT911_IntPinInput)
    {
        BOARD_TOUCH_INT_GPIO->DIR[BOARD_TOUCH_INT_PORT] &= ~(1UL << BOARD_TOUCH_INT_PIN);
    }
    else
    {
        if (mode == kGT911_IntPinPullDown)
        {
            GPIO_PinWrite(BOARD_TOUCH_INT_GPIO, BOARD_TOUCH_INT_PORT, BOARD_TOUCH_INT_PIN, 0);
        }
        else
        {
            GPIO_PinWrite(BOARD_TOUCH_INT_GPIO, BOARD_TOUCH_INT_PORT, BOARD_TOUCH_INT_PIN, 1);
        }

        BOARD_TOUCH_INT_GPIO->DIR[BOARD_TOUCH_INT_PORT] |= 1UL << BOARD_TOUCH_INT_PIN;
    }
}
#endif

status_t APP_Touch_Init(void)
{
    status_t status;
    i2c_master_config_t masterConfig;
    gpio_pin_config_t pin_config = {
        kGPIO_DigitalOutput,
        0,
    };

    I2C_MasterGetDefaultConfig(&masterConfig);

    /* Change the default baudrate configuration */
    masterConfig.baudRate_Bps = I2C_BAUDRATE;

    /* Initialize the I2C master peripheral */
    I2C_MasterInit(EXAMPLE_I2C_MASTER, &masterConfig, I2C_MASTER_CLOCK_FREQUENCY);

    /* Enable touch panel controller */
    GPIO_PinInit(BOARD_TOUCH_RST_GPIO, BOARD_TOUCH_RST_PORT, BOARD_TOUCH_RST_PIN, &pin_config);
    GPIO_PinWrite(BOARD_TOUCH_RST_GPIO, BOARD_TOUCH_RST_PORT, BOARD_TOUCH_RST_PIN, 1);

    GPIO_PinInit(BOARD_TOUCH_INT_GPIO, BOARD_TOUCH_INT_PORT, BOARD_TOUCH_INT_PIN, &pin_config);
    GPIO_PinWrite(BOARD_TOUCH_INT_GPIO, BOARD_TOUCH_INT_PORT, BOARD_TOUCH_INT_PIN, 0);

    /* Initialize touch panel controller */
#if (DEMO_PANEL == DEMO_PANEL_RK043FN66HS)
    status = GT911_Init(&s_touchHandle, &s_touchConfig);
#else
    status = FT5406_Init(&touch_handle, EXAMPLE_I2C_MASTER);
#endif
    if (status != kStatus_Success)
    {
        PRINTF("Touch panel init failed\n");
    }
    assert(status == kStatus_Success);

    return kStatus_Success;
}

int BOARD_Touch_Poll(void)
{
#if (DEMO_PANEL == DEMO_PANEL_RK043FN02H)
    touch_event_t touch_event;
#endif
    int touch_x;
    int touch_y;
    static GUI_PID_STATE pid_state;
    pid_state.Layer = 0;

#if (DEMO_PANEL == DEMO_PANEL_RK043FN66HS)
    status_t st = GT911_GetSingleTouch(&s_touchHandle, &touch_x, &touch_y);
    if ((st == kStatus_Success) || (st == kStatus_TOUCHPANEL_NotTouched))
    {
        if (st == kStatus_Success)
        {
            pid_state.x = touch_x;
            pid_state.y = touch_y;
        }
        pid_state.Pressed = (st == kStatus_Success);
        GUI_TOUCH_StoreStateEx(&pid_state);
    }
#else
    if (kStatus_Success == FT5406_GetSingleTouch(&touch_handle, &touch_event, &touch_x, &touch_y))
    {
        if ((touch_event == kTouch_Contact) || (touch_event == kTouch_Up))
        {
            if (touch_event == kTouch_Contact)
            {
                pid_state.x = touch_y;
                pid_state.y = touch_x;
            }
            pid_state.Pressed = (touch_event == kTouch_Contact);
            GUI_TOUCH_StoreStateEx(&pid_state);
        }
    }
#endif
    return 1;
}

/*******************************************************************************
 * Application implemented functions required by emWin library
 ******************************************************************************/
void LCD_X_Config(void)
{
    status_t status;
    GUI_MULTIBUF_Config(GUI_BUFFERS);
    GUI_DEVICE_CreateAndLink(GUIDRV_LIN_32, GUICC_M8888I, 0, 0);

    LCD_SetSizeEx(0, LCD_WIDTH, LCD_HEIGHT);
    LCD_SetVSizeEx(0, LCD_WIDTH, LCD_HEIGHT);

    LCD_SetVRAMAddrEx(0, (void *)VRAM_ADDR);

    /* Initialize touch panel controller */
    status = APP_Touch_Init();
    if (status != kStatus_Success)
    {
        PRINTF("Touch init failed\n");
    }
    assert(status == kStatus_Success);
}

int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void *p)
{
    GUI_USE_PARA(LayerIndex);

    uint32_t addr;
    status_t status;
    int r = 0;
    LCD_X_SHOWBUFFER_INFO *pData;
    switch (Cmd)
    {
        case LCD_X_INITCONTROLLER:
        {
            /* Initialize LCD controller */
            status = APP_LCDC_Init();
            if (status != kStatus_Success)
            {
                PRINTF("LCD init failed\n");
            }
            assert(status == kStatus_Success);
            break;
        }
        case LCD_X_SHOWBUFFER:
        {
            pData = (LCD_X_SHOWBUFFER_INFO *)p;
            /* Calculate address of the given buffer */
            addr = VRAM_ADDR + VRAM_SIZE * pData->Index;
            /* Make the given buffer visible */
            LCDC_SetPanelAddr(APP_LCD, kLCDC_UpperPanel, addr);
            //
            // Remember buffer index to be used by ISR
            //
            s_LCDpendingBuffer = pData->Index;
            while (s_LCDpendingBuffer >= 0)
                ;
            return 0;
        }
        default:
            r = -1;
    }
    return r;
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
    GUI_X_Delay(1);
}

GUI_TIMER_TIME GUI_X_GetTime(void)
{
    return CTIMER_GetTimerCountValue(CTIMER);
}

void GUI_X_Delay(int Period)
{
    volatile uint32_t tNow = CTIMER_GetTimerCountValue(CTIMER);
    while ((CTIMER_GetTimerCountValue(CTIMER) - tNow) < Period)
        ;
}

void *emWin_memcpy(void *pDst, const void *pSrc, long size)
{
    return memcpy(pDst, pSrc, size);
}
