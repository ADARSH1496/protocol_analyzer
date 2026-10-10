/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_


/*******************************************************************************
 * Definitions
 ******************************************************************************/  

/*! @brief Direction type  */
typedef enum _pin_mux_direction
{
  kPIN_MUX_DirectionInput = 0U,         /* Input direction */
  kPIN_MUX_DirectionOutput = 1U,        /* Output direction */
  kPIN_MUX_DirectionInputOrOutput = 2U  /* Input or output direction */
} pin_mux_direction_t;

/*!
 * @addtogroup pin_mux
 * @{
 */

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

/* U0_RXD (number 31), U13[4]/TO_MUX_P0_0-ISP_RX */
#define BOARD_INITPINS_DEBUG_UART_RX_PERIPHERAL                           USART0   /*!< Device name: USART0 */
#define BOARD_INITPINS_DEBUG_UART_RX_SIGNAL                                  RXD   /*!< USART0 signal: RXD */
#define BOARD_INITPINS_DEBUG_UART_RX_PIN_NAME                             U0_RXD   /*!< Pin name */
#define BOARD_INITPINS_DEBUG_UART_RX_LABEL           "U13[4]/TO_MUX_P0_0-ISP_RX"   /*!< Label */
#define BOARD_INITPINS_DEBUG_UART_RX_NAME                        "DEBUG_UART_RX"   /*!< Identifier name */

/* U0_TXD (number 32), U15[3]/U4[4]/P0_1-ISP_TX */
#define BOARD_INITPINS_DEBUG_UART_TX_PERIPHERAL                           USART0   /*!< Device name: USART0 */
#define BOARD_INITPINS_DEBUG_UART_TX_SIGNAL                                  TXD   /*!< USART0 signal: TXD */
#define BOARD_INITPINS_DEBUG_UART_TX_PIN_NAME                             U0_TXD   /*!< Pin name */
#define BOARD_INITPINS_DEBUG_UART_TX_LABEL            "U15[3]/U4[4]/P0_1-ISP_TX"   /*!< Label */
#define BOARD_INITPINS_DEBUG_UART_TX_NAME                        "DEBUG_UART_TX"   /*!< Identifier name */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void); /* Function assigned for the Cortex-M0P */

/* I2C0_SDA (number 2), J1[3]/SW1/P0_24-I2C0_SDA-WAKEUP */
#define I2C0_INITPINS_SW1_PERIPHERAL                                        I2C0   /*!< Device name: I2C0 */
#define I2C0_INITPINS_SW1_SIGNAL                                             SDA   /*!< I2C0 signal: SDA */
#define I2C0_INITPINS_SW1_PIN_NAME                                      I2C0_SDA   /*!< Pin name */
#define I2C0_INITPINS_SW1_LABEL                "J1[3]/SW1/P0_24-I2C0_SDA-WAKEUP"   /*!< Label */
#define I2C0_INITPINS_SW1_NAME                                             "SW1"   /*!< Identifier name */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void I2C0_InitPins(void); /* Function assigned for the Cortex-M0P */

/* PIO0_24 (number 2), J1[3]/SW1/P0_24-I2C0_SDA-WAKEUP */
#define I2C0_DEINITPINS_SW1_GPIO                                            GPIO   /*!< GPIO device name: GPIO */
#define I2C0_DEINITPINS_SW1_PORT                                              0U   /*!< PORT device index: 0 */
#define I2C0_DEINITPINS_SW1_GPIO_PIN                                         24U   /*!< PIO0 pin index: 24 */
#define I2C0_DEINITPINS_SW1_PIN_NAME                                     PIO0_24   /*!< Pin name */
#define I2C0_DEINITPINS_SW1_LABEL              "J1[3]/SW1/P0_24-I2C0_SDA-WAKEUP"   /*!< Label */
#define I2C0_DEINITPINS_SW1_NAME                                           "SW1"   /*!< Identifier name */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void I2C0_DeinitPins(void); /* Function assigned for the Cortex-M0P */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void SPI_InitPins(void); /* Function assigned for the Cortex-M0P */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void SPI_DeinitPins(void); /* Function assigned for the Cortex-M0P */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/*******************************************************************************
 * EOF
 ******************************************************************************/
