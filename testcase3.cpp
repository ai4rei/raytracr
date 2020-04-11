#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include "simplewnd.tpp"

#include "raytracr.hpp"

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
{
private:
    typedef CTestWindow CSelf;
    typedef CSimpleWindow CSuper;

protected:
    int m_nImageW;
    int m_nImageH;
    HBITMAP m_hbmOutput;

public:
    CTestWindow(const HINSTANCE hInstance, const INT nClientW, const INT nClientH, const DWORD dwStyle, const HBITMAP hbmOutput)
        : CSimpleWindow(hInstance, nClientW, nClientH, dwStyle)
        , m_nImageW(nClientW)
        , m_nImageH(nClientH)
        , m_hbmOutput(hbmOutput)
    {
    }

    virtual ~CTestWindow()
    {
        if(m_hbmOutput!=NULL)
        {
            DeleteBitmap(m_hbmOutput);
            m_hbmOutput = NULL;
        }
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
                    hPrevObj = SelectObject(hDC, m_hbmOutput);
                    BitBlt(Ps.hdc, 0, 0, m_nImageW, m_nImageH, hDC, 0, 0, SRCCOPY);
                    SelectObject(hDC, hPrevObj);
                }

                DeleteDC(hDC);

                EndPaint(hWnd, &Ps);
            }
        }
    }

    virtual UINT WndProcOnNCHitTest(HWND hWnd, int nX, int nY)
    {
        UINT uHitTest = FORWARD_WM_NCHITTEST(hWnd, nX, nY, DefWindowProc);

        if(uHitTest==HTCLIENT)
        {
            uHitTest = HTCAPTION;
        }

        return uHitTest;
    }

    virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch(uMsg)
        {
        HANDLE_MSG(hWnd, WM_PAINT, WndProcOnPaint);
        HANDLE_MSG(hWnd, WM_NCHITTEST, WndProcOnNCHitTest);
        }

        return CSuper::WndProc(hWnd, uMsg, wParam, lParam);
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

                ucBits[0] = static_cast< unsigned char >(Px.B()*255U);
                ucBits[1] = static_cast< unsigned char >(Px.G()*255U);
                ucBits[2] = static_cast< unsigned char >(Px.R()*255U);
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
    const float PI = acos(0.0f)*2.0f;

    Raytracer R;

    // origin
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+0.0f, +0.0f, +0.0f), 0.1f, Raytracer::CreateColor(1.0f, 0.0f, 0.0f)));

/**/
    // Up-side down pyramid
    static const float z_anPyVertices[] =
    {
        -2.5f, +4.0f, -0.5f,
        -2.0f, +4.0f, +0.5f,
        -2.0f, +3.0f, +0.0f,
        -1.5f, +4.0f, -0.5f,
        -2.5f, +4.0f, -0.5f,
        //-2.0f, +4.0f, +0.5f,  // lid
    };
    AddTriangleStrip(R, z_anPyVertices, _countof(z_anPyVertices), Raytracer::CreateColor(0.75f, 0.75f, 0.25f));
/**/
    // Dice
    // Cube: https://stackoverflow.com/questions/28375338/cube-using-single-gl-triangle-strip
    static const float z_anDcVertices[] =
    {
        -1.0f, +2.0f, -5.0f,  // Front-top-left
        +1.0f, +2.0f, -5.0f,  // Front-top-right
        -1.0f, +0.0f, -5.0f,  // Front-bottom-left
        +1.0f, +0.0f, -5.0f,  // Front-bottom-right
        +1.0f, +0.0f, -7.0f,  // Back-bottom-right
        +1.0f, +2.0f, -5.0f,  // Front-top-right
        +1.0f, +2.0f, -7.0f,  // Back-top-right
        -1.0f, +2.0f, -5.0f,  // Front-top-left
        -1.0f, +2.0f, -7.0f,  // Back-top-left
        -1.0f, +0.0f, -5.0f,  // Front-bottom-left
        -1.0f, +0.0f, -7.0f,  // Back-bottom-left
        +1.0f, +0.0f, -7.0f,  // Back-bottom-right
        -1.0f, +2.0f, -7.0f,  // Back-top-left
        +1.0f, +2.0f, -7.0f,  // Back-top-right
    };
    AddTriangleStrip(R, z_anDcVertices, _countof(z_anDcVertices), Raytracer::CreateColor(0.9f, 0.9f, 0.9f));
    // ooo
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(-0.5f, +0.5f, -5.0f), 0.1f, Raytracer::CreateColor(0.5f, 0.5f, 0.5f)));
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+0.0f, +1.0f, -5.0f), 0.1f, Raytracer::CreateColor(0.5f, 0.5f, 0.5f)));
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+0.5f, +1.5f, -5.0f), 0.1f, Raytracer::CreateColor(0.5f, 0.5f, 0.5f)));
    // oo
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+0.5f, +2.0f, -6.5f), 0.1f, Raytracer::CreateColor(0.5f, 0.5f, 0.5f)));
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(-0.5f, +2.0f, -5.5f), 0.1f, Raytracer::CreateColor(0.5f, 0.5f, 0.5f)));
    // o
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+1.0f, +1.0f, -6.0f), 0.1f, Raytracer::CreateColor(0.5f, 0.0f, 0.0f)));
/**/

    // Simple objects
    R.AddObject(Raytracer::CreatePlane(Raytracer::CreateVector3d(+0.0f, +0.0f, +0.0f), Raytracer::CreateVector3d(+0.0f, +1.0f, +0.0f), Raytracer::CreateColor(1.0f, 1.0f, 1.0f)));
    R.AddObject(Raytracer::CreatePlane(Raytracer::CreateVector3d(-6.0f, +0.0f, +0.0f), Raytracer::CreateVector3d(+1.0f, +0.0f, +0.0f), Raytracer::CreateColor(0.0f, 0.5f, 0.5f)));
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+2.0f, +1.0f, -3.0f), 1.0f, Raytracer::CreateColor(0.25f, 0.25f, 0.75f)));
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(-2.0f, +1.0f, -3.0f), 1.0f, Raytracer::CreateColor(0.75f, 0.25f, 0.25f)));
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+0.0f, +1.0f, -2.0f), 1.0f, Raytracer::CreateColor(0.25f, 0.75f, 0.25f)));

    // Lights
    R.AddLight(Raytracer::CreateLight(Raytracer::CreateVector3d(+9.0f, +9.0f, +0.0f), Raytracer::CreateColor(1.0f, 1.0f, 1.0f), 50.0f));
    //R.AddLight(Raytracer::CreateLight(Raytracer::CreateVector3d(+9.0f, +9.0f, -9.0f), Raytracer::CreateColor(1.0f, 0.0f, 0.0f), 50.0f));
    //R.AddLight(Raytracer::CreateLight(Raytracer::CreateVector3d(+9.0f, +9.0f, +0.0f), Raytracer::CreateColor(0.0f, 1.0f, 0.0f), 50.0f));
    //R.AddLight(Raytracer::CreateLight(Raytracer::CreateVector3d(+9.0f, +9.0f, +9.0f), Raytracer::CreateColor(0.0f, 0.0f, 1.0f), 50.0f));

    if(nArgc>2 && atoi(lppszArgv[1])>0 && atoi(lppszArgv[2])>0)
    {
        R.SetScene(atoi(lppszArgv[1])/2, atoi(lppszArgv[2])/2);
    }
    else
    {
        R.SetScene(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    }

    puts("Rendering...");

    const auto ovecEye = Raytracer::CreateVector3d(+0.0f, +1.0f, +10.0f);
    const auto ovecTarget = Raytracer::CreateVector3d(+0.0f, +1.0f, +9.0f);
    const auto uvecUp = Raytracer::CreateVector3d(+0.0f, +1.0f, +0.0f);
    const auto mtxFront = Raytracer::CreateMatrix3d().SetIdentity().AddYRotation(PI/180.0f*0.0f);
    const auto mtxSide = Raytracer::CreateMatrix3d().SetIdentity().AddYRotation(PI/180.0f*90.0f);
    const auto mtxTop = Raytracer::CreateMatrix3d().SetIdentity().AddXRotation(PI/180.0f*-90.0f);
    const auto mtxIso = Raytracer::CreateMatrix3d().SetIdentity().AddYRotation(PI/180.0f*45.0f).AddXRotation(PI/180.0f*-45.0f);

    // Iso
    R.SetCamera(ovecEye.MatrixProduct(mtxIso), ovecTarget.MatrixProduct(mtxIso), uvecUp.MatrixProduct(mtxIso));
    HBITMAP hbmOutput = RenderJob(R);

    puts("Present.");

    return CTestWindow(GetModuleHandle(NULL), R.GetSceneWidth(), R.GetSceneHeight(), WS_VISIBLE|WS_POPUPWINDOW|WS_SYSMENU|WS_CAPTION|WS_BORDER|WS_MINIMIZEBOX, hbmOutput);
}
