/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RTE_DEVICE_H
#define _RTE_DEVICE_H

extern void SPI1_InitPins();
extern void SPI1_DeinitPins();
extern void I2C0_InitPins();
extern void I2C0_DeinitPins();
extern void SPI1_InitPins();
extern void SPI1_DeinitPins();

/* Driver name mapping. */
/* User needs to provide the implementation of SPIX_GetFreq/SPIX_InitPins/SPIX_DeinitPins for the enabled SPI instance.
 */
#define RTE_SPI1            1
#define RTE_SPI1_PIN_INIT   SPI1_InitPins
#define RTE_SPI1_PIN_DEINIT SPI1_DeinitPins
#define RTE_SPI1_DMA_EN     0

/* User needs to provide the implementation of I2CX_GetFreq/I2CX_InitPins/I2CX_DeinitPins for the enabled I2C instance.
 */
#define RTE_I2C0            1
#define RTE_I2C0_PIN_INIT   I2C0_InitPins
#define RTE_I2C0_PIN_DEINIT I2C0_DeinitPins
#define RTE_I2C0_DMA_EN     0

/* SPI configuration. */
#define SPI0_RX_FIFO_SIZE 8
#define SPI0_TX_FIFO_SIZE 8
#define SPI1_RX_FIFO_SIZE 8
#define SPI1_TX_FIFO_SIZE 8

#define RTE_SPI1_SSEL_NUM        kSPI_Ssel0
#define RTE_SPI1_PIN_INIT        SPI1_InitPins
#define RTE_SPI1_PIN_DEINIT      SPI1_DeinitPins
#define RTE_SPI1_DMA_TX_CH       11
#define RTE_SPI1_DMA_TX_DMA_BASE DMA0
#define RTE_SPI1_DMA_RX_CH       10
#define RTE_SPI1_DMA_RX_DMA_BASE DMA0

#endif /* _RTE_DEVICE_H */
