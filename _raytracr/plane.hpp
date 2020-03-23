class CPlane
    : public CObject
{
private:
    const CVector3d m_ovecOrigin;
    const CVector3d m_uvecNormal;

public:
    CPlane(const CVector3d& ovecOrigin, const CVector3d& rvecNormal, const CColor& clrColor)
        : m_ovecOrigin(ovecOrigin)
        , m_uvecNormal(rvecNormal.UnitVector())
        , CObject(clrColor)
    {
    }

    virtual CIntersection Intersect(const CVector3d& ovecOrigin, const CVector3d& rvecRay) const
    {
        const float nC = m_uvecNormal.DotProduct(m_ovecOrigin-ovecOrigin);
        const float nD = m_uvecNormal.DotProduct(rvecRay);

        if(nD==0)
        {
            return CIntersection();
        }

        const float nT = nC/nD;

        if(nT<0)
        {
            return CIntersection();
        }

        return CIntersection(nT);
    }

    virtual bool IsLight() const
    {
        return false;
    }

    virtual const CVector3d& Origin() const
    {
        return m_ovecOrigin;
    }
};
