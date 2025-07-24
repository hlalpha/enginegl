# TODO: Linux

cmake_minimum_required( VERSION 3.20 )

set( PROQCC_SOURCES # "Source Files"
	"proqcc/src/cmdlib.c"
	"proqcc/src/decomp.c"
	"proqcc/src/pr_comp.c"
	"proqcc/src/pr_lex.c"
	"proqcc/src/qcc.c"
	"proqcc/src/screen.c"
)

set( PROQCC_HEADERS # "Header Files"
	"proqcc/src/cmdlib.h"
	"proqcc/src/decomp.h"
	"proqcc/src/pr_comp.h"
	"proqcc/src/qcc.h"
	"proqcc/src/screen.h"
)

add_executable( proqcc ${PROQCC_SOURCES} ${PROQCC_HEADERS} )
install_multiplatform( proqcc "${SRCDIR}/game" )
