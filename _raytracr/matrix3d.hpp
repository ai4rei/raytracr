#define PrintMatrix(VAR) printf("%s:\n| %+2.3f,%+2.3f,%+2.3f |\n| %+2.3f,%+2.3f,%+2.3f |\n| %+2.3f,%+2.3f,%+2.3f |\n", #VAR, VAR.X1(), VAR.X2(), VAR.X3(), VAR.Y1(), VAR.Y2(), VAR.Y3(), VAR.Z1(), VAR.Z2(), VAR.Z3())

class CMatrix3d
{
private:
    typedef CMatrix3d CSelf;

private:
    float m_nX1;
    float m_nX2;
    float m_nX3;
    float m_nY1;
    float m_nY2;
    float m_nY3;
    float m_nZ1;
    float m_nZ2;
    float m_nZ3;

public:
    CMatrix3d()
        : m_nX1(0.0f), m_nX2(0.0f), m_nX3(0.0f)
        , m_nY1(0.0f), m_nY2(0.0f), m_nY3(0.0f)
        , m_nZ1(0.0f), m_nZ2(0.0f), m_nZ3(0.0f)
    {
    }

    CMatrix3d(const float nX1, const float nX2, const float nX3, const float nY1, const float nY2, const float nY3, const float nZ1, const float nZ2, const float nZ3)
        : m_nX1(nX1), m_nX2(nX2), m_nX3(nX3)
        , m_nY1(nY1), m_nY2(nY2), m_nY3(nY3)
        , m_nZ1(nZ1), m_nZ2(nZ2), m_nZ3(nZ3)
    {
    }

    CSelf operator*(const CSelf& mtxOther)
    {
        return CSelf
        (
            m_nX1*mtxOther.m_nX1+m_nX2*mtxOther.m_nY1+m_nX3*mtxOther.m_nZ1, m_nX1*mtxOther.m_nX2+m_nX2*mtxOther.m_nY2+m_nX3*mtxOther.m_nZ2, m_nX1*mtxOther.m_nX3+m_nX2*mtxOther.m_nY3+m_nX3*mtxOther.m_nZ3,
            m_nY1*mtxOther.m_nX1+m_nY2*mtxOther.m_nY1+m_nY3*mtxOther.m_nZ1, m_nY1*mtxOther.m_nX2+m_nY2*mtxOther.m_nY2+m_nY3*mtxOther.m_nZ2, m_nY1*mtxOther.m_nX3+m_nY2*mtxOther.m_nY3+m_nY3*mtxOther.m_nZ3,
            m_nZ1*mtxOther.m_nX1+m_nZ2*mtxOther.m_nY1+m_nZ3*mtxOther.m_nZ1, m_nZ1*mtxOther.m_nX2+m_nZ2*mtxOther.m_nY2+m_nZ3*mtxOther.m_nZ2, m_nZ1*mtxOther.m_nX3+m_nZ2*mtxOther.m_nY3+m_nZ3*mtxOther.m_nZ3
        );
    }

    CSelf& SetIdentity()
    {
        m_nX1 = +1.0; m_nX2 = +0.0; m_nX3 = +0.0;
        m_nY1 = +0.0; m_nY2 = +1.0; m_nY3 = +0.0;
        m_nZ1 = +0.0; m_nZ2 = +0.0; m_nZ3 = +1.0;

        return *this;
    }

    CSelf& SetFromUpVector(const CVector3d& uvecUp, const CVector3d& uvecRight)
    {
        const CVector3d uvecBack = uvecUp.CrossProduct(uvecRight).UnitVector();

        m_nX1 = uvecRight.X(); m_nX2 = uvecUp.X(); m_nX3 = uvecBack.X();
        m_nY1 = uvecRight.Y(); m_nY2 = uvecUp.Y(); m_nY3 = uvecBack.Y();
        m_nZ1 = uvecRight.Z(); m_nZ2 = uvecUp.Z(); m_nZ3 = uvecBack.Z();

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
};
