/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*${header:start}*/
#include "pin_mux.h"
#include "board.h"
#include <stdbool.h>
#include "fsl_dma.h"
/*${header:end}*/

/*${macro:start}*/
#define EXAMPLE_LPSPI_MASTER_DMA_BASEADDR DMA0
/*${macro:end}*/

/*${function:start}*/
void BOARD_InitHardware(void)
{
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* attach 12 MHz clock to I2C0 */
    CLOCK_AttachClk(kIRC12M_to_I2C);

    /* attach 12 MHz clock to SPI1 */
    CLOCK_AttachClk(kIRC12M_to_SPI);

    CLOCK_EnableClock(kCLOCK_Gpio0);

    BOARD_InitPins();
    BOARD_BootClockPLL150M();
    BOARD_InitDebugConsole();
    DMA_Init(EXAMPLE_LPSPI_MASTER_DMA_BASEADDR);
}

uint32_t SPI1_GetFreq(void)
{
    return CLOCK_GetSpiClkFreq();
}

uint32_t I2C0_GetFreq(void)
{
    return CLOCK_GetI2cClkFreq();
}

/*${function:end}*/
