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
    IDirectDraw7* m_lpDDraw;
    IDirect3DDevice7* m_lpDD;
    IDirectDrawSurface7* m_lpDDSPrimary;

    ID3DXSimpleShape* m_lpSphere1Mesh;
    ID3DXSimpleShape* m_lpSphere01Mesh;

    RECT m_rcClientCoords;
    RECT m_rcScreenCoords;

public:
    virtual ~CTestWindow()
    {
    }

    CTestWindow(HINSTANCE hInstance)
        : CSimpleWindow(hInstance, 640, 480, WS_VISIBLE|WS_POPUPWINDOW|WS_CAPTION|WS_MINIMIZEBOX)
        , m_lpDDraw(nullptr)
        , m_lpDD(nullptr)
        , m_lpDDSPrimary(nullptr)
        , m_lpSphere1Mesh(nullptr)
        , m_lpSphere01Mesh(nullptr)
        , m_rcClientCoords()
        , m_rcScreenCoords()
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
        ReleaseCOM(&m_lpDDSPrimary);
        ReleaseCOM(&m_lpDD);
        ReleaseCOM(&m_lpDDraw);
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

        D3DVIEWPORT7 V = { 0 };

        V.dwX = m_rcClientCoords.left;
        V.dwY = m_rcClientCoords.top;
        V.dwWidth = m_rcClientCoords.right;
        V.dwHeight = m_rcClientCoords.bottom;
        V.dvMinZ = 0.0;
        V.dvMaxZ = 1.0;

        if(FAILED(m_lpDD->SetViewport(&V)))
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

    HRESULT InitializePrimarySurface(IDirectDraw7* lpDD, HWND hWnd, IDirectDrawSurface7** lppDDSOut)
    {
        HRESULT hr = E_UNEXPECTED;
        IDirectDrawSurface7* lpDDS = NULL;
        DDSURFACEDESC2 Sd = { sizeof(Sd) };

        lppDDSOut[0] = NULL;

        Sd.dwFlags = DDSD_CAPS;
        Sd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

        hr = lpDD->CreateSurface(&Sd, &lpDDS, NULL);

        if(!FAILED(hr))
        {
            IDirectDrawClipper* lpClipper = NULL;

            hr = lpDD->CreateClipper(0, &lpClipper, NULL);

            if(!FAILED(hr))
            {
                hr = lpClipper->SetHWnd(0, hWnd);

                if(!FAILED(hr))
                {
                    hr = lpDDS->SetClipper(lpClipper);

                    if(!FAILED(hr))
                    {
                        lpDDS->AddRef();
                        lppDDSOut[0] = lpDDS;
                    }
                }

                lpClipper->Release();
                lpClipper = NULL;
            }

            lpDDS->Release();
            lpDDS = NULL;
        }

        return hr;
    }

    static HRESULT CALLBACK InitializeBackBufferZBufferCB(DDPIXELFORMAT* lpDDPixFmt, void* lpContext)
    {
        if(lpDDPixFmt->dwFlags==(DDPF_STENCILBUFFER|DDPF_ZBUFFER))
        {
            DDPIXELFORMAT* lpDDPixFmtOut = (DDPIXELFORMAT*)lpContext;

            lpDDPixFmtOut[0] = lpDDPixFmt[0];

            return D3DENUMRET_CANCEL;
        }

        return D3DENUMRET_OK;
    }

    HRESULT InitializeBackBuffer(IDirectDraw7* lpDD, HWND hWnd, IDirectDrawSurface7** lppDDSOut)
    {
        HRESULT hr = E_UNEXPECTED;
        IDirectDrawSurface7* lpDDS = NULL;
        DDSURFACEDESC2 Sd = { sizeof(Sd) };

        lppDDSOut[0] = NULL;

        GetClientRect(hWnd, &m_rcClientCoords);

        m_rcScreenCoords = m_rcClientCoords;
        MapWindowPoints(hWnd, NULL, (POINT*)&m_rcScreenCoords, sizeof(m_rcScreenCoords)/sizeof(POINT));

        Sd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;
        Sd.ddsCaps.dwCaps = DDSCAPS_3DDEVICE|DDSCAPS_OFFSCREENPLAIN;
        Sd.dwWidth = m_rcClientCoords.right-m_rcClientCoords.left;
        Sd.dwHeight = m_rcClientCoords.bottom-m_rcClientCoords.top;

        hr = lpDD->CreateSurface(&Sd, &lpDDS, NULL);

        if(!FAILED(hr))
        {
            IDirect3D7* lpD3D = NULL;

            hr = lpDD->QueryInterface(IID_IDirect3D7, (void**)&lpD3D);

            if(!FAILED(hr))
            {
                hr = lpD3D->EnumZBufferFormats(IID_IDirect3DHALDevice, &CSelf::InitializeBackBufferZBufferCB, &Sd.ddpfPixelFormat);

                if(!FAILED(hr))
                {
                    IDirectDrawSurface7* lpDDSZBuffer = NULL;

                    Sd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
                    Sd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
                    // width and height is left over from previous call

                    hr = lpDD->CreateSurface(&Sd, &lpDDSZBuffer, NULL);

                    if(!FAILED(hr))
                    {
                        hr = lpDDS->AddAttachedSurface(lpDDSZBuffer);

                        if(!FAILED(hr))
                        {
                            lpDDS->AddRef();
                            lppDDSOut[0] = lpDDS;
                        }

                        lpDDSZBuffer->Release();
                        lpDDSZBuffer = NULL;
                    }
                }

                lpD3D->Release();
                lpD3D = NULL;
            }

            lpDDS->Release();
            lpDDS = NULL;
        }

        return hr;
    }

    bool InitializeDirectX(HWND hWnd)
    {
        if(m_lpDD==nullptr)
        {
            if(FAILED(DirectDrawCreateEx(NULL, (void**)&m_lpDDraw, IID_IDirectDraw7, NULL)))
            {
                return false;
            }

            if(FAILED(m_lpDDraw->SetCooperativeLevel(hWnd, DDSCL_NORMAL)))
            {
                return false;
            }

            if(FAILED(InitializePrimarySurface(m_lpDDraw, hWnd, &m_lpDDSPrimary)))
            {
                return false;
            }

            IDirectDrawSurface7* lpDDSBackBuffer = NULL;

            if(FAILED(InitializeBackBuffer(m_lpDDraw, hWnd, &lpDDSBackBuffer)))
            {
                return false;
            }

            IDirect3D7* lpD3D = NULL;

            if(FAILED(m_lpDDraw->QueryInterface(IID_IDirect3D7, (void**)&lpD3D)))
            {
                lpDDSBackBuffer->Release();
                return false;
            }

            HRESULT hr = lpD3D->CreateDevice(IID_IDirect3DHALDevice, lpDDSBackBuffer, &m_lpDD);

            lpDDSBackBuffer->Release();  // should have kept a reference
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

        IDirectDrawSurface7* lpDDSBackBuffer = NULL;

        if(!FAILED(m_lpDD->GetRenderTarget(&lpDDSBackBuffer)))
        {
            m_lpDDSPrimary->Blt(&m_rcScreenCoords, lpDDSBackBuffer, &m_rcClientCoords, DDBLT_WAIT, NULL);

            lpDDSBackBuffer->Release();
            lpDDSBackBuffer = NULL;
        }
    }

    virtual BOOL WndProcOnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
    {
        if(InitializeDirectX(hWnd))
        {
            if(CSuper::WndProcOnCreate(hWnd, lpCreateStruct))
            {// this has to come last, because it triggers onWindowPosChanged
                SetWindowText(hWnd, "DirectX7 TestCase (press ESC to exit)");
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
            ReinitializeDirectX(hWnd);

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
