LINK = link

CPPFLAGS = -nologo -W3 -O2x -GF -EHsc -D_CRT_SECURE_NO_WARNINGS
LFLAGS = -nologo -release user32.lib gdi32.lib comctl32.lib
RFLAGS = -nologo

LDXFLAGS = ole32.lib d3d9.lib uuid.lib
LGLFLAGS = opengl32.lib glu32.lib

RTPATH = $(MAKEDIR)/_raytracr/
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
    testcase1.exe \
    testcase2.exe \
    testcase2dx.exe \
    testcase2gl.exe \
    testcase2w.exe \
    testcase3.exe

testcase1.exe : testcase1.obj
    $(LINK) $(LFLAGS) -out:$@ $**

testcase2.exe : testcase2.obj
    $(LINK) $(LFLAGS) -out:$@ $**

testcase2dx.exe : testcase2dx.obj
    $(LINK) $(LFLAGS) $(LDXFLAGS) -out:$@ $**

testcase2gl.exe : testcase2gl.obj
    $(LINK) $(LFLAGS) $(LGLFLAGS) -out:$@ $**

testcase2w.exe : testcase2w.obj
    $(LINK) $(LFLAGS) -out:$@ $**

testcase3.exe : testcase3.obj testcase3.res
    $(LINK) $(LFLAGS) -out:$@ $**

testcase1.cpp testcase2.cpp testcase2w.cpp testcase3.cpp : raytracr.hpp

testcase1.cpp testcase2dx.cpp testcase2gl.cpp testcase2w.cpp testcase3.cpp : simplewnd.tpp utility.hpp

testcase3.cpp :  testcase3.h

raytracr.hpp : $(RTHEADERS)

clean :
    for %%i in (testcase1 testcase2 testcase2dx testcase2gl testcase2w testcase3) do @for %%j in (exe obj res pdb) do @if exist %i.%j del %i.%j
    if exist vc*.pdb del vc*.pdb
