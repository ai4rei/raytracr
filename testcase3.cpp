#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "simplewnd.tpp"
#include "utility.hpp"

#include "raytracr.hpp"

#include "testcase3.h"

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
    DWORD m_dwRenderTime;
    DWORD m_dwDrawingTime;
    HBITMAP m_hbmOutput;
    HACCEL m_hAccl;
    SIZE m_LastWndSize;

    STEERINGPLANE m_nSteeringPlane;
    float m_nSteeringPower;
    float m_nSteeringX;
    float m_nSteeringY;
    float m_nSteeringZ;
    float m_nSteeringA;
    float m_nSteeringB;
    int m_nPixelRatio;
    unsigned int m_uAntiAliasing;
    unsigned char m_uRenderProgress;
    bool m_bShowHelp;
    bool m_bRunOnce;
    bool m_bCameraDirty;

public:
    CTestWindow(const HINSTANCE hInstance)
        : CSimpleWindow(hInstance, 480, 320, WS_VISIBLE|WS_POPUPWINDOW|WS_SYSMENU|WS_CAPTION|WS_BORDER|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SIZEBOX)
        , m_dwRenderTime(0)
        , m_dwDrawingTime(0)
        , m_hbmOutput(nullptr)
        , m_hAccl(LoadAccelerators(hInstance, MAKEINTRESOURCE(IDA_MAINACCL)))
        , m_LastWndSize()
        , m_nSteeringPlane(PLANE_XZ)
        , m_nSteeringPower(+1.0f)
        , m_nSteeringX(+0.0f)
        , m_nSteeringY(+1.0f)
        , m_nSteeringZ(+0.0f)
        , m_nSteeringA(PI*+0.0f/+180.0f)
        , m_nSteeringB(PI*+0.0f/+180.0f)
        , m_nPixelRatio(1)
        , m_uAntiAliasing(0)
        , m_uRenderProgress(0)
        , m_bShowHelp(false)
        , m_bRunOnce(false)
        , m_bCameraDirty(false)
    {
        // Objects
        AddObject(CreatePlane(CreateVector3d(+0.0f, +0.0f, +0.0f), CreateVector3d(+0.0f, +1.0f, +0.0f), CreateColor(1.0f, 1.0f, 1.0f)));

        const float SF_RADIUS_MIN = 0.5f;
        const float SF_RADIUS_INT = 0.5f;
        const float SF_RADIUS_MAX = 1.5f;
        const float SF_SPHERE_DST = 2.5f;
        const int   SF_SPHERE_ROW = 10;

        srand(GetTickCount());

        for(int nY = 0; nY<SF_SPHERE_ROW; nY++)
        {
            for(int nX = 0; nX<SF_SPHERE_ROW; nX++)
            {
                const float nPosX   = (nX-SF_SPHERE_ROW/2.0f+0.5f)*SF_SPHERE_DST;
                const float nPosY   = (nY-SF_SPHERE_ROW/2.0f+0.5f)*SF_SPHERE_DST;
                const float nRadius = SF_RADIUS_MIN+(rand()%(int(SF_RADIUS_MAX-SF_RADIUS_MIN)+1))*SF_RADIUS_INT;

                AddObject(CreateSphere(CreateVector3d(nPosX, nRadius, nPosY), nRadius, CreateColor((rand()%100)/100.0f, (rand()%100)/100.0f, (rand()%100)/100.0f)));
            }
        }

        // Lights
        AddLight(CreateLight(CreateVector3d(+0.0f, +9.0f, +0.0f), CreateColor(1.0f, 1.0f, 1.0f), 50.0f, 0.0f, 0.1f, 0.0f));

        SetBounceDepth(2);
        SetCallback(&UpdateRenderProgressCB);

        UpdateCamera();
    }

    virtual ~CTestWindow()
    {
        SetOutputBitmap(nullptr);
    }

    bool SetOutputBitmap(HBITMAP hbmOutput, const bool bOnlyIfNotNull = false)
    {
        bool bSuccess = true;

        Enter();
        {
            if(m_hbmOutput!=nullptr)
            {
                if(bOnlyIfNotNull)
                {
                    bSuccess = false;
                }
                else
                {
                    DeleteBitmap(m_hbmOutput);
                    m_hbmOutput = hbmOutput;
                }
            }
            else
            {
                m_hbmOutput = hbmOutput;
            }
        }
        Leave();

        return bSuccess;
    }

    void SetProgress(HWND hWndCtl, unsigned char ucPercent)
    {
        // first update the control itself
        SendMessage(hWndCtl, PBM_SETPOS, ucPercent, 0);

        if(ucPercent==100)
        {// hide
            if(GetWindowStyle(hWndCtl)&WS_VISIBLE)
            {
                ShowWindow(hWndCtl, SW_HIDE);
            }
        }
        else
        if(ucPercent>0)
        {// show
            if(!(GetWindowStyle(hWndCtl)&WS_VISIBLE))
            {
                ShowWindow(hWndCtl, SW_SHOW);
            }
        }
    }

    static void DrawCake(HDC hDC, const int nWidth, const int nHeight, int nPenWidth, COLORREF clrStroke, COLORREF clrBack)
    {
        RECT rcAll;

        SetRect(&rcAll, 0, 0, nWidth, nHeight);

        RECT rcPie = rcAll;

        // calculate
        if(nWidth/2>nHeight)
        {
            InflateRect(&rcPie, nHeight-nWidth/2, 0);
        }
        else
        {
            InflateRect(&rcPie, 0, nWidth/4-nHeight/2);
        }

        InflateRect(&rcPie, (rcPie.left-rcPie.right)/4, (rcPie.top-rcPie.bottom)/4);

        const int nPieWidth = rcPie.right-rcPie.left;
        const int nPieHeight = rcPie.bottom-rcPie.top;

        OffsetRect(&rcPie, -nPieWidth/4, 0);

        const float nOneAngle = -20.0f/180.0f*PI;
        const float nTwoAngle = -60.0f/180.0f*PI;

        POINT aptPoly[4];

        aptPoly[0].x = rcPie.left+(nPieWidth/2)+int((nPieWidth/2)*cos(nOneAngle));
        aptPoly[0].y = rcPie.top+(nPieHeight/2)+int((nPieHeight/2)*sin(nOneAngle));

        aptPoly[1].x = aptPoly[0].x;
        aptPoly[1].y = aptPoly[0].y+nPieHeight/4;

        aptPoly[3].x = rcPie.left+nPieWidth/2;
        aptPoly[3].y = rcPie.top+nPieHeight/2;

        aptPoly[2].x = aptPoly[3].x;
        aptPoly[2].y = aptPoly[3].y+nPieHeight/4;

        POINT& ptRadial1 = aptPoly[0];
        POINT ptRadial2;

        ptRadial2.x = rcPie.left+(nPieWidth/2)+int((nPieWidth/2)*cos(nTwoAngle));
        ptRadial2.y = rcPie.top+(nPieHeight/2)+int((nPieHeight/2)*sin(nTwoAngle));

        SaveDC(hDC);
        {
            CSolidPen hpnPen(nPenWidth, clrStroke);
            SelectObject(hDC, hpnPen);

            // background
            FillRect(hDC, &rcAll, CSolidBrush(clrBack));

            // side
            CSolidBrush hbrSide(RGB(GetRValue(clrBack)/2, GetGValue(clrBack)/2, GetBValue(clrBack)/2));
            SelectObject(hDC, hbrSide);
            Polygon(hDC, aptPoly, _ARRAYSIZE(aptPoly));

            //top
            CSolidBrush hbrTop(RGB(GetRValue(clrBack)*3/2, GetGValue(clrBack)*3/2, GetBValue(clrBack)*3/2));
            SelectObject(hDC, hbrTop);
            Pie(hDC, rcPie.left, rcPie.top, rcPie.right, rcPie.bottom, ptRadial1.x, ptRadial1.y, ptRadial2.x, ptRadial2.y);
        }
        RestoreDC(hDC, -1);
    }

    static HBITMAP BitmapFromPixels2(const std::vector< CColor >& aclrPixels, const int nImageW, const int nImageH)
    {
        HBITMAP hbmOutput = nullptr;
        BITMAPINFO bmiOutput = { 0 };
        VOID* lpBits = nullptr;

        bmiOutput.bmiHeader.biSize = sizeof(bmiOutput.bmiHeader);
        bmiOutput.bmiHeader.biWidth = nImageW;
        bmiOutput.bmiHeader.biHeight = nImageH;  // bottom-up, just like our pixel buffer
        bmiOutput.bmiHeader.biPlanes = 1;
        bmiOutput.bmiHeader.biBitCount = 24;
        bmiOutput.bmiHeader.biCompression = BI_RGB;

        hbmOutput = CreateDIBSection(CGetDC(nullptr), &bmiOutput, DIB_RGB_COLORS, &lpBits, nullptr, 0);

        if(hbmOutput!=nullptr)
        {
            BITMAP bmInfo = { 0 };

            GetObject(hbmOutput, sizeof(BITMAP), &bmInfo);

            const int nWidthBytes = bmInfo.bmWidthBytes;
            const int nPixelBytes = bmInfo.bmBitsPixel/8;

            for(int nImageY = 0; nImageY<nImageH; nImageY++)
            {
                for(int nImageX = 0; nImageX<nImageW; nImageX++)
                {
                    const auto& Px = aclrPixels[nImageX+nImageY*nImageW];
                    unsigned char* ucBits = &static_cast< unsigned char* >(lpBits)[nImageY*nWidthBytes+nImageX*nPixelBytes];

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
        const CVector3d ovecEye = CVector3d(+0.0f, +0.0f, +0.0f)
            .MatrixProduct(CMatrix3d()
                .SetIdentity()
                .AddXTranslation(m_nSteeringX)
                .AddYTranslation(m_nSteeringY)
                .AddZTranslation(m_nSteeringZ)
            );

        const CMatrix3d mtxRotation = CMatrix3d()
            .SetIdentity()
            .AddYRotation(m_nSteeringB)
            .AddXRotation(m_nSteeringA)
            ;

        const CVector3d rvecLookAt = CVector3d(+0.0f, +0.0f, -1.0f)
            .MatrixProduct(mtxRotation);

        const CVector3d uvecUp = CVector3d(+0.0f, +1.0f, -0.0f)
            .MatrixProduct(mtxRotation);

        SetCamera(ovecEye, rvecLookAt, uvecUp);

        m_bCameraDirty = true;
    }

    bool UpdateRenderProgress(const float nR, const float nG, const float nB, const int nX, const int nY)
    {
        m_uRenderProgress = (nY*100)/GetImageHeight();

        return true;
    }

    static bool UpdateRenderProgressCB(const float nR, const float nG, const float nB, const int nX, const int nY, void* lpContext)
    {
        CSelf* lpThis = static_cast< CSelf* >(lpContext);

        return lpThis->UpdateRenderProgress(nR, nG, nB, nX, nY);
    }

    void UpdateRenderAsync()
    {
        DWORD dwStart;

        dwStart = GetTickCount();
        Render(this);
        m_dwRenderTime = GetTickCount()-dwStart;

        dwStart = GetTickCount();
        HBITMAP hbmOutput = BitmapFromPixels2(GetResult(), GetImageWidth(), GetImageHeight());
        m_dwDrawingTime = GetTickCount()-dwStart;

        SetOutputBitmap(hbmOutput);
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
        HANDLE hThread = CreateThread(nullptr, 0, &UpdateRenderAsyncCB, this, CREATE_SUSPENDED, &dwThreadId);
        RECT rcWnd;

        GetClientRect(hWnd, &rcWnd);
        SetImageSize((rcWnd.right-rcWnd.left)/m_nPixelRatio, (rcWnd.bottom-rcWnd.top)/m_nPixelRatio);

        m_bCameraDirty = false;

        if(hThread!=nullptr)
        {
            m_uRenderProgress = 0;

            SetProgress(GetDlgItem(hWnd, IDC_PROGRESSBAR), 0);
            SetTimer(hWnd, IDT_RENDERPROGRESS, 500, nullptr);
            ResumeThread(hThread);

            for(;;)
            {
                DWORD dwWait = MsgWaitForMultipleObjects(1, &hThread, FALSE, INFINITE, QS_ALLINPUT);

                if(dwWait==WAIT_OBJECT_0+0)
                {
                    InvalidateRect(hWnd, nullptr, FALSE);
                    break;
                }
                else
                if(dwWait==WAIT_OBJECT_0+1)
                {
                    MSG Msg;

                    while(PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
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
            hThread = nullptr;

            InvalidateRect(hWnd, nullptr, FALSE);
            KillTimer(hWnd, IDT_RENDERPROGRESS);
            SetProgress(GetDlgItem(hWnd, IDC_PROGRESSBAR), 100);
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
        if(m_hAccl!=nullptr && TranslateAccelerator(hWnd, m_hAccl, lpMsg))
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
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        case IDC_SET_PLANE_XY:
            m_nSteeringPlane = PLANE_XY;
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        case IDC_SET_AXES_AB:
            m_nSteeringPlane = AXES_AB;
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        case IDC_SET_VIEW_BZ:
            m_nSteeringPlane = VIEW_BZ;
            InvalidateRect(hWnd, nullptr, FALSE);
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
        case IDC_STEER_POWER_UP:
            m_nSteeringPower++;
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        case IDC_STEER_POWER_DOWN:
            m_nSteeringPower--;  // NOTE: Not limiting. Negative power gives you a reverse-gear.
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        case IDC_TOGGLE_HELP:
            m_bShowHelp = !m_bShowHelp;
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        case IDC_PIXELRATIO_UP:
            m_nPixelRatio++;
            m_bCameraDirty = true;
            break;
        case IDC_PIXELRATIO_DOWN:
            if(m_nPixelRatio>1)
            {
                m_nPixelRatio--;
                m_bCameraDirty = true;
            }
            break;
        case IDC_ANTIALIASING_UP:
            m_uAntiAliasing++;
            SetAntiAliasing(m_uAntiAliasing);
            m_bCameraDirty = true;
            break;
        case IDC_ANTIALIASING_DOWN:
            if(m_uAntiAliasing>0)
            {
                m_uAntiAliasing--;
                SetAntiAliasing(m_uAntiAliasing);
                m_bCameraDirty = true;
            }
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
            if(CreateWindowEx(0, PROGRESS_CLASS, "", WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDC_PROGRESSBAR, nullptr, nullptr))
            {
                SetWindowText(hWnd, "TestCase (press F1 for help, ESC to exit)");

                return TRUE;
            }
        }

        return FALSE;
    }

    virtual VOID WndProcOnPaint(HWND hWnd)
    {
        if(GetUpdateRect(hWnd, nullptr, FALSE))
        {
            PAINTSTRUCT Ps = { 0 };

            if(BeginPaint(hWnd, &Ps))
            {
                RECT rcWnd;

                GetClientRect(hWnd, &rcWnd);

                if(m_hbmOutput==nullptr)
                {// when there is nothing to draw, make something up
                    DrawCake(Ps.hdc, rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top, 2, RGB(255, 255, 255), RGB(0, 128, 128));
                }

                Enter();
                {// do not mess with the bitmap if currently drawing
                    if(m_hbmOutput!=nullptr)
                    {
                        BITMAP bmInfo;

                        GetObject(m_hbmOutput, sizeof(bmInfo), &bmInfo);

                        HDC hDC = CreateCompatibleDC(nullptr);

                        if(hDC!=nullptr)
                        {
                            SaveDC(hDC);
                            {
                                SelectObject(hDC, m_hbmOutput);
                                StretchBlt(Ps.hdc, 0, 0, rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top, hDC, 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, SRCCOPY);
                            }
                            RestoreDC(hDC, -1);

                            DeleteDC(hDC);
                        }
                    }
                }
                Leave();

                if(m_bShowHelp)
                {
                    SaveDC(Ps.hdc);
                    {
                        char szHelpText[1024];

                        _snprintf(szHelpText, _countof(szHelpText),
                            "X: %+2.3f Y: %+2.3f Z: %+2.3f A: %+2.3f B: %+2.3f\r\n"
                            "Steering mode (%d): 1=XZ 2=XY 3=AB 4=BZ\r\n"
                            "Steering power (%+.1f): PAGE UP+ PAGE DOWN-\r\n"
                            "Steering: UP RIGHT DOWN LEFT or WASD\r\n"
                            "Pixel ratio (%d): M+ N-\r\n"
                            "Anti-aliasing (%u): B+ V-\r\n"
                            "Last render took: %ums+%ums\r\n",
                            m_nSteeringX,
                            m_nSteeringY,
                            m_nSteeringZ,
                            m_nSteeringA*180.0f/PI,
                            m_nSteeringB*180.0f/PI,
                            m_nSteeringPlane+1,
                            m_nSteeringPower,
                            m_nPixelRatio,
                            m_uAntiAliasing,
                            m_dwRenderTime,
                            m_dwDrawingTime
                        );

                        SetTextColor(Ps.hdc, RGB(255, 255, 0));
                        SetBkColor(Ps.hdc, RGB(0, 0, 255));
                        SetBkMode(Ps.hdc, TRANSPARENT);
                        SelectObject(Ps.hdc, GetStockObject(SYSTEM_FIXED_FONT));
                        DrawTextEx(Ps.hdc, szHelpText, -1, &rcWnd, DT_WORDBREAK|DT_TOP|DT_LEFT, nullptr);
                    }
                    RestoreDC(Ps.hdc, -1);
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
        case IDT_RENDER:
            KillTimer(hWnd, IDT_RENDER);

            if(!m_bRunOnce)
            {
                m_bRunOnce = true;
            }

            UpdateRender(hWnd);
            return;
        case IDT_RENDERPROGRESS:
            SetProgress(GetDlgItem(hWnd, IDC_PROGRESSBAR), m_uRenderProgress);
            return;
        }

        CSuper::WndProcOnTimer(hWnd, uId);
    }

    virtual VOID WndProcOnWindowPosChanged(HWND hWnd, CONST WINDOWPOS* lpWP)
    {
        RECT rcWnd;

        GetClientRect(hWnd, &rcWnd);

        if(!m_bRunOnce || (!IsIconic(hWnd) && (rcWnd.right-rcWnd.left!=m_LastWndSize.cx || rcWnd.bottom-rcWnd.top!=m_LastWndSize.cy)))
        {
            m_LastWndSize.cx = rcWnd.right-rcWnd.left;
            m_LastWndSize.cy = rcWnd.bottom-rcWnd.top;

            if(m_hbmOutput!=nullptr)
            {
                SetOutputBitmap(nullptr);
            }

            InvalidateRect(hWnd, nullptr, FALSE);
            SetTimer(hWnd, IDT_RENDER, m_bRunOnce ? 500 : 0, nullptr);
        }

        // update controls
        MoveWindow(GetDlgItem(hWnd, IDC_PROGRESSBAR), rcWnd.left, rcWnd.bottom-15, rcWnd.right-rcWnd.left, 15, FALSE);

        CSuper::WndProcOnWindowPosChanged(hWnd, lpWP);
    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, INT nShowCmd)
{
    InitCommonControls();

    return CTestWindow(hInstance);
}
