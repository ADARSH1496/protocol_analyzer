# meterlib (MMAU)
if (CONFIG_MCUX_COMPONENT_middleware.metering.meterlib)
	mcux_component_version(4.1.0)

    mcux_add_include(
        INCLUDES
            meterlib
            meterlib/inc
    )

    mcux_add_source(
        SOURCES
            meterlib/inc/meterlib.h
    )

    mcux_add_library(
        LIBS meterlib/meterlib_cm0p_mmau_armcc.lib
        TOOLCHAINS mdk
        CORES cm0p
    )

    mcux_add_library(
        LIBS meterlib/libmeter_cm0p_mmau_gcc.a
        TOOLCHAINS armgcc mcux
        CORES cm0p
    )

    mcux_add_library(
        LIBS meterlib/meterlib_cm0p_mmau_iar.a
        TOOLCHAINS iar
        CORES cm0p
    )
endif()

# meterliblp (MMAU)
if (CONFIG_MCUX_COMPONENT_middleware.metering.meterliblp)
	mcux_component_version(4.1.0)
	
    mcux_add_include(
        INCLUDES
            meterliblp
            meterliblp/inc
    )

    mcux_add_source(
        SOURCES
            meterliblp/inc/meterliblp.h
    )

    mcux_add_library(
        LIBS meterliblp/meterliblp_cm0p_mmau_armcc.lib
        TOOLCHAINS mdk
        CORES cm0p
    )

    mcux_add_library(
        LIBS meterliblp/libmeterlp_cm0p_mmau_gcc.a
        TOOLCHAINS armgcc mcux
        CORES cm0p
    )

    mcux_add_library(
        LIBS meterliblp/meterliblp_cm0p_mmau_iar.a
        TOOLCHAINS iar
        CORES cm0p
    )
endif()

# meterfft (MMAU)
if (CONFIG_MCUX_COMPONENT_middleware.metering.meterfft)
	mcux_component_version(4.1.0)
	
    mcux_add_include(
        INCLUDES
            meterlibFFT/inc
            meterlibFFT/inc/metering
            meterlibFFT/inc/math
            meterlibFFT/inc/FFT
    )

    mcux_add_source(
        SOURCES
            meterlibFFT/inc/meterlibfft.h
            meterlibFFT/inc/metering/metering.h
            meterlibFFT/inc/math/SIN_LUT1024.h
            meterlibFFT/inc/math/ATAN_LUT2048.h
            meterlibFFT/inc/FFT/fft.h
    )

    mcux_add_library(
        LIBS meterlibFFT/meterlibFFT_cm0p_mmau_armcc.lib
        TOOLCHAINS mdk
        CORES cm0p
    )

    mcux_add_library(
        LIBS meterlibFFT/libmeterFFT_cm0p_mmau_gcc.a
        TOOLCHAINS armgcc mcux
        CORES cm0p
    )

    mcux_add_library(
        LIBS meterlibFFT/meterlibFFT_cm0p_mmau_iar.a
        TOOLCHAINS iar
        CORES cm0p
    )
endif()

# fraclib (MMAU)
if (CONFIG_MCUX_COMPONENT_middleware.metering.fraclib)
	mcux_component_version(4.1.0)
	
    mcux_add_include(
        INCLUDES
            fraclib
            fraclib/inc
    )

    mcux_add_include(
        INCLUDES fraclib/inc/cm0p_mmau
        CORES cm0p
    )

    mcux_add_include(
        INCLUDES fraclib/inc/cm0p_mmau/armcc
        TOOLCHAINS mdk
        CORES cm0p
    )

    mcux_add_include(
        INCLUDES fraclib/inc/cm0p_mmau/gcc
        TOOLCHAINS armgcc mcux
        CORES cm0p
    )

    mcux_add_include(
        INCLUDES fraclib/inc/cm0p_mmau/iar
        TOOLCHAINS iar
        CORES cm0p
    )

    mcux_add_source(
        SOURCES fraclib/inc/cm0p_mmau/fraclib_inlines.h
		CORES cm0p
    )
	
    mcux_add_source(
        SOURCES
            fraclib/inc/fraclib.h
            fraclib/inc/types.h
    )

    mcux_add_source(
        SOURCES fraclib/inc/cm0p_mmau/armcc/mmau.h
        TOOLCHAINS mdk
        CORES cm0p
    )

    mcux_add_source(
        SOURCES fraclib/inc/cm0p_mmau/gcc/mmau.h
        TOOLCHAINS armgcc mcux
        CORES cm0p
    )

    mcux_add_source(
        SOURCES fraclib/inc/cm0p_mmau/iar/mmau.h
        TOOLCHAINS iar
        CORES cm0p
    )

    mcux_add_library(
        LIBS fraclib/fraclib_cm0p_mmau_armcc.lib
        TOOLCHAINS mdk
        CORES cm0p
    )

    mcux_add_library(
        LIBS fraclib/libfrac_cm0p_mmau_gcc.a
        TOOLCHAINS armgcc mcux
        CORES cm0p
    )

    mcux_add_library(
        LIBS fraclib/fraclib_cm0p_mmau_iar.a
        TOOLCHAINS iar
        CORES cm0p
    )
endif()

# meterlib (no MMAU)
if (CONFIG_MCUX_COMPONENT_middleware.metering.meterlib_no_mmau)
	mcux_component_version(4.1.0)

    mcux_add_include(
        INCLUDES
            meterlib
            meterlib/inc
    )

    mcux_add_source(
        SOURCES
            meterlib/inc/meterlib.h
    )

    mcux_add_library(
        LIBS meterlib/libmeter_cm0p_gcc.a
        TOOLCHAINS armgcc mcux
        CORES cm0p
    )

    mcux_add_library(
        LIBS meterlib/meterlib_cm0p_iar.a
        TOOLCHAINS iar
        CORES cm0p
    )

	mcux_add_library(
        LIBS meterlib/meterlib_cm4_armcc.lib
        TOOLCHAINS mdk
        CORES cm4
    )
	
	mcux_add_library(
        LIBS meterlib/libmeter_cm4_gcc.a
        TOOLCHAINS armgcc mcux
        CORES cm4
    )

    mcux_add_library(
        LIBS meterlib/meterlib_cm4_iar.a
        TOOLCHAINS iar
        CORES cm4
    )
endif()

# meterliblp (no MMAU)
if (CONFIG_MCUX_COMPONENT_middleware.metering.meterliblp_no_mmau)
	mcux_component_version(4.1.0)
	
    mcux_add_include(
        INCLUDES
            meterliblp
            meterliblp/inc
    )

    mcux_add_source(
        SOURCES
            meterliblp/inc/meterliblp.h
    )

    mcux_add_library(
        LIBS meterliblp/meterliblp_cm0p_armcc.lib
        TOOLCHAINS mdk
        CORES cm0p
    )

    mcux_add_library(
        LIBS meterliblp/libmeterlp_cm0p_gcc.a
        TOOLCHAINS armgcc mcux
        CORES cm0p
    )

    mcux_add_library(
        LIBS meterliblp/meterliblp_cm0p_iar.a
        TOOLCHAINS iar
        CORES cm0p
    )
	
    mcux_add_library(
        LIBS meterliblp/meterliblp_cm4_armcc.lib
        TOOLCHAINS mdk
        CORES cm4
    )

    mcux_add_library(
        LIBS meterliblp/libmeterlp_cm4_gcc.a
        TOOLCHAINS armgcc mcux
        CORES cm4
    )

    mcux_add_library(
        LIBS meterliblp/meterliblp_cm4_iar.a
        TOOLCHAINS iar
        CORES cm4
    )
endif()

# meterfft (no MMAU)
if (CONFIG_MCUX_COMPONENT_middleware.metering.meterfft_no_mmau)
	mcux_component_version(4.1.0)
	
    mcux_add_include(
        INCLUDES
            meterlibFFT/inc
            meterlibFFT/inc/metering
            meterlibFFT/inc/math
            meterlibFFT/inc/FFT
    )

    mcux_add_source(
        SOURCES
            meterlibFFT/inc/meterlibfft.h
            meterlibFFT/inc/metering/metering.h
            meterlibFFT/inc/math/SIN_LUT1024.h
            meterlibFFT/inc/math/ATAN_LUT2048.h
            meterlibFFT/inc/FFT/fft.h
    )

    mcux_add_library(
        LIBS meterlibFFT/meterlibFFT_cm0p_armcc.lib
        TOOLCHAINS mdk
        CORES cm0p
    )

    mcux_add_library(
        LIBS meterlibFFT/libmeterFFT_cm0p_gcc.a
        TOOLCHAINS armgcc mcux
        CORES cm0p
    )

    mcux_add_library(
        LIBS meterlibFFT/meterlibFFT_cm0p_iar.a
        TOOLCHAINS iar
        CORES cm0p
    )
	
    mcux_add_library(
        LIBS meterlibFFT/meterlibFFT_cm4_armcc.lib
        TOOLCHAINS mdk
        CORES cm4
    )

    mcux_add_library(
        LIBS meterlibFFT/libmeterFFT_cm4_gcc.a
        TOOLCHAINS armgcc mcux
        CORES cm4
    )

    mcux_add_library(
        LIBS meterlibFFT/meterlibFFT_cm4_iar.a
        TOOLCHAINS iar
        CORES cm4
    )
	
    mcux_add_library(
        LIBS meterlibFFT/libmeterFFT_cm33_gcc.a
        TOOLCHAINS armgcc mcux
        CORES cm33
    )
	
    mcux_add_library(
        LIBS meterlibFFT/meterlibFFT_cm33_iar.a
        TOOLCHAINS iar
        CORES cm33
    )
endif()

# fraclib (no MMAU)
if (CONFIG_MCUX_COMPONENT_middleware.metering.fraclib_no_mmau)
	mcux_component_version(4.1.0)

    mcux_add_include(
        INCLUDES
            fraclib
            fraclib/inc
    )

    mcux_add_include(
        INCLUDES fraclib/inc/cm0p
		CORES cm0p
    )

    mcux_add_include(
        INCLUDES fraclib/inc/cm4
		CORES cm4
    )
	
    mcux_add_include(
        INCLUDES fraclib/inc/cm33
		CORES cm33
    )
	
    mcux_add_source(
        SOURCES
            fraclib/inc/fraclib.h
            fraclib/inc/types.h
    )

    mcux_add_source(
        SOURCES
			fraclib/inc/cm0p/fraclib_inlines.h
			fraclib/inc/cm0p/fraclib_mul32.h
		CORES cm0p
    )
	
    mcux_add_source(
        SOURCES fraclib/inc/cm4/fraclib_inlines.h
		CORES cm4
    )
	
    mcux_add_source(
        SOURCES fraclib/inc/cm33/fraclib_inlines.h
		CORES cm33
    )

    mcux_add_library(
        LIBS fraclib/fraclib_cm0p_iar.a
        TOOLCHAINS iar
        CORES cm0p
    )
	
    mcux_add_library(
        LIBS fraclib/fraclib_cm0p_armcc.lib
        TOOLCHAINS mdk
        CORES cm0p
    )

    mcux_add_library(
        LIBS fraclib/libfrac_cm0p_gcc.a
        TOOLCHAINS armgcc mcux
        CORES cm0p
    )
	
    mcux_add_library(
        LIBS fraclib/fraclib_cm4_iar.a
        TOOLCHAINS iar
        CORES cm4
    )
	
    mcux_add_library(
        LIBS fraclib/fraclib_cm4_armcc.lib
        TOOLCHAINS mdk
        CORES cm4
    )

    mcux_add_library(
        LIBS fraclib/libfrac_cm4_gcc.a
        TOOLCHAINS armgcc mcux
        CORES cm4
    )
	
    mcux_add_library(
        LIBS fraclib/libfrac_cm33_gcc.a
        TOOLCHAINS armgcc mcux
        CORES cm33
    )
	
    mcux_add_library(
        LIBS fraclib/fraclib_cm33_iar.a
        TOOLCHAINS iar
        CORES cm33
    )
endif()