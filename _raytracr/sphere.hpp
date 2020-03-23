class CSphere
    : public CObject
{
private:
    const CVector3d m_ovecOrigin;
    const float m_nRadius;
    const float m_nRadius2;

public:
    CSphere(const CVector3d& ovecOrigin, const float nRadius, const CColor& clrColor, const float nIntensity = 0.0f)
        : m_ovecOrigin(ovecOrigin)
        , m_nRadius(nRadius)
        , m_nRadius2(nRadius*nRadius)
        , CObject(clrColor, nIntensity)
    {
    }

    virtual CIntersection Intersect(const CVector3d& ovecOrigin, const CVector3d& rvecRay) const
    {// https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
        const float nVcbs = rvecRay.DotProduct(ovecOrigin-m_ovecOrigin);

        // value under square-root
        const float nVusr = pow(nVcbs, 2)
                          - (ovecOrigin-m_ovecOrigin).Magnitude2()
                          + m_nRadius2
                          ;

        if(nVusr<0)
        {
            return CIntersection();
        }

        // value before square-root
        const float nVbsr = -nVcbs;

        if(nVusr>0)
        {
            // value of square-root
            const float nVosr = sqrt(nVusr);

            const float nT1 = nVbsr+nVosr;
            const float nT2 = nVbsr-nVosr;

            if(nT1<0)
            {
                if(nT2<0)
                {
                    return CIntersection();
                }

                return CIntersection(nT2);
            }
            else
            if(nT2<0)
            {
                return CIntersection(nT1);
            }

            return CIntersection(nT1, nT2);
        }

        return CIntersection(nVbsr);
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
