/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*${header:start}*/
#include "pin_mux.h"
#include "board.h"
#include <stdbool.h>
/*${header:end}*/

/*${function:start}*/
void BOARD_InitHardware(void)
{
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* attach 12 MHz clock to I2C0 */
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM4);

    /* attach 12 MHz clock to I2C1 */
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM5);

    /* attach 12 MHz clock to I2C1 */
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM7);

    CLOCK_EnableClock(kCLOCK_Gpio1);

    /* reset FLEXCOMM for SPI and I2C */
    RESET_PeripheralReset(kFC4_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kFC5_RST_SHIFT_RSTn);
    RESET_PeripheralReset(kGPIO1_RST_SHIFT_RSTn);

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
}

uint32_t SPI5_GetFreq(void)
{
    return CLOCK_GetFlexCommClkFreq(5);
}

uint32_t I2C4_GetFreq(void)
{
    return CLOCK_GetFlexCommClkFreq(4);
}

uint32_t I2C7_GetFreq(void)
{
    return CLOCK_GetFlexCommClkFreq(7);
}
/*${function:end}*/
