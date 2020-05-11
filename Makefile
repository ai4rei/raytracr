# Command macros
LINK = link

# Command configuration
CPPFLAGS = -nologo -W3 -O2x -GF -EHsc -D_CRT_SECURE_NO_WARNINGS -I$(DXSDK)\Include
LFLAGS = -nologo -release user32.lib gdi32.lib comctl32.lib
RFLAGS = -nologo

# Dynamic configuration
!IF [$(CC) -nologo -c 2> NUL > NUL] == 0
NOOUTPUT = > NUL
!ELSE
NOOUTPUT = 2> NUL > NUL
!ENDIF

!IF [$(CC) -nologo -EP -P msvc\msvc_detect_version.cpp $(NOOUTPUT)] == 0
!INCLUDE msvc_detect_version.i
!IF [DEL msvc_detect_version.i]
!ENDIF
!MESSAGE Detected $(CC) version: $(CCVERSION)
!ELSE
!ERROR Failed to retrieve $(CC) version.
!ENDIF

# Per-target configuration
LDX8FLAGS = -libpath:$(DXSDK)\Lib advapi32.lib ole32.lib d3d8.lib d3dx8.lib uuid.lib -nodefaultlib:libci.lib
LGLFLAGS = opengl32.lib glu32.lib

# Targets
all : \
    testcase1.exe \
    testcase2.exe \
    testcase2dx8.exe \
    testcase2gl.exe \
    testcase2w.exe \
    testcase3.exe

testcase1.exe : testcase1.obj
    $(LINK) $(LFLAGS) -out:$@ $**

testcase2.exe : testcase2.obj
    $(LINK) $(LFLAGS) -out:$@ $**

testcase2dx8.exe : testcase2dx8.obj
    $(LINK) $(LFLAGS) $(LDX8FLAGS) -out:$@ $**

testcase2gl.exe : testcase2gl.obj
    $(LINK) $(LFLAGS) $(LGLFLAGS) -out:$@ $**

testcase2w.exe : testcase2w.obj
    $(LINK) $(LFLAGS) -out:$@ $**

testcase3.exe : testcase3.obj testcase3.res
    $(LINK) $(LFLAGS) -out:$@ $**

testcase1.cpp testcase2.cpp testcase2w.cpp testcase3.cpp : raytracr.hpp

testcase1.cpp testcase2dx8.cpp testcase2gl.cpp testcase2w.cpp testcase3.cpp : simplewnd.tpp utility.hpp

testcase3.cpp :  testcase3.h

raytracr.hpp : _raytracr\*.hpp

clean :
    for %%i in (testcase1 testcase2 testcase2dx8 testcase2gl testcase2w testcase3) do @for %%j in (exe obj res pdb) do @if exist %i.%j del %i.%j
    if exist vc*.pdb del vc*.pdb
