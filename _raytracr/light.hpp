class CLight
    : public CSphere
{
private:

public:
    CLight(const CVector3d& ovecOrigin, const CColor& clrColor, const float nIntensity)
        : CSphere(ovecOrigin, 1.0f, clrColor, nIntensity)
    {
    }

    virtual bool IsLight() const
    {
        return true;
    }
};
