#include <d3d9.h>
#include <windows.h>
#include <windowsx.h>

#include "simplewnd.tpp"

HRESULT WINAPI Direct3DCreate9(UINT SDKVersion, IDirect3D9** lppD3DOut)
{
    IDirect3D9* lpD3D = Direct3DCreate9(SDKVersion);

    lppD3DOut[0] = lpD3D;

    return lpD3D!=NULL ? S_OK : D3DERR_NOTAVAILABLE;
}

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
    IDirect3D9* m_lpD3D;
    IDirect3DDevice9* m_lpDD;

public:
    virtual ~CTestWindow()
    {
        UninitializeDirectX(true);
    }

    CTestWindow(HINSTANCE hInstance)
        : CSimpleWindow(hInstance, 640, 480, WS_VISIBLE|WS_POPUPWINDOW|WS_CAPTION|WS_MINIMIZEBOX)
        , m_lpD3D(nullptr)
        , m_lpDD(nullptr)
    {
    }

    void UninitializeDirectX(const bool bFinal = false)
    {
        if(m_lpDD!=nullptr)
        {
            m_lpDD->Release();
            m_lpDD = nullptr;
        }

        if(bFinal && m_lpD3D!=nullptr)
        {
            m_lpD3D->Release();
            m_lpD3D = nullptr;
        }
    }

    bool InitializeDirectX(HWND hWnd)
    {
        HRESULT hr;

        if(m_lpD3D==nullptr)
        {
            hr = Direct3DCreate9(D3D_SDK_VERSION, &m_lpD3D);

            if(FAILED(hr))
            {
                return false;
            }
        }

        if(m_lpDD==nullptr)
        {
            D3DPRESENT_PARAMETERS D3dpp = { 0 };

            D3dpp.Windowed = TRUE;
            D3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;

            hr = m_lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &D3dpp, &m_lpDD);

            if(FAILED(hr))
            {
                return false;
            }
        }

        return true;
    }

    bool ReinitializeDirectX(HWND hWnd)
    {
        UninitializeDirectX();

        return InitializeDirectX(hWnd);
    }

    void Render()
    {
        if(m_lpDD==nullptr)
        {
            return;
        }

        m_lpDD->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 127, 127), 1.0f, 0);
        m_lpDD->BeginScene();
        m_lpDD->EndScene();
        m_lpDD->Present(NULL, NULL, NULL, NULL);
    }

    virtual BOOL WndProcOnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
    {
        if(CSuper::WndProcOnCreate(hWnd, lpCreateStruct))
        {
            if(InitializeDirectX(hWnd))
            {
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
};

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, INT nShowCmd)
{
    return CTestWindow(hInstance);
}
