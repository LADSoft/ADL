Build instructions:
 
	run config.bat to set up the environment.   It calls VCVARS32.bat which attempts to install the VS2019 command line environment (32 bit).    VCVARS32.bat may need updating if you have a different version or flavor of VS.

	after that run build.bat.  It will compile the programs then create the distribution archives in \orc\dist.

	if you are compiling via the IDE please compile and run ADL.EXE first; the IDE debug settings are set up to use it to generate files that are used when compiling SASS.EXE.