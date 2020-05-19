#include <windows.h>
#include <windowsx.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "simplewnd.hpp"
#include "utility.hpp"

class CTestWindow
    : public CSimpleWindow
{
private:
    typedef CTestWindow CSelf;
    typedef CSimpleWindow CSuper;

protected:
    SIZE m_LastWndSize;

public:
    virtual ~CTestWindow()
    {
    }

    CTestWindow(HINSTANCE hInstance)
        : CSimpleWindow(hInstance, 640, 480, WS_VISIBLE|WS_POPUPWINDOW|WS_CAPTION|WS_MINIMIZEBOX|WS_CLIPCHILDREN|WS_CLIPSIBLINGS)
        , m_LastWndSize()
    {
    }

    void Render()
    {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        // R.AddObject(Raytracer::CreatePlane(Raytracer::CreateVector3d(+0.0f, +0.0f, +0.0f), Raytracer::CreateVector3d(+0.0f, +1.0f, +0.0f), Raytracer::CreateColor(1.0f, 1.0f, 1.0f)));
        glPopMatrix();
        glPushMatrix();
        glBegin(GL_TRIANGLE_FAN);
            glColor3f(1.0f, 1.0f, 1.0f);
            glNormal3f(+0.0f, +1.0f, +0.0f);
            glVertex3f(  +0.0f, +0.0f,   +0.0f);
            glVertex3f(-100.0f, +0.0f, -100.0f);
            glVertex3f(-100.0f, +0.0f, +100.0f);
            glVertex3f(+100.0f, +0.0f, +100.0f);
            glVertex3f(+100.0f, +0.0f, -100.0f);
            glVertex3f(-100.0f, +0.0f, -100.0f);
        glEnd();

        GLUquadricObj* lpQuadric = gluNewQuadric();

        if(lpQuadric!=NULL)
        {
            gluQuadricDrawStyle(lpQuadric, GLU_FILL);

            //R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+0.0f, +0.0f, +0.0f), 0.1f, Raytracer::CreateColor(1.0f, 0.0f, 0.0f)));
            glPopMatrix();
            glPushMatrix();
            glColor3f(1.0f, 0.0f, 0.0f);
            gluSphere(lpQuadric, 0.1f, 20, 20);

            // R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+2.0f, +1.0f, -3.0f), 1.0f, Raytracer::CreateColor(0.25f, 0.25f, 0.75f)));
            glPopMatrix();
            glPushMatrix();
            glTranslatef(+2.0f, +1.0f, -3.0f);
            glColor3f(0.25f, 0.25f, 0.75f);
            gluSphere(lpQuadric, 1.0f, 20, 20);

            // R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(-2.0f, +1.0f, -3.0f), 1.0f, Raytracer::CreateColor(0.75f, 0.25f, 0.25f)));
            glPopMatrix();
            glPushMatrix();
            glTranslatef(-2.0f, +1.0f, -3.0f);
            glColor3f(0.75f, 0.25f, 0.25f);
            gluSphere(lpQuadric, 1.0f, 20, 20);

            // R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+0.0f, +1.0f, -2.0f), 1.0f, Raytracer::CreateColor(0.25f, 0.75f, 0.25f)));
            glPopMatrix();
            glPushMatrix();
            glTranslatef(+0.0f, +1.0f, -2.0f);
            glColor3f(0.25f, 0.75f, 0.25f);
            gluSphere(lpQuadric, 1.0f, 20, 20);

            gluDeleteQuadric(lpQuadric);
        }

        glFlush();
    }

    virtual BOOL WndProcOnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
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
                    //R.AddLight(Raytracer::CreateLight(Raytracer::CreateVector3d(+9.0f, +9.0f, +0.0f), Raytracer::CreateColor(1.0f, 1.0f, 1.0f), 50.0f));
                    GLfloat aLightPos[] = { 9.0f, 9.0f, 0.0f, 1.0f };

                    glMatrixMode(GL_MODELVIEW);
                    glLoadIdentity();
                    gluLookAt
                    (
                        +0.0f, +1.0f, +10.0f,  // eye
                        +0.0f, +1.0f,  +9.0f,  // target
                        +0.0f, +1.0f,  +0.0f   // up
                    );
                    glPushMatrix();
                    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                    glClearDepth(1.0f);
                    glLightfv(GL_LIGHT0, GL_POSITION, aLightPos);
                    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f);
                    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1f);
                    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
                    glEnable(GL_LIGHTING);
                    glEnable(GL_LIGHT0);
                    glEnable(GL_COLOR_MATERIAL);
                    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LEQUAL);

                    if(CSuper::WndProcOnCreate(hWnd, lpCreateStruct))
                    {// this has to come last, because it triggers onWindowPosChanged
                        SetWindowText(hWnd, "OpenGL TestCase (press ESC to exit)");
                        return TRUE;
                    }

                    wglMakeCurrent(NULL, NULL);
                }

                wglDeleteContext(hGLRC);
            }
        }

        ReleaseDC(hWnd, hDC);
        hDC = NULL;

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

        if(m_LastWndSize.cx!=rcWnd.right-rcWnd.left || m_LastWndSize.cy!=rcWnd.bottom-rcWnd.top)
        {
            m_LastWndSize.cx = rcWnd.right-rcWnd.left;
            m_LastWndSize.cy = rcWnd.bottom-rcWnd.top;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(180.0f/4, float(m_LastWndSize.cx)/float(m_LastWndSize.cy), 1.0f, 100.0f);
            glViewport(0, 0, m_LastWndSize.cx, m_LastWndSize.cy);
            InvalidateRect(hWnd, NULL, FALSE);
        }

        CSuper::WndProcOnWindowPosChanged(hWnd, lpWP);
    }
};

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, INT nShowCmd)
{
    return CTestWindow(hInstance);
}
