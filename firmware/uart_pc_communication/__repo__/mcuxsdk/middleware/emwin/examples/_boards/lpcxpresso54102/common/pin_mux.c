/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v3.0
processor: LPC54102J512
package_id: LPC54102J512BD64
mcu_data: ksdk2_0
processor_version: 0.0.2
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

#include "fsl_common.h"
#include "fsl_iocon.h"
#include "pin_mux.h"

/*FUNCTION**********************************************************************
 * 
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 * 
 *END**************************************************************************/
void BOARD_InitBootPins(void) {
    BOARD_InitPins();
}

#define IOCON_PIO_DIGITAL_EN          0x80u   /*!< Enables digital function */
#define IOCON_PIO_FUNC1               0x01u   /*!< Selects pin function 1 */
#define IOCON_PIO_INPFILT_OFF       0x0100u   /*!< Input filter disabled */
#define IOCON_PIO_INV_DI              0x00u   /*!< Input function is not inverted */
#define IOCON_PIO_MODE_INACT          0x00u   /*!< No addition pin function */
#define IOCON_PIO_OPENDRAIN_DI        0x00u   /*!< Open drain is disabled */
#define IOCON_PIO_SLEW_STANDARD       0x00u   /*!< Standard mode, output slew rate control is enabled */
#define PIN0_IDX                         0u   /*!< Pin number for pin 0 in a port 0 */
#define PIN1_IDX                         1u   /*!< Pin number for pin 1 in a port 0 */
#define PIO03_DIGIMODE_DIGITAL        0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO03_FUNC_ALT0               0x00u   /*!< Selects pin function.: Alternative connection 0. */
#define PORT0_IDX                        0u   /*!< Port index */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', coreID: cm4, enableClock: 'true'}
- pin_list:
  - {pin_num: '31', peripheral: USART0, signal: RXD, pin_signal: PIO0_0/U0_RXD/SPI0_SSELSN0/CT32B0_CAP0/SCT0_OUT3, mode: inactive, invert: disabled, glitch_filter: disabled,
    slew_rate: standard, open_drain: disabled}
  - {pin_num: '32', peripheral: USART0, signal: TXD, pin_signal: PIO0_1/U0_TXD/SPI0_SSELSN1/CT32B0_CAP1/SCT0_OUT1, mode: inactive, invert: disabled, glitch_filter: disabled,
    slew_rate: standard, open_drain: disabled}
  - {pin_num: '37', peripheral: GPIO, signal: 'PIO0, 3', pin_signal: PIO0_3/U0_RTS/CT32B1_MAT3}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitPins(void) { /* Function assigned for the Cortex-M0P */
  CLOCK_EnableClock(kCLOCK_Iocon);                           /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */

  const uint32_t port0_pin0_config = (
    IOCON_PIO_FUNC1 |                                        /* Pin is configured as U0_RXD */
    IOCON_PIO_MODE_INACT |                                   /* No addition pin function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    IOCON_PIO_SLEW_STANDARD |                                /* Standard mode, output slew rate control is enabled */
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN0_IDX, port0_pin0_config); /* PORT0 PIN0 (coords: 31) is configured as U0_RXD */
  const uint32_t port0_pin1_config = (
    IOCON_PIO_FUNC1 |                                        /* Pin is configured as U0_TXD */
    IOCON_PIO_MODE_INACT |                                   /* No addition pin function */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    IOCON_PIO_SLEW_STANDARD |                                /* Standard mode, output slew rate control is enabled */
    IOCON_PIO_OPENDRAIN_DI                                   /* Open drain is disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN1_IDX, port0_pin1_config); /* PORT0 PIN1 (coords: 32) is configured as U0_TXD */
  IOCON->PIO[0][3] = ((IOCON->PIO[0][3] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO03_FUNC_ALT0)                      /* Selects pin function.: PORT03 (pin 37) is configured as PIO0_3 */
      | IOCON_PIO_DIGIMODE(PIO03_DIGIMODE_DIGITAL)           /* Select Analog/Digital mode.: Digital mode. */
    );
}


#define IOCON_PIO_DIGITAL_EN          0x80u   /*!< Enables digital function */
#define IOCON_PIO_FUNC1               0x01u   /*!< Selects pin function 1 */
#define IOCON_PIO_I2CDRIVE_LOW        0x00u   /*!< Low drive: 4 mA */
#define IOCON_PIO_I2CFILTER_DI      0x0400u   /*!< I2C 50 ns glitch filter disabled */
#define IOCON_PIO_I2CSLEW_I2C         0x00u   /*!< I2C mode */
#define IOCON_PIO_INPFILT_OFF       0x0100u   /*!< Input filter disabled */
#define IOCON_PIO_INV_DI              0x00u   /*!< Input function is not inverted */
#define PIN23_IDX                       23u   /*!< Pin number for pin 23 in a port 0 */
#define PIN24_IDX                       24u   /*!< Pin number for pin 24 in a port 0 */
#define PORT0_IDX                        0u   /*!< Port index */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
I2C0_InitPins:
- options: {coreID: cm4, enableClock: 'true'}
- pin_list:
  - {pin_num: '1', peripheral: I2C0, signal: SCL, pin_signal: PIO0_23/I2C0_SCL/CT32B0_CAP0, invert: disabled, glitch_filter: disabled, i2c_slew: i2c, i2c_drive: low,
    i2c_filter: disabled}
  - {pin_num: '2', peripheral: I2C0, signal: SDA, pin_signal: PIO0_24/I2C0_SDA/CT32B0_CAP1/CT32B0_MAT0, invert: disabled, glitch_filter: disabled, i2c_slew: i2c,
    i2c_drive: low, i2c_filter: disabled}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : I2C0_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void I2C0_InitPins(void) { /* Function assigned for the Cortex-M0P */
  CLOCK_EnableClock(kCLOCK_Iocon);                           /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */

  const uint32_t port0_pin23_config = (
    IOCON_PIO_FUNC1 |                                        /* Pin is configured as I2C0_SCL */
    IOCON_PIO_I2CSLEW_I2C |                                  /* I2C mode */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    IOCON_PIO_I2CDRIVE_LOW |                                 /* Low drive: 4 mA */
    IOCON_PIO_I2CFILTER_DI                                   /* I2C 50 ns glitch filter disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN23_IDX, port0_pin23_config); /* PORT0 PIN23 (coords: 1) is configured as I2C0_SCL */
  const uint32_t port0_pin24_config = (
    IOCON_PIO_FUNC1 |                                        /* Pin is configured as I2C0_SDA */
    IOCON_PIO_I2CSLEW_I2C |                                  /* I2C mode */
    IOCON_PIO_INV_DI |                                       /* Input function is not inverted */
    IOCON_PIO_DIGITAL_EN |                                   /* Enables digital function */
    IOCON_PIO_INPFILT_OFF |                                  /* Input filter disabled */
    IOCON_PIO_I2CDRIVE_LOW |                                 /* Low drive: 4 mA */
    IOCON_PIO_I2CFILTER_DI                                   /* I2C 50 ns glitch filter disabled */
  );
  IOCON_PinMuxSet(IOCON, PORT0_IDX, PIN24_IDX, port0_pin24_config); /* PORT0 PIN24 (coords: 2) is configured as I2C0_SDA */
}


#define PIO023_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO023_FUNC_ALT0              0x00u   /*!< Selects pin function.: Alternative connection 0. */
#define PIO024_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO024_FUNC_ALT0              0x00u   /*!< Selects pin function.: Alternative connection 0. */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
I2C0_DeinitPins:
- options: {coreID: cm4, enableClock: 'true'}
- pin_list:
  - {pin_num: '1', peripheral: GPIO, signal: 'PIO0, 23', pin_signal: PIO0_23/I2C0_SCL/CT32B0_CAP0}
  - {pin_num: '2', peripheral: GPIO, signal: 'PIO0, 24', pin_signal: PIO0_24/I2C0_SDA/CT32B0_CAP1/CT32B0_MAT0}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : I2C0_DeinitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void I2C0_DeinitPins(void) { /* Function assigned for the Cortex-M0P */
  CLOCK_EnableClock(kCLOCK_Iocon);                           /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */

  IOCON->PIO[0][23] = ((IOCON->PIO[0][23] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO023_FUNC_ALT0)                     /* Selects pin function.: PORT023 (pin 1) is configured as PIO0_23 */
      | IOCON_PIO_DIGIMODE(PIO023_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[0][24] = ((IOCON->PIO[0][24] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO024_FUNC_ALT0)                     /* Selects pin function.: PORT024 (pin 2) is configured as PIO0_24 */
      | IOCON_PIO_DIGIMODE(PIO024_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
    );
}


#define PIO114_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO114_FUNC_ALT4              0x04u   /*!< Selects pin function.: Alternative connection 4. */
#define PIO115_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO115_FUNC_ALT4              0x04u   /*!< Selects pin function.: Alternative connection 4. */
#define PIO16_DIGIMODE_DIGITAL        0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO16_FUNC_ALT2               0x02u   /*!< Selects pin function.: Alternative connection 2. */
#define PIO17_DIGIMODE_DIGITAL        0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO17_FUNC_ALT2               0x02u   /*!< Selects pin function.: Alternative connection 2. */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
SPI_InitPins:
- options: {coreID: cm4, enableClock: 'true'}
- pin_list:
  - {pin_num: '26', peripheral: SPI1, signal: SCK, pin_signal: PIO1_6/ADC_9/SPI1_SCK/CT32B1_CAP2/CT32B1_MAT2}
  - {pin_num: '57', peripheral: SPI1, signal: MISO, pin_signal: PIO1_14/U2_RXD/SCT0_OUT7/SPI1_MISO}
  - {pin_num: '27', peripheral: SPI1, signal: MOSI, pin_signal: PIO1_7/ADC_10/SPI1_MOSI/CT32B1_MAT2/CT32B1_CAP2}
  - {pin_num: '62', peripheral: SPI1, signal: SSELSN0, pin_signal: PIO1_15/SCT0_OUT5/CT32B1_CAP3/SPI1_SSELSN0}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI1_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void SPI1_InitPins(void) { /* Function assigned for the Cortex-M0P */
  CLOCK_EnableClock(kCLOCK_Iocon);                           /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */

  IOCON->PIO[1][14] = ((IOCON->PIO[1][14] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO114_FUNC_ALT4)                     /* Selects pin function.: PORT114 (pin 57) is configured as SPI1_MISO */
      | IOCON_PIO_DIGIMODE(PIO114_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[1][15] = ((IOCON->PIO[1][15] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO115_FUNC_ALT4)                     /* Selects pin function.: PORT115 (pin 62) is configured as SPI1_SSELSN0 */
      | IOCON_PIO_DIGIMODE(PIO115_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[1][6] = ((IOCON->PIO[1][6] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO16_FUNC_ALT2)                      /* Selects pin function.: PORT16 (pin 26) is configured as SPI1_SCK */
      | IOCON_PIO_DIGIMODE(PIO16_DIGIMODE_DIGITAL)           /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[1][7] = ((IOCON->PIO[1][7] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO17_FUNC_ALT2)                      /* Selects pin function.: PORT17 (pin 27) is configured as SPI1_MOSI */
      | IOCON_PIO_DIGIMODE(PIO17_DIGIMODE_DIGITAL)           /* Select Analog/Digital mode.: Digital mode. */
    );
}


#define PIO114_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO114_FUNC_ALT0              0x00u   /*!< Selects pin function.: Alternative connection 0. */
#define PIO115_DIGIMODE_DIGITAL       0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO115_FUNC_ALT0              0x00u   /*!< Selects pin function.: Alternative connection 0. */
#define PIO16_DIGIMODE_DIGITAL        0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO16_FUNC_ALT0               0x00u   /*!< Selects pin function.: Alternative connection 0. */
#define PIO17_DIGIMODE_DIGITAL        0x01u   /*!< Select Analog/Digital mode.: Digital mode. */
#define PIO17_FUNC_ALT0               0x00u   /*!< Selects pin function.: Alternative connection 0. */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
SPI_DeinitPins:
- options: {coreID: cm4, enableClock: 'true'}
- pin_list:
  - {pin_num: '26', peripheral: GPIO, signal: 'PIO1, 6', pin_signal: PIO1_6/ADC_9/SPI1_SCK/CT32B1_CAP2/CT32B1_MAT2}
  - {pin_num: '57', peripheral: GPIO, signal: 'PIO1, 14', pin_signal: PIO1_14/U2_RXD/SCT0_OUT7/SPI1_MISO}
  - {pin_num: '27', peripheral: GPIO, signal: 'PIO1, 7', pin_signal: PIO1_7/ADC_10/SPI1_MOSI/CT32B1_MAT2/CT32B1_CAP2}
  - {pin_num: '62', peripheral: GPIO, signal: 'PIO1, 15', pin_signal: PIO1_15/SCT0_OUT5/CT32B1_CAP3/SPI1_SSELSN0}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : SPI1_DeinitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void SPI1_DeinitPins(void) { /* Function assigned for the Cortex-M0P */
  CLOCK_EnableClock(kCLOCK_Iocon);                           /* Enables the clock for the IOCON block. 0 = Disable; 1 = Enable.: 0x01u */

  IOCON->PIO[1][14] = ((IOCON->PIO[1][14] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO114_FUNC_ALT0)                     /* Selects pin function.: PORT114 (pin 57) is configured as PIO1_14 */
      | IOCON_PIO_DIGIMODE(PIO114_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[1][15] = ((IOCON->PIO[1][15] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO115_FUNC_ALT0)                     /* Selects pin function.: PORT115 (pin 62) is configured as PIO1_15 */
      | IOCON_PIO_DIGIMODE(PIO115_DIGIMODE_DIGITAL)          /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[1][6] = ((IOCON->PIO[1][6] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO16_FUNC_ALT0)                      /* Selects pin function.: PORT16 (pin 26) is configured as PIO1_6 */
      | IOCON_PIO_DIGIMODE(PIO16_DIGIMODE_DIGITAL)           /* Select Analog/Digital mode.: Digital mode. */
    );
  IOCON->PIO[1][7] = ((IOCON->PIO[1][7] &
    (~(IOCON_PIO_FUNC_MASK | IOCON_PIO_DIGIMODE_MASK)))      /* Mask bits to zero which are setting */
      | IOCON_PIO_FUNC(PIO17_FUNC_ALT0)                      /* Selects pin function.: PORT17 (pin 27) is configured as PIO1_7 */
      | IOCON_PIO_DIGIMODE(PIO17_DIGIMODE_DIGITAL)           /* Select Analog/Digital mode.: Digital mode. */
    );
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
