#include <windows.h>
#include <windowsx.h>
#include <d3d8.h>
#include <d3dx8.h>

#include "simplewnd.tpp"

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
    D3DFORMAT m_nBackBufferFormat;
    IDirect3DDevice8* m_lpDD;

    ID3DXMesh* m_lpSphere1Mesh;
    ID3DXMesh* m_lpSphere01Mesh;

public:
    virtual ~CTestWindow()
    {
    }

    CTestWindow(HINSTANCE hInstance)
        : CSimpleWindow(hInstance, 640, 480, WS_VISIBLE|WS_POPUPWINDOW|WS_CAPTION|WS_MINIMIZEBOX)
        , m_nBackBufferFormat(D3DFMT_UNKNOWN)
        , m_lpDD(nullptr)
        , m_lpSphere1Mesh(nullptr)
        , m_lpSphere01Mesh(nullptr)
    {
    }

    void UninitializeDirectX(const bool bFinal = false)
    {
        if(m_lpSphere01Mesh!=nullptr)
        {
            m_lpSphere01Mesh->Release();
            m_lpSphere01Mesh = nullptr;
        }

        if(m_lpSphere1Mesh!=nullptr)
        {
            m_lpSphere1Mesh->Release();
            m_lpSphere1Mesh = nullptr;
        }

        if(m_lpDD!=nullptr)
        {
            m_lpDD->Release();
            m_lpDD = nullptr;
        }
    }

    bool InitializeDevice()
    {
        if(FAILED(m_lpDD->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE)))
        {
            return false;
        }

        // R.AddLight(Raytracer::CreateLight(Raytracer::CreateVector3d(+9.0f, +9.0f, +0.0f), Raytracer::CreateColor(1.0f, 1.0f, 1.0f), 50.0f));
        D3DLIGHT8 Li = { D3DLIGHT_POINT };
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

        if(FAILED(m_lpDD->SetTransform(D3DTS_VIEW, &mtxV)))
        {
            return false;
        }

        if(FAILED(D3DXCreateSphere(m_lpDD, 1.0f, 20, 20, &m_lpSphere1Mesh, NULL)))
        {
            return false;
        }

        if(FAILED(D3DXCreateSphere(m_lpDD, 0.1f, 20, 20, &m_lpSphere01Mesh, NULL)))
        {
            return false;
        }

        return true;
    }

    bool InitializeDirectX(HWND hWnd)
    {
        if(m_lpDD==nullptr)
        {
            IDirect3D8* lpD3D = Direct3DCreate8(D3D_SDK_VERSION);

            if(lpD3D==nullptr)
            {
                return false;
            }

            D3DDISPLAYMODE Dm = { 0 };

            if(FAILED(lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Dm)))
            {
                return false;
            }

            m_nBackBufferFormat = Dm.Format;

            D3DPRESENT_PARAMETERS PP = { 0 };

            PP.BackBufferFormat = m_nBackBufferFormat;
            PP.Windowed = TRUE;
            PP.SwapEffect = D3DSWAPEFFECT_FLIP;
            PP.EnableAutoDepthStencil = TRUE;
            PP.AutoDepthStencilFormat = D3DFMT_D16;

            HRESULT hr = lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &PP, &m_lpDD);

            lpD3D->Release();  // no longer needed

            if(FAILED(hr))
            {
                return false;
            }

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
        m_lpDD->SetTransform(D3DTS_WORLD, &mtxWorld);

        D3DMATERIAL8 matMaterial = { 0 };

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
            const struct XYZ_N_VECTOR
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
            D3DMATERIAL8 matWhite = { 0 };
            matWhite.Diffuse = matWhite.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
            m_lpDD->SetMaterial(&matWhite);
            m_lpDD->SetVertexShader(D3DFVF_XYZ|D3DFVF_NORMAL);
            m_lpDD->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, _countof(avtxFan)-2, &avtxFan, sizeof(avtxFan[0]));

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

        m_lpDD->Present(NULL, NULL, NULL, NULL);
    }

    virtual BOOL WndProcOnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
    {
        if(InitializeDirectX(hWnd))
        {
            if(CSuper::WndProcOnCreate(hWnd, lpCreateStruct))
            {// this has to come last, because it triggers onWindowPosChanged
                SetWindowText(hWnd, "DirectX TestCase (press ESC to exit)");
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

        D3DPRESENT_PARAMETERS PP = { 0 };
        PP.BackBufferFormat = m_nBackBufferFormat;
        PP.Windowed = TRUE;
        PP.SwapEffect = D3DSWAPEFFECT_FLIP;
        PP.EnableAutoDepthStencil = TRUE;
        PP.AutoDepthStencilFormat = D3DFMT_D16;
        m_lpDD->Reset(&PP);
        InitializeDevice();

        D3DXMATRIX mtxP;
        D3DXMatrixPerspectiveFovLH(&mtxP, D3DX_PI/4, float(rcWnd.right-rcWnd.left)/float(rcWnd.bottom-rcWnd.top), 1.0f, 100.0f);
        m_lpDD->SetTransform(D3DTS_PROJECTION, &mtxP);

        CSuper::WndProcOnWindowPosChanged(hWnd, lpWP);
    }
};

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, INT nShowCmd)
{
    return CTestWindow(hInstance);
}
