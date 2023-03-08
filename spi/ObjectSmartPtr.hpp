/*

    Sartorial Programming Interface (SPI) runtime libraries
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA

*/

#ifndef SPI_OBJECT_SMART_PTR_HPP
#define SPI_OBJECT_SMART_PTR_HPP

#include "RefCounter.hpp"
#include <spi_boost/intrusive_ptr.hpp>

#undef SPI_DECLARE_OBJECT_CLASS
#define SPI_DECLARE_OBJECT_CLASS(T) \
class T;\
typedef spi::ObjectSmartPtr<T> T##ConstSP

SPI_BEGIN_NAMESPACE

class Object;

// ObjectSmartPtr is spi_boost::intrusive_ptr plus some extra constructors
// and cast operators.
//
// In addition ObjectSmartPtr is assumed to consume either newly allocated
// pointers (non-const) but only serves up const pointers.
//
// Construction from a const pointer requires that the reference count is
// non-zero.
//
// The goal is to allow automatic coercion when there is an available
// Coerce method for the class.
//
// we use ObjectSmartPtr in place of spi_boost::intrusive_ptr for sub-classes
// of Object - not for Object itself
//
// A lot of the details of this class are copied directly from the definition
// of boost::intrusive_ptr - including all macros etc

template<class T> class ObjectSmartPtr
{
private:

    typedef ObjectSmartPtr this_type;

public:

    typedef const T element_type;

    SPI_BOOST_CONSTEXPR ObjectSmartPtr() SPI_BOOST_SP_NOEXCEPT : px( 0 )
    {
    }

    ObjectSmartPtr(T * p) : px(p)
    {
        if (px != 0) intrusive_ptr_add_ref(px);
    }

    ObjectSmartPtr(const T * p) : px(p)
    {
        if (px != 0)
        {
            if (px->getRefCount() <= 0)
                throw std::runtime_error("Attempt to share non-allocated pointer.");
            intrusive_ptr_add_ref(px);
        }
    }

#if !defined(SPI_BOOST_NO_MEMBER_TEMPLATES) || defined(SPI_BOOST_MSVC6_MEMBER_TEMPLATES)

    template<class U>
#if !defined( SPI_BOOST_SP_NO_SP_CONVERTIBLE )

    ObjectSmartPtr( ObjectSmartPtr<U> const & rhs, typename spi_boost::detail::sp_enable_if_convertible<U,T>::type = spi_boost::detail::sp_empty() )

#else

    ObjectSmartPtr( ObjectSmartPtr<U> const & rhs )

#endif
    : px( rhs.get() )
    {
        if( px != 0 ) intrusive_ptr_add_ref( px );
    }

#endif

    ObjectSmartPtr(ObjectSmartPtr const & rhs): px( rhs.px )
    {
        if( px != 0 ) intrusive_ptr_add_ref( px );
    }

    ~ObjectSmartPtr()
    {
        if( px != 0 ) intrusive_ptr_release( px );
    }

#if !defined(SPI_BOOST_NO_MEMBER_TEMPLATES) || defined(SPI_BOOST_MSVC6_MEMBER_TEMPLATES)

    template<class U> ObjectSmartPtr & operator=(ObjectSmartPtr<U> const & rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }

#endif

// Move support

#if !defined( SPI_BOOST_NO_CXX11_RVALUE_REFERENCES )

    ObjectSmartPtr(ObjectSmartPtr && rhs) SPI_BOOST_SP_NOEXCEPT : px( rhs.px )
    {
        rhs.px = 0;
    }

    ObjectSmartPtr & operator=(ObjectSmartPtr && rhs) SPI_BOOST_SP_NOEXCEPT
    {
        this_type( static_cast< ObjectSmartPtr && >( rhs ) ).swap(*this);
        return *this;
    }

    template<class U> friend class ObjectSmartPtr;

    template<class U>
#if !defined( SPI_BOOST_SP_NO_SP_CONVERTIBLE )

    ObjectSmartPtr(ObjectSmartPtr<U> && rhs, typename spi_boost::detail::sp_enable_if_convertible<U,T>::type = spi_boost::detail::sp_empty())

#else

    ObjectSmartPtr(ObjectSmartPtr<U> && rhs)

#endif
    : px( rhs.px )
    {
        rhs.px = 0;
    }

    template<class U>
    ObjectSmartPtr & operator=(ObjectSmartPtr<U> && rhs) SPI_BOOST_SP_NOEXCEPT
    {
        this_type( static_cast< ObjectSmartPtr<U> && >( rhs ) ).swap(*this);
        return *this;
    }

#endif

    ObjectSmartPtr & operator=(ObjectSmartPtr const & rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    ObjectSmartPtr & operator=(T * rhs)
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    void reset()
    {
        this_type().swap( *this );
    }

    void reset( T * rhs )
    {
        this_type( rhs ).swap( *this );
    }

    const T * get() const SPI_BOOST_SP_NOEXCEPT
    {
        return px;
    }

    const T & operator*() const SPI_BOOST_SP_NOEXCEPT_WITH_ASSERT
    {
        SPI_BOOST_ASSERT( px != 0 );
        return *px;
    }

    const T * operator->() const SPI_BOOST_SP_NOEXCEPT_WITH_ASSERT
    {
        SPI_BOOST_ASSERT( px != 0 );
        return px;
    }

// implicit conversion to "bool"
#include <spi_boost/smart_ptr/detail/operator_bool.hpp>

    void swap(ObjectSmartPtr & rhs) SPI_BOOST_SP_NOEXCEPT
    {
        const T * tmp = px;
        px = rhs.px;
        rhs.px = tmp;
    }

    // this is the extra code for ObjectSmartPtr above and beyond the
    // code that we copied from intrusive_ptr
    template<typename U>
    ObjectSmartPtr(const U& rhs) : px(0)
    {
        ObjectSmartPtr tmp = T::Coerce(rhs);
        swap(tmp);
    }

#ifndef SPI_BOOST_NO_CXX11_DELETED_FUNCTIONS
    ObjectSmartPtr(const spi_boost::intrusive_ptr<T>& rhs) = delete;
    ObjectSmartPtr(const spi_boost::intrusive_ptr<T const>& rhs) = delete;
#endif

    operator spi_boost::intrusive_ptr<const Object>() const
    {
        return spi_boost::intrusive_ptr<const Object>(px);
    }

    // end of extra code for ObjectSmartPtr

private:

    const T * px;
};

template<class T, class U> inline bool operator==(ObjectSmartPtr<T> const & a, ObjectSmartPtr<U> const & b) SPI_BOOST_SP_NOEXCEPT
{
    return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(ObjectSmartPtr<T> const & a, ObjectSmartPtr<U> const & b) SPI_BOOST_SP_NOEXCEPT
{
    return a.get() != b.get();
}

template<class T, class U> inline bool operator==(ObjectSmartPtr<T> const & a, U * b) SPI_BOOST_SP_NOEXCEPT
{
    return a.get() == b;
}

template<class T, class U> inline bool operator!=(ObjectSmartPtr<T> const & a, U * b) SPI_BOOST_SP_NOEXCEPT
{
    return a.get() != b;
}

template<class T, class U> inline bool operator==(T * a, ObjectSmartPtr<U> const & b) SPI_BOOST_SP_NOEXCEPT
{
    return a == b.get();
}

template<class T, class U> inline bool operator!=(T * a, ObjectSmartPtr<U> const & b) SPI_BOOST_SP_NOEXCEPT
{
    return a != b.get();
}

#if __GNUC__ == 2 && __GNUC_MINOR__ <= 96

// Resolve the ambiguity between our op!= and the one in rel_ops

template<class T> inline bool operator!=(ObjectSmartPtr<T> const & a, ObjectSmartPtr<T> const & b) SPI_BOOST_SP_NOEXCEPT
{
    return a.get() != b.get();
}

#endif

#if !defined( SPI_BOOST_NO_CXX11_NULLPTR )

template<class T> inline bool operator==( ObjectSmartPtr<T> const & p, spi_boost::detail::sp_nullptr_t ) SPI_BOOST_SP_NOEXCEPT
{
    return p.get() == 0;
}

template<class T> inline bool operator==( spi_boost::detail::sp_nullptr_t, ObjectSmartPtr<T> const & p ) SPI_BOOST_SP_NOEXCEPT
{
    return p.get() == 0;
}

template<class T> inline bool operator!=( ObjectSmartPtr<T> const & p, spi_boost::detail::sp_nullptr_t ) SPI_BOOST_SP_NOEXCEPT
{
    return p.get() != 0;
}

template<class T> inline bool operator!=( spi_boost::detail::sp_nullptr_t, ObjectSmartPtr<T> const & p ) SPI_BOOST_SP_NOEXCEPT
{
    return p.get() != 0;
}

#endif

template<class T> inline bool operator<(ObjectSmartPtr<T> const & a, ObjectSmartPtr<T> const & b) SPI_BOOST_SP_NOEXCEPT
{
    return std::less<T *>()(a.get(), b.get());
}

template<class T> void swap(ObjectSmartPtr<T> & lhs, ObjectSmartPtr<T> & rhs) SPI_BOOST_SP_NOEXCEPT
{
    lhs.swap(rhs);
}

// mem_fn support

template<class T> T * get_pointer(ObjectSmartPtr<T> const & p) SPI_BOOST_SP_NOEXCEPT
{
    return p.get();
}

template<class T, class U> ObjectSmartPtr<T> static_pointer_cast(ObjectSmartPtr<U> const & p)
{
    return static_cast<T *>(p.get());
}

template<class T, class U> ObjectSmartPtr<T> const_pointer_cast(ObjectSmartPtr<U> const & p)
{
    return const_cast<T *>(p.get());
}

template<class T, class U> ObjectSmartPtr<T> dynamic_pointer_cast(ObjectSmartPtr<U> const & p)
{
    return dynamic_cast<T *>(p.get());
}

// operator<<

#if !defined(SPI_BOOST_NO_IOSTREAM)

#if defined(SPI_BOOST_NO_TEMPLATED_IOSTREAMS) || ( defined(__GNUC__) &&  (__GNUC__ < 3) )

template<class Y> std::ostream & operator<< (std::ostream & os, ObjectSmartPtr<Y> const & p)
{
    os << p.get();
    return os;
}

#else

// in STLport's no-iostreams mode no iostream symbols can be used
#ifndef _STLP_NO_IOSTREAMS

template<class E, class T, class Y> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, ObjectSmartPtr<Y> const & p)
{
    os << p.get();
    return os;
}

#endif // _STLP_NO_IOSTREAMS

#endif // __GNUC__ < 3

#endif // !defined(SPI_BOOST_NO_IOSTREAM)

// hash_value

template< class T > struct hash;

template< class T > std::size_t hash_value( spi::ObjectSmartPtr<T> const & p ) SPI_BOOST_SP_NOEXCEPT
{
    return spi_boost::hash< T* >()( p.get() );
}

SPI_END_NAMESPACE

#endif


