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

    virtual CHit3d HitTest(const CRay3d& Ray, const float nDistanceMin, const float nDistanceMax) const
    {
        const float nDenominator = m_uvecNormal.DotProduct(Ray.GetLookAt());

        if(nDenominator==0)
        {// parallel to
            return CHit3d();
        }

        const float nCounter = m_uvecNormal.DotProduct(m_ovecOrigin-Ray.GetEye());
        const float nDistance = nCounter/nDenominator;

        if(nDistance<nDistanceMin || nDistance>nDistanceMax)
        {
            return CHit3d();
        }

        return CHit3d(nDistance, m_clrColor, m_uvecNormal);
    }

    virtual const CVector3d& GetOrigin() const
    {
        return m_ovecOrigin;
    }
};
