/*

===== h_export.cpp ========================================================

  Entity classes exported by Halflife.

*/

#include "extdll.h"
#include "util.h"

#include "cbase.h"

// Holds engine functionality callbacks
enginefuncs_t g_engfuncs;

// Required DLL entry point
BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
	}

	return TRUE;
}

extern "C" void __stdcall GiveFnptrsToDll( enginefuncs_t *pengfuncsFromEngine )
{
	memcpy( &g_engfuncs, pengfuncsFromEngine, sizeof( enginefuncs_t ) );
}

