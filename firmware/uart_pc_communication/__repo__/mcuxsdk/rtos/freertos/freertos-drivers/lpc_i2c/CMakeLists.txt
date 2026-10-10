#
# Copyright 2024 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

if (CONFIG_MCUX_COMPONENT_driver.lpc_i2c_freertos)

    mcux_component_version(2.1.0)

    mcux_add_source(
        SOURCES
            fsl_i2c_freertos.h
            fsl_i2c_freertos.c
    )

    mcux_add_include(
        INCLUDES .
    )
endif()
