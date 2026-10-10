/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Standard C Included Files */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"
#include "fsl_lpuart.h"
#include "app.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_I2C_SLAVE              ((LPI2C_Type *)EXAMPLE_I2C_SLAVE_BASE)
#define I2C_MASTER_SLAVE_ADDR_7BIT     0x7EU
#define I2C_DATA_LENGTH                34U

/* Fallback definitions for LPUART in case app.h does not define them */
#ifndef DEMO_LPUART
#define DEMO_LPUART                    ((LPUART_Type *)BOARD_DEBUG_UART_BASEADDR)
#endif

#ifndef DEMO_LPUART_CLK_FREQ
#define DEMO_LPUART_CLK_FREQ           BOARD_DEBUG_UART_CLK_FREQ
#endif

#ifndef BOARD_DEBUG_UART_BAUDRATE
#define BOARD_DEBUG_UART_BAUDRATE      115200U
#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t g_slave_buff[I2C_DATA_LENGTH];
lpi2c_slave_handle_t g_s_handle;
volatile bool g_SlaveCompletionFlag = false;
volatile size_t g_rxCount = 0U;

/*******************************************************************************
 * Code
 ******************************************************************************/

static void lpi2c_slave_callback(LPI2C_Type *base, lpi2c_slave_transfer_t *xfer, void *param)
{
    switch (xfer->event)
    {
        /* Address match event */
        case kLPI2C_SlaveAddressMatchEvent:
            xfer->data     = NULL;
            xfer->dataSize = 0;
            break;

        /* Transmit request */
        case kLPI2C_SlaveTransmitEvent:
            xfer->data     = &g_slave_buff[2];
            xfer->dataSize = g_slave_buff[1];
            break;

        /* Receive request: prepare buffer for incoming bus data */
        case kLPI2C_SlaveReceiveEvent:
            xfer->data     = g_slave_buff;
            xfer->dataSize = I2C_DATA_LENGTH;
            break;

        /* Transfer done: STOP condition detected */
        case kLPI2C_SlaveCompletionEvent:
            g_SlaveCompletionFlag = true;
            g_rxCount             = xfer->transferredCount;
            xfer->data            = NULL;
            xfer->dataSize        = 0;
            break;

        default:
            g_SlaveCompletionFlag = false;
            break;
    }
}

/*!
 * @brief Main function
 */
int main(void)
{
    lpi2c_slave_config_t slaveConfig;
    lpuart_config_t uartConfig;
    status_t reVal = kStatus_Fail;

    /* Initialize board pins, clocks, and debug console */
    BOARD_InitHardware();

    /* 1. Configure and initialize LPUART transmitter */
    LPUART_GetDefaultConfig(&uartConfig);
    uartConfig.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    uartConfig.enableTx     = true;
    uartConfig.enableRx     = true;

    LPUART_Init(DEMO_LPUART, &uartConfig, DEMO_LPUART_CLK_FREQ);

    /* 2. Configure and initialize LPI2C Slave */
    LPI2C_SlaveGetDefaultConfig(&slaveConfig);
    slaveConfig.address0 = I2C_MASTER_SLAVE_ADDR_7BIT;

    LPI2C_SlaveInit(EXAMPLE_I2C_SLAVE, &slaveConfig, LPI2C_SLAVE_CLOCK_FREQUENCY);

    memset(g_slave_buff, 0, sizeof(g_slave_buff));

    /* 3. Create LPI2C transfer handle and bind interrupt callback */
    LPI2C_SlaveTransferCreateHandle(EXAMPLE_I2C_SLAVE, &g_s_handle, lpi2c_slave_callback, NULL);

    /* 4. Start listening for incoming I2C transfers non-blocking */
    reVal = LPI2C_SlaveTransferNonBlocking(EXAMPLE_I2C_SLAVE, &g_s_handle,
                                           kLPI2C_SlaveCompletionEvent | kLPI2C_SlaveAddressMatchEvent);
    if (reVal != kStatus_Success)
    {
        return -1;
    }

    /* Send a boot string over UART to confirm firmware is running */
    uint8_t readyMsg[] = "[READY] I2C-to-UART Bridge Initialized\r\n";
    LPUART_WriteBlocking(DEMO_LPUART, readyMsg, sizeof(readyMsg) - 1);

    /* Continuous operation loop */
    while (1)
    {
        if (g_SlaveCompletionFlag)
        {
            g_SlaveCompletionFlag = false;

            /* If payload bytes were received, transmit them over LPUART immediately */
            if (g_rxCount > 0U)
            {
                LPUART_WriteBlocking(DEMO_LPUART, g_slave_buff, g_rxCount);
                g_rxCount = 0U;
            }

            /* Clear buffer */
            memset(g_slave_buff, 0, sizeof(g_slave_buff));

            /* Re-arm the I2C slave to accept the next incoming transmission */
            LPI2C_SlaveTransferNonBlocking(EXAMPLE_I2C_SLAVE, &g_s_handle,
                                           kLPI2C_SlaveCompletionEvent | kLPI2C_SlaveAddressMatchEvent);
        }
    }
}