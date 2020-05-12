#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <d3d.h>
#include <d3dx.h>

#include "simplewnd.tpp"

// DX9 -> DX7
typedef ID3DXSimpleShape ID3DXMesh;
#define D3DCOLOR_COLORVALUE D3DRGBA
#define D3DCOLOR_XRGB RGB_MAKE
#define D3DRS_AMBIENT D3DRENDERSTATE_AMBIENT
#define D3DRS_LIGHTING D3DRENDERSTATE_LIGHTING
#define D3DRS_ZENABLE D3DRENDERSTATE_ZENABLE
#define D3DTS_WORLD D3DTRANSFORMSTATE_WORLD
#define D3DTS_VIEW D3DTRANSFORMSTATE_VIEW
#define D3DTS_PROJECTION D3DTRANSFORMSTATE_PROJECTION
#define Ambient dcvAmbient
#define Diffuse dcvDiffuse
#define Specular dcvSpecular
#define Position dvPosition
#define Range dvRange
#define Falloff dvFalloff
#define Attenuation1 dvAttenuation1
#define DrawSubset(X) Draw()

class CCoInitialize
{
private:
    const HRESULT m_hr;

public:
    virtual ~CCoInitialize()
    {
        if(!FAILED(m_hr))
        {
            CoUninitialize();
        }
    }

    CCoInitialize()
        : m_hr(CoInitialize(NULL))
    {
    }
};

class CTestWindow
    : public CSimpleWindow
{
private:
    typedef CTestWindow CSelf;
    typedef CSimpleWindow CSuper;

protected:
    ID3DXContext* m_lpDC;
    IDirect3DDevice7* m_lpDD;

    ID3DXMesh* m_lpSphere1Mesh;
    ID3DXMesh* m_lpSphere01Mesh;

public:
    virtual ~CTestWindow()
    {
    }

    CTestWindow(HINSTANCE hInstance)
        : CSimpleWindow(hInstance, 640, 480, WS_VISIBLE|WS_POPUPWINDOW|WS_CAPTION|WS_MINIMIZEBOX)
        , m_lpDC(nullptr)
        , m_lpDD(nullptr)
        , m_lpSphere1Mesh(nullptr)
        , m_lpSphere01Mesh(nullptr)
    {
    }

    template< typename T > static void ReleaseCOM(T** lppInOut)
    {
        T* lpObj = lppInOut[0];

        if(lpObj!=nullptr)
        {
            lppInOut[0] = nullptr;

            lpObj->Release();
            lpObj = NULL;
        }
    }

    void UninitializeDirectX(const bool bFinal = false)
    {
        ReleaseCOM(&m_lpSphere01Mesh);
        ReleaseCOM(&m_lpSphere1Mesh);
        ReleaseCOM(&m_lpDD);
        ReleaseCOM(&m_lpDC);

        D3DXUninitialize();
    }

    bool InitializeDevice()
    {
        if(FAILED(m_lpDD->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE)))
        {
            return false;
        }

        // R.AddLight(Raytracer::CreateLight(Raytracer::CreateVector3d(+9.0f, +9.0f, +0.0f), Raytracer::CreateColor(1.0f, 1.0f, 1.0f), 50.0f));
        D3DLIGHT7 Li = { D3DLIGHT_POINT };
        Li.Diffuse =
        Li.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
        Li.Ambient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
        Li.Position = D3DXVECTOR3(9.0f, 9.0f, 0.0f);
        Li.Range = 50.0f;
        Li.Falloff = 1.0f;
        Li.Attenuation1 = 0.1f;

        if(FAILED(m_lpDD->SetLight(0, &Li)))
        {
            return false;
        }

        if(FAILED(m_lpDD->LightEnable(0, TRUE)))
        {
            return false;
        }

        if(FAILED(m_lpDD->SetRenderState(D3DRS_LIGHTING, TRUE)))
        {
            return false;
        }

        if(FAILED(m_lpDD->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_COLORVALUE(0.2f, 0.2f, 0.2f, 1.0f))))
        {
            return false;
        }

        D3DXMATRIX mtxV;
        D3DXVECTOR3 ovecEye(+0.0f, +1.0f, -10.0f);
        D3DXVECTOR3 ovecTarget(+0.0f, +1.0f, -9.0f);
        D3DXVECTOR3 uvecUp(+0.0f, +1.0f, +0.0f);

        D3DXMatrixLookAtLH(&mtxV, &ovecEye, &ovecTarget, &uvecUp);

        if(FAILED(m_lpDD->SetTransform(D3DTS_VIEW, mtxV)))
        {
            return false;
        }

        if(FAILED(D3DXCreateSphere(m_lpDD, 1.0f, 20, 20, D3DX_DEFAULT, &m_lpSphere1Mesh)))
        {
            return false;
        }

        if(FAILED(D3DXCreateSphere(m_lpDD, 0.1f, 20, 20, D3DX_DEFAULT, &m_lpSphere01Mesh)))
        {
            return false;
        }

        return true;
    }

    bool InitializeDirectX(HWND hWnd)
    {
        if(m_lpDC==nullptr)
        {
            if(FAILED(D3DXInitialize()))
            {
                return false;
            }

            if(FAILED(D3DXCreateContextEx(D3DX_DEFAULT, 0, hWnd, NULL, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, &m_lpDC)))
            {
                return false;
            }

            m_lpDD = m_lpDC->GetD3DDevice();

            return InitializeDevice();
        }

        return true;
    }

    bool ReinitializeDirectX(HWND hWnd)
    {
        UninitializeDirectX();

        return InitializeDirectX(hWnd);
    }

    void RenderSphere(ID3DXMesh* lpSphere, const D3DVECTOR& ovecPosition, const D3DCOLORVALUE& clrMaterial)
    {
        D3DXMATRIX mtxWorld;

        D3DXMatrixTranslation(&mtxWorld, ovecPosition.x, ovecPosition.y, ovecPosition.z);
        m_lpDD->SetTransform(D3DTS_WORLD, mtxWorld);

        D3DMATERIAL7 matMaterial = { 0 };

        matMaterial.Diffuse = matMaterial.Ambient = clrMaterial;
        m_lpDD->SetMaterial(&matMaterial);

        lpSphere->DrawSubset(0);
    }

    void Render()
    {
        m_lpDD->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

        if(!FAILED(m_lpDD->BeginScene()))
        {
            D3DXMATRIX matWorld;

            //R.AddObject(Raytracer::CreatePlane(Raytracer::CreateVector3d(+0.0f, +0.0f, +0.0f), Raytracer::CreateVector3d(+0.0f, +1.0f, +0.0f), Raytracer::CreateColor(1.0f, 1.0f, 1.0f)));
            struct XYZ_N_VECTOR
            {
                float x, y, z;
                float nx, ny, nz;
            }
            avtxFan[] =
            {
                {   +0.0f, +0.0f,   +0.0f, 0.0f, 1.0f, 0.0f },
                { -100.0f, +0.0f, -100.0f, 0.0f, 1.0f, 0.0f },
                { -100.0f, +0.0f, +100.0f, 0.0f, 1.0f, 0.0f },
                { +100.0f, +0.0f, +100.0f, 0.0f, 1.0f, 0.0f },
                { +100.0f, +0.0f, -100.0f, 0.0f, 1.0f, 0.0f },
                { -100.0f, +0.0f, -100.0f, 0.0f, 1.0f, 0.0f },
            };
            D3DMATERIAL7 matWhite = { 0 };
            matWhite.Diffuse = matWhite.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
            m_lpDD->SetMaterial(&matWhite);
            m_lpDD->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_XYZ|D3DFVF_NORMAL, avtxFan, _countof(avtxFan), 0);

            //R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+0.0f, +0.0f, +0.0f), 0.1f, Raytracer::CreateColor(1.0f, 0.0f, 0.0f)));
            RenderSphere(m_lpSphere01Mesh, D3DXVECTOR3(+0.0f, +0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));

            //R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+2.0f, +1.0f, -3.0f), 1.0f, Raytracer::CreateColor(0.25f, 0.25f, 0.75f)));
            RenderSphere(m_lpSphere1Mesh, D3DXVECTOR3(+2.0f, +1.0f, +3.0f), D3DXCOLOR(0.25f, 0.25f, 0.75f, 1.0f));

            //R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(-2.0f, +1.0f, -3.0f), 1.0f, Raytracer::CreateColor(0.75f, 0.25f, 0.25f)));
            RenderSphere(m_lpSphere1Mesh, D3DXVECTOR3(-2.0f, +1.0f, +3.0f), D3DXCOLOR(0.75f, 0.25f, 0.25f, 1.0f));

            //R.AddObject(Raytracer::CreateSphere(Raytracer::CreateVector3d(+0.0f, +1.0f, -2.0f), 1.0f, Raytracer::CreateColor(0.25f, 0.75f, 0.25f)));
            RenderSphere(m_lpSphere1Mesh, D3DXVECTOR3(+0.0f, +1.0f, +2.0f), D3DXCOLOR(0.25f, 0.75f, 0.25f, 1.0f));

            m_lpDD->EndScene();
        }

        m_lpDC->UpdateFrame(0);
    }

    virtual BOOL WndProcOnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
    {
        if(InitializeDirectX(hWnd))
        {
            if(CSuper::WndProcOnCreate(hWnd, lpCreateStruct))
            {// this has to come last, because it triggers onWindowPosChanged
                SetWindowText(hWnd, "DirectX7X TestCase (press ESC to exit)");
                return TRUE;
            }
        }

        return FALSE;
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

        if(!IsMinimized(hWnd))
        {
            m_lpDC->Resize(rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top);

            // NOTE: Compared to DX8/DX9, the aspect ratio is 1/x
            D3DXMATRIX mtxP;
            D3DXMatrixPerspectiveFovLH(&mtxP, D3DX_PI/4, float(rcWnd.bottom-rcWnd.top)/float(rcWnd.right-rcWnd.left), 1.0f, 100.0f);
            m_lpDD->SetTransform(D3DTS_PROJECTION, mtxP);
        }

        CSuper::WndProcOnWindowPosChanged(hWnd, lpWP);
    }
};

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, INT nShowCmd)
{
    return CTestWindow(hInstance);
}
