#ifndef SNIPPETS_RAYTRACER_HPP
#define SNIPPETS_RAYTRACER_HPP

class Raytracer
{
public:
    #include "_raytracr/color.hpp"

protected:
    #include "_raytracr/matrix.hpp"
    #include "_raytracr/vector3d.hpp"
    #include "_raytracr/matrix3d.hpp"
    #include "_raytracr/plane3d.hpp"
    #include "_raytracr/sphere3d.hpp"
    #include "_raytracr/ray3d.hpp"
    #include "_raytracr/hit3d.hpp"
    #include "_raytracr/object.hpp"
    #include "_raytracr/triangle.hpp"
    #include "_raytracr/plane.hpp"
    #include "_raytracr/sphere.hpp"
    #include "_raytracr/light.hpp"  // diffuse, ambient, spot light
    #include "_raytracr/camera.hpp"
    #include "_raytracr/camera2.hpp"

    typedef CCamera CCameraType;

protected:
    typedef std::vector< std::shared_ptr< CObject > > OBJECTVECTOR;
    typedef std::vector< std::shared_ptr< CLight > > LIGHTVECTOR;
    typedef std::vector< CColor > PIXELVECTOR;
    typedef bool (*PROGRESSCB)(const float nR, const float nG, const float nB, const int nX, const int nY, void* lpContext);

protected:
    static const float PI;
#ifndef FLD_MAX
    #define FLD_MAX 1e37f
#endif  /* FLD_MAX */

protected:
    int m_nImageW;
    int m_nImageH;
    unsigned int m_uBounceDepth;
    unsigned int m_uAntiAliasing;
    CVector3d m_ovecEye;
    CVector3d m_rvecLookAt;
    CVector3d m_rvecUp;
    PIXELVECTOR m_aclrPixels;
    OBJECTVECTOR m_aobjObjects;
    LIGHTVECTOR m_aobjLights;
    PROGRESSCB m_lpfnProgress;

public:
    Raytracer()
        : m_nImageW(0)
        , m_nImageH(0)
        , m_uBounceDepth(0)
        , m_uAntiAliasing(0)
        , m_ovecEye(0.0f, 0.0f, 0.0f)
        , m_rvecLookAt(0.0f, 0.0f, 0.0f)
        , m_rvecUp(0.0f, 0.0f, 0.0f)
        , m_lpfnProgress(nullptr)
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

    void SetImageSize(const int nImageW, const int nImageH)
    {
        m_nImageW = nImageW;
        m_nImageH = nImageH;
    }

    void SetBounceDepth(const unsigned int uBounceDepth)
    {//HACK: Should be defined by the object material's reflectivity
        m_uBounceDepth = uBounceDepth;
    }

    void SetAntiAliasing(const unsigned int uLevel)
    {
        m_uAntiAliasing = uLevel;
    }

    void SetCamera(const CVector3d& ovecEye, const CVector3d& rvecLookAt, const CVector3d& rvecUp)
    {
        m_ovecEye = ovecEye;
        m_rvecLookAt = rvecLookAt;
        m_rvecUp = rvecUp;
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

    CColor RenderColorAt(const CCameraType& Cam, const int nImageX, const int nImageY)
    {
        CColor clrHit(0, 0, 0);

        if(m_uAntiAliasing)
        {
            const float nPoints = m_uAntiAliasing+1.0f;
            const float nInc = 2.0f/float(m_uAntiAliasing);
            float nT = -1.0f;

            for(unsigned int uIdx = 0; uIdx<=m_uAntiAliasing; uIdx++)
            {
                const float nSubX = cos((nPoints)*nT);
                const float nSubY = sin((nPoints-1.0f)*nT);

                clrHit+= HitTest(Cam.GetRay(nImageX, nImageY, nSubX, nSubY), 0.0f, FLD_MAX, false, m_uBounceDepth).GetColor();

                nT+= nInc;
            }

            clrHit/= nPoints;
        }
        else
        {
            clrHit = HitTest(Cam.GetRay(nImageX, nImageY), 0.0f, FLD_MAX, false, m_uBounceDepth).GetColor();
        }

        return clrHit;
    }

    void Render(void* const lpContext = nullptr)
    {
        CCameraType Cam(m_ovecEye, m_rvecLookAt, m_rvecUp, CCameraType::TYPE_PERSPECTIVE, 45.0f, m_nImageW, m_nImageH);

        // initialize pixel buffer
        m_aclrPixels.assign(m_nImageW*m_nImageH, CColor(0.0f, 0.0f, 0.0f));

        for(int nImageY = 0; nImageY<m_nImageH; nImageY++)
        {
            for(int nImageX = 0; nImageX<m_nImageW; nImageX++)
            {
                const CColor clrPixel = RenderColorAt(Cam, nImageX, nImageY);

                m_aclrPixels[m_nImageW*nImageY+nImageX] = clrPixel;

                if(m_lpfnProgress!=nullptr && !m_lpfnProgress(clrPixel.R(), clrPixel.G(), clrPixel.B(), nImageX, nImageY, lpContext))
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

    int GetImageWidth() const
    {
        return m_nImageW;
    }

    int GetImageHeight() const
    {
        return m_nImageH;
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

#endif  /* SNIPPETS_RAYTRACER_HPP */
