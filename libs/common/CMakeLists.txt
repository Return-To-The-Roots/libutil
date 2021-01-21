set(_BoostComponents filesystem)
if(WIN32)
	# Locale only on windows
	set(_BoostComponents ${_BoostComponents} locale)
endif()
find_package(Boost 1.69 REQUIRED COMPONENTS ${_BoostComponents})


FILE(GLOB SRC_FILES src/*.cpp)
FILE(GLOB HEADER_FILES include/s25util/*.*)

conditional_src(SRC_FILES WIN32 src/System_Win32.cpp src/System_Other.cpp)

add_library(s25util_common STATIC ${SRC_FILES} ${HEADER_FILES})
add_library(s25util::common ALIAS s25util_common)

target_include_directories(s25util_common PUBLIC include PRIVATE include/s25util)
target_link_libraries(s25util_common PUBLIC endian::static Boost::nowide Boost::filesystem)
target_compile_features(s25util_common PUBLIC cxx_std_14)
# PIC Required for linking into shared libs
set_target_properties(s25util_common PROPERTIES CXX_EXTENSIONS OFF POSITION_INDEPENDENT_CODE ON)

IF (WIN32)
    target_compile_definitions(s25util_common PRIVATE WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0600) # Win Vista
    # Exclude some windows defines
    target_compile_definitions(s25util_common PRIVATE NOMINMAX NODRAWTEXT NOSOUND NOTEXTMETRIC NOCOMM NOMCX)
	target_link_libraries(s25util_common PUBLIC Boost::locale Boost::disable_autolinking iphlpapi)
ENDIF ()

include(EnableWarnings)
enable_warnings(s25util_common)
