#ifndef SNIPPETS_RAYTRACER_SPHERE_HPP
#define SNIPPETS_RAYTRACER_SPHERE_HPP

class CSphere
    : public CObject
{
private:
    const CVector3d m_ovecOrigin;
    const float m_nRadius;
    const float m_nRadius2;

protected:
    /*u*/CVector3d GetSurfaceNormalForRayPoint(const CRay3d& Ray, const float nDistance) const
    {
        const CVector3d ovecPointOnSurface = Ray.GetPointAt(nDistance);

        return (ovecPointOnSurface-m_ovecOrigin).UnitVector();
    }

public:
    CSphere(const CVector3d& ovecOrigin, const float nRadius, const CColor& clrColor)
        : m_ovecOrigin(ovecOrigin)
        , m_nRadius(nRadius)
        , m_nRadius2(nRadius*nRadius)
        , CObject(clrColor)
    {
    }

    virtual CHit3d HitTest(const CRay3d& Ray, const float nDistanceMin, const float nDistanceMax) const
    {// https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
        const float nVcbs = Ray.GetLookAt().DotProduct(Ray.GetEye()-m_ovecOrigin);

        // value under square-root
        const float nVusr = nVcbs*nVcbs
                          - (Ray.GetEye()-m_ovecOrigin).Magnitude2()
                          + m_nRadius2
                          ;

        if(nVusr<0)
        {
            return CHit3d();
        }

        // value before square-root
        const float nVbsr = -nVcbs;

        if(nVusr>0)
        {
            // value of square-root
            const float nVosr = sqrt(nVusr);

            const float nDistance1 = nVbsr+nVosr;
            const float nDistance2 = nVbsr-nVosr;

            if(nDistance1<nDistanceMin || nDistance1>nDistanceMax)
            {
                if(nDistance2<nDistanceMin || nDistance2>nDistanceMax)
                {
                    return CHit3d();
                }
                else
                {
                    return CHit3d(nDistance2, m_clrColor, GetSurfaceNormalForRayPoint(Ray, nDistance2));
                }
            }
            else
            {
                if(nDistance2<nDistanceMin || nDistance2>nDistanceMax)
                {
                    return CHit3d(nDistance1, m_clrColor, GetSurfaceNormalForRayPoint(Ray, nDistance1));
                }
                else
                {
                    return CHit3d(min(nDistance1, nDistance2), m_clrColor, GetSurfaceNormalForRayPoint(Ray, min(nDistance1, nDistance2)));
                }
            }
        }

        if(nVbsr<nDistanceMin || nVbsr>nDistanceMax)
        {
            return CHit3d();
        }

        return CHit3d(nVbsr, m_clrColor, GetSurfaceNormalForRayPoint(Ray, nVbsr));
    }

    virtual const CVector3d& GetOrigin() const
    {
        return m_ovecOrigin;
    }
};

#endif  /* SNIPPETS_RAYTRACER_SPHERE_HPP */
