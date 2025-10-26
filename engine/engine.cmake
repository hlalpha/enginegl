# TODO: Linux
# TODO: ASM

set( QUIVER  1 )

set( ENGINE_SOURCES # "Source Files"
	"cd_win.c"
	"chase.c"
	"cl_demo.c"
	"cl_input.c"
	"cl_main.c"
	"cl_parse.c"
	"cl_tent.c"
	"cmd.c"
	"common.c"
	"conproc.c"
	"console.c"
	"crc.c"
	"cvar.c"
	#"$<$<NOT:${GLQUAKE}>:d_draw.s>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_draw16.s>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_edge.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_fill.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_init.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_modech.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_part.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_parta.s>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_polysa.s>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_polyse.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_scan.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_scana.s>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_sky.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_spr8.s>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_sprite.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_surf.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_vars.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_varsa.s>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:d_zpoint.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:draw.c>" # [!$GL]
	#"$<${GLQUAKE}:gl_draw.c>" # [$GL]
	#"$<${GLQUAKE}:gl_mesh.c>" # [$GL]
	#"$<${GLQUAKE}:gl_model.c>" # [$GL]
	#"$<${GLQUAKE}:gl_refrag.c>" # [$GL]
	#"$<${GLQUAKE}:gl_rlight.c>" # [$GL]
	#"$<${GLQUAKE}:gl_rmain.c>" # [$GL]
	#"$<${GLQUAKE}:gl_rmisc.c>" # [$GL]
	#"$<${GLQUAKE}:gl_rsurf.c>" # [$GL]
	#"$<${GLQUAKE}:gl_screen.c>" # [$GL]
	#"$<${GLQUAKE}:gl_test.c>" # [$GL]
	#"$<${GLQUAKE}:gl_vidnt.c>" # [$GL]
	#"$<${GLQUAKE}:gl_warp.c>" # [$GL]
	"host.c"
	"host_cmd.c"
	"in_win.c"
	"keys.c"
	#"math.s"
	"mathlib.c"
	"menu.c"
	#"$<$<NOT:${GLQUAKE}>:model.c>" # [!$GL]
	"net_dgrm.c"
	"net_loop.c"
	"net_main.c"
	"net_vcr.c"
	"net_win.c"
	"net_wins.c"
	"net_wipx.c"
	"pr_cmds.c"
	"pr_edict.c"
	"pr_exec.c"
	#"$<$<NOT:${GLQUAKE}>:r_aclip.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_aclipa.s>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_alias.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_aliasa.s>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_bsp.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_draw.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_drawa.s>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_edge.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_edgea.s>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_efrag.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_light.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_main.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_misc.c>" # [!$GL]
	"r_part.c"
	#"$<$<NOT:${GLQUAKE}>:r_sky.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_sprite.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_surf.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_vars.c>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:r_varsa.s>" # [!$GL]
	"sbar.c"
	#"$<$<NOT:${GLQUAKE}>:screen.c>" # [!$GL]
	"snd_dma.c"
	"snd_mem.c"
	"snd_mix.c"
	#"snd_mixa.s"
	"snd_win.c"
	#"$<$<NOT:${GLQUAKE}>:surf16.s>" # [!$GL]
	#"$<$<NOT:${GLQUAKE}>:surf8.s>" # [!$GL]
	"sv_main.c"
	"sv_move.c"
	"sv_phys.c"
	"sv_user.c"
	"sys_win.c"
	#"sys_wina.s"
	#"$<$<NOT:${GLQUAKE}>:vid_win.c>" # [!$GL]
	"view.c"
	"wad.c"
	#"winquake.rc"
	"world.c"
	#"worlda.s"
	"zone.c"

	"sys_dll.c"
	"decals.c"
	"r_trans.c"
	"snd_dsp.c"
	"in_camera.c"

	"progdefs.q1"
	"progdefs.q2"
)

set( ENGINE_HW_SOURCES
	"gl_draw.c" # [$GL]
	"gl_mesh.c" # [$GL]
	"gl_model.c" # [$GL]
	"gl_refrag.c" # [$GL]
	"gl_rlight.c" # [$GL]
	"gl_rmain.c" # [$GL]
	"gl_rmisc.c" # [$GL]
	"gl_rsurf.c" # [$GL]
	"gl_screen.c" # [$GL]
	"gl_test.c" # [$GL]
	"gl_vidnt.c" # [$GL]
	"gl_warp.c" # [$GL]
)

set( ENGINE_SW_SOURCES
	#"d_draw.s" # [!$GL]
	#"d_draw16.s" # [!$GL]
	"d_edge.c" # [!$GL]
	"d_fill.c" # [!$GL]
	"d_init.c" # [!$GL]
	"d_modech.c" # [!$GL]
	"d_part.c" # [!$GL]
	#"d_parta.s" # [!$GL]
	#"d_polysa.s" # [!$GL]
	"d_polyse.c" # [!$GL]
	"d_scan.c" # [!$GL]
	"d_scana.s" # [!$GL]
	"d_sky.c" # [!$GL]
	#"d_spr8.s" # [!$GL]
	"d_sprite.c" # [!$GL]
	"d_surf.c" # [!$GL]
	"d_vars.c" # [!$GL]
	#"d_varsa.s" # [!$GL]
	"d_zpoint.c" # [!$GL]
	"draw.c" # [!$GL]

	"model.c" # [!$GL]

	"r_aclip.c" # [!$GL]
	#"r_aclipa.s" # [!$GL]
	"r_alias.c" # [!$GL]
	#"r_aliasa.s" # [!$GL]
	"r_bsp.c" # [!$GL]
	"r_draw.c" # [!$GL]
	#"r_drawa.s" # [!$GL]
	"r_edge.c" # [!$GL]
	#"r_edgea.s" # [!$GL]
	"r_efrag.c" # [!$GL]
	"r_light.c" # [!$GL]
	"r_main.c" # [!$GL]
	"r_misc.c" # [!$GL]

	"r_sky.c" # [!$GL]
	"r_sprite.c" # [!$GL]
	"r_surf.c" # [!$GL]
	"r_vars.c" # [!$GL]
	#"r_varsa.s" # [!$GL]

	"screen.c" # [!$GL]

	#"surf16.s" # [!$GL]
	#"surf8.s" # [!$GL]

	"vid_win.c" # [!$GL]
)

set( ENGINE_HEADERS # "Header Files"
	"eiface.h"
	"${SRCDIR}/public/port.h"
	"${SRCDIR}/public/qlimits.h"
	"${SRCDIR}/public/edict.h"
	"${SRCDIR}/public/qfont.h"
	"${SRCDIR}/common/const.h"

	"anorm_dots.h"
	"anorms.h"
	"bspfile.h"
	"cdaudio.h"
	"client.h"
	"cmd.h"
	"common.h"
	"conproc.h"
	"console.h"
	"crc.h"
	"cvar.h"
	"d_iface.h"
	"dosisms.h"
	"draw.h"
	"gl_model.h"
	"gl_warp_sin.h"
	"glquake.h"
	"input.h"
	"keys.h"
	"mathlib.h"
	"menu.h"
	"model.h"
	"modelgen.h"
	"net.h"
	"net_dgrm.h"
	"net_loop.h"
	"net_ser.h"
	"net_vcr.h"
	"net_wins.h"
	"net_wipx.h"
	"pr_comp.h"
	"pr_cmds.h"
	"progdefs.h"
	"progs.h"
	"protocol.h"
	"quakedef.h"
	"r_local.h"
	"r_shared.h"
	"render.h"
	"sbar.h"
	"screen.h"
	"server.h"
	"sound.h"
	"spritegn.h"
	"sys.h"
	"vid.h"
	"view.h"
	"wad.h"
	"winquake.h"
	"world.h"
	"zone.h"

	"sys_dll.h"
	"decals.h"
	"r_trans.h"
	"snd_dsp.h"
	"in_camera.h"
)

set( ENGINE_RESOURCES # "Resource Files"
	"quake.ico"
)

function( add_engine )
	cmake_parse_arguments(
		ARGS
		"HW;SW"
		"TARGET"
		""
		${ARGN}
	)
	
	if ( NOT ARGS_HW AND NOT ARGS_SW )
		message( FATAL_ERROR "add_engine must have HW or SW set!" )
	endif()

	set( IS_HW $<BOOL:${ARGS_HW}> )
	set( IS_SW $<BOOL:${ARGS_SW}> )

	add_executable(
		${ARGS_TARGET} WIN32
		${ENGINE_SOURCES} ${ENGINE_HEADERS} ${ENGINE_RESOURCES}
		$<${IS_HW}:${ENGINE_HW_SOURCES}> $<${IS_SW}:${ENGINE_SW_SOURCES}>
	)
	install_multiplatform(
		${ARGS_TARGET}
		"${SRCDIR}/game"
	)

	target_compile_definitions(
		${ARGS_TARGET} PRIVATE

		"$<${IS_HW}:GLQUAKE>"
		"$<${QUIVER}:QUIVER_QUAKE_COMPAT>"
		"$<${QUIVER}:QUIVER>"
		"$<${QUIVER}:QUIVER_TESTS>"
		#"$<${QUIVER}:QUAKE2>" # TODO
	)
	target_include_directories(
		${ARGS_TARGET} PRIVATE

		"${SRCDIR}/engine/scitech/include"
		"${SRCDIR}/engine/dxsdk/sdk/inc"
	)
	target_link_libraries(
		${ARGS_TARGET} PRIVATE

		dxguid
		winmm
		wsock32
		opengl32
		glu32
		gdi32
		comctl32
	)
endfunction()
