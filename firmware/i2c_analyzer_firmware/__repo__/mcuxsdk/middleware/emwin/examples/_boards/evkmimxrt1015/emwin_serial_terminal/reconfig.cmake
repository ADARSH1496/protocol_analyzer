
mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES middleware/emwin/examples/_boards/${board}/emwin_serial_terminal/RTE_Device.h
            middleware/emwin/examples/_boards/${board}/common/pin_mux.c
            middleware/emwin/examples/_boards/${board}/common/pin_mux.h
            middleware/emwin/examples/_boards/${board}/common/hardware_init.c
            middleware/emwin/examples/_boards/${board}/common/app.h
)

mcux_add_include(
    INCLUDES .
)

# Add or remove Linker File Configurations
mcux_remove_iar_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS flexspi_nor_debug flexspi_nor_release
    LINKER devices/RT/RT1015/MIMXRT1015/iar/MIMXRT1015xxxxx_flexspi_nor.icf
)
mcux_remove_mdk_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS flexspi_nor_debug flexspi_nor_release
    LINKER devices/RT/RT1015/MIMXRT1015/arm/MIMXRT1015xxxxx_flexspi_nor.scf
)
mcux_remove_armgcc_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS flexspi_nor_debug flexspi_nor_release
    LINKER devices/RT/RT1015/MIMXRT1015/gcc/MIMXRT1015xxxxx_flexspi_nor.ld
)

# Add or remove Linker File Configurations
mcux_add_iar_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS flexspi_nor_debug flexspi_nor_release
    LINKER devices/RT/RT1015/MIMXRT1015/iar/MIMXRT1015xxxxx_flexspi_nor_ocram.icf
)
mcux_add_mdk_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS flexspi_nor_debug flexspi_nor_release
    LINKER devices/RT/RT1015/MIMXRT1015/arm/MIMXRT1015xxxxx_flexspi_nor_ocram.scf
)
mcux_add_armgcc_linker_script(
    BASE_PATH ${SdkRootDirPath}
    TARGETS flexspi_nor_debug flexspi_nor_release
    LINKER devices/RT/RT1015/MIMXRT1015/gcc/MIMXRT1015xxxxx_flexspi_nor_ocram.ld
)