cmake_minimum_required(VERSION 3.22.0)

include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

project(emwin_slide_show LANGUAGES C CXX ASM PROJECT_BOARD_PORT_PATH middleware/emwin/examples/_boards/${board}/emwin_slide_show)

include(${SdkRootDirPath}/CMakeLists.txt)
mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES middleware/emwin/examples/emwin_slide_show/emwin_slide_show.c
            middleware/emwin/examples/emwin_slide_show/Alarm.c
            middleware/emwin/examples/emwin_slide_show/Info.c
            middleware/emwin/examples/emwin_slide_show/Weather.c
            middleware/emwin/examples/emwin_slide_show/Resource.h
            middleware/emwin/examples/emwin_slide_show/bmNXP_110x44.c
            middleware/emwin/examples/emwin_slide_show/bmSeggerLogo_110x55.c
            middleware/emwin/examples/emwin_slide_show/bmBell_40x40.c
            middleware/emwin/examples/emwin_slide_show/bmBell_60x60.c
            middleware/emwin/examples/emwin_slide_show/bmArrowLeft_23x24.c
            middleware/emwin/examples/emwin_slide_show/bmArrowRight_23x24.c
            middleware/emwin/examples/emwin_slide_show/bmRainy_45x34.c
            middleware/emwin/examples/emwin_slide_show/bmStormy_45x34.c
            middleware/emwin/examples/emwin_slide_show/bmSunny_45x34.c
            middleware/emwin/examples/emwin_slide_show/AA4_B_20.c
            middleware/emwin/examples/emwin_slide_show/AA4_B_40.c
            middleware/emwin/examples/emwin_slide_show/AA4_B_100.c
            middleware/emwin/examples/emwin_slide_show/Font_13_R.c
            middleware/emwin/examples/_boards/${board}/common/emwin_support.c
            middleware/emwin/examples/_boards/${board}/common/emwin_support.h
)

mcux_add_include(
    BASE_PATH ${SdkRootDirPath}
    INCLUDES middleware/emwin/examples/_boards/${board}/common
             middleware/emwin/examples/_boards/${board}/emwin_slide_show
             middleware/emwin/examples/emwin_slide_show
             middleware/emwin/examples/_boards/${board}/common/${core_id}
)

include(${SdkRootDirPath}/middleware/emwin/examples/_boards/${board}/emwin_slide_show/reconfig.cmake OPTIONAL)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)