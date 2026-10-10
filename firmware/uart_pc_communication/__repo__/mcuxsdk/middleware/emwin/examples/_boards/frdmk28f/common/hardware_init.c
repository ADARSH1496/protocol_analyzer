/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*${header:start}*/
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "app.h"
#include "fsl_qspi.h"
#include "clock_config.h"
#include "board.h"
#include "emwin_support.h"
/*${header:end}*/

/*${macro:start}*/
#define I2C_RELEASE_SDA_PORT  PORTA
#define I2C_RELEASE_SCL_PORT  PORTA
#define I2C_RELEASE_SDA_GPIO  GPIOA
#define I2C_RELEASE_SDA_PIN   9U
#define I2C_RELEASE_SCL_GPIO  GPIOA
#define I2C_RELEASE_SCL_PIN   8U
#define I2C_RELEASE_BUS_COUNT 100U
/*${macro:end}*/

/*${function:start}*/
void BOARD_I2C_ReleaseBus(void);

void BOARD_InitHardware(void)
{
    const gpio_pin_config_t resetPinConfig = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic  = 1,
    };

    BOARD_BootClockRUN();
    BOARD_I2C_ReleaseBus();
    BOARD_InitPins();
    BOARD_InitDebugConsole();

    /* Use system clock as FlexIO clock source. */
    CLOCK_SetFlexio0Clock(0U);

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

static void i2c_release_bus_delay(void)
{
    SDK_DelayAtLeastUs(100U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
}

void BOARD_I2C_ReleaseBus(void)
{
    uint8_t i = 0;
    gpio_pin_config_t pin_config;
    port_pin_config_t i2c_pin_config = {0};

    /* Config pin mux as gpio */
    i2c_pin_config.pullSelect = kPORT_PullUp;
    i2c_pin_config.mux        = kPORT_MuxAsGpio;

    pin_config.pinDirection = kGPIO_DigitalOutput;
    pin_config.outputLogic  = 1U;
    CLOCK_EnableClock(kCLOCK_PortA);
    PORT_SetPinConfig(I2C_RELEASE_SCL_PORT, I2C_RELEASE_SCL_PIN, &i2c_pin_config);
    PORT_SetPinConfig(I2C_RELEASE_SCL_PORT, I2C_RELEASE_SDA_PIN, &i2c_pin_config);

    GPIO_PinInit(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, &pin_config);
    GPIO_PinInit(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, &pin_config);

    /* Drive SDA low first to simulate a start */
    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();

    /* Send 9 pulses on SCL and keep SDA high */
    for (i = 0; i < 9; i++)
    {
        GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
        i2c_release_bus_delay();

        GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
        i2c_release_bus_delay();

        GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
        i2c_release_bus_delay();
        i2c_release_bus_delay();
    }

    /* Send stop */
    GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
    i2c_release_bus_delay();

    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
    i2c_release_bus_delay();
}
/*${function:end}*/
