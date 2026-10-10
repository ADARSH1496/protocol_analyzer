/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*${header:start}*/
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "app.h"
#include "clock_config.h"
#include "board.h"
#include "emwin_support.h"
/*${header:end}*/

/*${macro:start}*/
/*${macro:end}*/

/*${function:start}*/
void BOARD_InitHardware(void)
{
    /* attach FRO 12M to FLEXCOMM4 (debug console) */
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom4Clk, 1u);
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* attach FRO 12M to FLEXCOMM2 */
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom2Clk, 1u);
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2);

    CLOCK_SetClkDiv(kCLOCK_DivFlexioClk, 1u);
    CLOCK_AttachClk(kPLL0_to_FLEXIO);
    
    CLOCK_EnableClock(kCLOCK_Port4);

    BOARD_InitBootClocks();
    BOARD_I2C_ReleaseBus(2);
    BOARD_InitBootPins();
    BOARD_InitDebugConsole();

    const gpio_pin_config_t resetPinConfig = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic  = 1,
    };

    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio4);

    /* Set SSD1963 CS, RS, and reset pin to output. */
    GPIO_PinInit(BOARD_SSD1963_RST_GPIO, BOARD_SSD1963_RST_PIN, &resetPinConfig);
    GPIO_PinInit(BOARD_SSD1963_CS_GPIO, BOARD_SSD1963_CS_PIN, &resetPinConfig);
    GPIO_PinInit(BOARD_SSD1963_RS_GPIO, BOARD_SSD1963_RS_PIN, &resetPinConfig);
}

void BOARD_SetCSPin(bool set)
{
    GPIO_PinWrite(BOARD_SSD1963_CS_GPIO, BOARD_SSD1963_CS_PIN, (uint8_t)set);
}

void BOARD_SetRSPin(bool set)
{
    GPIO_PinWrite(BOARD_SSD1963_RS_GPIO, BOARD_SSD1963_RS_PIN, (uint8_t)set);
}
/*${function:end}*/
