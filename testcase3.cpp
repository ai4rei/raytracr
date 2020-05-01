#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include "simplewnd.tpp"

#include "raytracr.hpp"

#include "testcase3.h"

class CCriticalSection
    : public CRITICAL_SECTION
{
public:
    ~CCriticalSection()
    {
        DeleteCriticalSection(this);
    }
    CCriticalSection()
    {
        InitializeCriticalSection(this);
    }
    void Enter()
    {
        EnterCriticalSection(this);
    }
    void Leave()
    {
        LeaveCriticalSection(this);
    }
};

class CGetDC
{
private:
    CONST HWND m_hWnd;
    CONST HDC m_hDC;

public:
    ~CGetDC()
    {
        if(m_hDC!=NULL)
        {
            ReleaseDC(m_hWnd, m_hDC);
        }
    }

    CGetDC(CONST HWND hWnd)
        : m_hDC(GetDC(hWnd))
        , m_hWnd(hWnd)
    {
    }

    operator HDC() const
    {
        return m_hDC;
    }
};

class CTestWindow
    : public CSimpleWindow
    , protected CCriticalSection
    , protected Raytracer
{
private:
    typedef CTestWindow CSelf;
    typedef CSimpleWindow CSuper;

protected:
    enum STEERINGPLANE
    {
        PLANE_XZ,
        PLANE_XY,
        AXES_AB,
        VIEW_BZ,
    };

protected:
    HBITMAP m_hbmOutput;
    HACCEL m_hAccl;

    STEERINGPLANE m_nSteeringPlane;
    float m_nSteeringPower;
    float m_nSteeringX;
    float m_nSteeringY;
    float m_nSteeringZ;
    float m_nSteeringA;
    float m_nSteeringB;
    bool m_bShowHelp;
    bool m_bRunOnce;
    bool m_bCameraDirty;

public:
    CTestWindow(const HINSTANCE hInstance)
        : CSimpleWindow(hInstance, 320/*GetSystemMetrics(SM_CXSCREEN)*/, 240/*GetSystemMetrics(SM_CYSCREEN)*/, WS_VISIBLE|WS_POPUPWINDOW|WS_SYSMENU|WS_CAPTION|WS_BORDER|WS_MINIMIZEBOX)
        , m_hbmOutput(NULL)
        , m_hAccl(LoadAccelerators(hInstance, MAKEINTRESOURCE(IDA_MAINACCL)))
        , m_nSteeringPlane(PLANE_XZ)
        , m_nSteeringPower(+1.0f)
        , m_nSteeringX(+0.0f)
        , m_nSteeringY(+1.0f)
        , m_nSteeringZ(+0.0f)
        , m_nSteeringA(PI*+0.0f/+180.0f)
        , m_nSteeringB(PI*+0.0f/+180.0f)
        , m_bShowHelp(false)
        , m_bRunOnce(false)
        , m_bCameraDirty(false)
    {
        // Objects
        AddObject(CreatePlane(CreateVector3d(+0.0f, +0.0f, +0.0f), CreateVector3d(+0.0f, +1.0f, +0.0f), CreateColor(1.0f, 1.0f, 1.0f)));

        const float nSphereRange = 25;
        const float nSphereIncrement = 5;

        for(float nY = -nSphereRange/2.0f; nY<=nSphereRange/2.0f; nY+= nSphereIncrement)
        {
            for(float nX = -nSphereRange/2.0f; nX<=nSphereRange/2.0f; nX+= nSphereIncrement)
            {
                const float nRadius = 0.5f+(rand()%3)/2.0f;

                AddObject(CreateSphere(CreateVector3d(nX, nRadius, nY), nRadius, CreateColor((rand()%100)/100.0f, (rand()%100)/100.0f, (rand()%100)/100.0f)));
            }
        }

        // Lights
        AddLight(CreateLight(CreateVector3d(+9.0f, +9.0f, +0.0f), CreateColor(1.0f, 1.0f, 1.0f), 50.0f));

        SetScene(320 /*GetSystemMetrics(SM_CXSCREEN)/1*/, 240/*GetSystemMetrics(SM_CYSCREEN)/1*/);
        SetBounceDepth(3);

        UpdateCamera();
    }

    virtual ~CTestWindow()
    {
        if(m_hbmOutput!=NULL)
        {
            DeleteBitmap(m_hbmOutput);
            m_hbmOutput = NULL;
        }
    }

    static HBITMAP BitmapFromPixels2(const std::vector< CColor >& aclrPixels, const int nWidth, const int nHeight)
    {
        HBITMAP hbmOutput = NULL;
        BITMAPINFO bmiOutput = { 0 };
        VOID* lpBits = NULL;

        bmiOutput.bmiHeader.biSize = sizeof(bmiOutput.bmiHeader);
        bmiOutput.bmiHeader.biWidth = nWidth;
        bmiOutput.bmiHeader.biHeight = nHeight;  // bottom-up, just like our pixel buffer
        bmiOutput.bmiHeader.biPlanes = 1;
        bmiOutput.bmiHeader.biBitCount = 24;
        bmiOutput.bmiHeader.biCompression = BI_RGB;

        hbmOutput = CreateDIBSection(CGetDC(NULL), &bmiOutput, DIB_RGB_COLORS, &lpBits, NULL, 0);

        if(hbmOutput!=NULL)
        {
            BITMAP bmInfo = { 0 };

            GetObject(hbmOutput, sizeof(BITMAP), &bmInfo);

            const int nWidthBytes = bmInfo.bmWidthBytes;
            const int nPixelBytes = bmInfo.bmBitsPixel/8;

            for(int nY = 0; nY<nHeight; nY++)
            {
                for(int nX = 0; nX<nWidth; nX++)
                {
                    const auto& Px = aclrPixels[nX+nY*nWidth];
                    unsigned char* ucBits = &static_cast< unsigned char* >(lpBits)[nY*nWidthBytes+nX*nPixelBytes];

                    ucBits[0] = static_cast< unsigned char >(sqrt(Px.B())*255U);
                    ucBits[1] = static_cast< unsigned char >(sqrt(Px.G())*255U);
                    ucBits[2] = static_cast< unsigned char >(sqrt(Px.R())*255U);
                }
            }

            GdiFlush();
        }

        return hbmOutput;
    }

    void UpdateCamera()
    {
        puts("");
        printf("X:%+2.3f Y:%+2.3f Z:%+2.3f A:%+2.3f B:%+2.3f\n", m_nSteeringX, m_nSteeringY, m_nSteeringZ, m_nSteeringA*180.0f/PI, m_nSteeringB*180.0f/PI);

        const CVector3d ovecEye = CVector3d(+0.0f, +0.0f, +0.0f)
            .MatrixProduct(CMatrix3d()
                .SetIdentity()
                .AddXTranslation(m_nSteeringX)
                .AddYTranslation(m_nSteeringY)
                .AddZTranslation(m_nSteeringZ)
            );

        const CVector3d rvecLookAt = CVector3d(+0.0f, +0.0f, -1.0f)
            .MatrixProduct(CMatrix3d()
                .SetIdentity()
                .AddYRotation(m_nSteeringB)
                .AddXRotation(m_nSteeringA)
            );

        const CVector3d uvecUp = CVector3d(+0.0f, +1.0f, -0.0f)
            .MatrixProduct(CMatrix3d()
                .SetIdentity()
                .AddYRotation(m_nSteeringB)
                .AddXRotation(m_nSteeringA)
            );

        SetCamera(ovecEye, ovecEye+rvecLookAt, uvecUp);

        m_bCameraDirty = true;
    }

    void UpdateRenderAsync()
    {
        DWORD dwStart;

        dwStart = GetTickCount();
        //Render(&TrackProgress, NULL);
        Render(NULL);
        printf("%ums", GetTickCount()-dwStart);

        dwStart = GetTickCount();
        HBITMAP hbmOutput = BitmapFromPixels2(GetResult(), GetSceneWidth(), GetSceneHeight());
        printf("+%ums\n", GetTickCount()-dwStart);

        Enter();
        {
            DeleteObject(m_hbmOutput);
            m_hbmOutput = hbmOutput;
        }
        Leave();
    }

    static DWORD CALLBACK UpdateRenderAsyncCB(LPVOID lpParam)
    {
        CSelf* lpThis = static_cast< CSelf* >(lpParam);

        lpThis->UpdateRenderAsync();

        return 0;
    }

    void UpdateRender(HWND hWnd)
    {
        DWORD dwThreadId;
        HANDLE hThread = CreateThread(NULL, 0, &UpdateRenderAsyncCB, this, CREATE_SUSPENDED, &dwThreadId);

        m_bCameraDirty = false;

        if(hThread!=NULL)
        {
            ResumeThread(hThread);

            for(;;)
            {
                DWORD dwWait = MsgWaitForMultipleObjects(1, &hThread, FALSE, INFINITE, QS_ALLINPUT);

                if(dwWait==WAIT_OBJECT_0+0)
                {
                    InvalidateRect(hWnd, NULL, FALSE);
                    break;
                }
                else
                if(dwWait==WAIT_OBJECT_0+1)
                {
                    MSG Msg;

                    while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
                    {
                        if(Msg.message==WM_QUIT)
                        {
                            PostQuitMessage((int)Msg.wParam);
                            break;
                        }

                        DispatchMessage(&Msg);
                    }
                }
            }

            CloseHandle(hThread);
            hThread = NULL;

            InvalidateRect(hWnd, NULL, FALSE);
        }
    }

    /*
            B (rotational axis)
            Y (linar direction)
            |
            o---X (linear direction)
           /    A (rotational axis)
          Z (linear direction)
    */
    void SteerX(const float nDirection)
    {
        m_nSteeringX+= nDirection*m_nSteeringPower;
        UpdateCamera();
    }

    void SteerY(const float nDirection)
    {
        m_nSteeringY+= nDirection*m_nSteeringPower;
        UpdateCamera();
    }

    void SteerZ(const float nDirection)
    {
        m_nSteeringZ+= nDirection*m_nSteeringPower;
        UpdateCamera();
    }

    void SteerA(const float nDirection)
    {
        m_nSteeringA+= PI/180.0f*nDirection*m_nSteeringPower;
        UpdateCamera();
    }

    void SteerB(const float nDirection)
    {
        m_nSteeringB+= PI/180.0f*nDirection*m_nSteeringPower;
        UpdateCamera();
    }

    void SteerViewZ(const float nDirection)
    {// forward-backward translation
        const CVector3d rvecMove = CVector3d(+0.0f, +0.0f, nDirection).MatrixProduct(CMatrix3d().SetIdentity().AddYRotation(m_nSteeringB));

        m_nSteeringX+= rvecMove.X();
        m_nSteeringZ+= rvecMove.Z();
        UpdateCamera();
    }

    void SteerUp()
    {
        switch(m_nSteeringPlane)
        {
        case PLANE_XZ:  // -Z
            SteerZ(-1.0f);
            break;
        case PLANE_XY:  // +Y
            SteerY(+1.0f);
            break;
        case AXES_AB:   // +A
            SteerA(+1.0f);
            break;
        case VIEW_BZ:  // -Z
            SteerViewZ(-1.0f);
        }
    }

    void SteerRight()
    {
        switch(m_nSteeringPlane)
        {
        case PLANE_XZ:  // +X
            SteerX(+1.0f);
            break;
        case PLANE_XY:  // +X
            SteerX(+1.0f);
            break;
        case AXES_AB:   // +B
            SteerB(+1.0f);
            break;
        case VIEW_BZ:  // -B
            SteerB(-1.0f);
            break;
        }
    }

    void SteerDown()
    {
        switch(m_nSteeringPlane)
        {
        case PLANE_XZ:  // +Z
            SteerZ(+1.0f);
            break;
        case PLANE_XY:  // -Y
            SteerY(-1.0f);
            break;
        case AXES_AB:   // -A
            SteerA(-1.0f);
            break;
        case VIEW_BZ:  // +Z
            SteerViewZ(+1.0f);
            break;
        }
    }

    void SteerLeft()
    {
        switch(m_nSteeringPlane)
        {
        case PLANE_XZ:  // -X
            SteerX(-1.0f);
            break;
        case PLANE_XY:  // -X
            SteerX(-1.0f);
            break;
        case AXES_AB:   // -B
            SteerB(-1.0f);
            break;
        case VIEW_BZ:  // +B
            SteerB(+1.0f);
            break;
        }
    }

    virtual BOOL IsAcceleratorMessage(HWND hWnd, LPMSG lpMsg)
    {
        if(m_hAccl!=NULL && TranslateAccelerator(hWnd, m_hAccl, lpMsg))
        {
            return TRUE;
        }

        return FALSE;
    }

    virtual VOID WndProcOnCommand(HWND hWnd, INT nId, HWND hWndCtl, UINT uCodeNotify)
    {
        switch(nId)
        {
        case IDC_SET_PLANE_XZ:
            m_nSteeringPlane = PLANE_XZ;
            puts("Steering: XZ");
            break;
        case IDC_SET_PLANE_XY:
            m_nSteeringPlane = PLANE_XY;
            puts("Steering: XY");
            break;
        case IDC_SET_AXES_AB:
            m_nSteeringPlane = AXES_AB;
            puts("Steering: AB");
            break;
        case IDC_SET_VIEW_BZ:
            m_nSteeringPlane = VIEW_BZ;
            puts("Steering: BZ");
            break;
        case IDC_STEER_UP:
            SteerUp();
            break;
        case IDC_STEER_RIGHT:
            SteerRight();
            break;
        case IDC_STEER_DOWN:
            SteerDown();
            break;
        case IDC_STEER_LEFT:
            SteerLeft();
            break;
        case IDC_TOGGLE_HELP:
            m_bShowHelp = !m_bShowHelp;
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }

        if(m_bCameraDirty)
        {
            UpdateRender(hWnd);
        }

        CSuper::WndProcOnCommand(hWnd, nId, hWndCtl, uCodeNotify);
    }

    virtual BOOL WndProcOnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
    {
        if(CSuper::WndProcOnCreate(hWnd, lpCreateStruct))
        {
            SetWindowText(hWnd, "TestCase (press F1 for help, ESC to exit)");

            return TRUE;
        }

        return FALSE;
    }

    virtual VOID WndProcOnPaint(HWND hWnd)
    {
        if(GetUpdateRect(hWnd, NULL, FALSE))
        {
            PAINTSTRUCT Ps = { 0 };

            if(BeginPaint(hWnd, &Ps))
            {
                HDC hDC = CreateCompatibleDC(NULL);
                HGDIOBJ hPrevObj;
                RECT rcWnd;

                GetClientRect(hWnd, &rcWnd);

                if(m_hbmOutput!=NULL)
                {
                    BITMAP bmInfo;

                    GetObject(m_hbmOutput, sizeof(bmInfo), &bmInfo);

                    const int nDstW = rcWnd.right-rcWnd.left;
                    const int nDstH = rcWnd.bottom-rcWnd.top;
                    const int nSrcW = bmInfo.bmWidth;
                    const int nSrcH = bmInfo.bmHeight;

                    hPrevObj = SelectObject(hDC, m_hbmOutput);
                    StretchBlt(Ps.hdc, 0, 0, nDstW, nDstH, hDC, 0, 0, nSrcW, nSrcH, SRCCOPY);
                    SelectObject(hDC, hPrevObj);
                }

                DeleteDC(hDC);

                if(m_bShowHelp)
                {
                    const int nSavedTextDC = SaveDC(Ps.hdc);

                    SetTextColor(Ps.hdc, RGB(255, 255, 0));
                    SetBkColor(Ps.hdc, RGB(0, 0, 255));
                    SetBkMode(Ps.hdc, TRANSPARENT);
                    DrawTextEx
                    (
                        Ps.hdc,
                        "Steering mode: 1=XZ 2=XY 3=AB 4=BZ\r\n"
                        "Steering: UP RIGHT DOWN LEFT or WASD\r\n",
                        -1,
                        &rcWnd,
                        DT_WORDBREAK|DT_TOP|DT_LEFT,
                        NULL
                    );

                    RestoreDC(Ps.hdc, nSavedTextDC);
                }

                EndPaint(hWnd, &Ps);
            }
        }
    }

    virtual UINT WndProcOnNCHitTest(HWND hWnd, int nX, int nY)
    {
        UINT uHitTest = CSuper::WndProcOnNCHitTest(hWnd, nX, nY);

        if(uHitTest==HTCLIENT)
        {
            uHitTest = HTCAPTION;
        }

        return uHitTest;
    }

    virtual VOID WndProcOnTimer(HWND hWnd, UINT uId)
    {
        switch(uId)
        {
        case IDT_RUNONCE:
            KillTimer(hWnd, IDT_RUNONCE);

            if(!m_bRunOnce)
            {
                m_bRunOnce = true;

                UpdateRender(hWnd);
            }
            return;
        }

        CSuper::WndProcOnTimer(hWnd, uId);
    }

    virtual VOID WndProcOnWindowPosChanged(HWND hWnd, CONST WINDOWPOS* lpWP)
    {
        if(!m_bRunOnce)
        {
            SetTimer(hWnd, IDT_RUNONCE, 0, NULL);
        }

        CSuper::WndProcOnWindowPosChanged(hWnd, lpWP);
    }
};

int __cdecl main(int nArgc, char** lppszArgv)
{
    srand(GetTickCount());

    return CTestWindow(GetModuleHandle(NULL));
}
