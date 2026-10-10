
mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES middleware/emwin/examples/_boards/${board}/emwin_gui_demo/RTE_Device.h
            middleware/emwin/examples/_boards/${board}/common/pin_mux.c
            middleware/emwin/examples/_boards/${board}/common/pin_mux.h
            middleware/emwin/examples/_boards/${board}/common/hardware_init.c
            middleware/emwin/examples/_boards/${board}/common/app.h
)

mcux_add_include(
    INCLUDES .
)
