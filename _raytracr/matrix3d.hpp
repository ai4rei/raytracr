#ifndef SNIPPETS_RAYTRACER_MATRIX3D_HPP
#define SNIPPETS_RAYTRACER_MATRIX3D_HPP

#define PrintMatrix(VAR) printf("%s:\n| %+2.3f,%+2.3f,%+2.3f,%+2.3f |\n| %+2.3f,%+2.3f,%+2.3f,%+2.3f |\n| %+2.3f,%+2.3f,%+2.3f,%+2.3f |\n", #VAR, VAR.X1(), VAR.X2(), VAR.X3(), VAR.X4(), VAR.Y1(), VAR.Y2(), VAR.Y3(), VAR.Y4(), VAR.Z1(), VAR.Z2(), VAR.Z3(), VAR.Z4())

class CMatrix3d
    : public CBaseSquareMatrix< CMatrix3d, float, 4 >
{
private:
    typedef CMatrix3d CSelf;

public:
    static const int ROW_X = 0;
    static const int ROW_Y = 1;
    static const int ROW_Z = 2;
    static const int ROW_W = 3;

public:
    CMatrix3d()
    {
    }

    CMatrix3d(const float nX1, const float nX2, const float nX3, const float nY1, const float nY2, const float nY3, const float nZ1, const float nZ2, const float nZ3)
    {
        // SetZero() is implied during construction
        SetIdentity();

        SetItem(ROW_X, 0, nX1); SetItem(ROW_X, 1, nX2); SetItem(ROW_X, 2, nX3);
        SetItem(ROW_Y, 0, nY1); SetItem(ROW_Y, 1, nY2); SetItem(ROW_Y, 2, nY3);
        SetItem(ROW_Z, 0, nZ1); SetItem(ROW_Z, 1, nZ2); SetItem(ROW_Z, 2, nZ3);
    }

    CSelf& SetFromUpVector(const CVector3d& uvecUp, const CVector3d& uvecRight)
    {
        const CVector3d uvecBack = uvecUp.CrossProduct(uvecRight).UnitVector();

        SetZero();
        SetIdentity();

        SetItem(ROW_X, 0, uvecRight.X()); SetItem(ROW_X, 1, uvecUp.X()); SetItem(ROW_X, 2, uvecBack.X());
        SetItem(ROW_Y, 0, uvecRight.Y()); SetItem(ROW_Y, 1, uvecUp.Y()); SetItem(ROW_Y, 2, uvecBack.Y());
        SetItem(ROW_Z, 0, uvecRight.Z()); SetItem(ROW_Z, 1, uvecUp.Z()); SetItem(ROW_Z, 2, uvecBack.Z());

        return *this;
    }

    CSelf& AddXTranslation(const float nOffset)
    {
        const CSelf mtxOffset = CSelf().SetIdentity()
            .SetItem(ROW_X, 3, nOffset)
            ;

        *this = *this*mtxOffset;

        return *this;
    }

    CSelf& AddYTranslation(const float nOffset)
    {
        const CSelf mtxOffset = CSelf().SetIdentity()
            .SetItem(ROW_Y, 3, nOffset)
            ;

        *this = *this*mtxOffset;

        return *this;
    }

    CSelf& AddZTranslation(const float nOffset)
    {
        const CSelf mtxOffset = CSelf().SetIdentity()
            .SetItem(ROW_Z, 3, nOffset)
            ;

        *this = *this*mtxOffset;

        return *this;
    }

    CSelf& AddXRotation(const float nRad)
    {
        const CSelf mtxRoll = CSelf().SetIdentity()
            .SetItem(ROW_Y, 1, +cos(nRad)).SetItem(ROW_Y, 2, -sin(nRad))
            .SetItem(ROW_Z, 1, +sin(nRad)).SetItem(ROW_Z, 2, +cos(nRad))
            ;

        *this = *this*mtxRoll;

        return *this;
    }

    CSelf& AddYRotation(const float nRad)
    {
        const CSelf mtxPitch = CSelf().SetIdentity()
            .SetItem(ROW_X, 0, +cos(nRad)).SetItem(ROW_X, 2, +sin(nRad))
            .SetItem(ROW_Z, 0, -sin(nRad)).SetItem(ROW_Z, 2, +cos(nRad))
            ;

        *this = *this*mtxPitch;

        return *this;
    }

    CSelf& AddZRotation(const float nRad)
    {
        const CSelf mtxYaw = CSelf().SetIdentity()
            .SetItem(ROW_X, 0, +cos(nRad)).SetItem(ROW_X, 1, -sin(nRad))
            .SetItem(ROW_Y, 0, +sin(nRad)).SetItem(ROW_Y, 1, +cos(nRad))
            ;

        *this = *this*mtxYaw;

        return *this;
    }

    float X1() const { return operator()(ROW_X, 0); }
    float X2() const { return operator()(ROW_X, 1); }
    float X3() const { return operator()(ROW_X, 2); }
    float X4() const { return operator()(ROW_X, 3); }
    float Y1() const { return operator()(ROW_Y, 0); }
    float Y2() const { return operator()(ROW_Y, 1); }
    float Y3() const { return operator()(ROW_Y, 2); }
    float Y4() const { return operator()(ROW_Y, 3); }
    float Z1() const { return operator()(ROW_Z, 0); }
    float Z2() const { return operator()(ROW_Z, 1); }
    float Z3() const { return operator()(ROW_Z, 2); }
    float Z4() const { return operator()(ROW_Z, 3); }
};

#endif  /* SNIPPETS_RAYTRACER_MATRIX3D_HPP */
