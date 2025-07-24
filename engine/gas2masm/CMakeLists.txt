cmake_minimum_required( VERSION 3.20 )

set( GASTOMASM_SOURCES # "Source Files"
	"gas2masm.c"
)

add_executable( gas2masm ${GASTOMASM_SOURCES} )
install_multiplatform( gas2masm "${SRCDIR}/game" )
