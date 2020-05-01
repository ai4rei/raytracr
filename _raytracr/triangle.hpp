#ifndef SNIPPETS_RAYTRACER_TRIANGLE_HPP
#define SNIPPETS_RAYTRACER_TRIANGLE_HPP

class CTriangle
    : public CObject
{
private:
    const CVector3d m_ovecA;
    const CVector3d m_ovecB;
    const CVector3d m_ovecC;
    const CVector3d m_uvecNormal;

public:
    CTriangle(const CVector3d& ovecA, const CVector3d& ovecB, const CVector3d& ovecC, const CColor& clrColor)
        : m_ovecA(ovecA)
        , m_ovecB(ovecB)
        , m_ovecC(ovecC)
        , m_uvecNormal((m_ovecB-m_ovecA).CrossProduct(m_ovecC-m_ovecA).UnitVector())
        , CObject(clrColor)
    {
    }

    CVector3d Centroid() const
    {// possibly wrong
        return ((m_ovecC-m_ovecA)-(m_ovecB-m_ovecA)/2.0f)/3.0f;
    }

    virtual CHit3d HitTest(const CRay3d& Ray, const float nDistanceMin, const float nDistanceMax) const
    {
        const float nDenominator = m_uvecNormal.DotProduct(Ray.GetLookAt());

        if(nDenominator==0)
        {// parallel to
            return CHit3d();
        }

        const float nCounter = m_uvecNormal.DotProduct(m_ovecA-Ray.GetEye());
        const float nDistance = nCounter/nDenominator;

        if(nDistance<nDistanceMin || nDistance>nDistanceMax)
        {
            return CHit3d();
        }

        const CVector3d ovecP = Ray.GetPointAt(nDistance);

        if(m_uvecNormal.DotProduct((m_ovecB-m_ovecA).CrossProduct(ovecP-m_ovecA))<0
        || m_uvecNormal.DotProduct((m_ovecC-m_ovecB).CrossProduct(ovecP-m_ovecB))<0
        || m_uvecNormal.DotProduct((m_ovecA-m_ovecC).CrossProduct(ovecP-m_ovecC))<0
        )
        {
            return CHit3d();
        }

        return CHit3d(nDistance, m_clrColor, m_uvecNormal);
    }

    virtual const CVector3d& GetOrigin() const
    {
        return m_ovecA;  // this should be centroid
    }
};

#endif  /* SNIPPETS_RAYTRACER_TRIANGLE_HPP */
