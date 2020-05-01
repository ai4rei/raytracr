#ifndef SNIPPETS_RAYTRACER_LIGHT_HPP
#define SNIPPETS_RAYTRACER_LIGHT_HPP

class CLight
{
private:
    const CVector3d m_ovecOrigin;
    const CColor m_clrColor;
    const float m_nIntensity;

public:
    CLight(const CVector3d& ovecOrigin, const CColor& clrColor, const float nIntensity)
        : m_ovecOrigin(ovecOrigin)
        , m_clrColor(clrColor)
        , m_nIntensity(nIntensity)
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

    float GetIntensity() const
    {
        return m_nIntensity;
    }
};

#endif  /* SNIPPETS_RAYTRACER_LIGHT_HPP */
