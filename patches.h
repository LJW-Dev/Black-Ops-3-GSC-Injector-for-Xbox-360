struct ScriptParseTree;

void inline WriteInt(int offset, int value);

void inline WriteShort(int offset, short value);

//converts the BO3 asset name to an xbox filepath, credit to CraigChrist8239
char *ConvertAssetNameToFileName(char * assetName, char * outputBuff);

//overwrites GSC files with our custom ones, credit to CraigChrist8239 for the original
ScriptParseTree *GscObjLinkHook(char *AssetHeaderName);

//this edits the GSC linking stage to show what GSC function is creating an error, instead of showing nothing
void ApplyGscObjResolvePatches(bool IsMP);

//hook GscObjLink to load custom GSC files
void ApplyGscObjLinkPatches(bool IsMP);


//patch 2 functions to use the upgraded dynamic config string buffer, as mod menus would quickly run out of space with the small MP 0x400 and ZM 0x800 string buffer

//This "Dynamic Config string buffer" is completely unique to old gen, and not on BO2 or BO3 PC. There is no way to change it or reset it, as the only 3 functions that change it's length 
//are the 2 functions that are patched and the game init function. This means that the upgraded buffer is only on the host's end, and anyone that joins will still have the small buffer.
//This makes a hard decision, as it it better to keep the small buffer and allow others to join or use the large buffer and allow much better mod menus to be used. In my (australian) experience,
//there is almost nobody who plays online so i have decided to use the upgraded buffer.
void ApplyDynamicConfigStringBufferPatches(bool IsMP);

//remove dvar write/cheat protection
void ApplyDvarPatches(bool IsMP);