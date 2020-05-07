#include <windows.h>
#include <windowsx.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "simplewnd.tpp"
#include "utility.hpp"

class CTestWindow
    : public CSimpleWindow
{
private:
    typedef CTestWindow CSelf;
    typedef CSimpleWindow CSuper;

protected:

public:
    virtual ~CTestWindow()
    {
    }

    CTestWindow(HINSTANCE hInstance)
        : CSimpleWindow(hInstance, 640, 480, WS_VISIBLE|WS_POPUPWINDOW|WS_CAPTION|WS_MINIMIZEBOX|WS_CLIPCHILDREN|WS_CLIPSIBLINGS)
    {
    }

    void Render()
    {
        glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glFlush();
    }

    virtual BOOL WndProcOnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
    {
        if(CSuper::WndProcOnCreate(hWnd, lpCreateStruct))
        {
            PIXELFORMATDESCRIPTOR Pfd = { sizeof(Pfd) };

            Pfd.nVersion = 1;
            Pfd.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL;
            Pfd.iPixelType = PFD_TYPE_RGBA;
            Pfd.cColorBits = 32;

            HDC hDC = GetDC(hWnd);
            int nPixelFormat = ChoosePixelFormat(hDC, &Pfd);

            if(nPixelFormat!=0 && SetPixelFormat(hDC, nPixelFormat, &Pfd))
            {
                HGLRC hGLRC = wglCreateContext(hDC);

                if(hGLRC!=NULL)
                {
                    if(wglMakeCurrent(hDC, hGLRC))
                    {
                        SetWindowText(hWnd, "OpenGL TestCase (press ESC to exit)");
                        return TRUE;
                    }

                    wglDeleteContext(hGLRC);
                }
            }

            ReleaseDC(hWnd, hDC);
            hDC = NULL;
        }

        return FALSE;
    }

    virtual VOID WndProcOnDestroy(HWND hWnd)
    {
        HGLRC hGLRC = wglGetCurrentContext();

        if(hGLRC!=NULL)
        {
            HDC hDC = wglGetCurrentDC();

            wglMakeCurrent(NULL, NULL);
            ReleaseDC(hWnd, hDC);
            wglDeleteContext(hGLRC);
        }

        CSuper::WndProcOnDestroy(hWnd);
    }

    virtual VOID WndProcOnPaint(HWND hWnd)
    {
        Render();
        ValidateRect(hWnd, NULL);
    }

    virtual VOID WndProcOnWindowPosChanged(HWND hWnd, const WINDOWPOS* lpWP)
    {
        RECT rcWnd;

        GetClientRect(hWnd, &rcWnd);
        glViewport(0, 0, rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top);
        InvalidateRect(hWnd, NULL, FALSE);

        CSuper::WndProcOnWindowPosChanged(hWnd, lpWP);
    }
};

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, INT nShowCmd)
{
    return CTestWindow(hInstance);
}
