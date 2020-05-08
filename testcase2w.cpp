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

public:
    CTestWindow(const HINSTANCE hInstance, const INT nClientW, const INT nClientH, const DWORD dwStyle, const HBITMAP hbmOutput1)
        : CSimpleWindow(hInstance, nClientW, nClientH, dwStyle)
        , m_nImageW(nClientW)
        , m_nImageH(nClientH)
        , m_hbmOutput1(hbmOutput1)
    {
    }

    virtual ~CTestWindow()
    {
        if(m_hbmOutput1!=nullptr)
        {
            DeleteBitmap(m_hbmOutput1);
            m_hbmOutput1 = nullptr;
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

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, INT nShowCmd)
{
    const float PI = acos(0.0f)*2.0f;

    Raytracer R;

    // origin
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+0.0f, +0.0f, +0.0f), 0.1f, Raytracer::CreateColor(1.0f, 0.0f, 0.0f)));

    // Simple objects
    R.AddObject(Raytracer::CreatePlane(Raytracer::CreateVector3d(+0.0f, +0.0f, +0.0f), Raytracer::CreateVector3d(+0.0f, +1.0f, +0.0f), Raytracer::CreateColor(1.0f, 1.0f, 1.0f)));
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+2.0f, +1.0f, -3.0f), 1.0f, Raytracer::CreateColor(0.25f, 0.25f, 0.75f)));
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(-2.0f, +1.0f, -3.0f), 1.0f, Raytracer::CreateColor(0.75f, 0.25f, 0.25f)));
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+0.0f, +1.0f, -2.0f), 1.0f, Raytracer::CreateColor(0.25f, 0.75f, 0.25f)));

    // Lights
    R.AddLight(Raytracer::CreateLight(Raytracer::CreateVector3d(+9.0f, +9.0f, +0.0f), Raytracer::CreateColor(1.0f, 1.0f, 1.0f), 50.0f));

    R.SetImageSize(640, 480);

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

    puts("Present.");

    return CTestWindow(hInstance, R.GetImageWidth(), R.GetImageHeight(), WS_VISIBLE|WS_POPUPWINDOW|WS_SYSMENU|WS_CAPTION|WS_BORDER|WS_MINIMIZEBOX, hbmOutput1);
}
