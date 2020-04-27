CPPFLAGS = -nologo -EHsc @syslibs

all : @syslibs testcase testcase2 testcase3

testcase : testcase.exe

testcase2 : testcase2.exe

testcase3 : testcase3.exe

testcase.exe : testcase.cpp

testcase2.exe : testcase2.cpp

testcase3.exe : testcase3.cpp testcase3.res

@syslibs :
    @rem <<syslibs
/link user32.lib
/link gdi32.lib
<<

clean :
    for %%i in (testcase testcase2 testcase3) do @if exist %i.exe del %i.exe
    for %%i in (testcase testcase2 testcase3) do @if exist %i.obj del %i.obj
    for %%i in (testcase testcase2 testcase3) do @if exist %i.res del %i.res
    for %%i in (testcase testcase2 testcase3) do @if exist %i.pdb del %i.pdb
    if exist vc*.pdb del vc*.pdb
    if exist syslibs del syslibs
