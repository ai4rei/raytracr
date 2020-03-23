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
        if(m_hbmOutput1!=NULL)
        {
            DeleteBitmap(m_hbmOutput1);
            m_hbmOutput1 = NULL;
        }
        if(m_hbmOutput2!=NULL)
        {
            DeleteBitmap(m_hbmOutput2);
            m_hbmOutput2 = NULL;
        }
        if(m_hbmOutput3!=NULL)
        {
            DeleteBitmap(m_hbmOutput3);
            m_hbmOutput3 = NULL;
        }
        if(m_hbmOutput4!=NULL)
        {
            DeleteBitmap(m_hbmOutput4);
            m_hbmOutput4 = NULL;
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

                if(m_hbmOutput1!=NULL)
                {
                    hPrevObj = SelectObject(hDC, m_hbmOutput1);
                    BitBlt(Ps.hdc, 0, 0, m_nImageW, m_nImageH, hDC, 0, 0, SRCCOPY);
                    SelectObject(hDC, hPrevObj);
                }
                if(m_hbmOutput2!=NULL)
                {
                    hPrevObj = SelectObject(hDC, m_hbmOutput2);
                    BitBlt(Ps.hdc, m_nImageW, 0, m_nImageW, m_nImageH, hDC, 0, 0, SRCCOPY);
                    SelectObject(hDC, hPrevObj);
                }
                if(m_hbmOutput3!=NULL)
                {
                    hPrevObj = SelectObject(hDC, m_hbmOutput3);
                    BitBlt(Ps.hdc, 0, m_nImageH, m_nImageW, m_nImageH, hDC, 0, 0, SRCCOPY);
                    SelectObject(hDC, hPrevObj);
                }
                if(m_hbmOutput4!=NULL)
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

    virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch(uMsg)
        {
        HANDLE_MSG(hWnd, WM_CREATE, WndProcOnCreate);
        HANDLE_MSG(hWnd, WM_PAINT, WndProcOnPaint);
        }

        return CSuper::WndProc(hWnd, uMsg, wParam, lParam);
    }
};

bool TrackProgress(unsigned int uDone, unsigned int uTotal, void* lpContext)
{
    printf(".");

    return true;
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

void AddTriangleStrip(Raytracer& R, const float anVertices[], const size_t uCount, const Raytracer::CColor& clrColor)
{
    size_t uIdx = 0;

    while(uCount-uIdx>=9)
    {
        R.AddObject(Raytracer::CreateTriangle(Raytracer::CreateVector3d(anVertices[uIdx+0], anVertices[uIdx+1], anVertices[uIdx+2]), Raytracer::CreateVector3d(anVertices[uIdx+3], anVertices[uIdx+4], anVertices[uIdx+5]), Raytracer::CreateVector3d(anVertices[uIdx+6], anVertices[uIdx+7], anVertices[uIdx+8]), clrColor));

        uIdx+= 3;
    }
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

    R.AddObject(Raytracer::CreatePlane(Raytracer::CreateVector3d(+0.0f, +0.0f, +0.0f), Raytracer::CreateVector3d(+0.0f, +1.0f, +0.0f), Raytracer::CreateColor(1.0f, 1.0f, 1.0f)));
    R.AddObject(Raytracer::CreatePlane(Raytracer::CreateVector3d(-6.0f, +0.0f, +0.0f), Raytracer::CreateVector3d(+1.0f, +0.0f, +0.0f), Raytracer::CreateColor(0.0f, 0.5f, 0.5f)));
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+2.0f, +1.0f, -3.0f), 1.0f, Raytracer::CreateColor(0.25f, 0.25f, 0.75f)));
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(-2.0f, +1.0f, -3.0f), 1.0f, Raytracer::CreateColor(0.75f, 0.25f, 0.25f)));
    R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+0.0f, +1.0f, -2.0f), 1.0f, Raytracer::CreateColor(0.25f, 0.75f, 0.25f)));

    R.AddObject(Raytracer::CreateLight(Raytracer::CreateVector3d(+9.0f, +9.0f, +9.0f), Raytracer::CreateColor(1.0f, 1.0f, 1.0f), 60.0f));
    //R.AddObject(Raytracer::CreateLight(Raytracer::CreateVector3d(+9.0f, +9.0f, -9.0f), Raytracer::CreateColor(1.0f, 0.0f, 0.0f), 50.0f));
    //R.AddObject(Raytracer::CreateLight(Raytracer::CreateVector3d(+9.0f, +9.0f, +0.0f), Raytracer::CreateColor(0.0f, 1.0f, 0.0f), 50.0f));
    //R.AddObject(Raytracer::CreateLight(Raytracer::CreateVector3d(+9.0f, +9.0f, +9.0f), Raytracer::CreateColor(0.0f, 0.0f, 1.0f), 50.0f));

    R.SetScene(250, 250);

    puts("Rendering...");

    const auto ovecEye = Raytracer::CreateVector3d(+0.0f, +1.0f, +10.0f);
    const auto ovecTarget = Raytracer::CreateVector3d(+0.0f, +1.0f, +9.0f);
    const auto uvecUp = Raytracer::CreateVector3d(+0.0f, +1.0f, +0.0f);
    const auto mtxFront = Raytracer::CreateMatrix3d().SetIdentity().AddYRotation(PI/180.0f*0.0f);
    const auto mtxSide = Raytracer::CreateMatrix3d().SetIdentity().AddYRotation(PI/180.0f*90.0f);
    const auto mtxTop = Raytracer::CreateMatrix3d().SetIdentity().AddXRotation(PI/180.0f*-90.0f);
    const auto mtxIso = Raytracer::CreateMatrix3d().SetIdentity().AddYRotation(PI/180.0f*45.0f).AddXRotation(PI/180.0f*-45.0f);

    // Front
    CONST DWORD dwStart1 = GetTickCount();
    R.SetCamera(ovecEye.MatrixProduct(mtxFront), ovecTarget.MatrixProduct(mtxFront), uvecUp.MatrixProduct(mtxFront));
    R.Render(&TrackProgress, NULL);
    printf("%ums\n", GetTickCount()-dwStart1);
    HBITMAP hbmOutput1 = BitmapFromPixels(R.GetResult(), R.GetSceneWidth(), R.GetSceneHeight());

    // Side
    CONST DWORD dwStart2 = GetTickCount();
    R.SetCamera(ovecEye.MatrixProduct(mtxSide), ovecTarget.MatrixProduct(mtxSide), uvecUp.MatrixProduct(mtxSide));
    R.Render(&TrackProgress, NULL);
    printf("%ums\n", GetTickCount()-dwStart2);
    HBITMAP hbmOutput2 = BitmapFromPixels(R.GetResult(), R.GetSceneWidth(), R.GetSceneHeight());

    // Top
    CONST DWORD dwStart3 = GetTickCount();
    R.SetCamera(ovecEye.MatrixProduct(mtxTop), ovecTarget.MatrixProduct(mtxTop), uvecUp.MatrixProduct(mtxTop));
    R.Render(&TrackProgress, NULL);
    printf("%ums\n", GetTickCount()-dwStart3);
    HBITMAP hbmOutput3 = BitmapFromPixels(R.GetResult(), R.GetSceneWidth(), R.GetSceneHeight());

    // Iso
    CONST DWORD dwStart4 = GetTickCount();
    R.SetCamera(ovecEye.MatrixProduct(mtxIso), ovecTarget.MatrixProduct(mtxIso), uvecUp.MatrixProduct(mtxIso));
    R.Render(&TrackProgress, NULL);
    printf("%ums\n", GetTickCount()-dwStart4);
    HBITMAP hbmOutput4 = BitmapFromPixels(R.GetResult(), R.GetSceneWidth(), R.GetSceneHeight());

    puts("Present.");

    return CTestWindow(GetModuleHandle(NULL), R.GetSceneWidth(), R.GetSceneHeight(), WS_VISIBLE|WS_POPUPWINDOW|WS_SYSMENU|WS_CAPTION|WS_BORDER|WS_MINIMIZEBOX, hbmOutput1, hbmOutput2, hbmOutput3, hbmOutput4);
}
