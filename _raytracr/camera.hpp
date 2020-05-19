#ifndef SNIPPETS_RAYTRACER_CAMERA_HPP
#define SNIPPETS_RAYTRACER_CAMERA_HPP

class CCamera
{
public:
    enum TYPE
    {
        TYPE_PERSPECTIVE,
    };

private:
    const CVector3d m_ovecEye;
    const CVector3d m_rvecLookAt;
    const CVector3d m_uvecRight;
    const CVector3d m_uvecUp;
    const float m_nFovH;
    const float m_nFovW;
    const float m_nFovXUnit;
    const float m_nFovYUnit;
    const CVector3d m_rvecLookAt0;
    const TYPE m_nType;

public:
    CCamera(const CVector3d& ovecEye, const CVector3d& rvecLookAt, const CVector3d& rvecUp, const TYPE nType, const float nFOV, const int nImageW, const int nImageH)
        : m_ovecEye(ovecEye)
        , m_rvecLookAt(rvecLookAt)
        , m_uvecRight(rvecLookAt.CrossProduct(rvecUp).UnitVector())
        , m_uvecUp(m_uvecRight.CrossProduct(rvecLookAt).UnitVector())  // align the up vector to the camera
        , m_nFovH(tan(PI*nFOV/360.0f)*2.0f*rvecLookAt.Magnitude())
        , m_nFovW(m_nFovH*float(nImageW)/float(nImageH))
        , m_nFovXUnit(m_nFovW/float(nImageW))
        , m_nFovYUnit(m_nFovH/float(nImageH))
        , m_rvecLookAt0(rvecLookAt+m_uvecRight*(-m_nFovW/2.0f)+m_uvecUp*(-m_nFovH/2.0f))
        , m_nType(nType)
    {
    }

    /**
        @brief  Retrieves a ray for a given point in camera's FOV.
        @param  nRenderX
        @param  nRenderY
        @param  nSubX
                Sub-point X-offset in the range of [-1;+1].
        @param  nSubY
                Sub-point Y-offset in the range of [-1;+1].
    */
    CRay3d GetRay(const int nImageX, const int nImageY, const float nSubX = 0.0f, const float nSubY = 0.0f) const
    {
        const CVector3d rvecX = m_uvecRight*(m_nFovXUnit*(nImageX+nSubX/2.0f+0.5f));
        const CVector3d rvecY = m_uvecUp*(m_nFovYUnit*(nImageY+nSubY/2.0f+0.5f));

        // point on image plane
        const CVector3d rvecXY = rvecX+rvecY;

        // ray
        const CVector3d rvecRay = m_rvecLookAt0+rvecXY;
        const CVector3d uvecRay = rvecRay.UnitVector();

        return CRay3d(m_ovecEye, uvecRay);
    }
};

#endif  /* SNIPPETS_RAYTRACER_CAMERA_HPP */
