//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_IMPL_STORAGE_IPP
#define BOOST_BEAST_JSON_IMPL_STORAGE_IPP

#include <boost/beast/_experimental/json/storage.hpp>
#include <boost/beast/_experimental/json/detail/storage_adaptor.hpp>
#include <boost/core/exchange.hpp>
#include <boost/assert.hpp>
#include <memory>

namespace boost {
namespace beast {
namespace json {

storage_ptr::
~storage_ptr()
{
    if(p_)
        p_->release();
}

storage_ptr::
storage_ptr(storage_ptr&& other) noexcept
    : p_(boost::exchange(other.p_, nullptr))
{
}

storage_ptr::
storage_ptr(storage_ptr const& other) noexcept
    : p_(other.p_)
{
    if(p_)
        p_->addref();
}

storage_ptr&
storage_ptr::
operator=(storage_ptr&& other) noexcept
{
    BOOST_ASSERT(this != &other);
    if(p_)
        p_->release();
    p_ = boost::exchange(other.p_, nullptr);
    return *this;
}

storage_ptr&
storage_ptr::
operator=(storage_ptr const& other) noexcept
{
    if(other.p_)
        other.p_->addref();
    if(p_)
        p_->release();
    p_ = other.p_;
    return *this;
}

storage_ptr::
storage_ptr(storage* p) noexcept
    : p_(p)
{
}

storage*
storage_ptr::
release() noexcept
{
    return boost::exchange(p_, nullptr);
}

//------------------------------------------------------------------------------

namespace detail {

inline
storage_ptr&
raw_default_storage() noexcept
{
    static storage_ptr sp =
        make_storage_ptr(
            std::allocator<void>{});
    return sp;
}

} // detail

storage_ptr
default_storage()
{
    return detail::raw_default_storage();
}

void
default_storage(storage_ptr sp)
{
    detail::raw_default_storage() = std::move(sp);
}


} // json
} // beast
} // boost

#endif
