#ifndef SNIPPETS_RAYTRACER_LIGHT_HPP
#define SNIPPETS_RAYTRACER_LIGHT_HPP

class CLight
{
private:
    const CVector3d m_ovecOrigin;
    const CColor m_clrColor;
    const float m_nAtt0;
    const float m_nAtt1;
    const float m_nAtt2;
    const float m_nRange;

public:
    CLight(const CVector3d& ovecOrigin, const CColor& clrColor, const float nRange, const float nAtt0 = 1.0f, const float nAtt1 = 0.1f, const float nAtt2 = 0.01f)
        : m_ovecOrigin(ovecOrigin)
        , m_clrColor(clrColor)
        , m_nAtt0(nAtt0)
        , m_nAtt1(nAtt1)
        , m_nAtt2(nAtt2)
        , m_nRange(nRange)
    {
    }

    const CVector3d& GetOrigin() const
    {
        return m_ovecOrigin;
    }

    const CColor& GetColor() const
    {
        return m_clrColor;
    }

    CColor GetAttenuatedColor(const float nDistance) const
    {
        if(nDistance<m_nRange)
        {
            const float nAtt = m_nAtt0+m_nAtt1*nDistance+m_nAtt2*nDistance*nDistance;

            return m_clrColor/nAtt;
        }

        return CColor(0, 0, 0);
    }

    float GetRange() const
    {
        return m_nRange;
    }
};

#endif  /* SNIPPETS_RAYTRACER_LIGHT_HPP */
