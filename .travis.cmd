@ECHO OFF
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
SET DX7SDK=D3D7SDK-Mini
SET DX8SDK=D3D8SDK-Mini
SET DX9SDK=D3D9SDK-Mini
NMAKE /P CLEAN ALL
