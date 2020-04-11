class CHit3d
{
private:
    float m_nDistance;
    CColor m_clrColor;
    bool m_bHit;

public:
    CHit3d()
        : m_nDistance(0)
        , m_clrColor(0, 0, 0)
        , m_bHit(false)
    {
    }

    CHit3d(const float nDistance, const CColor& clrColor)
        : m_nDistance(nDistance)
        , m_clrColor(clrColor)
        , m_bHit(true)
    {
    }

    operator bool() const
    {
        return m_bHit;
    }

    float GetDistance() const
    {
        return m_nDistance;
    }

    const CColor& GetColor() const
    {
        return m_clrColor;
    }
};
