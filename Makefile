LINK = link

CPPFLAGS = -nologo -W3 -O2x -GF -EHsc -D_CRT_SECURE_NO_WARNINGS
LFLAGS = -nologo -release user32.lib gdi32.lib
RFLAGS = -nologo

RTPATH = _raytracr/
RTHEADERS = \
    $(RTPATH)camera.hpp \
    $(RTPATH)color.hpp \
    $(RTPATH)hit3d.hpp \
    $(RTPATH)light.hpp \
    $(RTPATH)matrix.hpp \
    $(RTPATH)matrix3d.hpp \
    $(RTPATH)object.hpp \
    $(RTPATH)plane.hpp \
    $(RTPATH)plane3d.hpp \
    $(RTPATH)ray3d.hpp \
    $(RTPATH)sphere.hpp \
    $(RTPATH)sphere3d.hpp \
    $(RTPATH)triangle.hpp \
    $(RTPATH)vector3d.hpp

all : \
    testcase.exe \
    testcase2.exe \
    testcase3.exe

testcase.exe : testcase.obj
    $(LINK) $(LFLAGS) -out:$@ $**

testcase2.exe : testcase2.obj
    $(LINK) $(LFLAGS) -out:$@ $**

testcase3.exe : testcase3.obj testcase3.res
    $(LINK) $(LFLAGS) -out:$@ $**

testcase.cpp testcase2.cpp testcase3.cpp : raytracr.hpp

testcase3.cpp :  testcase3.h

raytracr.hpp : $(RTHEADERS)

clean :
    for %%i in (testcase testcase2 testcase3) do @for %%j in (exe obj res pdb) do @if exist %i.%j del %i.%j
    if exist vc*.pdb del vc*.pdb
