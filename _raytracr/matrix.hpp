#ifndef SNIPPETS_RAYTRACER_MATRIX_HPP
#define SNIPPETS_RAYTRACER_MATRIX_HPP

template< typename T, unsigned int M, unsigned int N > class CMatrix;

template< typename D, typename T, unsigned int M, unsigned int N > class CBaseMatrix
{
private:
    //typedef CMatrix< T, M, N > CSelf;
    typedef D CSelf;

protected:
    T m_aItem[M][N];

public:
    CBaseMatrix()
    {
        SetZero();
    }

    inline T& operator()(unsigned int I, unsigned int J = 0)
    {
        return m_aItem[I][J];
    }

    inline const T& operator()(unsigned int I, unsigned int J = 0) const
    {
        return m_aItem[I][J];
    }

    CSelf& SetZero()
    {
        for(unsigned int J = 0; J < N; J++)
        {
            for(unsigned int I = 0; I < M; I++)
            {
                SetItem(I, J, 0);
            }
        }

        return *static_cast< CSelf* >(this);
    }

    inline CSelf& SetItem(const unsigned int I, T nValue)
    {
        operator()(I) = nValue;

        return *static_cast< CSelf* >(this);
    }

    inline CSelf& SetItem(const unsigned int I, const unsigned int J, T nValue)
    {
        operator()(I, J) = nValue;

        return *static_cast< CSelf* >(this);
    }

    CSelf operator+(const CSelf& mtxOther) const
    {
        CSelf mtxResult;

        for(unsigned int J = 0; J < N; J++)
        {
            for(unsigned int I = 0; I < M; I++)
            {
                mtxResult(I, J) = operator()(I, J)+mtxOther(I, J);
            }
        }

        return mtxResult;
    }

    CSelf operator-(const CSelf& mtxOther) const
    {
        CSelf mtxResult;

        for(unsigned int J = 0; J < N; J++)
        {
            for(unsigned int I = 0; I < M; I++)
            {
                mtxResult(I, J) = operator()(I, J)-mtxOther(I, J);
            }
        }

        return mtxResult;
    }

    CSelf operator*(const T& nScalar) const
    {
        CSelf mtxResult;

        for(unsigned int J = 0; J < N; J++)
        {
            for(unsigned int I = 0; I < M; I++)
            {
                mtxResult(I, J) = operator()(I, J)*nScalar;
            }
        }

        return mtxResult;
    }

    CSelf operator/(const T& nScalar) const
    {
        CSelf mtxResult;

        for(unsigned int J = 0; J < N; J++)
        {
            for(unsigned int I = 0; I < M; I++)
            {
                mtxResult(I, J) = operator()(I, J)/nScalar;
            }
        }

        return mtxResult;
    }

    CSelf operator-() const
    {
        CSelf mtxResult;

        for(unsigned int J = 0; J < N; J++)
        {
            for(unsigned int I = 0; I < M; I++)
            {
                mtxResult(I, J) = -operator()(I, J);
            }
        }

        return mtxResult;
    }

    CMatrix< T, N, M > Transpose() const
    {
        CMatrix< T, N, M > mtxResult;

        for(unsigned int J = 0; J < N; J++)
        {
            for(unsigned int I = 0; I < M; I++)
            {
                mtxResult(I, J) = operator()(J, I);
            }
        }

        return mtxResult;
    }

    template< unsigned int P > CMatrix< T, M, P > operator*(const CMatrix< T, N, P >& mtxOther) const
    {
        CMatrix< T, M, P > mtxResult;

        for(unsigned int J = 0; J < P; J++)
        {
            for(unsigned int I = 0; I < M; I++)
            {
                for(unsigned int R = 0; R < N; R++)
                {
                    mtxResult(I, J)+= operator()(I, R)*mtxOther(R, J);
                }
            }
        }

        return mtxResult;
    }
};

template< typename T, unsigned int M, unsigned int N > class CMatrix
    : public CBaseMatrix< CMatrix< T, M, N >, T, M, N >
{
};

template< typename D, typename T, unsigned int N > class CBaseSquareMatrix
    : public CBaseMatrix< D, T, N, N >
{
private:
    //typedef CSquareMatrix< T, N > CSelf;
    typedef D CSelf;
    typedef CBaseMatrix< D, T, N, N > CSuper;

public:
    CSelf& SetIdentity()
    {
        for(unsigned int J = 0; J < N; J++)
        {
            CSuper::SetItem(J, J, 1);
        }

        return *static_cast< CSelf* >(this);
    }

    CSelf operator*(const CSelf& mtxOther) const
    {
        CSelf mtxResult;

        for(unsigned int J = 0; J < N; J++)
        {
            for(unsigned int I = 0; I < N; I++)
            {
                for(unsigned int R = 0; R < N; R++)
                {
                    mtxResult(I, J)+= CSuper::operator()(I, R)*mtxOther(R, J);
                }
            }
        }

        return mtxResult;
    }
};

template< typename T, unsigned int N > class CSquareMatrix
    : public CBaseSquareMatrix< CSquareMatrix< T, N >, T, N >
{
};

#endif  /* SNIPPETS_RAYTRACER_MATRIX_HPP */
