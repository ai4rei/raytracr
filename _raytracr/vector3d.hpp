#define PrintVector(VAR) printf("%s: %+2.3f,%+2.3f,%+2.3f\n", #VAR, VAR.X(), VAR.Y(), VAR.Z())

class CMatrix3d;

// ovec = ortsvektor
// rvec = richtungsvektor
// uvec = einheitsvektor
//  vec = unbestimmter vektor
// Xi+Yj+Zk=0
//
class CVector3d
{
private:
    typedef CVector3d CSelf;

private:
    float m_nX;
    float m_nY;
    float m_nZ;
    float m_nMagnitude;

public:
    CVector3d(const float nX, const float nY, const float nZ)
        : m_nX(nX)
        , m_nY(nY)
        , m_nZ(nZ)
        , m_nMagnitude(-1.0f)
    {
    }

    CSelf& operator=(const CSelf& vecOther)
    {
        if(*this!=vecOther)
        {
            m_nX = vecOther.m_nX;
            m_nY = vecOther.m_nY;
            m_nZ = vecOther.m_nZ;

            // invalidate cache
            m_nMagnitude = -1.0f;
        }

        return *this;
    }

    bool operator==(const CSelf& vecOther) const
    {
        return m_nX==vecOther.m_nX
            && m_nY==vecOther.m_nY
            && m_nZ==vecOther.m_nZ;
    }

    bool operator!=(const CSelf& vecOther) const
    {
        return !(*this==vecOther);
    }

    CSelf operator+(const CSelf& vecOther) const
    {
        return CSelf(m_nX+vecOther.m_nX, m_nY+vecOther.m_nY, m_nZ+vecOther.m_nZ);
    }

    CSelf operator-(const CSelf& vecOther) const
    {
        return CSelf(m_nX-vecOther.m_nX, m_nY-vecOther.m_nY, m_nZ-vecOther.m_nZ);
    }

    CSelf operator*(const float nScale) const
    {
        return CSelf(m_nX*nScale, m_nY*nScale, m_nZ*nScale);
    }

    CSelf operator*(const CSelf& vecOther) const
    {// a°b (Hadamard/Schur Product)
        return CSelf(m_nX*vecOther.m_nX, m_nY*vecOther.m_nY, m_nZ*vecOther.m_nZ);
    }

    CSelf operator/(const float nScale) const
    {
        return CSelf(m_nX/nScale, m_nY/nScale, m_nZ/nScale);
    }

    CSelf operator-() const
    {
        return CSelf(-m_nX, -m_nY, -m_nZ);
    }

    CSelf UnitVector() const
    {
        return *this/Magnitude();
    }

    /*
            Y
          b |
            |   a
            o----X
       a×b /
          Z
    */
    CSelf CrossProduct(const CSelf& vecOther) const
    {// a×b
        return CSelf
        (
            m_nY*vecOther.m_nZ-m_nZ*vecOther.m_nY,
            m_nZ*vecOther.m_nX-m_nX*vecOther.m_nZ,
            m_nX*vecOther.m_nY-m_nY*vecOther.m_nX
        );
    }

    /*

        | A11 A12 A13 |   | B11 |   | A11*B11 + A12*B21 + A13*B31 |
        | A21 A22 A23 | × | B21 | = | A21*B11 + A22*B21 + A23*B31 |
        | A31 A32 A33 |   | B31 |   | A31*B11 + A32*B21 + A33*B31 |

        |  X1  X2  X3 |   |  X  |   |  X1* X  +  X2* Y  +  X3* Z  |
        |  Y1  Y2  Y3 | × |  Y  | = |  Y1* X  +  Y2* Y  +  Y3* Z  |
        |  Z1  Z2  Z3 |   |  Z  |   |  Z1* X  +  Z2* Y  +  Z3* Z  |

    */
    CSelf MatrixProduct(const CMatrix3d& mtxOther) const
    {
        return CSelf
        (
            m_nX*mtxOther.X1()+m_nY*mtxOther.X2()+m_nZ*mtxOther.X3()+1.0f*mtxOther.X4(),
            m_nX*mtxOther.Y1()+m_nY*mtxOther.Y2()+m_nZ*mtxOther.Y3()+1.0f*mtxOther.Y4(),
            m_nX*mtxOther.Z1()+m_nY*mtxOther.Z2()+m_nZ*mtxOther.Z3()+1.0f*mtxOther.Z4()
        );
    }

    /*
        cos(theta between a and b) = a.b
    */
    float DotProduct(const CSelf& vecOther) const
    {// a.b
        return m_nX*vecOther.m_nX+m_nY*vecOther.m_nY+m_nZ*vecOther.m_nZ;
    }

    float Magnitude2() const
    {
        return DotProduct(*this);
    }

    float Magnitude() const
    {
        if(m_nMagnitude<0)
        {
            CSelf* lpThis = const_cast< CSelf* >(this);

            lpThis->m_nMagnitude = sqrt(Magnitude2());
        }

        return m_nMagnitude;
    }

    bool IsPointInVector(const CVector3d& ovecPoint, const CSelf& ovecOrigin) const
    {
        return UnitVector()==(ovecPoint-ovecOrigin).UnitVector();
    }

    bool IsParallelTo(const CSelf& vecOther) const
    {
        return UnitVector()==vecOther.UnitVector() || UnitVector()==-vecOther.UnitVector();
    }

    float AngleTo(const CSelf& vecOther) const
    {
        return acos(UnitVector().DotProduct(vecOther.UnitVector()));
    }

    CVector3d ProjectionTo(const CSelf& vecOther) const
    {// https://en.wikipedia.org/wiki/Vector_projection
        //return DotProduct(vecOther.UnitVector())*vecOther.UnitVector();
        return vecOther*(DotProduct(vecOther)/vecOther.Magnitude2());
    }
    
    CVector3d RejectionTo(const CSelf& vecOther) const
    {
        return *this-ProjectionTo(vecOther);
    }

    float X() const { return m_nX; }
    float Y() const { return m_nY; }
    float Z() const { return m_nZ; }

    static int _SelfTest()
    {
        const CSelf vecA(+1.0f, +0.0f, +0.0f);
        const CSelf vecB(+0.0f, +1.0f, +0.0f);
        const CSelf vecC(+0.0f, +0.0f, +1.0f);

        if(vecA.CrossProduct(vecB)!=vecC || vecB.CrossProduct(vecC)!=vecA || vecC.CrossProduct(vecA)!=vecB)
        {
            return 1;
        }

        if(vecA.DotProduct(vecB)!=0 || vecB.DotProduct(vecC)!=0 || vecC.DotProduct(vecA)!=0)
        {
            return 2;
        }

        return 0;
    }
};
