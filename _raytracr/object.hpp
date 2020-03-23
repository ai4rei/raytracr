class CObject
{
private:
    const CColor m_clrColor;
    const float m_nIntensity;

protected:
    CObject(const CColor& clrColor, const float nIntensity = 0.0f)
        : m_clrColor(clrColor)
        , m_nIntensity(nIntensity)
    {
    }

public:
    const CColor& GetColor() const
    {
        return m_clrColor;
    }

    const CObject* GetObject() const
    {
        return this;
    }

    float Intensity() const
    {
        return m_nIntensity;
    }

    virtual CIntersection Intersect(const CVector3d& ovecOrigin, const CVector3d& rvecRay) const = 0;
    virtual bool IsLight() const = 0;
    virtual const CVector3d& Origin() const = 0;
};
