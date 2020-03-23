class CTraceHit
{
private:
    const CObject* m_objHit;
    const CVector3d m_ovecEye;
    const CVector3d m_rvecRay;
    const float m_nDmin;
    const float m_nDmax;
    float m_nDhit;

public:
    CTraceHit(const CVector3d& ovecEye, const CVector3d& rvecRay, const float nDmin = 0.0f, const float nDmax = FLT_MAX)
        : m_ovecEye(ovecEye)
        , m_rvecRay(rvecRay)
        , m_nDmin(nDmin)
        , m_nDmax(nDmax)
        , m_objHit(NULL)
        , m_nDhit(nDmax)
    {
    }

    const CVector3d& Eye() const
    {
        return m_ovecEye;
    }

    const CVector3d& Ray() const
    {
        return m_rvecRay;
    }

    float Dmin() const
    {
        return m_nDmin;
    }

    float Dmax() const
    {
        return m_nDmax;
    }

    float Dhit() const
    {
        return m_nDhit;
    }

    const CObject* HitObject() const
    {
        return m_objHit;
    }

    void SetObject(const CObject* objHit)
    {
        m_objHit = objHit;
    }

    void SetD(const float nD)
    {
        m_nDhit = nD;
    }
};
