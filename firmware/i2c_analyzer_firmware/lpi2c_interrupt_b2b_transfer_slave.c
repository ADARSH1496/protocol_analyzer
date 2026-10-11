/*
 * I2C Protocol Analyzer & Slave Emulator - Production Build
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"
#include "fsl_lpuart.h"
#include "app.h"
#include "fsl_port.h"

#define EXAMPLE_I2C_SLAVE              LPI2C2
#define I2C_MASTER_SLAVE_ADDR_7BIT     0x7EU
#define I2C_DATA_LENGTH                34U

#ifndef DEMO_LPUART
#define DEMO_LPUART                    ((LPUART_Type *)BOARD_DEBUG_UART_BASEADDR)
#endif
#ifndef DEMO_LPUART_CLK_FREQ
#define DEMO_LPUART_CLK_FREQ           BOARD_DEBUG_UART_CLK_FREQ
#endif
#ifndef BOARD_DEBUG_UART_BAUDRATE
#define BOARD_DEBUG_UART_BAUDRATE      115200U
#endif

uint8_t g_slave_buff[I2C_DATA_LENGTH];
lpi2c_slave_handle_t g_s_handle;
volatile bool g_SlaveCompletionFlag = false;
volatile size_t g_rxCount = 0U;

static void lpi2c_slave_callback(LPI2C_Type *base, lpi2c_slave_transfer_t *xfer, void *param)
{
    switch (xfer->event)
    {
        case kLPI2C_SlaveReceiveEvent:
            xfer->data     = g_slave_buff;
            xfer->dataSize = I2C_DATA_LENGTH;
            break;
        case kLPI2C_SlaveCompletionEvent:
            g_SlaveCompletionFlag = true;
            g_rxCount             = xfer->transferredCount;
            xfer->data            = NULL;
            xfer->dataSize        = 0;
            break;
        default:
            break;
    }
}

int main(void)
{
    lpi2c_slave_config_t slaveConfig;
    lpuart_config_t uartConfig;

    BOARD_InitHardware();

    /* --- 1. INITIALIZE PC BRIDGE --- */
    LPUART_GetDefaultConfig(&uartConfig);
    uartConfig.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    uartConfig.enableTx     = true;
    uartConfig.enableRx     = true;
    LPUART_Init(DEMO_LPUART, &uartConfig, DEMO_LPUART_CLK_FREQ);
    
    uint8_t bootMsg[] = "\r\n[BOOT] Firmware initialized. Slave Emulator Active.\r\n";
    LPUART_WriteBlocking(DEMO_LPUART, bootMsg, sizeof(bootMsg) - 1);

    /* --- 2. HARDWARE ROUTING & PULL-UPS --- */
    CLOCK_EnableClock(kCLOCK_Port4);

    uint32_t i2c_pin_config = PORT_PCR_ODE_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

    /* Route Flexcomm 2 (Analyzer/Slave) to J5 Pins 5 & 6 */
    PORT_SetPinMux(PORT4, 0U, kPORT_MuxAlt2);
    PORT4->PCR[0] |= i2c_pin_config; 
    PORT_SetPinMux(PORT4, 1U, kPORT_MuxAlt2);
    PORT4->PCR[1] |= i2c_pin_config; 

    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2);

    /* --- 3. INITIALIZE FC2 SLAVE --- */
    LPI2C_SlaveGetDefaultConfig(&slaveConfig);
    slaveConfig.address0 = I2C_MASTER_SLAVE_ADDR_7BIT;
    LPI2C_SlaveInit(EXAMPLE_I2C_SLAVE, &slaveConfig, CLOCK_GetLPFlexCommClkFreq(2U));

    memset(g_slave_buff, 0, sizeof(g_slave_buff));
    LPI2C_SlaveTransferCreateHandle(EXAMPLE_I2C_SLAVE, &g_s_handle, lpi2c_slave_callback, NULL);
    LPI2C_SlaveTransferNonBlocking(EXAMPLE_I2C_SLAVE, &g_s_handle, kLPI2C_SlaveReceiveEvent | kLPI2C_SlaveCompletionEvent);

    uint8_t readyMsg[] = "[READY] Listening on Address 0x7E. Waiting for external Master...\r\n";
    LPUART_WriteBlocking(DEMO_LPUART, readyMsg, sizeof(readyMsg) - 1);

    while (1)
    {
        /* Simply wait for traffic to arrive on the bus */
        if (g_SlaveCompletionFlag)
        {
            g_SlaveCompletionFlag = false;

            if (g_rxCount > 0U)
            {
                uint8_t frame_buffer[64];
                frame_buffer[0] = 0xAA; 
                frame_buffer[1] = (uint8_t)g_rxCount;
                memcpy(&frame_buffer[2], (const void*)g_slave_buff, g_rxCount);
                frame_buffer[2 + g_rxCount] = 0x55;

                LPUART_WriteBlocking(DEMO_LPUART, frame_buffer, g_rxCount + 3);
            }

            g_rxCount = 0U;
            memset(g_slave_buff, 0, sizeof(g_slave_buff));
            LPI2C_SlaveTransferNonBlocking(EXAMPLE_I2C_SLAVE, &g_s_handle, kLPI2C_SlaveReceiveEvent | kLPI2C_SlaveCompletionEvent);
        }
    }
}