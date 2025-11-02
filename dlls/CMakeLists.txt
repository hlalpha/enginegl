# TODO: Linux

cmake_minimum_required( VERSION 3.20 )

set( DLLS_SOURCES # "Source Files"
	"h_export.cpp"
	"hl.def"
	"util.cpp"
	"cbase.cpp"
	"lights.cpp"
	"world.cpp"
	"sound.cpp"
)

set( DLLS_HEADERS # "Header Files"
	"${SRCDIR}/engine/eiface.h"
	"${SRCDIR}/public/port.h"
	"${SRCDIR}/public/qlimits.h"
	"${SRCDIR}/public/edict.h"
	"${SRCDIR}/common/winsani_in.h"
	"${SRCDIR}/common/winsani_out.h"
	"extdll.h"
	"enginecallback.h"

	"util.h"
	"cbase.h"
	"vector.h"
)

add_library( hl SHARED ${DLLS_SOURCES} ${DLLS_HEADERS} )
install_multiplatform( hl "${SRCDIR}/game/valve/dlls" )

target_compile_definitions( hl PRIVATE GLQUAKE QUIVER )
target_include_directories( hl PRIVATE "../engine" "." )
