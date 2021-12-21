#include <xtl.h>
#include <xbox.h>
#include <xam.h>
#include <stdio.h>
#include "utility.h"
#include "patches.h"

typedef UINT32 (__cdecl *XamGetCurrentTitleId_t)(void);
XamGetCurrentTitleId_t XamGetCurrentTitleId = (XamGetCurrentTitleId_t)resolveFunct("xam.xex", 0x1CF);

bool IsMP()
{
	byte test = *(byte*)0x826EF174; //just a random byte that is different on both versions
	if(test == 0x99) 
		return true;
	else
		return false;
}

void DoGamePatches(bool IsMP)
{
	ApplyGscObjResolvePatches(IsMP);
	ApplyGscObjLinkPatches(IsMP);
	ApplyDvarPatches(IsMP);
	ApplyDynamicConfigStringBufferPatches(IsMP);
}

BOOL APIENTRY DllMain(HANDLE hInstDLL, DWORD dreason, LPVOID lpReserved)
{
	if(dreason == DLL_PROCESS_ATTACH)
	{
		if(XamGetCurrentTitleId() == 0x4156091D)
			DoGamePatches(IsMP());
	}
	return TRUE;
}