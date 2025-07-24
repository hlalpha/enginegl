cmake_minimum_required( VERSION 3.20 )

project( GLQuake LANGUAGES C CXX )

set( CMAKE_POSITION_INDEPENDENT_CODE ON )
set_property( GLOBAL PROPERTY USE_FOLDERS ON )

set( SRCDIR "${CMAKE_CURRENT_LIST_DIR}" )

include_directories(
	"${SRCDIR}/common"
	"${SRCDIR}/public"
)

if( WIN32 )
	set( _DLL_EXT ".dll" )
elseif( LINUX )
	set( _DLL_EXT ".so" )
endif()

add_compile_definitions(
	_DLL_EXT=${_DLL_EXT}
)

if( WIN32 )
	add_compile_definitions(
		_WIN32
		WIN32
		_WINDOWS
		WINDOWS
	)
elseif( LINUX )
	add_compile_definitions(
		_LINUX
		LINUX
		_POSIX
		POSIX
	)
endif()

function( install_multiplatform target outdir )
	install(
		TARGETS ${target}
		RUNTIME DESTINATION "${outdir}"
		LIBRARY DESTINATION "${outdir}"
	)

	if( WIN32 )
		install(
			FILES $<TARGET_PDB_FILE:${target}>
			DESTINATION "${outdir}"
			OPTIONAL
		)
	endif()
endfunction()

add_subdirectory( engine )
add_subdirectory( engine/gas2masm )
add_subdirectory( thirdparty/proqcc )
