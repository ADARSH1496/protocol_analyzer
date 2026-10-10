cmake_minimum_required(VERSION 3.22.0)

include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

project(emwin_serial_terminal LANGUAGES C CXX ASM PROJECT_BOARD_PORT_PATH middleware/emwin/examples/_boards/${board}/emwin_serial_terminal)

include(${SdkRootDirPath}/CMakeLists.txt)
mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES middleware/emwin/examples/emwin_serial_terminal/emwin_serial_terminal.c
            middleware/emwin/examples/_boards/${board}/common/emwin_support.c
            middleware/emwin/examples/_boards/${board}/common/emwin_support.h
)

mcux_add_include(
    BASE_PATH ${SdkRootDirPath}
    INCLUDES middleware/emwin/examples/_boards/${board}/common
             middleware/emwin/examples/_boards/${board}/common/${core_id}
)

include(${SdkRootDirPath}/middleware/emwin/examples/_boards/${board}/emwin_serial_terminal/reconfig.cmake OPTIONAL)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)