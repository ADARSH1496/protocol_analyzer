/*
 * I2C Protocol Analyzer & Slave Emulator - Step 1B: R/W Decoding & Active Slave Emulator
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

uint8_t g_slave_rx_buff[I2C_DATA_LENGTH];
/* Default Fault-Injection / Emulator payload sent when Master READS from us */
uint8_t g_emulator_tx_buff[I2C_DATA_LENGTH] = {0xFA, 0xCE, 0xB0, 0x0C};

lpi2c_slave_handle_t g_s_handle;
volatile bool g_SlaveCompletionFlag = false;
volatile size_t g_transferredCount = 0U;
volatile uint8_t g_matchedAddress = 0U; /* Full 8-bit hardware address: [7-bit Addr | R/W bit] */
volatile bool g_isReadTransaction = false;

static void lpi2c_slave_callback(LPI2C_Type *base, lpi2c_slave_transfer_t *xfer, void *param)
{
    switch (xfer->event)
    {
        case kLPI2C_SlaveAddressMatchEvent:
            g_matchedAddress = (uint8_t)(xfer->receivedAddress & 0xFFU);
            xfer->data       = NULL;
            xfer->dataSize   = 0;
            break;

        /* Master is WRITING to us (Analyzer / Slave RX) */
        case kLPI2C_SlaveReceiveEvent:
            g_matchedAddress    = (uint8_t)(xfer->receivedAddress & 0xFFU);
            g_isReadTransaction = false;
            xfer->data          = g_slave_rx_buff;
            xfer->dataSize      = I2C_DATA_LENGTH;
            break;

        /* Master is READING from us (Slave Emulator / Fault Injection TX) */
        case kLPI2C_SlaveTransmitEvent:
            g_matchedAddress    = (uint8_t)(xfer->receivedAddress & 0xFFU);
            g_isReadTransaction = true;
            xfer->data          = g_emulator_tx_buff;
            xfer->dataSize      = I2C_DATA_LENGTH;
            break;

        case kLPI2C_SlaveCompletionEvent:
            g_SlaveCompletionFlag = true;
            g_transferredCount    = xfer->transferredCount;
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
    
    uint8_t bootMsg[] = "\r\n[BOOT] Firmware initialized. Analyzer + Emulator Active.\r\n";
    LPUART_WriteBlocking(DEMO_LPUART, bootMsg, sizeof(bootMsg) - 1);

    /* --- 2. HARDWARE ROUTING & PULL-UPS --- */
    CLOCK_EnableClock(kCLOCK_Port4);

    uint32_t i2c_pin_config = PORT_PCR_ODE_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

    PORT_SetPinMux(PORT4, 0U, kPORT_MuxAlt2);
    PORT4->PCR[0] |= i2c_pin_config; 
    PORT_SetPinMux(PORT4, 1U, kPORT_MuxAlt2);
    PORT4->PCR[1] |= i2c_pin_config; 

    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2);

    /* --- 3. INITIALIZE FC2 SLAVE --- */
    LPI2C_SlaveGetDefaultConfig(&slaveConfig);
    slaveConfig.address0 = I2C_MASTER_SLAVE_ADDR_7BIT;
    LPI2C_SlaveInit(EXAMPLE_I2C_SLAVE, &slaveConfig, CLOCK_GetLPFlexCommClkFreq(2U));

    memset(g_slave_rx_buff, 0, sizeof(g_slave_rx_buff));
    LPI2C_SlaveTransferCreateHandle(EXAMPLE_I2C_SLAVE, &g_s_handle, lpi2c_slave_callback, NULL);
    
    /* Enable Address Match, Receive (Master Write), Transmit (Master Read), and Completion */
    uint32_t eventMask = kLPI2C_SlaveAddressMatchEvent | 
                         kLPI2C_SlaveReceiveEvent | 
                         kLPI2C_SlaveTransmitEvent | 
                         kLPI2C_SlaveCompletionEvent;
    LPI2C_SlaveTransferNonBlocking(EXAMPLE_I2C_SLAVE, &g_s_handle, eventMask);

    uint8_t readyMsg[] = "[READY] Listening on 0x7E (RX & Fault Injection TX enabled)...\r\n";
    LPUART_WriteBlocking(DEMO_LPUART, readyMsg, sizeof(readyMsg) - 1);

    while (1)
    {
        if (g_SlaveCompletionFlag)
        {
            g_SlaveCompletionFlag = false;

            if (g_transferredCount > 0U)
            {
                /* 
                 * Frame Format:
                 * Byte 0: 0xAA (SOF)
                 * Byte 1: Payload Length (1 byte for 8-bit Addr+RW + transferred bytes)
                 * Byte 2: Full 8-bit Address (Bits 7:1 = 7-bit Addr, Bit 0 = 1 for READ / 0 for WRITE)
                 * Byte 3+: Transferred Data Bytes (either received from Master or injected by Emulator)
                 * Last Byte: 0x55 (EOF)
                 */
                uint8_t frame_buffer[64];
                uint8_t total_payload_len = (uint8_t)(g_transferredCount + 1U);

                frame_buffer[0] = 0xAA; 
                frame_buffer[1] = total_payload_len;
                frame_buffer[2] = g_matchedAddress; 
                
                if (g_isReadTransaction)
                {
                    /* Copy the bytes our Emulator injected onto the bus */
                    memcpy(&frame_buffer[3], (const void*)g_emulator_tx_buff, g_transferredCount);
                }
                else
                {
                    /* Copy the bytes the Master wrote to us */
                    memcpy(&frame_buffer[3], (const void*)g_slave_rx_buff, g_transferredCount);
                }

                frame_buffer[2 + total_payload_len] = 0x55;
                LPUART_WriteBlocking(DEMO_LPUART, frame_buffer, total_payload_len + 3U);
            }

            g_transferredCount = 0U;
            memset(g_slave_rx_buff, 0, sizeof(g_slave_rx_buff));
            LPI2C_SlaveTransferNonBlocking(EXAMPLE_I2C_SLAVE, &g_s_handle, eventMask);
        }
    }
}