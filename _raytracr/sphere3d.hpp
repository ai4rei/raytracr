#ifndef SNIPPETS_RAYTRACER_SPHERE3D_HPP
#define SNIPPETS_RAYTRACER_SPHERE3D_HPP

class CSphere3d
{
private:
    typedef CSphere3d CSelf;

private:
    const CVector3d m_ovecOrigin;
    const float m_nRadius;
    const float m_nRadius2;
    
    CSphere3d(const CVector3d& ovecOrigin, const float nRadius)
        : m_ovecOrigin(ovecOrigin)
        , m_nRadius(nRadius)
        , m_nRadius2(nRadius*nRadius)
    {
    }
    
    float AngleTo(const CVector3d& ovecPointOnSurface, const CVector3d& rvecDirection) const
    {
        return CPlane3d(ovecPointOnSurface, ovecPointOnSurface-m_ovecOrigin).AngleTo(rvecDirection);
    }
};

#endif  /* SNIPPETS_RAYTRACER_SPHERE3D_HPP */
