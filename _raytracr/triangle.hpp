class CTriangle
    : public CObject
{
private:
    const CVector3d m_ovecA;
    const CVector3d m_ovecB;
    const CVector3d m_ovecC;

public:
    CTriangle(const CVector3d& ovecA, const CVector3d& ovecB, const CVector3d& ovecC, const CColor& clrColor)
        : m_ovecA(ovecA)
        , m_ovecB(ovecB)
        , m_ovecC(ovecC)
        , CObject(clrColor)
    {
    }

    CVector3d Centroid() const
    {// possibly wrong
        return ((m_ovecC-m_ovecA)-(m_ovecB-m_ovecA)/2.0f)/3.0f;
    }

    virtual CIntersection Intersect(const CVector3d& ovecOrigin, const CVector3d& rvecRay) const
    {
        const CVector3d uvecNormal = (m_ovecB-m_ovecA).CrossProduct(m_ovecC-m_ovecA).UnitVector();
        const float nC = uvecNormal.DotProduct(m_ovecA-ovecOrigin);
        const float nD = uvecNormal.DotProduct(rvecRay);

        if(nD==0)
        {
            return CIntersection();
        }

        const float nT = nC/nD;

        if(nT<0)
        {
            return CIntersection();
        }

        const CVector3d ovecP = ovecOrigin+rvecRay*nT;

        if(uvecNormal.DotProduct((m_ovecB-m_ovecA).CrossProduct(ovecP-m_ovecA))<0
        || uvecNormal.DotProduct((m_ovecC-m_ovecB).CrossProduct(ovecP-m_ovecB))<0
        || uvecNormal.DotProduct((m_ovecA-m_ovecC).CrossProduct(ovecP-m_ovecC))<0
        )
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
        return m_ovecA;  // this should be centroid
    }
};
