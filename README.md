# Secret of Evermore Rom Info
A program that allows you to extract information in CSV format from the SNES Secret of Evermore game ROMs. Works with both Headered and Unheadered ROMs. Currently the program extracts information from the following data:
- ROM File
- NPC Monsters
- Ingredients
- Alchemy
- Call Bead Spells
- Weapons
- Armors
- Boy & Dog

In the *res_originalroms* folder the information already extracted from the original SoE ROMs is included:
- USA - NTSC
- Europe - PAL
- Germany - PAL
- France - PAL
- Spain - PAL
- Spain (Rev 2) - PAL

The easiest way to use the program is to pass the ROM from which to extract the data as the only parameter. On Windows this can be easily achieved by dragging the ROM into the executable (only one file, not multiple). The program has some additional configuration parameters that normally do not need to be specified.

A pre-compiled executables for Windows (x32 and x64 versions) are provided, compiled using MinGW and MinGW-w64. If you want to compile the executable again or for another OS just run this command in the directory:
```
make all
```
To achieve static linking requires that the archive (.a) versions of your libraries exist on the system, if this gives you any problem, you can try to compile the executables using dynamic libraries with the following command instead of the above:
```
make all NOSTATIC=1
```

In the future it is planned to add more data information to be extracted, such as merchants, friendly NPCs, charms, rare items, trade good items, and more miscellaneous data.

A more detailed description of the data extracted for each file can be checked in the [Wiki](https://github.com/millanzarreta/soerominfo/wiki), where more details about the program are also reviewed. If you find any errors, either in the program or in a misunderstanding of the extracted data, you can open a new Issue in the [Issue Tracker](https://github.com/millanzarreta/soerominfo/issues).

Most of the information from where to extract the data has been obtained from the following page: https://datacrystal.romhacking.net/wiki/Secret_of_Evermore:ROM_map

Another utility to extract data from SoE ROMs should be highlighted, especially useful for extracting Tiles, Graphics and certain Scripts: https://github.com/black-sliver/SoETilesViewer
