#
# Copyright 2024 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

if (CONFIG_MCUX_COMPONENT_driver.flexcomm_spi_freertos)

    mcux_component_version(2.1.0)

    mcux_add_source(
        SOURCES
            fsl_spi_freertos.h
            fsl_spi_freertos.c
    )

    mcux_add_include(
        INCLUDES .
    )
endif()
