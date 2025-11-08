// h_export.cpp
#include "extdll.h"
#include "util.h"
#include "cbase.h"

enginefuncs_t g_engfuncs;

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
	return TRUE;
}

extern "C" void __stdcall GiveFnptrsToDll( enginefuncs_t *pengfuncsFromEngine )
{
	memcpy( &g_engfuncs, pengfuncsFromEngine, sizeof( enginefuncs_t ) );
}

