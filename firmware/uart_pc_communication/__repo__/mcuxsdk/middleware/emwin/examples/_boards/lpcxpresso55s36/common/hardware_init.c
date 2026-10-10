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
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom0Clk, 0u, false);
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom0Clk, 1u, true);
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    /* attach 12 MHz clock to FLEXCOMM3 (I2C master) */
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom3Clk, 0u, false);
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom3Clk, 1u, true);
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM3);

    CLOCK_SetClkDiv(kCLOCK_DivPllClk, 0U, true);
    CLOCK_SetClkDiv(kCLOCK_DivPllClk, 3U, false);

    /* attach 50 MHz clock to HSLSPI */
    CLOCK_AttachClk(kPLL0_to_PLLCLKDIV);
    CLOCK_AttachClk(kPLL_CLK_DIV_to_HSLSPI);

    /* reset FLEXCOMM for SPI */
    RESET_PeripheralReset(kHSLSPI_RST_SHIFT_RSTn);
    /* reset FLEXCOMM for I2C */
    RESET_PeripheralReset(kFC3_RST_SHIFT_RSTn);

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
