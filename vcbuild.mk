all:
	VCBuild.exe /nologo PESetEnv.vcproj /rebuild

clean:
	VCBuild.exe /nologo PESetEnv.vcproj /clean
	
upgrade:
	devenv PESetEnv.sln /upgrade

.PHONY:	all clean upgrade

