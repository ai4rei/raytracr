LINK = link.exe

CPPFLAGS = -nologo -W3 -O2x -GF -Zi -EHsc -D_CRT_SECURE_NO_WARNINGS @linker
LFLAGS = -nologo -release user32.lib gdi32.lib
RFLAGS = -nologo

all : @linker testcase testcase2 testcase3

testcase : testcase.exe

testcase2 : testcase2.exe

testcase3 : testcase3.exe

testcase.exe : testcase.cpp

testcase2.exe : testcase2.cpp

testcase3.exe : testcase3.obj testcase3.res
    $(LINK) $(LFLAGS) -out:$@ $**

@linker :
    @rem <<linker
/link /RELEASE
/link user32.lib
/link gdi32.lib
<<

clean :
    for %%i in (testcase testcase2 testcase3) do @for %%j in (exe obj res pdb) do @if exist %i.%j del %i.%j
    if exist vc*.pdb del vc*.pdb
    if exist linker del linker
