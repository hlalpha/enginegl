# TODO: Linux

cmake_minimum_required( VERSION 3.20 )

set( QUIVER  1 )

set( QLUMPY_SOURCES # "Source Files"
	"../common/cmdlib.c"
	"../common/lbmlib.c"
	"./qlumpy.c"
	"./quakegrb.c"
	"../common/scriplib.c"
	"../common/wadlib.c"
)

set( QLUMPY_HEADERS # "Header Files"
	"../common/cmdlib.h"
	"../common/lbmlib.h"
	"./qlumpy.h"
	"../common/scriplib.h"
	"../common/wadlib.h"
)

add_executable( qlumpy ${QLUMPY_SOURCES} ${QLUMPY_HEADERS} )
install_multiplatform( TARGET qlumpy OUTDIR "${SRCDIR}/game" )

target_compile_definitions( qlumpy PRIVATE "$<${QUIVER}:QUIVER>" )
target_include_directories( qlumpy PRIVATE "." "../common" )
