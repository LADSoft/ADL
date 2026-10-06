@echo off
omake -C adl
omake -C sass adl
omake -C sass
mkdir ..\dist
copy adl\adl.exe ..
copy sass\sass.exe ..
copy cpufiles\*.adl ..
..\sass < sass\test.s > q.txt
cd ..\..
adl\src\omake -f adl/src/zip.mak
cd adl\src