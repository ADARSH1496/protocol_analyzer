
mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES middleware/emwin/examples/_boards/${board}/common/pin_mux.c
            middleware/emwin/examples/_boards/${board}/common/pin_mux.h
            middleware/emwin/examples/_boards/${board}/common/${core_id}/hardware_init.c
            middleware/emwin/examples/_boards/${board}/common/${core_id}/app.h
)

mcux_add_linker_symbol(
    SYMBOLS "__stack_size__=0x1000"
)

mcux_add_configuration(
        CC "-DSDK_I2C_BASED_COMPONENT_USED=1\
            -DBOARD_ENABLE_PSRAM_CACHE=0\
            -DSSD1963_DATA_WITDH=8\
            -DFLEXIO_MCULCD_DATA_BUS_WIDTH=8\
            -DMCUX_DBI_LEGACY=0\
            -DDBI_USE_MIPI_PANEL=1\
            -DSDK_DEBUGCONSOLE_UART=1"
)