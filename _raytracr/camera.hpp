class CCamera
{
public:
    enum TYPE
    {
        TYPE_PERSPECTIVE,
    };

private:
    const CVector3d m_ovecEye;
    const CVector3d m_ovecTarget;
    const CVector3d m_rvecLookAt;
    const CVector3d m_uvecRight;
    const CVector3d m_uvecUp;
    const CMatrix3d m_mtxW2C;
    const float m_nFOV;
    const float m_nFOVRatio;
    const float m_nFOVHeight;
    const float m_nFOVWidth;
    const CVector3d m_rvecFOV0;
    const CVector3d m_rvecFOVW;
    const CVector3d m_rvecFOVH;
    const CVector3d m_rvecFOVX;
    const CVector3d m_rvecFOVY;
    const CVector3d m_rvecLookAt0;
    const TYPE m_nType;
    const int m_nRenderWidth;
    const int m_nRenderHeight;

public:
    CCamera(const CVector3d& ovecEye, const CVector3d& ovecTarget, const CVector3d& rvecUp, const TYPE nType, const float nFOV, const int nRenderWidth, const int nRenderHeight)
        : m_ovecEye(ovecEye)
        , m_ovecTarget(ovecTarget)
        , m_rvecLookAt(ovecTarget-ovecEye)
        , m_uvecRight(m_rvecLookAt.CrossProduct(rvecUp).UnitVector())
        , m_uvecUp(m_uvecRight.CrossProduct(m_rvecLookAt).UnitVector())  // align the up vector to the camera
        , m_mtxW2C(CMatrix3d().SetFromUpVector(m_uvecUp, m_uvecRight))  // world-to-camera coordinates transformation matrix
        , m_nFOV(nFOV)
        , m_nFOVRatio(static_cast< float >(nRenderWidth)/static_cast< float >(nRenderHeight))
        , m_nFOVHeight(tan(PI/180.0f*nFOV))
        , m_nFOVWidth(m_nFOVHeight*m_nFOVRatio)
        , m_rvecFOV0(CVector3d(-m_nFOVWidth/2.0f, -m_nFOVHeight/2.0f, 0.0f).MatrixProduct(m_mtxW2C))
        , m_rvecFOVW(CVector3d(+m_nFOVWidth/2.0f, -m_nFOVHeight/2.0f, 0.0f).MatrixProduct(m_mtxW2C))
        , m_rvecFOVH(CVector3d(-m_nFOVWidth/2.0f, +m_nFOVHeight/2.0f, 0.0f).MatrixProduct(m_mtxW2C))
        , m_rvecFOVX((m_rvecFOVW-m_rvecFOV0).UnitVector())
        , m_rvecFOVY((m_rvecFOVH-m_rvecFOV0).UnitVector())
        , m_rvecLookAt0(m_rvecLookAt+m_rvecFOV0)
        , m_nType(nType)
        , m_nRenderWidth(nRenderWidth)
        , m_nRenderHeight(nRenderHeight)
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
    CRay3d GetRay(const int nRenderX, const int nRenderY, const float nSubX = 0.0f, const float nSubY = 0.0f)
    {
        const CVector3d rvecX = m_rvecFOVX*(m_nFOVWidth*(nRenderX+nSubX/2.0f+0.5f)/static_cast< float >(m_nRenderWidth));
        const CVector3d rvecY = m_rvecFOVY*(m_nFOVHeight*(nRenderY+nSubY/2.0f+0.5f)/static_cast< float >(m_nRenderHeight));

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
            m_ovecEye+m_rvecFOV0+rvecXY;
            ;

        return CRay3d(ovecEye, uvecRay);
    }
};
