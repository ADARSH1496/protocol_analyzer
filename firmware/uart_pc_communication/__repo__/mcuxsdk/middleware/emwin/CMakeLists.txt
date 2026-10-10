
if(CONFIG_MCUX_COMPONENT_middleware.emwin.template)
    mcux_component_version(${emwin_version})
    mcux_add_source(
        SOURCES template/emwin_support.c
                template/emwin_support.h
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_include(
        INCLUDES emWin_Config
                 emWin_header
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_include(
        TARGET_FILES emwin_support.h
        INCLUDES template
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
endif()

if(CONFIG_MCUX_COMPONENT_middleware.emwin)
    mcux_component_version(${emwin_version})
    mcux_add_source(
        SOURCES ChangeLogKSDK.txt
                readme.txt
                emWin_header/*.h
                emWin_Config/*.h
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/ARMGCC/libemWin_M0P.a
        TOOLCHAINS armgcc mcux
        CORES cm0p
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/IAR/emWin_M0P.a
        TOOLCHAINS iar
        CORES cm0p
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/Keil/emWin_M0P.lib
        TOOLCHAINS mdk
        CORES cm0p
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/ARMGCC/libemWin_M4F.a
        TOOLCHAINS armgcc mcux
        CORES cm4f
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/IAR/emWin_M4F.a
        TOOLCHAINS iar
        CORES cm4f
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/Keil/emWin_M4F.lib
        TOOLCHAINS mdk
        CORES cm4f
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/ARMGCC/libemWin_M7.a
        TOOLCHAINS armgcc mcux
        CORES cm7f
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/IAR/emWin_M7.a
        TOOLCHAINS iar
        CORES cm7f
        FPU DP_FPU
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/IAR/emWin_M7_sp.a
        TOOLCHAINS iar
        CORES cm7f
        FPU SP_FPU
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/Keil/emWin_M7.lib
        TOOLCHAINS mdk
        CORES cm7f
        FPU DP_FPU
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/Keil/emWin_M7_sp.lib
        TOOLCHAINS mdk
        CORES cm7f
        FPU SP_FPU
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/ARMGCC/libemWin_M33.a
        TOOLCHAINS armgcc mcux
        CORES cm33
        DSP HAS_DSP
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/IAR/emWin_M33.a
        TOOLCHAINS iar
        CORES cm33
        DSP HAS_DSP
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/Keil/emWin_M33.lib
        TOOLCHAINS mdk
        CORES cm33
        DSP HAS_DSP
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/ARMGCC/libemWin_M33_nodsp.a
        TOOLCHAINS armgcc mcux
        CORES cm33
        FPU NO_FPU
        DSP NO_DSP
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/IAR/emWin_M33_nodsp.a
        TOOLCHAINS iar
        CORES cm33
        FPU NO_FPU
        DSP NO_DSP
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/Keil/emWin_M33_nodsp.lib
        TOOLCHAINS mdk
        CORES cm33
        FPU NO_FPU
        DSP NO_DSP
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/ARMGCC/libemWin_M33_nodsp_fpu.a
        TOOLCHAINS armgcc mcux
        CORES cm33
        FPU SP_FPU
        DSP NO_DSP
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/IAR/emWin_M33_nodsp_fpu.a
        TOOLCHAINS iar
        CORES cm33
        FPU SP_FPU
        DSP NO_DSP
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/Keil/emWin_M33_nodsp_fpu.lib
        TOOLCHAINS mdk
        CORES cm33
        FPU SP_FPU
        DSP NO_DSP
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/ARMGCC/libemWin_M33_nodsp_nofpu.a
        TOOLCHAINS armgcc mcux
        CORES cm33
        FPU NO_FPU
        DSP NO_DSP
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/IAR/emWin_M33_nodsp_nofpu.a
        TOOLCHAINS iar
        CORES cm33
        FPU NO_FPU
        DSP NO_DSP
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_library(
        LIBS emWin_library/Keil/emWin_M33_nodsp_nofpu.lib
        TOOLCHAINS mdk
        CORES cm33
        FPU NO_FPU
        DSP NO_DSP
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
    mcux_add_include(
        INCLUDES emWin_header
                 emWin_Config
        BASE_PATH ${SdkRootDirPath}/middleware/emwin/
    )
endif()

