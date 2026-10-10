/*
 * Copyright  2017 NXP
 * All rights reserved.
 *
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
    /* Disable SysTick interrupt as it seems to be enabled upon startup */
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;

    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    /* attach 12 MHz clock to FLEXCOMM3 (I2C master) */
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM3);

    CLOCK_SetClkDiv(kCLOCK_DivFrohfClk, 0U, true);  /*!< Reset FROHFDIV divider counter and halt it */
    CLOCK_SetClkDiv(kCLOCK_DivFrohfClk, 2U, false); /*!< Set FROHFDIV divider to value 2 */

    /* attach 48 MHz clock to HSLSPI */
    CLOCK_AttachClk(kFRO_HF_DIV_to_HSLSPI); /*!< Switch HSLSPI to FRO_HF_DIV */

    /* reset FLEXCOMM for SPI */
    RESET_PeripheralReset(kHSLSPI_RST_SHIFT_RSTn);

    /* reset DMA for SPI */
    RESET_PeripheralReset(kDMA0_RST_SHIFT_RSTn);

    DMA_Init(EXAMPLE_LPSPI_MASTER_DMA_BASEADDR);
}

uint32_t SPI8_GetFreq(void)
{
    return CLOCK_GetHsLspiClkFreq();
}

uint32_t I2C3_GetFreq(void)
{
    return CLOCK_GetFlexCommClkFreq(3U);
}

/*${function:end}*/
