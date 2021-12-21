#include <xtl.h>
#include <xbox.h>
#include <xam.h>
#include <stdio.h>
#include "utility.h"
#include "main.h"

int NOP = 0x60000000;

char FilePathBuffer[0xA0];

struct ScriptParseTree 
{
  const char *name;
  int len;
  char *buffer;
};

typedef ScriptParseTree* (__cdecl *DB_FindXAssetHeader_t)(int type, const char *name, bool errorIfMissing, int waitTime);
DB_FindXAssetHeader_t DB_FindXAssetHeader = (DB_FindXAssetHeader_t)(IsMP() ? 0x82328110 : 0x82323E90);

void inline WriteInt(int offset, int value)
{
	*(int*)offset = value;
}

void inline WriteShort(int offset, short value)
{
	*(short*)offset = value;
}

char *ConvertAssetNameToFileName(char * assetName, char * outputBuff)
{
	if(!assetName || !outputBuff) return 0;
	int fileNameOffset = 0;
	for(int i = 0;; i++)
	{
		if(assetName[i] =='/')
			outputBuff[i] = '\\';
		else if(assetName[i] == '+')
			outputBuff[i] = '_';
		else
			outputBuff[i] = assetName[i];
		if(assetName[i] == 0x00)
			break;
	}
	return outputBuff;
}

ScriptParseTree *GscObjLinkHook(char *AssetHeaderName)
{
	ScriptParseTree *AssetHeader = DB_FindXAssetHeader(0x37, AssetHeaderName, true, -1); //0x37 is the scriptparsetree asset type
	if(!AssetHeader)
		return AssetHeader; //game checks it a second time so it doesn't matter if we return nothing

	sprintf(FilePathBuffer, "game:\\raw\\%s", ConvertAssetNameToFileName(AssetHeaderName, AssetHeaderName));

	if(fileExists(FilePathBuffer))
	{
		HANDLE scriptLoader = CreateFile(FilePathBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL); //using OPEN_EXISTING opens the file, doesn't create a new file
		int numberOfBytesRead = GetFileSize(scriptLoader, NULL);
				
		AssetHeader->len = numberOfBytesRead;
		AssetHeader->buffer = new char[AssetHeader->len]; //change the length and the gsc pointer to our one
		
		ReadFile(scriptLoader, AssetHeader->buffer, AssetHeader->len, (LPDWORD)&numberOfBytesRead, 0); //read the file into the assetheader buffer
		CloseHandle(scriptLoader); //close the stream
	}
	return AssetHeader; 
}

void ApplyGscObjResolvePatches(bool IsMP)
{
	if (IsMP)
	{
		WriteInt(0x820B2B64, 0x25580A00);  //"%X\n"								//overwrite the original string with the formatted string
		
		WriteInt(0x822C93B4, 0x7E258B78); //mr        r5, r17					//load the format string
		WriteInt(0x822C93B8, 0x7C6B5214); //add       r3, r11, r10				//load the next available free character pointer for snprintf to print to
		WriteInt(0x822C93BC, 0x7CC43378); //mr        r4, r6					//load the length of the error string buffer
		WriteInt(0x822C93C0, 0x7F86E378); //mr        r6, r28					//load the hash
		WriteInt(0x822C93C4, 0x483F575D); //bl        com_snprintf
		
		WriteInt(0x822C93D0, 0x48000484); //b         loc_822C5A14				//branch so the linker can continue parsing the GSC file
		
		WriteInt(0x822C9850, 0x4BFFFB34); //b         loc_822C5544				//branch to where the error string buffer is checked for any free characters
	}
	else
	{
		WriteInt(0x820B29F4, 0x25580A00);  //"%X\n"								//overwrite the original string with the formatted string


		WriteInt(0x822C5574, 0x7E258B78); //mr        r5, r17					//load the format string
		WriteInt(0x822C5578, 0x7C6B5214); //add       r3, r11, r10				//load the next available free character pointer for snprintf to print to
		WriteInt(0x822C557C, 0x7CC43378); //mr        r4, r6					//load the length of the error string buffer
		WriteInt(0x822C5580, 0x7F86E378); //mr        r6, r28					//load the hash
		WriteInt(0x822C5584, 0x483ED8C5); //bl        com_snprintf

		WriteInt(0x822C5590, 0x48000484); //b         loc_822C5A14				//branch so the linker can continue parsing the GSC file


		WriteInt(0x822C5A10, 0x4BFFFB34); //b         loc_822C5544				//branch to where the error string buffer is checked for any free characters
	}
}

void ApplyGscObjLinkPatches(bool IsMP)
{
	if (IsMP)
	{
		WriteInt(0x822C9EB0, 0x38610100);  //addi r3, r1, 0x100 as this is where com_sprintf stored the GSC name char*
		patchInJump((DWORD*)0x822C9EB4,  (DWORD)GscObjLinkHook, true);
	}
	else
	{
		WriteInt(0x822C6070, 0x38610100); //addi r3, r1, 0x100 as this is where com_sprintf stored the GSC name char*
		patchInJump((DWORD*)0x822C6074,  (DWORD)GscObjLinkHook, true);
	}
}

char BetterDynamicConfigStringBuffer[0x8000]; //i doubt 20x original buffer size of strings are going to be stored, but im going to be safe as possible so i don't have to change it again

void ApplyDynamicConfigStringBufferPatches(bool IsMP)
{
	DWORD strBuffer = (DWORD)BetterDynamicConfigStringBuffer;
	if (IsMP)
	{
		WriteInt(0x8217D1BC, 0x3D600000 + ((strBuffer >> 16) & 0xFFFF)); // lis %r11, dest>>16			//make the game use our upgraded string buffer
		WriteInt(0x8217D1C8, 0x388B0000 + (strBuffer & 0xFFFF)); // addi %r4, %r11, dest&0xFFFF
		
		WriteInt(0x8217D1D4, 0x3D6082D2); // lis %r11, buffersize
		WriteInt(0x8217D1DC, 0x38CB00B8); // addi %r6, %r11, buffersize		//set r6 to the buffer size
		
		WriteShort(0x8217D1E2, 0x7FFF); //change max buffer size
		
		
		//second function patch
		WriteInt(0x8217D374, 0x3D600000 + ((strBuffer >> 16) & 0xFFFF));
		WriteInt(0x8217D384, 0x388B0000 + (strBuffer & 0xFFFF));
		
		WriteInt(0x8217D390, 0x3D6082D2);
		WriteInt(0x8217D398, 0x38CB00B8);
		
		WriteShort(0x8217D39E, 0x7FFF);
	}
	else
	{
		WriteInt(0x8217D06C, 0x3D600000 + ((strBuffer >> 16) & 0xFFFF)); // lis %r11, dest>>16 + 1		//make the game use our upgraded string buffer
		WriteInt(0x8217D078, 0x388B0000 + (strBuffer & 0xFFFF)); // addi %r4, %r11, dest&0xFFFF

		WriteInt(0x8217D084, 0x3D6082D2); // lis %r11, buffersize
		WriteInt(0x8217D08C, 0x38CB7BE8); // addi %r6, %r11, buffersize		//set r6 to the buffer size

		WriteShort(0x8217D092, 0x7FFF); //change max buffer size


		//second function patch
		WriteInt(0x8217D224, 0x3D600000 + ((strBuffer >> 16) & 0xFFFF));
		WriteInt(0x8217D234, 0x388B0000 + (strBuffer & 0xFFFF));

		WriteInt(0x8217D240, 0x3D6082D2);
		WriteInt(0x8217D248, 0x38CB7BE8);

		WriteShort(0x8217D24E, 0x7FFF);
	}
}

void ApplyDvarPatches(bool IsMP)
{
	if (IsMP)
	{
		WriteInt(0x826B8150, NOP);
		WriteInt(0x826B8168, NOP);
	}
	else
	{
		WriteInt(0x826AC478, NOP);
		WriteInt(0x826AC490, NOP);
	}
}