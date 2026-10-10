
mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES middleware/emwin/examples/_boards/${board}/common/pin_mux.c
            middleware/emwin/examples/_boards/${board}/common/pin_mux.h
            middleware/emwin/examples/_boards/${board}/common/hardware_init.c
            middleware/emwin/examples/_boards/${board}/common/app.h
)

mcux_add_mdk_configuration(
    TARGETS flexspi_nor_debug flexspi_nor_release
    LD "--keep=*(.boot_hdr.dcd_data)"
)

mcux_add_macro(
    CC "-DSKIP_SYSCLK_INIT\
       -DXIP_BOOT_HEADER_DCD_ENABLE=1"
)
