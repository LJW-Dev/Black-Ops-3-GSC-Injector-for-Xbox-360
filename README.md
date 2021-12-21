# Black Ops 3 GSC Injector for Xbox 360
An Xbox 360 GSC injector for Black Ops 3 based on CraigChrist8239's BO2 Tesseract.

# Features
Ability to load custom GSC files from the hard drive. \
Removed DVAR write protection. \
Better error reporting when the GSC linker fails. \
Increased Dynamic Config string buffer size to 0x8000 chars.

# Installation
1. Move all TU8 files into your BO3 directory, overwriting anything.
2. Move the patched .xex files and the "ScriptLoader.dll" into your BO3 directory, overwriting anything.
3. Create a folder called "raw" in the main BO3 directory, this is where the custom GSC files will go. For example, a custom \_clientids.gsc file would go in "raw/scripts/mp/gametypes/\_clientids.gsc". \
All TU8 and patched .xex files can be downloaded here:
https://mega.nz/file/5QslCIIQ#iqS2JxA7dz1e-MWnHvrcmplvZhZWuI0UiNAk_xhnALw

# Usage
Requires Visual Studio 2010 and the Xbox 360 XDK to be installed.

# Note To Developers
The Dynamic Config string buffer stores strings used in mod menus, and its size increase means that better mod menus can be made, with the downside that other players will be kicked once the size passes the original size. This buffer is unique to last gen and there is no way to change or reset it on other player's xboxes. I chose to make the buffer bigger as not many people play online, but if you are going to make a mod menu to be used online remove the ApplyDynamicConfigStringBufferPatches function or keep the menu string sizes short.

The load address is 0x88000000.

# Credits
CraigChrist8239 for his Tesseract source.
