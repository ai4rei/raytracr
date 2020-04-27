#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include "simplewnd.tpp"

#include "raytracr.hpp"

#include "testcase3.h"

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
        ZOOM,
    };

protected:
    HBITMAP m_hbmOutput;
    HACCEL m_hAccl;

    STEERINGPLANE m_nSteeringPlane;
    float m_nSteeringPower;
    CVector3d m_ovecEye;
    CVector3d m_ovecTarget;
    CVector3d m_uvecUp;
    CMatrix3d m_mtxScene;
    bool m_bShowHelp;

public:
    CTestWindow(const HINSTANCE hInstance)
        : CSimpleWindow(hInstance, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), WS_VISIBLE|WS_POPUPWINDOW|WS_SYSMENU|WS_CAPTION|WS_BORDER|WS_MINIMIZEBOX)
        , m_hbmOutput(NULL)
        , m_hAccl(LoadAccelerators(hInstance, MAKEINTRESOURCE(IDA_MAINACCL)))
        , m_nSteeringPlane(PLANE_XZ)
        , m_nSteeringPower(1.0f)
        , m_ovecEye(+0.0f, +0.0f, +1.0f)
        , m_ovecTarget(+0.0f, +0.0f, +0.0f)
        , m_uvecUp(+0.0f, +1.0f, +0.0f)
        , m_bShowHelp(false)
    {
        // Objects
        AddObject(CreatePlane(CreateVector3d(+0.0f, +0.0f, +0.0f), CreateVector3d(+0.0f, +1.0f, +0.0f), CreateColor(1.0f, 1.0f, 1.0f)));

        const float nSphereRange = 16;
        const float nSphereIncrement = 4;

        for(float nY = 0; nY<nSphereRange; nY+= nSphereIncrement)
        {
        for(float nX = 0; nX<nSphereRange; nX+= nSphereIncrement)
        {
            const float nRadius = 0.5f+(rand()%3)/2.0f;

            AddObject(CreateSphere(CreateVector3d(nX-nSphereRange/2.0f, nRadius, nY-nSphereRange/2.0f), nRadius, CreateColor((rand()%100)/100.0f, (rand()%100)/100.0f, (rand()%100)/100.0f)));
        }
        }

        // Lights
        AddLight(CreateLight(CreateVector3d(+9.0f, +9.0f, +0.0f), CreateColor(1.0f, 1.0f, 1.0f), 50.0f));

        SetScene(GetSystemMetrics(SM_CXSCREEN)/1, GetSystemMetrics(SM_CYSCREEN)/1);

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

    void UpdateCamera()
    {
        SetCamera(m_ovecEye.MatrixProduct(m_mtxScene), m_ovecTarget.MatrixProduct(m_mtxScene), m_uvecUp.MatrixProduct(m_mtxScene));
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
        m_mtxScene.AddXTranslation(nDirection*m_nSteeringPower);
        UpdateCamera();
    }

    void SteerY(const float nDirection)
    {
        m_mtxScene.AddYTranslation(nDirection*m_nSteeringPower);
        UpdateCamera();
    }

    void SteerZ(const float nDirection)
    {
        m_mtxScene.AddZTranslation(nDirection*m_nSteeringPower);
        UpdateCamera();
    }

    void SteerA(const float nDirection)
     //     const float PI = acos(0.0f)*2.0f;
    {// Raytracer::CreateMatrix3d().SetIdentity().AddYRotation(PI/180.0f*45.0f).AddXRotation(PI/180.0f*-45.0f)
        m_mtxScene.AddXRotation(nDirection*m_nSteeringPower);
        UpdateCamera();
    }

    void SteerB(const float nDirection)
    {
        m_mtxScene.AddYRotation(nDirection*m_nSteeringPower);
        UpdateCamera();
    }

    void SteerZoom(const float nDirection)
    {
        CVector3d rvecLookAt = m_ovecTarget-m_ovecEye;
        float nZoom = rvecLookAt.Magnitude();
        float nNewZoom = max(0.1f, nZoom+nDirection*m_nSteeringPower);

        if(nZoom!=nNewZoom)
        {
            m_ovecEye = m_ovecTarget-rvecLookAt.UnitVector()*nNewZoom;
            UpdateCamera();
        }
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
        case ZOOM:      // zoom in
            SteerZoom(-1.0f);
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
        case ZOOM:      // zoom out
            SteerZoom(+1.0f);
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
            break;
        case IDC_SET_PLANE_XY:
            m_nSteeringPlane = PLANE_XY;
            break;
        case IDC_SET_AXES_AB:
            m_nSteeringPlane = AXES_AB;
            break;
        case IDC_SET_ZOOM:
            m_nSteeringPlane = ZOOM;
            break;
        case IDC_STEER_UP:
            SteerUp();
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        case IDC_STEER_RIGHT:
            SteerRight();
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        case IDC_STEER_DOWN:
            SteerDown();
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        case IDC_STEER_LEFT:
            SteerLeft();
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        case IDC_TOGGLE_HELP:
            m_bShowHelp = !m_bShowHelp;
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }

        CSuper::WndProcOnCommand(hWnd, nId, hWndCtl, uCodeNotify);
    }

    virtual BOOL WndProcOnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
    {
        if(CSuper::WndProcOnCreate(hWnd, lpCreateStruct))
        {
            SetWindowText(hWnd, "TestCase (press ESC to exit)");

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

                if(m_hbmOutput!=NULL)
                {
                    BITMAP bmInfo;
                    RECT rcWnd;

                    GetClientRect(hWnd, &rcWnd);
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
};

bool TrackProgress(unsigned int uDone, unsigned int uTotal, void* lpContext)
{
    printf("%3.1f%%\r", uDone*100.0f/uTotal);

    return true;

    UNREFERENCED_PARAMETER(lpContext);
}

HBITMAP BitmapFromPixels(const std::vector< Raytracer::CColor >& aclrPixels, const int nWidth, const int nHeight)
{
    HBITMAP hbmOutput = NULL;
    HDC hdcOutput = NULL;
    HGDIOBJ hPrevObj = NULL;

    hbmOutput = CreateCompatibleBitmap(CGetDC(NULL), nWidth, nHeight);
    hdcOutput = CreateCompatibleDC(NULL);

    hPrevObj = SelectObject(hdcOutput, hbmOutput);

    for(int nY = 0, nPY = nHeight-1; nY<nHeight; nY++, nPY--)
    {
        for(int nX = 0; nX<nWidth; nX++)
        {
            const auto& Px = aclrPixels[nX+nPY*nWidth];  // the buffer is up-side down

            SetPixelV(hdcOutput, nX, nY, RGB(Px.R()*255,Px.G()*255,Px.B()*255));
        }
    }

    SelectObject(hdcOutput, hPrevObj);
    DeleteDC(hdcOutput);

    return hbmOutput;
}

HBITMAP BitmapFromPixels2(const std::vector< Raytracer::CColor >& aclrPixels, const int nWidth, const int nHeight)
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

void AddTriangleStrip(Raytracer& R, const float anVertices[], const size_t uCount, const Raytracer::CColor& clrColor)
{
    size_t uIdx = 0;

    while(uCount-uIdx>=9)
    {
        R.AddObject(Raytracer::CreateTriangle(Raytracer::CreateVector3d(anVertices[uIdx+0], anVertices[uIdx+1], anVertices[uIdx+2]), Raytracer::CreateVector3d(anVertices[uIdx+3], anVertices[uIdx+4], anVertices[uIdx+5]), Raytracer::CreateVector3d(anVertices[uIdx+6], anVertices[uIdx+7], anVertices[uIdx+8]), clrColor));

        uIdx+= 3;
    }
}

HBITMAP RenderJob(Raytracer& R)
{
    DWORD dwStart;

    dwStart = GetTickCount();
    //R.Render(&TrackProgress, NULL);
    R.Render(NULL);
    printf("%ums", GetTickCount()-dwStart);

    dwStart = GetTickCount();
    HBITMAP hbmOutput = BitmapFromPixels2(R.GetResult(), R.GetSceneWidth(), R.GetSceneHeight());
    printf("+%ums\n", GetTickCount()-dwStart);

    return hbmOutput;
}

int __cdecl main(int nArgc, char** lppszArgv)
{
    return CTestWindow(GetModuleHandle(NULL));
}
