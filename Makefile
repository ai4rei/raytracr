# Command macros
LINK = link

# Command configuration
CPPFLAGS = -nologo -W3 -O2x -GF -EHsc -D_CRT_SECURE_NO_WARNINGS -I$(DXSDK)\Include
LFLAGS = -nologo -release user32.lib gdi32.lib comctl32.lib
RFLAGS = -nologo

# Per-target configuration
LDXFLAGS = -libpath:$(DXSDK)\Lib advapi32.lib ole32.lib d3d9.lib d3dx9.lib uuid.lib
LGLFLAGS = opengl32.lib glu32.lib

# Targets
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

raytracr.hpp : \
    _raytracr\camera.hpp \
    _raytracr\color.hpp \
    _raytracr\hit3d.hpp \
    _raytracr\light.hpp \
    _raytracr\matrix.hpp \
    _raytracr\matrix3d.hpp \
    _raytracr\object.hpp \
    _raytracr\plane.hpp \
    _raytracr\plane3d.hpp \
    _raytracr\ray3d.hpp \
    _raytracr\sphere.hpp \
    _raytracr\sphere3d.hpp \
    _raytracr\triangle.hpp \
    _raytracr\vector3d.hpp

clean :
    for %%i in (testcase1 testcase2 testcase2dx testcase2gl testcase2w testcase3) do @for %%j in (exe obj res pdb) do @if exist %i.%j del %i.%j
    if exist vc*.pdb del vc*.pdb
