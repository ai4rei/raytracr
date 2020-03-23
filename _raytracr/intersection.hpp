class CIntersection
{
public:
    typedef std::vector< float > HITVECTOR;

private:
    HITVECTOR m_anHits;

public:
    CIntersection()
    {
    }

    CIntersection(const float nOne)
    {
        m_anHits.push_back(nOne);
    }

    CIntersection(const float nOne, const float nTwo)
    {
        m_anHits.push_back(nOne);
        m_anHits.push_back(nTwo);
    }

    bool IsEmpty() const
    {
        return m_anHits.empty();
    }

    const HITVECTOR& Hits() const
    {
        return m_anHits;
    }
};
