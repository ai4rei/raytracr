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

    inline T& operator()(unsigned int i, unsigned int j)
    {
        return m_aItem[i][j];
    }

    inline const T& operator()(unsigned int i, unsigned int j) const
    {
        return m_aItem[i][j];
    }

    CSelf& SetZero()
    {
        for(unsigned int j = 0; j < N; j++)
        {
            for(unsigned int i = 0; i < M; i++)
            {
                SetItem(i, j, 0);
            }
        }

        return *static_cast< CSelf* >(this);
    }

    inline CSelf& SetItem(const unsigned int i, const unsigned int j, T nValue)
    {
        operator()(i, j) = nValue;

        return *static_cast< CSelf* >(this);
    }

    CSelf operator+(const CSelf& mtxOther) const
    {
        CSelf mtxResult;

        for(unsigned int j = 0; j < N; j++)
        {
            for(unsigned int i = 0; i < M; i++)
            {
                mtxResult(i, j) = operator()(i, j)+mtxOther(i, j);
            }
        }

        return mtxResult;
    }

    CSelf operator-(const CSelf& mtxOther) const
    {
        CSelf mtxResult;

        for(unsigned int j = 0; j < N; j++)
        {
            for(unsigned int i = 0; i < M; i++)
            {
                mtxResult(i, j) = operator()(i, j)-mtxOther(i, j);
            }
        }

        return mtxResult;
    }

    CSelf operator*(const T& nScalar) const
    {
        CSelf mtxResult;

        for(unsigned int j = 0; j < N; j++)
        {
            for(unsigned int i = 0; i < M; i++)
            {
                mtxResult(i, j) = operator()(i, j)*nScalar;
            }
        }

        return mtxResult;
    }

    CSelf operator/(const T& nScalar) const
    {
        CSelf mtxResult;

        for(unsigned int j = 0; j < N; j++)
        {
            for(unsigned int i = 0; i < M; i++)
            {
                mtxResult(i, j) = operator()(i, j)/nScalar;
            }
        }

        return mtxResult;
    }

    CMatrix< T, N, M > Transpose() const
    {
        CMatrix< T, N, M > mtxResult;

        for(unsigned int j = 0; j < N; j++)
        {
            for(unsigned int i = 0; i < M; i++)
            {
                mtxResult(i, j) = operator()(j, i);
            }
        }

        return mtxResult;
    }

    template< unsigned int P > CMatrix< T, M, P > operator*(const CMatrix< T, N, P >& mtxOther) const
    {
        CMatrix< T, M, P > mtxResult;

        for(unsigned int j = 0; j < P; j++)
        {
            for(unsigned int i = 0; i < M; i++)
            {
                for(unsigned int r = 0; r < N; r++)
                {
                    mtxResult(i, j)+= operator()(i, r)*mtxOther(r, j);
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

public:
    CSelf& SetIdentity()
    {
        for(unsigned int j = 0; j < N; j++)
        {
            SetItem(j, j, 1);
        }

        return *static_cast< CSelf* >(this);
    }

    CSelf operator*(const CSelf& mtxOther) const
    {
        CSelf mtxResult;

        for(unsigned int j = 0; j < N; j++)
        {
            for(unsigned int i = 0; i < N; i++)
            {
                for(unsigned int r = 0; r < N; r++)
                {
                    mtxResult(i, j)+= operator()(i, r)*mtxOther(r, j);
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
