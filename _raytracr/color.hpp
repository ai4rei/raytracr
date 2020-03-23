class CColor
{
private:
    float m_nR;
    float m_nG;
    float m_nB;

public:
    CColor(const float nR, const float nG, const float nB)
        : m_nR(nR)
        , m_nG(nG)
        , m_nB(nB)
    {
    }

    CColor operator+(const CColor& clrOther) const
    {
        return CColor(min(1.0f, m_nR+clrOther.m_nR), min(1.0f, m_nG+clrOther.m_nG), min(1.0f, m_nB+clrOther.m_nB));
    }

    CColor operator-(const CColor& clrOther) const
    {
        return CColor(max(0.0f, m_nR-clrOther.m_nR), max(0.0f, m_nG-clrOther.m_nG), max(0.0f, m_nB-clrOther.m_nB));
    }

    CColor operator*(const float nIntensity) const
    {
        return CColor(m_nR*nIntensity, m_nG*nIntensity, m_nB*nIntensity);
    }

    CColor operator*(const CColor& clrOther) const
    {
        return CColor(m_nR*clrOther.m_nR, m_nG*clrOther.m_nG, m_nB*clrOther.m_nB);
    }

    CColor operator~() const
    {
        return CColor(1.0f-m_nR, 1.0f-m_nG, 1.0f-m_nB);
    }

    float R() const { return m_nR; }
    float G() const { return m_nG; }
    float B() const { return m_nB; }
};
