#ifndef _SNIPPETS_RAYTRACER_HPP_
#define _SNIPPETS_RAYTRACER_HPP_

class Raytracer
{
public:
    #include "_raytracr/vector3d.hpp"
    #include "_raytracr/matrix3d.hpp"
    #include "_raytracr/color.hpp"
    #include "_raytracr/intersection.hpp"
    #include "_raytracr/object.hpp"
    #include "_raytracr/triangle.hpp"
    #include "_raytracr/plane.hpp"
    #include "_raytracr/sphere.hpp"
    #include "_raytracr/light.hpp"
    #include "_raytracr/tracehit.hpp"

    typedef std::vector< std::unique_ptr< CObject > > OBJECTVECTOR;
    typedef std::vector< CColor > PIXELVECTOR;

    int m_nSceneWidth;
    int m_nSceneHeight;
    CVector3d m_ovecEye;
    CVector3d m_ovecTarget;
    CVector3d m_rvecLookAt;
    CVector3d m_uvecRight;
    CVector3d m_uvecUp;
    PIXELVECTOR m_aclrPixels;
    OBJECTVECTOR m_aobjObjects;

    Raytracer()
        : m_nSceneWidth(0)
        , m_nSceneHeight(0)
        , m_ovecEye(0.0f, 0.0f, 0.0f)
        , m_ovecTarget(0.0f, 0.0f, 0.0f)
        , m_rvecLookAt(0.0f, 0.0f, 0.0f)
        , m_uvecRight(0.0f, 0.0f, 0.0f)
        , m_uvecUp(0.0f, 0.0f, 0.0f)
    {
    }

    template< typename T > void AddObject(const T& objObject)
    {
        m_aobjObjects.emplace_back(new T(objObject));
    }

    void SetScene(const int nWidth, const int nHeight)
    {
        m_nSceneWidth = nWidth;
        m_nSceneHeight = nHeight;
    }

    void SetCamera(const CVector3d& ovecEye, const CVector3d& ovecTarget, const CVector3d& rvecUp)
    {
        m_ovecEye = ovecEye;
        m_ovecTarget = ovecTarget;
        m_rvecLookAt = ovecTarget-ovecEye;
        m_uvecRight = m_rvecLookAt.CrossProduct(rvecUp).UnitVector();
        m_uvecUp = m_uvecRight.CrossProduct(m_rvecLookAt).UnitVector();  // align the up vector to the camera
    }

    void ForEachObject(bool (Raytracer::*Each)(const std::unique_ptr< CObject >& objObject, void* lpContext), void* lpContext)
    {
        for(auto itObj = m_aobjObjects.cbegin(); itObj!=m_aobjObjects.cend(); itObj++)
        {
            const auto& objObject = *itObj;

            if(!(this->*Each)(objObject, lpContext))
            {
                break;
            }
        }
    }

    void ForEachLight(bool (Raytracer::*Each)(const std::unique_ptr< CObject >& objObject, void* lpContext), void* lpContext)
    {
        for(auto itObj = m_aobjObjects.cbegin(); itObj!=m_aobjObjects.cend(); itObj++)
        {
            const auto& objObject = *itObj;

            if(!objObject->IsLight())
            {
                continue;
            }

            if(!(this->*Each)(objObject, lpContext))
            {
                break;
            }
        }
    }

    bool TracePrimaryRayEach(const std::unique_ptr< CObject >& objObject, void* lpContext)
    {
        CTraceHit* const lpTh = static_cast< CTraceHit* >(lpContext);
        const auto Is = objObject->Intersect(lpTh->Eye(), lpTh->Ray());

        if(!Is.IsEmpty())
        {
            for(auto itHit = Is.Hits().cbegin(); itHit!=Is.Hits().cend(); itHit++)
            {
                const float nD = *itHit;

                if(lpTh->Dhit()>nD && nD>=lpTh->Dmin())
                {
                    lpTh->SetD(nD);
                    lpTh->SetObject(objObject->GetObject());
                }
            }
        }

        return true;
    }

    CTraceHit TracePrimaryRay(const CVector3d& ovecEye, const CVector3d& uvecRay, const float nDmin = 0.0f)
    {
        CTraceHit Th(ovecEye, uvecRay, nDmin);

        ForEachObject(&Raytracer::TracePrimaryRayEach, &Th);

        return Th;
    }

    bool TraceShadowRayEach(const std::unique_ptr< CObject >& objObject, void* lpContext)
    {
        if(objObject->IsLight())
        {// lights do not affect shadow rays
            return true;
        }

        CTraceHit* const lpTh = static_cast< CTraceHit* >(lpContext);

        if(objObject->GetObject()==lpTh->HitObject())
        {// skip the object that was hit by the last ray
            return true;
        }

        const auto Is = objObject->Intersect(lpTh->Eye(), lpTh->Ray());

        if(!Is.IsEmpty())
        {
            for(auto itHit = Is.Hits().cbegin(); itHit!=Is.Hits().cend(); itHit++)
            {
                const float nD = *itHit;

                if(lpTh->Dhit()>nD && nD>=lpTh->Dmin() && nD<=lpTh->Dmax() /* overshoot beyond the light */)
                {
                    lpTh->SetD(nD);
                    lpTh->SetObject(objObject->GetObject());
                    return false;
                }
            }
        }

        return true;
    }

    CTraceHit TraceShadowRay(const CVector3d& ovecEye, const CVector3d& uvecRay, const float nDmax, const CObject* lpobjWhatToSkip)
    {
        CTraceHit Th(ovecEye, uvecRay, 0.0f, nDmax);

        Th.SetObject(lpobjWhatToSkip);

        ForEachObject(&Raytracer::TraceShadowRayEach, &Th);

        return Th;
    }

    void Render(bool (* Progress)(unsigned int uDone, unsigned int uTotal, void* lpContext), void* lpContext)
    {
#define PERSPECTIVE
        const float PI = acos(0.0f)*2.0f;

        // initialize pixel buffer
        m_aclrPixels.assign(m_nSceneWidth*m_nSceneHeight, CColor(0.0f, 0.0f, 0.0f));

        // world-to-camera transformation matrix
        const CMatrix3d mtxW2C = CMatrix3d().SetFromUpVector(m_uvecUp, m_uvecRight);

        // image plane setup
        const float nImageSize =
#ifdef PERSPECTIVE
            tan(PI/180.0f*45.0f)
#else  /* !PERSPECTIVE = ORTHOGRAPHIC */
            20.0f
#endif  /* !PERSPECTIVE */
            ;
        const float nImageRatio = static_cast< float >(m_nSceneWidth)/static_cast< float >(m_nSceneHeight);
        const float nImageHeight = nImageSize;
        const float nImageWidth = nImageSize*nImageRatio;
        const CVector3d rvecImage0 = CVector3d(-nImageWidth/2.0f, -nImageHeight/2.0f, 0.0f).MatrixProduct(mtxW2C);
        const CVector3d rvecImageW = CVector3d(+nImageWidth/2.0f, -nImageHeight/2.0f, 0.0f).MatrixProduct(mtxW2C);
        const CVector3d rvecImageH = CVector3d(-nImageWidth/2.0f, +nImageHeight/2.0f, 0.0f).MatrixProduct(mtxW2C);
        const CVector3d rvecImageX = (rvecImageW-rvecImage0).UnitVector();
        const CVector3d rvecImageY = (rvecImageH-rvecImage0).UnitVector();
        const CVector3d rvecLookAt0 = m_rvecLookAt+rvecImage0;

        // process each pixel
        for(int nY = 0; nY<m_nSceneHeight; nY++)
        {
            const float nImageY = nImageSize*(nY+0.5f)/static_cast< float >(m_nSceneHeight);
            const CVector3d rvecY = rvecImageY*nImageY;

            for(int nX = 0; nX<m_nSceneWidth; nX++)
            {
                const float nImageX = nImageSize*(nX+0.5f)/static_cast< float >(m_nSceneWidth);
                const CVector3d rvecX = rvecImageX*nImageX;

                // point on image plane
                const CVector3d rvecXY = rvecX+rvecY;

                // ray
                const CVector3d rvecRay =
#ifdef PERSPECTIVE
                    rvecLookAt0+rvecXY
#else  /* !PERSPECTIVE = ORTHOGRAPHIC */
                    m_rvecLookAt
#endif  /* !PERSPECTIVE */
                    ;
                const CVector3d uvecRay = rvecRay.UnitVector();

                // eye
                const CVector3d ovecEye =
#ifdef PERSPECTIVE
                    m_ovecEye
#else  /* !PERSPECTIVE = ORTHOGRAPHIC */
                    m_ovecEye+rvecImage0+rvecXY;
#endif  /* !PERSPECTIVE */
                    ;

                // primary ray
                CTraceHit Th = TracePrimaryRay(ovecEye, uvecRay, rvecRay.Magnitude());

                // colors
                CColor clrObject(0, 0, 0) /* none */, clrLight(0, 0, 0) /* shadow */;

                if(Th.HitObject()==NULL)
                {// nothing was hit
                    ;
                }
                else
                if(Th.HitObject()->IsLight())
                {// light does not interfere with shadow rays
                    clrObject = Th.HitObject()->GetColor();

                    clrLight = CColor(1, 1, 1);
                }
                else
                {
                    clrObject = Th.HitObject()->GetColor();

                    // shadow ray
                    const CVector3d ovecShadowEye = ovecEye+uvecRay*Th.Dhit();

                    for(auto itLight = m_aobjObjects.cbegin(); itLight!=m_aobjObjects.cend(); itLight++)
                    {
                        const auto& objLight = *itLight;

                        if(!objLight->IsLight())
                        {
                            continue;
                        }

                        const CVector3d rvecShadowRay = objLight->Origin()-ovecShadowEye;
                        const CVector3d uvecShadowRay = rvecShadowRay.UnitVector();

                        CTraceHit Ths = TraceShadowRay(ovecShadowEye, uvecShadowRay, rvecShadowRay.Magnitude(), Th.HitObject());

                        if(Ths.HitObject()==Th.HitObject())
                        {
                            clrLight = clrLight+(objLight->GetColor()*(1.0f-(min(rvecShadowRay.Magnitude(), objLight->Intensity())/objLight->Intensity())));
                        }
                    }
                }

                m_aclrPixels[m_nSceneWidth*nY+nX] = clrLight-~clrObject;
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

#endif  /* _SNIPPETS_RAYTRACER_HPP_ */
