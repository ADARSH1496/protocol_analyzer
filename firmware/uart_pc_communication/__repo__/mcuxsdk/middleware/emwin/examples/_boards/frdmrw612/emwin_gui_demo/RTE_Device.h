/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RTE_DEVICE_H
#define _RTE_DEVICE_H

extern void I2C2_InitPins();

/* User needs to provide the implementation of I2CX_GetFreq/I2CX_InitPins/I2CX_DeinitPins for the enabled I2C
 * instance. */
#define RTE_I2C2          1
#define RTE_I2C2_PIN_INIT I2C2_InitPins
#define RTE_I2C2_DMA_EN   0

#endif /* _RTE_DEVICE_H */
