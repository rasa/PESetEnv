.PHONY:	all clean realclean

all:
	$(MAKE) /f PESetEnv.mak CFG="PESetEnv - Win32 Release"	all
	$(MAKE) /f PESetEnv.mak CFG="PESetEnv - Win32 Debug"	all

clean:
	$(MAKE) /f PESetEnv.mak CFG="PESetEnv - Win32 Release"	clean
	$(MAKE) /f PESetEnv.mak CFG="PESetEnv - Win32 Debug"	clean

realclean: clean
	-cmd /c del /s *.bak *.bsc *.idb *.pdb *.lib *.ncb *.obj *.opt *.pch *.plg *.sbr

