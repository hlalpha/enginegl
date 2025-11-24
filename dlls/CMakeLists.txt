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
	"m44.cpp"
	"h_cine.cpp"
	"monsters.cpp"
	"h_cycler.cpp"
	"subs.cpp"
	"triggers.cpp"
	"items.cpp"
	"bmodels.cpp"
	"globals.cpp"
	"player.cpp"
	"client.cpp"
	"weapons.cpp"
	"doors.cpp"
	"plats.cpp"
	"john_train.cpp"
	"h_dispatch.cpp"
	"buttons.cpp"
	"func_break.cpp"
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
	"basemonster.h"
	"items.h"
	"player.h"
	"client.h"
	"weapons.h"
	"doors.h"
	"func_break.h"
)

add_library( hl SHARED ${DLLS_SOURCES} ${DLLS_HEADERS} )
install_multiplatform( hl "${SRCDIR}/game/valve/dlls" )

target_compile_definitions( hl PRIVATE GLQUAKE QUIVER QUAKE2 )
target_include_directories( hl PRIVATE "../engine" "." )
