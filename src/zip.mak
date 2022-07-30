# Software License Agreement
# 
#     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
# 
#     This file is part of the Orange C Compiler package.
# 
#     The Orange C Compiler package is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
# 
#     The Orange C Compiler package is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
# 
#     You should have received a copy of the GNU General Public License
#     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
# 
#     contact information:
#         email: TouchStone222@runbox.com <David Lindauer>
# 

ifneq "$(COMPILER)" "gcc-linux"
SHELL=cmd.exe
endif

ZIP:="c:/program files/7-zip/7z" -tzip -bd

OCC_VERSION := $(word 3, $(shell cmd /C type \adl\src\version.h))
OCC_VERSION := $(subst ",,$(OCC_VERSION))
export OCC_VERSION
VERNAME := $(subst .,,$(OCC_VERSION))

DISTEXE=/adl/dist/ZippedBinaries$(VERNAME).zip
DISTSRC=/adl/dist/ZippedSources$(VERNAME).zip

DISTRIBUTE:
	-del $(subst /,\,$(DISTEXE))
	-del $(subst /,\,$(DISTSRC))
	$(ZIP) -r0 a $(DISTEXE) adl/adl.exe adl/sass.exe adl/*.adl adl/doc/*
	$(ZIP) a $(DISTEXE) adl/LICENSE.txt adl/copying
	$(ZIP) -r0 -xr@adl/src/xclude.lst a $(DISTSRC) adl/src/*.adl adl/src/*.cpp adl/src/*.c adl/src/*.h adl/src/*.nas adl/src/*.ase adl/src/*.asi adl/src/*.inc adl/src/*.p adl/src/*.rc adl/src/*.spc adl/src/*.app adl/src/*.cfg adl/src/makefile* adl/src/*.mak adl/src/*.txt adl/src/copying adl/src/*.def adl/src/*.lst 
	$(ZIP) a $(DISTSRC) adl/doc/*
	$(ZIP) -r0 a $(DISTSRC) adl/src/*.vcxproj adl/src/*.vcxproj.filters adl/src/*.sln
	$(ZIP) a $(DISTSRC) adl/src/*.exe adl/src/config.bat
	$(ZIP) a $(DISTSRC) adl/src/LICENSE.TXT
	$(ZIP) a $(DISTSRC) adl/src/copying