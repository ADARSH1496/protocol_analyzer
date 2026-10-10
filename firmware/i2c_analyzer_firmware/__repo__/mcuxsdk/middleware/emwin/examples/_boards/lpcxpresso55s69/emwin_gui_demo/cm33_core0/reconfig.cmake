
mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES middleware/emwin/examples/_boards/${board}/emwin_gui_demo/RTE_Device.h
            middleware/emwin/examples/_boards/${board}/common/pin_mux.c
            middleware/emwin/examples/_boards/${board}/common/pin_mux.h
            middleware/emwin/examples/_boards/${board}/common/${core_id}/hardware_init.c
            middleware/emwin/examples/_boards/${board}/common/${core_id}/app.h
)

mcux_add_include(
    BASE_PATH ${SdkRootDirPath}
    INCLUDES middleware/emwin/examples/_boards/${board}/emwin_gui_demo
)
