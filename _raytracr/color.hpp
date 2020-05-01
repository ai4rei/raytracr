#ifndef SNIPPETS_RAYTRACER_COLOR_HPP
#define SNIPPETS_RAYTRACER_COLOR_HPP

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
        return CColor(m_nR+clrOther.m_nR, m_nG+clrOther.m_nG, m_nB+clrOther.m_nB);
    }

    CColor operator-(const CColor& clrOther) const
    {
        return CColor(m_nR-clrOther.m_nR, m_nG-clrOther.m_nG, m_nB-clrOther.m_nB);
    }

    CColor operator*(const float nScale) const
    {
        return CColor(m_nR*nScale, m_nG*nScale, m_nB*nScale);
    }

    CColor operator*(const CColor& clrOther) const
    {
        return CColor(m_nR*clrOther.m_nR, m_nG*clrOther.m_nG, m_nB*clrOther.m_nB);
    }

    CColor operator/(const float nScale) const
    {
        return CColor(m_nR/nScale, m_nG/nScale, m_nB/nScale);
    }

    CColor operator~() const
    {
        return CColor(1.0f-m_nR, 1.0f-m_nG, 1.0f-m_nB);
    }

    float R() const { return max(0.0f, min(1.0f, m_nR)); }
    float G() const { return max(0.0f, min(1.0f, m_nG)); }
    float B() const { return max(0.0f, min(1.0f, m_nB)); }
};

#endif  /* SNIPPETS_RAYTRACER_COLOR_HPP */
