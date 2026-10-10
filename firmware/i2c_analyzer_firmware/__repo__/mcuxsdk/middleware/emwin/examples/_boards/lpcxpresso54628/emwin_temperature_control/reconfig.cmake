
mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES middleware/emwin/examples/emwin_temperature_control/bmButtonRound_50x22.c
            middleware/emwin/examples/emwin_temperature_control/bmButtonRoundPressed_50x22.c
            middleware/emwin/examples/emwin_temperature_control/bmKNOB_124x124.c
            middleware/emwin/examples/emwin_temperature_control/bmSEGGER_80x40.c
            middleware/emwin/examples/emwin_temperature_control/Font_AA4_16_B.c
            middleware/emwin/examples/emwin_temperature_control/Font_AA4_16_L.c
            middleware/emwin/examples/emwin_temperature_control/Resource.h
            middleware/emwin/examples/_boards/${board}/common/pin_mux.c
            middleware/emwin/examples/_boards/${board}/common/pin_mux.h
            middleware/emwin/examples/_boards/${board}/common/hardware_init.c
            middleware/emwin/examples/_boards/${board}/common/app.h
)

mcux_add_include(
    BASE_PATH ${SdkRootDirPath}
    INCLUDES middleware/emwin/examples/emwin_temperature_control
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
