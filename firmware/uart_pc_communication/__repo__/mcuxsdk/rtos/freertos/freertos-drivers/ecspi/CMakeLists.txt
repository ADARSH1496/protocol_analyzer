#
# Copyright 2024 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

if (CONFIG_MCUX_COMPONENT_driver.ecspi_freertos)

    mcux_component_version(2.2.0)

    mcux_add_source(
        SOURCES
            fsl_ecspi_freertos.h
            fsl_ecspi_freertos.c
    )

    mcux_add_include(
        INCLUDES .
    )
endif()
