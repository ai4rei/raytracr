#ifndef RAYTRACR_UTILITY_HPP
#define RAYTRACR_UTILITY_HPP

class CCriticalSection
    : public CRITICAL_SECTION
{
public:
    ~CCriticalSection()
    {
        DeleteCriticalSection(this);
    }
    CCriticalSection()
    {
        InitializeCriticalSection(this);
    }
    void Enter()
    {
        EnterCriticalSection(this);
    }
    void Leave()
    {
        LeaveCriticalSection(this);
    }
};

class CGetDC
{
private:
    CONST HWND m_hWnd;
    CONST HDC m_hDC;

public:
    ~CGetDC()
    {
        if(m_hDC!=nullptr)
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

class CSolidBrush
{
private:
    HBRUSH m_hbrBrush;

public:
    ~CSolidBrush()
    {
        if(m_hbrBrush!=nullptr)
        {
            DeleteBrush(m_hbrBrush);
            m_hbrBrush = nullptr;
        }
    }

    CSolidBrush(COLORREF clrBrush)
        : m_hbrBrush(CreateSolidBrush(clrBrush))
    {
    }

    operator const HBRUSH&()
    {
        return m_hbrBrush;
    }
};

class CSolidPen
{
private:
    HPEN m_hpnPen;

public:
    ~CSolidPen()
    {
        if(m_hpnPen!=nullptr)
        {
            DeletePen(m_hpnPen);
            m_hpnPen = nullptr;
        }
    }

    CSolidPen(int nWidth, COLORREF clrPen)
        : m_hpnPen(CreatePen(PS_SOLID, nWidth, clrPen))
    {
    }

    operator const HPEN&()
    {
        return m_hpnPen;
    }
};

#endif  /* RAYTRACR_UTILITY_HPP */
