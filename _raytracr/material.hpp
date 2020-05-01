#ifndef SNIPPETS_RAYTRACER_MATERIAL_HPP
#define SNIPPETS_RAYTRACER_MATERIAL_HPP

class CMaterial
{
private:
    CColor m_clrDiffuse;
    CColor m_clrLuminescence;
    float m_nReflectivity;
    float m_nRefractiveIndex;
};

#endif  /* SNIPPETS_RAYTRACER_MATERIAL_HPP */
