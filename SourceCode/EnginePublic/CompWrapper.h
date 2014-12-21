#ifndef BEYOND_ENGINE_COMPWRAPPER_H__INCLUDE
#define BEYOND_ENGINE_COMPWRAPPER_H__INCLUDE

template <typename BaseType, typename OwnerType>
class CCompWrapper : public BaseType
{
public:
    CCompWrapper(OwnerType *owner)
        : m_pOwner(owner)
    {
    }

    virtual ~CCompWrapper()
    {
    }

    OwnerType *Owner() const
    {
        return m_pOwner;
    }

private:
    OwnerType *m_pOwner;
};
#endif
