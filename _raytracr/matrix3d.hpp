#define PrintMatrix(VAR) printf("%s:\n| %+2.3f,%+2.3f,%+2.3f |\n| %+2.3f,%+2.3f,%+2.3f |\n| %+2.3f,%+2.3f,%+2.3f |\n", #VAR, VAR.X1(), VAR.X2(), VAR.X3(), VAR.Y1(), VAR.Y2(), VAR.Y3(), VAR.Z1(), VAR.Z2(), VAR.Z3())

class CMatrix3d
    : public CBaseSquareMatrix< CMatrix3d, float, 3 >
{
private:
    typedef CMatrix3d CSelf;

private:
    #define m_nX1 (*this)(0, 0)
    #define m_nX2 (*this)(0, 1)
    #define m_nX3 (*this)(0, 2)
    #define m_nY1 (*this)(1, 0)
    #define m_nY2 (*this)(1, 1)
    #define m_nY3 (*this)(1, 2)
    #define m_nZ1 (*this)(2, 0)
    #define m_nZ2 (*this)(2, 1)
    #define m_nZ3 (*this)(2, 2)

public:
    CMatrix3d()
    {
    }

    CMatrix3d(const float nX1, const float nX2, const float nX3, const float nY1, const float nY2, const float nY3, const float nZ1, const float nZ2, const float nZ3)
    {
        m_nX1 = nX1; m_nX2 = nX2; m_nX3 = nX3;
        m_nY1 = nY1; m_nY2 = nY2; m_nY3 = nY3;
        m_nZ1 = nZ1; m_nZ2 = nZ2; m_nZ3 = nZ3;
    }

    CSelf& SetFromUpVector(const CVector3d& uvecUp, const CVector3d& uvecRight)
    {
        const CVector3d uvecBack = uvecUp.CrossProduct(uvecRight).UnitVector();

        m_nX1 = uvecRight.X(); m_nX2 = uvecUp.X(); m_nX3 = uvecBack.X();
        m_nY1 = uvecRight.Y(); m_nY2 = uvecUp.Y(); m_nY3 = uvecBack.Y();
        m_nZ1 = uvecRight.Z(); m_nZ2 = uvecUp.Z(); m_nZ3 = uvecBack.Z();

        return *this;
    }

    CSelf& AddXTranslation(const float nOffset)
    {
        return *this;
    }

    CSelf& AddYTranslation(const float nOffset)
    {
        return *this;
    }

    CSelf& AddZTranslation(const float nOffset)
    {
        return *this;
    }

    CSelf& AddXRotation(const float nRad)
    {
        const CSelf mtxRoll
        (
            +1.0, +0.0,       +0.0,
            +0.0, +cos(nRad), -sin(nRad),
            +0.0, +sin(nRad), +cos(nRad)
        );

        *this = *this*mtxRoll;

        return *this;
    }

    CSelf& AddYRotation(const float nRad)
    {
        const CSelf mtxPitch
        (
            +cos(nRad), +0.0, +sin(nRad),
            +0.0,       +1.0, +0.0,
            -sin(nRad), +0.0, +cos(nRad)
        );

        *this = *this*mtxPitch;

        return *this;
    }

    CSelf& AddZRotation(const float nRad)
    {
        const CSelf mtxYaw
        (
            +cos(nRad), -sin(nRad), +0.0,
            +sin(nRad), +cos(nRad), +0.0,
            +0.0,       +0.0,       +1.0
        );

        *this = *this*mtxYaw;

        return *this;
    }

    float X1() const { return m_nX1; }
    float X2() const { return m_nX2; }
    float X3() const { return m_nX3; }
    float Y1() const { return m_nY1; }
    float Y2() const { return m_nY2; }
    float Y3() const { return m_nY3; }
    float Z1() const { return m_nZ1; }
    float Z2() const { return m_nZ2; }
    float Z3() const { return m_nZ3; }

#undef m_nX1
#undef m_nX2
#undef m_nX3
#undef m_nY1
#undef m_nY2
#undef m_nY3
#undef m_nZ1
#undef m_nZ2
#undef m_nZ3
};
