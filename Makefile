# Command macros
LINK = link

# Command configuration
CPPFLAGS = -nologo -W3 -O2x -GF -EHsc -D_CRT_SECURE_NO_WARNINGS -I$(DX7SDK)\Include -I$(DX8SDK)\Include -I$(DX9SDK)\Include
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

!IFDEF DXSDK
!IFNDEF DX7SDK
DX7SDK = $(DXSDK)
!ENDIF
!IFNDEF DX8SDK
DX8SDK = $(DXSDK)
!ENDIF
!IFNDEF DX9SDK
DX9SDK = $(DXSDK)
!ENDIF
!ENDIF

# Per-target configuration
LDX7FLAGS = -libpath:$(DX7SDK)\Lib advapi32.lib ole32.lib ddraw.lib d3dx.lib dxguid.lib
LDX8FLAGS = -libpath:$(DX8SDK)\Lib advapi32.lib ole32.lib d3d8.lib d3dx8.lib -nodefaultlib:libci.lib
LDX9FLAGS = -libpath:$(DX9SDK)\Lib advapi32.lib ole32.lib d3d9.lib d3dx9.lib
!IF $(CCVERSION) >= 1900
LDX8FLAGS = $(LDX8FLAGS) legacy_stdio_definitions.lib
LDX9FLAGS = $(LDX9FLAGS) legacy_stdio_definitions.lib
!ENDIF
LGLFLAGS = opengl32.lib glu32.lib

# Targets
all : \
    testcase1.exe \
    testcase2.exe \
    testcase2dx7.exe \
    testcase2dx7x.exe \
    testcase2dx8.exe \
    testcase2dx9.exe \
    testcase2gl.exe \
    testcase2w.exe \
    testcase3.exe

testcase1.exe : testcase1.obj
    $(LINK) $(LFLAGS) -out:$@ $**

testcase2.exe : testcase2.obj
    $(LINK) $(LFLAGS) -out:$@ $**

testcase2dx7.exe : testcase2dx7.obj
    $(LINK) $(LFLAGS) $(LDX7FLAGS) -out:$@ $**

testcase2dx7x.exe : testcase2dx7x.obj
    $(LINK) $(LFLAGS) $(LDX7FLAGS) -out:$@ $**

testcase2dx8.exe : testcase2dx8.obj
    $(LINK) $(LFLAGS) $(LDX8FLAGS) -out:$@ $**

testcase2dx9.exe : testcase2dx9.obj
    $(LINK) $(LFLAGS) $(LDX9FLAGS) -out:$@ $**

testcase2gl.exe : testcase2gl.obj
    $(LINK) $(LFLAGS) $(LGLFLAGS) -out:$@ $**

testcase2w.exe : testcase2w.obj
    $(LINK) $(LFLAGS) -out:$@ $**

testcase3.exe : testcase3.obj testcase3.res
    $(LINK) $(LFLAGS) -out:$@ $**

testcase1.cpp testcase2.cpp testcase2w.cpp testcase3.cpp : raytracr.hpp

testcase1.cpp testcase2dx7.cpp testcase2dx7x.cpp testcase2dx8.cpp testcase2dx9.cpp testcase2gl.cpp testcase2w.cpp testcase3.cpp : simplewnd.tpp utility.hpp

testcase3.cpp :  testcase3.h

raytracr.hpp : _raytracr\*.hpp

clean :
    for %%i in (testcase1 testcase2 testcase2dx7 testcase2dx7x testcase2dx8 testcase2dx9 testcase2gl testcase2w testcase3) do @for %%j in (exe obj res pdb) do @if exist %i.%j del %i.%j
    if exist vc*.pdb del vc*.pdb
