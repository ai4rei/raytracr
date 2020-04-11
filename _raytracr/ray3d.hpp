class CRay3d
{
private:
    const CVector3d m_ovecEye;
    const CVector3d m_uvecLookAt;

public:
    CRay3d(const CVector3d& ovecEye, const CVector3d& uvecLookAt)
        : m_ovecEye(ovecEye)
        , m_uvecLookAt(uvecLookAt)
    {
    }

    const CVector3d& GetEye() const
    {
        return m_ovecEye;
    }

    const CVector3d& GetLookAt() const
    {
        return m_uvecLookAt;
    }

    /*o*/CVector3d GetPointAt(const float nDistance) const
    {
        return m_ovecEye+m_uvecLookAt*nDistance;
    }
};
