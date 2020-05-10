@ECHO OFF
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvars32.bat"
SET DXSDK=D3D9SDK-Mini
NMAKE CLEAN ALL
