@ECHO OFF
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
SET DX8SDK=D3D8SDK-Mini
NMAKE /P CLEAN ALL
