# Hydra Launcher
A remake of the old Hearts of Iron IV launcher (version 1.7.1 'Hydra') and making it usable on the newest versions of hoi4.
# Requirements
`HOI4.exe` - The executable itself.</br >
`res`- Has all of the launcher assets.</br >
`discord_game_sdk.dll` - To run the Rich Discord Presnce.</br >
`libRSGL.lib` - To help run the executable.
# How to build the launcher
To build you need:</br>
1. MinGW
2. Win32 headers and binaries
3. Makefile

To compile the whole thing, run `make windowsCompile`. If you need to build the entire thing (including RSGL), use `make`. 
# Roadmap
- Properly launch hoi4.
- Complete the DLC and mods tab (0/2).
- Implement proper fonts/text.
- Language support.
- Have old+new settings.
- Save on settings.
- Mod playsets.
- Recreate the animations and behaviours.
- Fix possible bugs/crashes.
- Implement useful features that the current hoi4 launcher has.
# Impossible/useless ideas that won't be worked on
- Make the launcher properly usable for older versions of hoi4 (<1.7.1 versions) (possible, not really useful to anyone).
- Port to Linux/MacOS/other platforms (Linux possibly but MacOS unlikely, use a compatibility layer like Wine to run the launcher for now).
- Do the news tab (unlikely/impossible).
- Integrate the account system (impossible).