
mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES middleware/emwin/examples/_boards/${board}/common/pin_mux.c
            middleware/emwin/examples/_boards/${board}/common/pin_mux.h
            middleware/emwin/examples/_boards/${board}/common/hardware_init.c
            middleware/emwin/examples/_boards/${board}/common/app.h
)

mcux_add_source(
    BASE_PATH ${SdkRootDirPath}
    SOURCES middleware/emwin/examples/emwin_temperature_control/AppWizard/Resource/Font/*.c
            middleware/emwin/examples/emwin_temperature_control/AppWizard/Resource/Font/*.xbf
            middleware/emwin/examples/emwin_temperature_control/AppWizard/Resource/Image/*.c
            middleware/emwin/examples/emwin_temperature_control/AppWizard/Resource/Image/*.png
            middleware/emwin/examples/emwin_temperature_control/AppWizard/Resource/Image/*.bmp
            middleware/emwin/examples/emwin_temperature_control/AppWizard/Resource/Text/*.c
            middleware/emwin/examples/emwin_temperature_control/AppWizard/Resource/Text/*.txt
            middleware/emwin/examples/emwin_temperature_control/AppWizard/Source/*.c
            middleware/emwin/examples/emwin_temperature_control/AppWizard/Source/CustomCode/*.c
            middleware/emwin/examples/emwin_temperature_control/AppWizard/Source/CustomCode/*.h
            middleware/emwin/examples/emwin_temperature_control/AppWizard/Source/Generated/*.c
            middleware/emwin/examples/emwin_temperature_control/AppWizard/Source/Generated/*.h
            middleware/emwin/examples/emwin_temperature_control/AppWizard/*.c
            middleware/emwin/examples/emwin_temperature_control/AppWizard/TemperatureControl.AppWizard
)

mcux_add_include(
    BASE_PATH ${SdkRootDirPath}
    INCLUDES middleware/emwin/examples/emwin_temperature_control/AppWizard/Source/CustomCode
            middleware/emwin/examples/emwin_temperature_control/AppWizard/Source/Generated
)

mcux_add_macro(
    CC "-DAPPWIZARD"
)