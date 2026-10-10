
mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES middleware/emwin/examples/_boards/${board}/common/pin_mux.c
            middleware/emwin/examples/_boards/${board}/common/pin_mux.h
            middleware/emwin/examples/_boards/${board}/common/hardware_init.c
            middleware/emwin/examples/_boards/${board}/common/app.h
)

mcux_add_macro(
    CC "-DBOARD_ENABLE_PSRAM_CACHE=0\
       -DSSD1963_DATA_WITDH=8\
       -DFLEXIO_MCULCD_DATA_BUS_WIDTH=8"
)

mcux_add_iar_configuration(
    LD "--config_def=__stack_size__=0x1000"
)
mcux_add_mdk_configuration(
    LD "--predefine=\"-D__stack_size__=0x1000\""
)
mcux_add_armgcc_configuration(
    LD "-Xlinker --defsym=__stack_size__=0x1000"
)
