# this makefile for microsoft C++
#

CC=cl.exe
CCFLAGS = /O2 /EHs /c /nologo /Dx64 /I/adl/src/util /I/adl/src/sass /I/adl/src/sass/generated

LINK=link.exe
LFLAGS=/LTCG:incremental /nologo /NXCOMPAT /DYNAMICBASE /MACHINE:x86 /OPT:REF /SAFESEH  /OPT:ICF /TLBID:1
COMPLIB:="kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "odbc32.lib" "odbccp32.lib" "uxtheme.lib" "comctl32.lib" "msimg32.lib"
UCRTPATH=C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10240.0\ucrt\x86

vpath %.cpp ..\util generated

%.obj: %.cpp
	$(CC) $(CCFLAGS) $<

EXE_dependencies = \
		CmdSwitch.obj \
		CmdFiles.obj \
		Utils.obj \
		AdlFunctions.obj \
		x64Instructions.obj \
		x64Parser.obj \
            InstructionParser.obj \
		Instruction.obj \
            x64stub.obj \
            sass.obj \

all: sass.exe
test: sass.exe
	sass < test.s
adl: ..\cpufiles\x64.adl 
	$(MAKE) /C generated

sass.exe: $(EXE_dependencies)
	$(LINK) $(TYPE) $(LFLAGS) $(COMPLIB) /LIBPATH:"$(UCRTPATH)" /LIBPATH:"$(VCINSTALLDIR)\lib" /OUT:$@  $(EXE_dependencies) 

clean:
	del *.obj *.exe
	omake /C generated clean
	del ..\..\*.exe ..\..\*.adl