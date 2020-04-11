class CObject
{
protected:
    const CColor m_clrColor;

protected:
    CObject(const CColor& clrColor)
        : m_clrColor(clrColor)
    {
    }

public:
    const CColor& GetColor() const
    {
        return m_clrColor;
    }

    virtual CHit3d HitTest(const CRay3d& Ray, const float nDistanceMin, const float nDistanceMax) const = 0;
    virtual const CVector3d& GetOrigin() const = 0;
};
