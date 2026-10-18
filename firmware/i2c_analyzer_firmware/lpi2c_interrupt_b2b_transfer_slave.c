/*
 * I2C Protocol Analyzer & Slave Emulator - Complete Production Firmware (Step 1C)
 * Supports:
 *   1. True Bus Analyzer Mode (Range Match 0x08 - 0x7E)
 *   2. Active Slave Emulator Mode (Dynamic Address & Custom Fault Payload Injection)
 *   3. Bi-Directional UART Command Listener (0xBB [Mode] [Addr] [B0] [B1] [B2] [B3] 0x55)
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
#define DEFAULT_SLAVE_ADDR_7BIT        0x7EU
#define I2C_DATA_LENGTH                34U
#define CMD_FRAME_LENGTH               8U

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
/* Dynamic Fault-Injection / Emulator payload sent when Master READS from us */
uint8_t g_emulator_tx_buff[I2C_DATA_LENGTH] = {0xFA, 0xCE, 0xB0, 0x0C};

lpi2c_slave_handle_t g_s_handle;
volatile bool g_SlaveCompletionFlag = false;
volatile size_t g_transferredCount = 0U;
volatile uint8_t g_matchedAddress = 0U; /* Full 8-bit hardware address: [7-bit Addr | R/W bit] */
volatile bool g_isReadTransaction = false;

/* Current operating state */
uint8_t g_currentMode = 0U; /* 0 = Analyzer (All Addresses), 1 = Emulator (Single Target Address) */
uint8_t g_targetAddr  = DEFAULT_SLAVE_ADDR_7BIT;

/* UART RX Command Buffer */
uint8_t g_uart_cmd_buff[CMD_FRAME_LENGTH];
uint8_t g_uart_cmd_idx = 0U;

uint32_t g_eventMask = kLPI2C_SlaveAddressMatchEvent | 
                       kLPI2C_SlaveReceiveEvent | 
                       kLPI2C_SlaveTransmitEvent | 
                       kLPI2C_SlaveCompletionEvent;

static void lpi2c_slave_callback(LPI2C_Type *base, lpi2c_slave_transfer_t *xfer, void *param)
{
    switch (xfer->event)
    {
        case kLPI2C_SlaveAddressMatchEvent:
            g_matchedAddress = (uint8_t)(xfer->receivedAddress & 0xFFU);
            xfer->data       = NULL;
            xfer->dataSize   = 0;
            break;

        case kLPI2C_SlaveReceiveEvent:
            g_matchedAddress    = (uint8_t)(xfer->receivedAddress & 0xFFU);
            g_isReadTransaction = false;
            xfer->data          = g_slave_rx_buff;
            xfer->dataSize      = I2C_DATA_LENGTH;
            break;

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

/* Dynamically (re)configures LPI2C2 hardware registers for Analyzer or Emulator mode */
static void Configure_I2C_Hardware(uint8_t mode, uint8_t target_addr_7bit)
{
    lpi2c_slave_config_t slaveConfig;

    LPI2C_SlaveDeinit(EXAMPLE_I2C_SLAVE);
    LPI2C_SlaveGetDefaultConfig(&slaveConfig);

    if (mode == 0U)
    {
        /* ANALYZER MODE: Match all standard 7-bit addresses from 0x08 to 0x7E */
        slaveConfig.address0         = 0x08U;
        slaveConfig.address1         = 0x7EU;
        slaveConfig.addressMatchMode = kLPI2C_MatchAddress0OrAddress1;
    }
    else
    {
        /* EMULATOR MODE: Match only the specific target address */
        slaveConfig.address0         = target_addr_7bit;
        slaveConfig.addressMatchMode = kLPI2C_MatchAddress0;
    }

    LPI2C_SlaveInit(EXAMPLE_I2C_SLAVE, &slaveConfig, CLOCK_GetLPFlexCommClkFreq(2U));
    LPI2C_SlaveTransferCreateHandle(EXAMPLE_I2C_SLAVE, &g_s_handle, lpi2c_slave_callback, NULL);
    LPI2C_SlaveTransferNonBlocking(EXAMPLE_I2C_SLAVE, &g_s_handle, g_eventMask);
}

int main(void)
{
    lpuart_config_t uartConfig;

    BOARD_InitHardware();

    /* --- 1. INITIALIZE PC BRIDGE --- */
    LPUART_GetDefaultConfig(&uartConfig);
    uartConfig.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    uartConfig.enableTx     = true;
    uartConfig.enableRx     = true;
    LPUART_Init(DEMO_LPUART, &uartConfig, DEMO_LPUART_CLK_FREQ);
    
    uint8_t bootMsg[] = "\r\n[BOOT] Firmware v1.0 Ready. Dynamic Analyzer + Fault Emulator.\r\n";
    LPUART_WriteBlocking(DEMO_LPUART, bootMsg, sizeof(bootMsg) - 1);

    /* --- 2. HARDWARE ROUTING & PULL-UPS --- */
    CLOCK_EnableClock(kCLOCK_Port4);

    uint32_t i2c_pin_config = PORT_PCR_ODE_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;

    PORT_SetPinMux(PORT4, 0U, kPORT_MuxAlt2);
    PORT4->PCR[0] |= i2c_pin_config; 
    PORT_SetPinMux(PORT4, 1U, kPORT_MuxAlt2);
    PORT4->PCR[1] |= i2c_pin_config; 

    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2);

    /* --- 3. START IN ANALYZER MODE BY DEFAULT (Sniff 0x08 - 0x7E) --- */
    memset(g_slave_rx_buff, 0, sizeof(g_slave_rx_buff));
    Configure_I2C_Hardware(g_currentMode, g_targetAddr);

    uint8_t readyMsg[] = "[READY] Mode: ANALYZER (Sniffing 0x08-0x7E) | Default Fault TX: FA CE B0 0C\r\n";
    LPUART_WriteBlocking(DEMO_LPUART, readyMsg, sizeof(readyMsg) - 1);

    while (1)
    {
        /* --- A. CHECK FOR INCOMING I2C BUS TRANSACTIONS --- */
        if (g_SlaveCompletionFlag)
        {
            g_SlaveCompletionFlag = false;

            if (g_transferredCount > 0U)
            {
                uint8_t frame_buffer[64];
                uint8_t total_payload_len = (uint8_t)(g_transferredCount + 1U);

                frame_buffer[0] = 0xAA; 
                frame_buffer[1] = total_payload_len;
                frame_buffer[2] = g_matchedAddress; 
                
                if (g_isReadTransaction)
                {
                    memcpy(&frame_buffer[3], (const void*)g_emulator_tx_buff, g_transferredCount);
                }
                else
                {
                    memcpy(&frame_buffer[3], (const void*)g_slave_rx_buff, g_transferredCount);
                }

                frame_buffer[2 + total_payload_len] = 0x55;
                LPUART_WriteBlocking(DEMO_LPUART, frame_buffer, total_payload_len + 3U);
            }

            g_transferredCount = 0U;
            memset(g_slave_rx_buff, 0, sizeof(g_slave_rx_buff));
            LPI2C_SlaveTransferNonBlocking(EXAMPLE_I2C_SLAVE, &g_s_handle, g_eventMask);
        }

        /* --- B. NON-BLOCKING CHECK FOR PC CONFIGURATION COMMANDS OVER UART --- */
        if (kLPUART_RxDataRegFullFlag & LPUART_GetStatusFlags(DEMO_LPUART))
        {
            uint8_t rxByte = LPUART_ReadByte(DEMO_LPUART);

            if (g_uart_cmd_idx == 0U)
            {
                if (rxByte == 0xBBU) /* Command SOF */
                {
                    g_uart_cmd_buff[g_uart_cmd_idx++] = rxByte;
                }
            }
            else
            {
                g_uart_cmd_buff[g_uart_cmd_idx++] = rxByte;

                if (g_uart_cmd_idx == CMD_FRAME_LENGTH)
                {
                    if (g_uart_cmd_buff[CMD_FRAME_LENGTH - 1U] == 0x55U) /* Valid Command EOF */
                    {
                        g_currentMode = g_uart_cmd_buff[1];
                        g_targetAddr  = g_uart_cmd_buff[2] & 0x7FU;

                        /* Update 4-byte Fault Injection Payload */
                        g_emulator_tx_buff[0] = g_uart_cmd_buff[3];
                        g_emulator_tx_buff[1] = g_uart_cmd_buff[4];
                        g_emulator_tx_buff[2] = g_uart_cmd_buff[5];
                        g_emulator_tx_buff[3] = g_uart_cmd_buff[6];

                        Configure_I2C_Hardware(g_currentMode, g_targetAddr);

                        char ackMsg[96];
                        snprintf(ackMsg, sizeof(ackMsg),
                                 "\r\n[CFG-ACK] Mode=%s | Addr=0x%02X | FaultPayload=%02X %02X %02X %02X\r\n",
                                 (g_currentMode == 0U) ? "ANALYZER(0x08-0x7E)" : "EMULATOR",
                                 g_targetAddr,
                                 g_emulator_tx_buff[0], g_emulator_tx_buff[1],
                                 g_emulator_tx_buff[2], g_emulator_tx_buff[3]);
                        LPUART_WriteBlocking(DEMO_LPUART, (uint8_t *)ackMsg, strlen(ackMsg));
                    }
                    g_uart_cmd_idx = 0U;
                }
            }
        }
    }
}