# TODO: Linux
# TODO: ASM

cmake_minimum_required( VERSION 3.20 )

set( GLQUAKE 1 )
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
	"$<$<NOT:${GLQUAKE}>:d_draw.s>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_draw16.s>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_edge.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_fill.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_init.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_modech.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_part.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_parta.s>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_polysa.s>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_polyse.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_scan.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_scana.s>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_sky.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_spr8.s>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_sprite.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_surf.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_vars.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_varsa.s>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:d_zpoint.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:draw.c>" # [!$GL]
	"$<${GLQUAKE}:gl_draw.c>" # [$GL]
	"$<${GLQUAKE}:gl_mesh.c>" # [$GL]
	"$<${GLQUAKE}:gl_model.c>" # [$GL]
	"$<${GLQUAKE}:gl_refrag.c>" # [$GL]
	"$<${GLQUAKE}:gl_rlight.c>" # [$GL]
	"$<${GLQUAKE}:gl_rmain.c>" # [$GL]
	"$<${GLQUAKE}:gl_rmisc.c>" # [$GL]
	"$<${GLQUAKE}:gl_rsurf.c>" # [$GL]
	"$<${GLQUAKE}:gl_screen.c>" # [$GL]
	"$<${GLQUAKE}:gl_test.c>" # [$GL]
	"$<${GLQUAKE}:gl_vidnt.c>" # [$GL]
	"$<${GLQUAKE}:gl_warp.c>" # [$GL]
	"host.c"
	"host_cmd.c"
	"in_win.c"
	"keys.c"
	#"math.s"
	"mathlib.c"
	"menu.c"
	"$<$<NOT:${GLQUAKE}>:model.c>" # [!$GL]
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
	"$<$<NOT:${GLQUAKE}>:r_aclip.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_aclipa.s>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_alias.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_aliasa.s>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_bsp.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_draw.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_drawa.s>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_edge.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_edgea.s>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_efrag.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_light.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_main.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_misc.c>" # [!$GL]
	"r_part.c"
	"$<$<NOT:${GLQUAKE}>:r_sky.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_sprite.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_surf.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_vars.c>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:r_varsa.s>" # [!$GL]
	"sbar.c"
	"$<$<NOT:${GLQUAKE}>:screen.c>" # [!$GL]
	"snd_dma.c"
	"snd_mem.c"
	"snd_mix.c"
	#"snd_mixa.s"
	"snd_win.c"
	"$<$<NOT:${GLQUAKE}>:surf16.s>" # [!$GL]
	"$<$<NOT:${GLQUAKE}>:surf8.s>" # [!$GL]
	"sv_main.c"
	"sv_move.c"
	"sv_phys.c"
	"sv_user.c"
	"sys_win.c"
	#"sys_wina.s"
	"$<$<NOT:${GLQUAKE}>:vid_win.c>" # [!$GL]
	"view.c"
	"wad.c"
	"winquake.rc"
	"world.c"
	#"worlda.s"
	"zone.c"

	"sys_dll.c"
	"r_trans.c"

	"progdefs.q1"
	"progdefs.q2"
)

set( ENGINE_HEADERS # "Header Files"
	"${SRCDIR}/public/extdll.h"
	"${SRCDIR}/public/port.h"
	"${SRCDIR}/public/qlimits.h"
	"${SRCDIR}/public/edict.h"
	"${SRCDIR}/public/qfont.h"
	"${SRCDIR}/public/rendermode.h"

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
	"r_trans.h"
)

set( ENGINE_RESOURCES # "Resource Files"
	"quake.ico"
)

add_executable( enginegl WIN32 ${ENGINE_SOURCES} ${ENGINE_HEADERS} ${ENGINE_RESOURCES} )
install_multiplatform( enginegl "${SRCDIR}/game" )

target_compile_definitions( enginegl PRIVATE "$<${GLQUAKE}:GLQUAKE>" "$<${QUIVER}:QUIVER_QUAKE_COMPAT>" "$<${QUIVER}:QUIVER>" ) # "$<${QUIVER}:QUAKE2>"
target_include_directories( enginegl PRIVATE "${SRCDIR}/engine/scitech/include" "${SRCDIR}/engine/dxsdk/sdk/inc" )
target_link_libraries( enginegl PRIVATE dxguid winmm wsock32 opengl32 glu32 gdi32 comctl32 )
