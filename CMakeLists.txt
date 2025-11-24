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

	add_compile_options(
		/MP
	)
elseif( LINUX )
	add_compile_definitions(
		_LINUX
		LINUX
		_POSIX
		POSIX
	)
endif()

function( install_multiplatform )
	cmake_parse_arguments(
		ARGS
		""
		"TARGET;OUTDIR;OUTNAME"
		""
		${ARGN}
	)

	if( NOT ARGS_OUTNAME )
		set( ARGS_OUTNAME "${ARGS_TARGET}" )
	endif()

	set_target_properties( ${ARGS_TARGET} PROPERTIES OUTPUT_NAME "${ARGS_OUTNAME}" )

	install(
		TARGETS ${ARGS_TARGET}
		RUNTIME DESTINATION "${ARGS_OUTDIR}"
		LIBRARY DESTINATION "${ARGS_OUTDIR}"
	)

	if( WIN32 )
		install(
			FILES $<TARGET_PDB_FILE:${ARGS_TARGET}>
			DESTINATION "${ARGS_OUTDIR}"
			OPTIONAL
		)
	endif()
endfunction()

add_subdirectory( engine )
add_subdirectory( engine/gas2masm )
add_subdirectory( utils/qlumpy )
add_subdirectory( dlls )

