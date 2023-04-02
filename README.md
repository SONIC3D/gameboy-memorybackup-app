# GameBoy MemoryBackup Homebrew Application

## Introduction
The purpose of this project is to create a usable application for my another project [Gameboy MemoryBackup Cartridge](https://github.com/SONIC3D/gameboy-memorybackup). As it's an enhanced version of the original Karat MemoryBackup(also known as "Mega Memory" or "Mega Memory Card" in other regions), this project will start from reversing and reimplementing the original "Mema Memory" program features.

## Updates
### 2023.Apr.2
* Tag: v1.0
* All the original features of the "Mega Memory" program are reimplemented. The build result is binary same as the original ROM([Reference ROM(Archive.org), filename "Mega Memory (World) (Unl).7z"](https://archive.org/details/nointro.gb)).

## How to build
### Prerequisites
* Install gnu make for your OS platform.
* Install the [RGBDS GameBoy development toolchains](https://rgbds.gbdev.io/)
### Using CLI
* Run `make build` or `make rebuild` to build the project.
* Run `make clean` to clean the build files.
* Build result will be stored in the `bin` folder.
### Using Visual Studio Code as IDE
* Install Visual Studio Code.
* Install the [RGBDS Z80 extension](https://marketplace.visualstudio.com/items?itemName=donaldhays.rgbds-z80).
* Open the project folder in Visual Studio Code.
* Press `Ctrl+Shift+P` to open the command palette and type `Tasks: Run Task` to open the task list and select `build`/`rebuild`/`clean` to build the project.
* For build error in assembling, please check the output window for more information. Clike the file path while holding the `⌘` key on Mac OS X or the `Ctrl` key on windows to open the file to the error line.
* For custom the build tasks, edit the tasks.json in .vscode directory to change the build options. Don't forget to reference the RGBDS Z80 extension documentation.

## Notes on the source code
* Version 1.0 source code is based on disassembly code of the original "Mega Memory" ROM.
* The main source file is the `membak-hb.asm`.
* I have added many, or even heavy comments in the source code to make the code understandable to people not familiar with GameBoy assembly code. And also, additional many local labels in subroutines to make the code more readable.
* I mixed using Chinese and English in disassembling and code analysis for trim the comment length. So in this reimplementing code project, the source code is also mixed using multiple languages, or pseudo codes. Please using translator tools.
* The checksum in header is not fixed by RGBFIX tool intentionally. The original ROM checksum is calculated by accumulating first 16KB from $0000-$4000. But the ROM dump has additional 16KB with 0xFF filled. So I preset the checksum value in source code.

## TODO List
### TODO in v1.0
- [x] Create source code project to make 1:1 ROM file as the original `Mega Memory` English version 1.0.
### TODO in v1.1
- [ ] Change all the hardcoded GameBoy special feature address value in source code to the pre-defined macros or variables in hardware.inc.
- [ ] Revise all the comments.
- [ ] Revise all the subroutine names based on their actual feature.
- [ ] Revise all variable names.
### TODO in v2.x
- [ ] Access external SD/TF card.
- [ ] Load custom code binary from external storage card.

## Special Thanks
* The original Mega Memory Card creators and publisher for their great product. ([More info on TCRF.net](https://tcrf.net/Mega_Memory_Card_(Game_Boy)))
* All the creators of [GameBoy PanDocs](https://problemkaputt.de/pandocs.htm).
* BGB emulator team for their great emulator.([Homepage](https://bgb.bircd.org/))
* People behind the Hex Rays IDA disassembler.([Homepage](https://hex-rays.com/))
* The [RGBDS GameBoy development toolchains](https://rgbds.gbdev.io/).
* The [RGBDS Z80](https://marketplace.visualstudio.com/items?itemName=donaldhays.rgbds-z80) Visual Studio Code Extension.
* The makefile of this project is based on the project [gb-boilerplate](https://github.com/ISSOtm/gb-boilerplate)
* The hardware.inc file of this project is grabbed from the project gameboy [hardware.inc](https://github.com/gbdev/hardware.inc).
