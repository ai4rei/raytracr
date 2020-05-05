#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include "simplewnd.tpp"
#include "utility.hpp"

#include "raytracr.hpp"

class CTestWindow
    : public CSimpleWindow
{
private:
    typedef CTestWindow CSelf;
    typedef CSimpleWindow CSuper;

protected:
    int m_nImageW;
    int m_nImageH;
    HBITMAP m_hbmOutput1;
    HBITMAP m_hbmOutput2;
    HBITMAP m_hbmOutput3;
    HBITMAP m_hbmOutput4;

public:
    CTestWindow(const HINSTANCE hInstance, const INT nClientW, const INT nClientH, const DWORD dwStyle, const HBITMAP hbmOutput1, const HBITMAP hbmOutput2, const HBITMAP hbmOutput3, const HBITMAP hbmOutput4)
        : CSimpleWindow(hInstance, nClientW*2, nClientH*2, dwStyle)
        , m_nImageW(nClientW)
        , m_nImageH(nClientH)
        , m_hbmOutput1(hbmOutput1)
        , m_hbmOutput2(hbmOutput2)
        , m_hbmOutput3(hbmOutput3)
        , m_hbmOutput4(hbmOutput4)
    {
    }

    virtual ~CTestWindow()
    {
        if(m_hbmOutput1!=nullptr)
        {
            DeleteBitmap(m_hbmOutput1);
            m_hbmOutput1 = nullptr;
        }
        if(m_hbmOutput2!=nullptr)
        {
            DeleteBitmap(m_hbmOutput2);
            m_hbmOutput2 = nullptr;
        }
        if(m_hbmOutput3!=nullptr)
        {
            DeleteBitmap(m_hbmOutput3);
            m_hbmOutput3 = nullptr;
        }
        if(m_hbmOutput4!=nullptr)
        {
            DeleteBitmap(m_hbmOutput4);
            m_hbmOutput4 = nullptr;
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
        if(GetUpdateRect(hWnd, nullptr, FALSE))
        {
            PAINTSTRUCT Ps = { 0 };

            if(BeginPaint(hWnd, &Ps))
            {
                HDC hDC = CreateCompatibleDC(nullptr);
                HGDIOBJ hPrevObj;

                if(m_hbmOutput1!=nullptr)
                {
                    hPrevObj = SelectObject(hDC, m_hbmOutput1);
                    BitBlt(Ps.hdc, 0, 0, m_nImageW, m_nImageH, hDC, 0, 0, SRCCOPY);
                    SelectObject(hDC, hPrevObj);
                }
                if(m_hbmOutput2!=nullptr)
                {
                    hPrevObj = SelectObject(hDC, m_hbmOutput2);
                    BitBlt(Ps.hdc, m_nImageW, 0, m_nImageW, m_nImageH, hDC, 0, 0, SRCCOPY);
                    SelectObject(hDC, hPrevObj);
                }
                if(m_hbmOutput3!=nullptr)
                {
                    hPrevObj = SelectObject(hDC, m_hbmOutput3);
                    BitBlt(Ps.hdc, 0, m_nImageH, m_nImageW, m_nImageH, hDC, 0, 0, SRCCOPY);
                    SelectObject(hDC, hPrevObj);
                }
                if(m_hbmOutput4!=nullptr)
                {
                    hPrevObj = SelectObject(hDC, m_hbmOutput4);
                    BitBlt(Ps.hdc, m_nImageW, m_nImageH, m_nImageW, m_nImageH, hDC, 0, 0, SRCCOPY);
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

HBITMAP BitmapFromPixels2(const std::vector< Raytracer::CColor >& aclrPixels, const int nImageW, const int nImageH)
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
    //R.Render(&TrackProgress, nullptr);
    R.Render(nullptr);
    printf("%ums", GetTickCount()-dwStart);

    dwStart = GetTickCount();
    HBITMAP hbmOutput = BitmapFromPixels2(R.GetResult(), R.GetImageWidth(), R.GetImageHeight());
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
        R.SetImageSize(atoi(lppszArgv[1])/2, atoi(lppszArgv[2])/2);
    }
    else
    {
        R.SetImageSize(GetSystemMetrics(SM_CXSCREEN)/3, GetSystemMetrics(SM_CYSCREEN)/3);
    }

    puts("Rendering...");

    const auto ovecEye = Raytracer::CreateVector3d(+0.0f, +1.0f, +10.0f);
    const auto rvecLookAt = Raytracer::CreateVector3d(+0.0f, +0.0f, -1.0f);
    const auto uvecUp = Raytracer::CreateVector3d(+0.0f, +1.0f, +0.0f);
    const auto mtxFront = Raytracer::CreateMatrix3d().SetIdentity().AddYRotation(PI/180.0f*0.0f);
    const auto mtxSide = Raytracer::CreateMatrix3d().SetIdentity().AddYRotation(PI/180.0f*90.0f);
    const auto mtxTop = Raytracer::CreateMatrix3d().SetIdentity().AddXRotation(PI/180.0f*-90.0f);
    const auto mtxIso = Raytracer::CreateMatrix3d().SetIdentity().AddYRotation(PI/180.0f*45.0f).AddXRotation(PI/180.0f*-45.0f);

    // Front
    R.SetCamera(ovecEye.MatrixProduct(mtxFront), rvecLookAt.MatrixProduct(mtxFront), uvecUp.MatrixProduct(mtxFront));
    HBITMAP hbmOutput1 = RenderJob(R);

    // Side
    R.SetCamera(ovecEye.MatrixProduct(mtxSide), rvecLookAt.MatrixProduct(mtxSide), uvecUp.MatrixProduct(mtxSide));
    HBITMAP hbmOutput2 = RenderJob(R);

    // Top
    R.SetCamera(ovecEye.MatrixProduct(mtxTop), rvecLookAt.MatrixProduct(mtxTop), uvecUp.MatrixProduct(mtxTop));
    HBITMAP hbmOutput3 = RenderJob(R);

    // Iso
    R.SetCamera(ovecEye.MatrixProduct(mtxIso), rvecLookAt.MatrixProduct(mtxIso), uvecUp.MatrixProduct(mtxIso));
    HBITMAP hbmOutput4 = RenderJob(R);

    puts("Present.");

    return CTestWindow(GetModuleHandle(nullptr), R.GetImageWidth(), R.GetImageHeight(), WS_VISIBLE|WS_POPUPWINDOW|WS_SYSMENU|WS_CAPTION|WS_BORDER|WS_MINIMIZEBOX, hbmOutput1, hbmOutput2, hbmOutput3, hbmOutput4);
}
