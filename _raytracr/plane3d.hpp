#ifndef SNIPPETS_RAYTRACER_PLANE3D_HPP
#define SNIPPETS_RAYTRACER_PLANE3D_HPP

class CPlane3d
{
private:
    typedef CPlane3d CSelf;

private:
    CVector3d m_ovecOrigin;
    CVector3d m_uvecNormal;

public:
    CPlane3d(const CVector3d& ovecOrigin, const CVector3d& uvecNormal)
        : m_ovecOrigin(ovecOrigin)
        , m_uvecNormal(uvecNormal)
    {
    }

    bool IsPointInPlane(const CVector3d& ovecPoint) const
    {
        return IsParallelTo(ovecPoint-m_ovecOrigin);
    }

    bool IsParallelTo(const CVector3d& vecVector) const
    {
        return m_uvecNormal.DotProduct(vecVector)==0;
    }

    bool IsParallelTo(const CSelf& plnOther) const
    {
        return m_uvecNormal.IsParallelTo(plnOther.m_uvecNormal);
    }

    float AngleTo(const CVector3d& vecVector) const
    {
        return m_uvecNormal.CrossProduct(vecVector.UnitVector().CrossProduct(m_uvecNormal)).AngleTo(vecVector.UnitVector());
    }

    float AngleTo(const CSelf& plnOther) const
    {
        return m_uvecNormal.AngleTo(plnOther.m_uvecNormal);
    }

    /*
         L--n->R
          \ ^ /
           \|/
        ----o----
    */
    /*r*/CVector3d Reflection(const CVector3d& vecVector) const
    {
        const CVector3d rvecOL = -vecVector;
        const CVector3d rvecON = rvecOL.ProjectionTo(m_uvecNormal);
        const CVector3d rvecLN = rvecON-rvecOL;
        const CVector3d rvecNR = rvecLN;
        const CVector3d rvecOR = rvecON+rvecNR;

        return rvecOR;
    }

    /*o*/CVector3d Intersection(const CVector3d& ovecOrigin, const CVector3d& rvecDirection) const
    {// prerequisites: not-inplane, not-parallel, only point-inplane
        return ovecOrigin+rvecDirection.UnitVector()*(m_uvecNormal.DotProduct(m_ovecOrigin-ovecOrigin)/m_uvecNormal.DotProduct(rvecDirection.UnitVector()));
    }

    CSelf CrossProduct(const CSelf& plnOther) const
    {
        return CSelf(CVector3d
        (
            (m_uvecNormal.X()*m_ovecOrigin.X()-plnOther.m_uvecNormal.X()*m_ovecOrigin.X())/(m_uvecNormal.X()-plnOther.m_uvecNormal.X()),
            (m_uvecNormal.Y()*m_ovecOrigin.Y()-plnOther.m_uvecNormal.Y()*m_ovecOrigin.Y())/(m_uvecNormal.Y()-plnOther.m_uvecNormal.Y()),
            (m_uvecNormal.Z()*m_ovecOrigin.Z()-plnOther.m_uvecNormal.Z()*m_ovecOrigin.Z())/(m_uvecNormal.Z()-plnOther.m_uvecNormal.Z())
        ), m_uvecNormal.CrossProduct(plnOther.m_uvecNormal));
    }
};

#endif  /* SNIPPETS_RAYTRACER_PLANE3D_HPP */
