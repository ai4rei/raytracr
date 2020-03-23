template< typename T > LRESULT CALLBACK WndProcPP(CONST HWND hWnd, CONST UINT uMsg, CONST WPARAM wParam, CONST LPARAM lParam, LRESULT (T::*WndProc)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam))
{
    T* lpThis = (T*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    if(lpThis!=NULL)
    {
        return (lpThis->WndProc)(hWnd, uMsg, wParam, lParam);
    }
    else
    if(uMsg==WM_NCCREATE)
    {
        LPCREATESTRUCT lpCreateStruct = (LPCREATESTRUCT)lParam;

        if(lpCreateStruct->lpCreateParams!=NULL)
        {
            lpThis = static_cast< T* >(lpCreateStruct->lpCreateParams);

            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)lpThis);

            return (lpThis->WndProc)(hWnd, uMsg, wParam, lParam);
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

class CSimpleWindow
{
private:
    typedef CSimpleWindow CSelf;

protected:
    const char* const WINDOWCLASSNAME;

    const HINSTANCE m_hInstance;
    const INT m_nClientW;
    const INT m_nClientH;
    const DWORD m_dwStyle;

public:
    CSimpleWindow(const HINSTANCE hInstance, const INT nClientW, const INT nClientH, const DWORD dwStyle)
        : m_hInstance(hInstance)
        , m_nClientW(nClientW)
        , m_nClientH(nClientH)
        , m_dwStyle(dwStyle)
        , WINDOWCLASSNAME("CSimpleWindowClass")
    {
    }

    virtual BOOL WndProcOnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
    {
        RECT rcWnd, rcWA;

        if(!SystemParametersInfo(SPI_GETWORKAREA, sizeof(rcWA), &rcWA, 0))
        {
            return FALSE;
        }

        SetRect(&rcWnd, 0, 0, m_nClientW, m_nClientH);
        AdjustWindowRectEx(&rcWnd, GetWindowStyle(hWnd), FALSE, GetWindowExStyle(hWnd));
        OffsetRect(&rcWnd, -rcWnd.left+(rcWA.right-rcWA.left-rcWnd.right+rcWnd.left)/2, -rcWnd.top+(rcWA.bottom-rcWA.top-rcWnd.bottom+rcWnd.top)/2);

        MoveWindow(hWnd, rcWnd.left, rcWnd.top, rcWnd.right-rcWnd.left, rcWnd.bottom-rcWnd.top, TRUE);

        return TRUE;
    }

    virtual VOID WndProcOnDestroy(HWND hWnd)
    {
        PostQuitMessage(EXIT_SUCCESS);
    }

    virtual VOID WndProcOnCommand(HWND hWnd, INT nId, HWND hWndCtl, UINT uCodeNotify)
    {
        switch(nId)
        {
        case IDCANCEL:
            DestroyWindow(hWnd);
            break;
        }
    }

    virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch(uMsg)
        {
        HANDLE_MSG(hWnd, WM_CREATE, WndProcOnCreate);
        HANDLE_MSG(hWnd, WM_DESTROY, WndProcOnDestroy);
        HANDLE_MSG(hWnd, WM_COMMAND, WndProcOnCommand);
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    static LRESULT CALLBACK WndProcCB(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        return WndProcPP(hWnd, uMsg, wParam, lParam, &CSelf::WndProc);
    }

public:
    operator int()
    {
        WNDCLASSEX Wc = { sizeof(Wc) };

        Wc.lpfnWndProc = &WndProcCB;
        Wc.hInstance = m_hInstance;
        Wc.lpszClassName = WINDOWCLASSNAME;
        Wc.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
        Wc.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
        Wc.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));

        if(RegisterClassEx(&Wc))
        {
            HWND hWnd = CreateWindowEx(0, WINDOWCLASSNAME, "", m_dwStyle, 0, 0, 0, 0, NULL, NULL, m_hInstance, this);

            if(hWnd!=NULL)
            {
                MSG Msg;

                while(GetMessage(&Msg, NULL, 0, 0)>0)
                {
                    if(!IsDialogMessage(hWnd, &Msg))
                    {
                        TranslateMessage(&Msg);
                        DispatchMessage(&Msg);
                    }
                }

                if(Msg.message==WM_QUIT)
                {
                    return (int)Msg.wParam;
                }
            }
        }

        return EXIT_FAILURE;
    }
};
