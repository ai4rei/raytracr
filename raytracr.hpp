#ifndef _SNIPPETS_RAYTRACER_HPP_
#define _SNIPPETS_RAYTRACER_HPP_

class Raytracer
{
public:
    #include "_raytracr/vector3d.hpp"
    #include "_raytracr/matrix.hpp"
    #include "_raytracr/matrix3d.hpp"
    #include "_raytracr/plane3d.hpp"
    #include "_raytracr/sphere3d.hpp"
    #include "_raytracr/ray3d.hpp"

    #include "_raytracr/color.hpp"
    #include "_raytracr/hit3d.hpp"

    #include "_raytracr/object.hpp"
    #include "_raytracr/triangle.hpp"
    #include "_raytracr/plane.hpp"
    #include "_raytracr/sphere.hpp"
    #include "_raytracr/light.hpp"  // diffuse, ambient, spot light
    #include "_raytracr/camera.hpp"

    typedef std::vector< std::shared_ptr< CObject > > OBJECTVECTOR;
    typedef std::vector< std::shared_ptr< CLight > > LIGHTVECTOR;
    typedef std::vector< CColor > PIXELVECTOR;
    typedef bool (*PROGRESSCB)(const float nR, const float nG, const float nB, const int nX, const int nY, void* lpContext);

    static const float PI;
#ifndef FLD_MAX
    #define FLD_MAX 1e37f
#endif  /* FLD_MAX */

    int m_nSceneWidth;
    int m_nSceneHeight;
    unsigned int m_uBounceDepth;
    CVector3d m_ovecEye;
    CVector3d m_ovecTarget;
    CVector3d m_rvecLookAt;
    CVector3d m_uvecRight;
    CVector3d m_uvecUp;
    PIXELVECTOR m_aclrPixels;
    OBJECTVECTOR m_aobjObjects;
    LIGHTVECTOR m_aobjLights;
    PROGRESSCB m_lpfnProgress;

    Raytracer()
        : m_nSceneWidth(0)
        , m_nSceneHeight(0)
        , m_uBounceDepth(0)
        , m_ovecEye(0.0f, 0.0f, 0.0f)
        , m_ovecTarget(0.0f, 0.0f, 0.0f)
        , m_rvecLookAt(0.0f, 0.0f, 0.0f)
        , m_uvecRight(0.0f, 0.0f, 0.0f)
        , m_uvecUp(0.0f, 0.0f, 0.0f)
        , m_lpfnProgress(NULL)
    {
    }

    template< typename T > void AddObject(const T& objObject)
    {
        m_aobjObjects.emplace_back(new T(objObject));
    }

    template< typename T > void AddLight(const T& objLight)
    {
        m_aobjLights.emplace_back(new T(objLight));
    }

    void SetScene(const int nWidth, const int nHeight)
    {
        m_nSceneWidth = nWidth;
        m_nSceneHeight = nHeight;
    }

    void SetBounceDepth(const unsigned int uBounceDepth)
    {
        m_uBounceDepth = uBounceDepth;
    }

    void SetCamera(const CVector3d& ovecEye, const CVector3d& ovecTarget, const CVector3d& rvecUp)
    {
        m_ovecEye = ovecEye;
        m_ovecTarget = ovecTarget;
        m_rvecLookAt = ovecTarget-ovecEye;
        m_uvecRight = m_rvecLookAt.CrossProduct(rvecUp).UnitVector();
        m_uvecUp = m_uvecRight.CrossProduct(m_rvecLookAt).UnitVector();  // align the up vector to the camera
    }

    void SetCallback(PROGRESSCB lpfnProgress)
    {
        m_lpfnProgress = lpfnProgress;
    }

    CHit3d HitTest(const CRay3d& Ray, const float nDistanceMin, const float nDistanceMax, const bool bSingleHit = false, const unsigned int uDepth = 0)
    {
        CHit3d ResultHit;

        for(size_t uIdx = 0; uIdx<m_aobjObjects.size(); uIdx++)
        {
            const CObject& objObject = *m_aobjObjects[uIdx];
            const CHit3d Hit = objObject.HitTest(Ray, nDistanceMin, nDistanceMax);

            if(Hit && (!ResultHit || ResultHit.GetDistance()>Hit.GetDistance()))
            {
                if(bSingleHit)
                {
                    return Hit;
                }

                ResultHit = Hit;
            }
        }

        if(ResultHit)
        {
            CColor clrLight(0, 0, 0);  // shadow
            const CVector3d ovecHitPoint = Ray.GetPointAt(ResultHit.GetDistance());

            for(size_t uIdx = 0; uIdx<m_aobjLights.size(); uIdx++)
            {
                const CLight& Light = *m_aobjLights[uIdx];

                const CVector3d rvecDirLight = Light.GetOrigin()-ovecHitPoint;
                const CRay3d ShadowRay(ovecHitPoint, rvecDirLight.UnitVector());
                const CHit3d ShadowHit = HitTest(ShadowRay, 0.001f, rvecDirLight.Magnitude(), true);

                if(!ShadowHit)
                {
                    clrLight = clrLight+(Light.GetColor()*(1.0f-(min(rvecDirLight.Magnitude(), Light.GetIntensity())/Light.GetIntensity())));
                }
            }

            if(uDepth>0U)
            {
                const CHit3d SecondaryHit = HitTest(CRay3d(ovecHitPoint, CPlane3d(ovecHitPoint, ResultHit.GetSurfaceNormal()).Reflection(Ray.GetLookAt()).UnitVector()), 0.001f, FLD_MAX, false, uDepth-1U);

                if(SecondaryHit)
                {
                    clrLight = clrLight+SecondaryHit.GetColor()/4.0f;
                }
            }

            ResultHit = CHit3d(ResultHit.GetDistance(), clrLight-~ResultHit.GetColor(), ResultHit.GetSurfaceNormal());
        }

        return ResultHit;
    }

    void Render(void* const lpContext = NULL)
    {
        CCamera Cam(m_ovecEye, m_ovecTarget, m_uvecUp, CCamera::TYPE_PERSPECTIVE, 45.0f, m_nSceneWidth/*m_nRenderWidth*/, m_nSceneHeight/*m_nRenderHeight*/);

        // initialize pixel buffer
        m_aclrPixels.assign(m_nSceneWidth*m_nSceneHeight, CColor(0.0f, 0.0f, 0.0f));

        for(int nY = 0; nY<m_nSceneHeight; nY++)
        {
            for(int nX = 0; nX<m_nSceneWidth; nX++)
            {
                const CColor clrPixel =
                    HitTest(Cam.GetRay(nX, nY), 0.0f, FLD_MAX, false, m_uBounceDepth).GetColor()  // TODO: average more of these for anti-aliasing
                    ;

                m_aclrPixels[m_nSceneWidth*nY+nX] = clrPixel;

                if(m_lpfnProgress!=NULL && !m_lpfnProgress(clrPixel.R(), clrPixel.G(), clrPixel.B(), nX, nY, lpContext))
                {
                    break;
                }
            }
        }
    }

    const PIXELVECTOR& GetResult() const
    {
        return m_aclrPixels;
    }

    int GetSceneWidth() const
    {
        return m_nSceneWidth;
    }

    int GetSceneHeight() const
    {
        return m_nSceneHeight;
    }

    static CTriangle CreateTriangle(const CVector3d& ovecA, const CVector3d& ovecB, const CVector3d& ovecC, const CColor& clrColor)
    {
        return CTriangle(ovecA, ovecB, ovecC, clrColor);
    }

    static CPlane CreatePlane(const CVector3d& ovecOrigin, const CVector3d& rvecNormal, const CColor& clrColor)
    {
        return CPlane(ovecOrigin, rvecNormal, clrColor);
    }

    static CSphere CreateSphere(const CVector3d& ovecOrigin, const float nRadius, const CColor& clrColor)
    {
        return CSphere(ovecOrigin, nRadius, clrColor);
    }

    static CLight CreateLight(const CVector3d& ovecOrigin, const CColor& clrColor, const float nIntensity)
    {
        return CLight(ovecOrigin, clrColor, nIntensity);
    }

    static CVector3d CreateVector3d(const float nX, const float nY, const float nZ)
    {
        return CVector3d(nX, nY, nZ);
    }

    static CMatrix3d CreateMatrix3d()
    {
        return CMatrix3d();
    }

    static CColor CreateColor(const float nR, const float nG, const float nB)
    {
        return CColor(nR, nG, nB);
    }
};

const float Raytracer::PI = acos(0.0f)*2.0f;

#endif  /* _SNIPPETS_RAYTRACER_HPP_ */
