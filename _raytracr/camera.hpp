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
    const CMatrix3d m_mtxW2C;
    const float m_nFOVHeight;
    const float m_nFOVWidth;
    const float m_nFovXUnit;
    const float m_nFovYUnit;
    const CVector3d m_rvecFOV0;
    const CVector3d m_rvecLookAt0;
    const TYPE m_nType;

public:
    CCamera(const CVector3d& ovecEye, const CVector3d& rvecLookAt, const CVector3d& rvecUp, const TYPE nType, const float nFOV, const int nImageW, const int nImageH)
        : m_ovecEye(ovecEye)
        , m_rvecLookAt(rvecLookAt)
        , m_uvecRight(rvecLookAt.CrossProduct(rvecUp).UnitVector())
        , m_uvecUp(m_uvecRight.CrossProduct(rvecLookAt).UnitVector())  // align the up vector to the camera
        , m_mtxW2C(CMatrix3d().SetFromUpVector(m_uvecUp, m_uvecRight))  // world-to-camera coordinates transformation matrix
        , m_nFOVHeight(tan(PI/180.0f*nFOV))
        , m_nFOVWidth(m_nFOVHeight*float(nImageW)/float(nImageH))
        , m_nFovXUnit(m_nFOVWidth/float(nImageW))
        , m_nFovYUnit(m_nFOVHeight/float(nImageH))
        , m_rvecFOV0(CVector3d(-m_nFOVWidth/2.0f, -m_nFOVHeight/2.0f, 0.0f).MatrixProduct(m_mtxW2C))
        , m_rvecLookAt0(rvecLookAt+m_rvecFOV0)
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
        const CVector3d rvecRay =
            m_rvecLookAt0+rvecXY
            //m_rvecLookAt
            ;
        const CVector3d uvecRay = rvecRay.UnitVector();

        // eye
        const CVector3d ovecEye =
            //m_ovecEye
            m_ovecEye+m_rvecLookAt0+rvecXY
            ;

        return CRay3d(ovecEye, uvecRay);
    }
};

#endif  /* SNIPPETS_RAYTRACER_CAMERA_HPP */
