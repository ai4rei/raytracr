class CHit3d
{
private:
    float m_nDistance;
    CColor m_clrColor;
    CVector3d m_uvecSurfaceNormal;
    bool m_bHit;

public:
    CHit3d()
        : m_nDistance(0)
        , m_clrColor(0, 0, 0)
        , m_uvecSurfaceNormal(0, 0, 0)
        , m_bHit(false)
    {
    }

    CHit3d(const float nDistance, const CColor& clrColor, const CVector3d& uvecSurfaceNormal)
        : m_nDistance(nDistance)
        , m_clrColor(clrColor)
        , m_uvecSurfaceNormal(uvecSurfaceNormal)
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

    const CVector3d& GetSurfaceNormal() const
    {
        return m_uvecSurfaceNormal;
    }
};
