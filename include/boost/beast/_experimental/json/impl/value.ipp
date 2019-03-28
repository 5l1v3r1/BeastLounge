//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_IMPL_VALUE_IPP
#define BOOST_BEAST_JSON_IMPL_VALUE_IPP

#include <boost/beast/_experimental/json/value.hpp>
#include <new>
#include <utility>

namespace boost {
namespace beast {
namespace json {

//------------------------------------------------------------------------------
//
// Special members
//
//------------------------------------------------------------------------------

value::
~value()
{
    clear_impl();
}

value::
value(value&& other) noexcept
{
    switch(other.kind_)
    {
    case json::kind::object:
        ::new(&obj_) object(
            std::move(other.obj_));
        other.obj_.~object();
        ::new(&other.nat_.sp_)
            storage_ptr(obj_.get_storage());
        break;

    case json::kind::array:
        ::new(&arr_) array(
            std::move(other.arr_));
        other.arr_.~array();
        ::new(&other.nat_.sp_)
            storage_ptr(arr_.get_storage());
        break;

    case json::kind::string:
        ::new(&str_) string(
            std::move(other.str_));
        other.str_.~string();
        ::new(&other.nat_.sp_) storage_ptr(
            str_.get_allocator().get_storage());
        break;

    case json::kind::number:
        ::new(&nat_.sp_)
            storage_ptr(other.nat_.sp_);
        ::new(&nat_.num_) number(
            other.nat_.num_);
        other.nat_.num_.~number();
        break;

    case json::kind::boolean:
        ::new(&nat_.sp_)
            storage_ptr(other.nat_.sp_);
        nat_.bool_ = other.nat_.bool_;
        break;

    case json::kind::null:
        ::new(&nat_.sp_)
            storage_ptr(other.nat_.sp_);
        break;
    }
    kind_ = other.kind_;
    other.kind_ = json::kind::null;
}

value::
value(
    value&& other,
    storage_ptr store)
{
    move(std::move(store), std::move(other));
}

value::
value(value const& other)
    : value(other, other.get_storage())
{
}

value::
value(
    value const& other,
    storage_ptr store)
{
    copy(std::move(store), other);
}

value&
value::
operator=(value&& other)
{
    auto sp = release_storage();
    clear_impl();
    move(std::move(sp), std::move(other));
    return *this;
}

value&
value::
operator=(value const& other)
{
    if(this != &other)
    {
        auto sp = release_storage();
        clear_impl();
        copy(std::move(sp), other);
    }
    return *this;
}

//------------------------------------------------------------------------------
//
// Construction and Assignment
//
//------------------------------------------------------------------------------

value::
value() noexcept
    : value(
        json::kind::null,
        default_storage())
{
}

value::
value(storage_ptr store) noexcept
    : value(
        json::kind::null,
        std::move(store))
{
}

value::
value(json::kind k) noexcept
    : value(
        k,
        default_storage())
{
}

value::
value(
    json::kind k,
    storage_ptr store) noexcept
{
    construct(k, std::move(store));
}

value::
value(object obj) noexcept
    : obj_(std::move(obj))
    , kind_(json::kind::object)
{
}

value::
value(
    object obj,
    storage_ptr store)
    : obj_(
        std::move(obj),
        std::move(store))
    , kind_(json::kind::object)
{
}

value::
value(array arr) noexcept
    : arr_(std::move(arr))
    , kind_(json::kind::array)
{
}

value::
value(
    array arr,
    storage_ptr store)
    : arr_(std::move(arr), std::move(store))
    , kind_(json::kind::array)
{
}

value::
value(string str) noexcept
    : str_(std::move(str))
    , kind_(json::kind::string)
{
}

value::
value(
    string str,
    storage_ptr store)
    : str_(std::move(str),
        string::allocator_type(
            std::move(store)))
    , kind_(json::kind::string)
{
}

value::
value(number num)
    : value(num,
        default_storage())
{
}

value::
value(
    number num,
    storage_ptr store)
    : kind_(json::kind::number)
{
    ::new(&nat_.num_) number(num);
    ::new(&nat_.sp_) storage_ptr(
        std::move(store));
}

value::
value(std::initializer_list<
    std::pair<string_view, value>> init)
    : value(init,
        default_storage())
{
}

value::
value(std::initializer_list<
    std::pair<string_view, value>> init,
    storage_ptr store)
    : value(json::kind::object, std::move(store))
{
    for(auto& e : init)
        obj_.emplace(e.first,
            std::move(e.second));
}

#if 0
value::
value(std::initializer_list<value> init)
    : value(init,
        default_storage())
{
}

value::
value(std::initializer_list<
    std::pair<string_view, value>> init,
    storage_ptr store)
    : value(json::kind::array, std::move(store))
{
    for(auto& e : init)
        arr_.emplace_back(
            std::move(e),
            get_storage());
}
#endif

value&
value::
operator=(object obj)
{
    object tmp(
        std::move(obj),
        get_storage());
    clear_impl();
    ::new(&obj_) object(
        std::move(tmp));
    kind_ = json::kind::object;
    return *this;
}

value&
value::
operator=(array arr)
{
    array tmp(
        std::move(arr),
        get_storage());
    clear_impl();
    ::new(&arr_) array(
        std::move(tmp));
    kind_ = json::kind::array;
    return *this;
}

value&
value::
operator=(string str)
{
    string tmp(
        std::move(str),
        string::allocator_type(
            get_storage()));
    clear_impl();
    ::new(&str_) string(
        std::move(tmp));
    kind_ = json::kind::string;
    return *this;
}

//------------------------------------------------------------------------------
//
// Modifiers
//
//------------------------------------------------------------------------------

void
value::
reset(json::kind k) noexcept
{
    auto sp = release_storage();
    clear_impl();
    construct(k, std::move(sp));
}

//------------------------------------------------------------------------------
//
// Structured
//
//------------------------------------------------------------------------------

//
// Capacity
//

bool
value::
empty() const
{
    BOOST_ASSERT(this->is_structured());
    if(is_object())
        return obj_.empty();
    return arr_.empty();
}

auto
value::
size() const ->
    size_type
{
    BOOST_ASSERT(this->is_structured());
    if(is_object())
        return obj_.size();
    return arr_.size();
}

//
// Iterator
//

auto
value::
begin() ->
    iterator
{
    BOOST_ASSERT(this->is_structured());
    if(is_object())
        return obj_.begin();
    return arr_.begin();
}

auto
value::
begin() const ->
    const_iterator
{
    BOOST_ASSERT(this->is_structured());
    if(is_object())
        return obj_.begin();
    return arr_.begin();
}

auto
value::
cbegin() ->
    const_iterator
{
    BOOST_ASSERT(this->is_structured());
    if(is_object())
        return obj_.cbegin();
    return arr_.cbegin();
}

auto
value::
end() ->
    iterator
{
    BOOST_ASSERT(this->is_structured());
    if(is_object())
        return obj_.end();
    return arr_.end();
}

auto
value::
end() const ->
    const_iterator
{
    BOOST_ASSERT(this->is_structured());
    if(is_object())
        return obj_.end();
    return arr_.end();
}

auto
value::
cend() ->
    const_iterator
{
    BOOST_ASSERT(this->is_structured());
    if(is_object())
        return obj_.cend();
    return arr_.cend();
}

auto
value::
rbegin() ->
    reverse_iterator
{
    BOOST_ASSERT(this->is_structured());
    return reverse_iterator(end());
}

auto
value::
rbegin() const ->
    const_reverse_iterator
{
    BOOST_ASSERT(this->is_structured());
    return const_reverse_iterator(end());
}

auto
value::
crbegin() ->
    const_reverse_iterator
{
    BOOST_ASSERT(this->is_structured());
    return const_reverse_iterator(cend());
}

auto
value::
rend() ->
    reverse_iterator
{
    BOOST_ASSERT(this->is_structured());
    return reverse_iterator(begin());
}

auto
value::
rend() const ->
    const_reverse_iterator
{
    BOOST_ASSERT(this->is_structured());
    return const_reverse_iterator(begin());
}

auto
value::
crend() ->
    const_reverse_iterator
{
    BOOST_ASSERT(this->is_structured());
    return const_reverse_iterator(cbegin());
}

//
// Lookup
//

value&
value::
at(key_type key)
{
    BOOST_ASSERT(this->is_object());
    return obj_.at(key);
}
    
value const&
value::
at(key_type key) const
{
    BOOST_ASSERT(this->is_object());
    return obj_.at(key);
}

value&
value::
operator[](key_type key)
{
    // implicit conversion to object
    if(this->is_null())
        *this = kind::object;
    BOOST_ASSERT(this->is_object());
    return obj_[key];
}

value const&
value::
operator[](key_type key) const
{
    BOOST_ASSERT(this->is_object());
    return obj_[key];
}

auto
value::
count(key_type key) const ->
    size_type
{
    BOOST_ASSERT(this->is_object());
    return obj_.count(key);
}

auto
value::
find(key_type key) ->
    iterator
{
    BOOST_ASSERT(this->is_object());
    return obj_.find(key);
}

auto
value::
find(key_type key) const ->
    const_iterator
{
    BOOST_ASSERT(this->is_object());
    return obj_.find(key);
}

bool
value::
contains(key_type key) const
{
    BOOST_ASSERT(this->is_object());
    return obj_.contains(key);
}

//
// Elements
//

auto
value::
at(size_type pos) ->
    reference
{
    BOOST_ASSERT(is_array());
    return { "", arr_.at(pos) };
}

auto
value::
at(size_type pos) const ->
    const_reference
{
    BOOST_ASSERT(is_array());
    return { "", arr_.at(pos) };
}

value&
value::
operator[](size_type i)
{
    BOOST_ASSERT(is_array());
    return arr_[i];
}

value const&
value::
operator[](size_type i) const
{
    BOOST_ASSERT(is_array());
    return arr_[i];
}

auto
value::
front() ->
    reference
{
    BOOST_ASSERT(is_structured());
    if(is_object())
        return *obj_.begin();
    return { "", arr_.front() };
}

auto
value::
front() const ->
    const_reference
{
    BOOST_ASSERT(is_structured());
    if(is_object())
        return *obj_.begin();
    return { "", arr_.front() };
}

auto
value::
back() ->
    reference
{
    BOOST_ASSERT(is_structured());
    if(is_object())
    {
        auto it = obj_.end();
        return *--it;
    }
    return { "", arr_.back() };
}

auto
value::
back() const ->
    const_reference
{
    BOOST_ASSERT(is_structured());
    if(is_object())
    {
        auto it = obj_.end();
        return *--it;
    }
    return { "", arr_.back() };
}

//
// Modifiers
//

void
value::
clear() noexcept
{
    switch(kind_)
    {
    case json::kind::object:
        obj_.clear();
        break;
    case json::kind::array:
        arr_.clear();
        break;
    case json::kind::string:
        str_.clear();
        break;
    default:
        break;
    }
}

auto
value::
erase(key_type key) ->
    size_type
{
    BOOST_ASSERT(is_object());
    return obj_.erase(key);
}

auto
value::
erase(const_iterator pos) ->
    iterator
{
    BOOST_ASSERT(is_structured());
    if(is_object())
        return obj_.erase(pos.obj_it_);
    return arr_.erase(pos.arr_it_);
}

auto
value::
erase(
    const_iterator first,
    const_iterator last) ->
        iterator
{
    BOOST_ASSERT(is_structured());
    if(is_object())
    {
        BOOST_ASSERT(! first.arr_);
        BOOST_ASSERT(! last.arr_);
        return obj_.erase(
            first.obj_it_, last.obj_it_);
    }
    BOOST_ASSERT(first.arr_);
    BOOST_ASSERT(last.arr_);
    return arr_.erase(
        first.arr_it_, last.arr_it_);
}


void
value::
pop_back()
{
    BOOST_ASSERT(is_structured());
    if(is_object())
    {
        auto it = obj_.end();
        obj_.erase(--it);
        return;
    }
    auto it = arr_.end();
    arr_.erase(--it);
}

//------------------------------------------------------------------------------

// private

storage_ptr
value::
get_storage() const noexcept
{
    switch(kind_)
    {
    case json::kind::object:
        return obj_.get_storage();

    case json::kind::array:
        return arr_.get_storage();

    case json::kind::string:
        return str_.get_allocator().get_storage();

    default:
        break;
    }
    return nat_.sp_;
}

storage_ptr
value::
release_storage() noexcept
{
    switch(kind_)
    {
    case json::kind::object:
        return obj_.get_storage();

    case json::kind::array:
        return arr_.get_storage();

    case json::kind::string:
        return str_.get_allocator().get_storage();

    default:
        break;
    }
    return std::move(nat_.sp_);
}

void
value::
construct(
    json::kind k,
    storage_ptr sp) noexcept
{
    switch(k)
    {
    case json::kind::object:
        // requires: noexcept construction
        ::new(&obj_) object(std::move(sp));
        break;

    case json::kind::array:
        // requires: noexcept construction
        ::new(&arr_) array(std::move(sp));
        break;

    case json::kind::string:
        // requires: noexcept construction
        ::new(&str_) string(
            string::allocator_type(
                std::move(sp)));
        break;

    case json::kind::number:
        ::new(&nat_.num_) number{};
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        break;

    case json::kind::boolean:
        ::new(&nat_.bool_) bool{};
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        break;

    case json::kind::null:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        break;
    }
    kind_ = k;
}

// doesn't set kind_
void
value::
clear_impl() noexcept
{
    switch(kind_)
    {
    case json::kind::object:
        obj_.~object();
        break;

    case json::kind::array:
        arr_.~array();
        break;

    case json::kind::string:
        str_.~string();
        break;

    case json::kind::number:
        nat_.num_.~number();

    case json::kind::boolean:
    case json::kind::null:
        nat_.sp_.~storage_ptr();
        break;
    }
}

// unchecked
void
value::
move(
    storage_ptr sp,
    value&& other)
{
    switch(other.kind_)
    {
    case json::kind::object:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&obj_) object(
                std::move(other.obj_), sp);
    #ifndef BOOST_NO_EXCEPTIONS
        } 
        catch(...)
        {
            kind_ = json::kind::null;
            ::new(&nat_.sp_)
                storage_ptr(std::move(sp));
            throw;
        }
    #endif
        sp = other.get_storage();
        other.obj_.~object();
        ::new(&other.nat_.sp_)
            storage_ptr(std::move(sp));
        break;

    case json::kind::array:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&arr_) array(
                std::move(other.arr_), sp);
    #ifndef BOOST_NO_EXCEPTIONS
        } 
        catch(...)
        {
            kind_ = json::kind::null;
            ::new(&nat_.sp_)
                storage_ptr(std::move(sp));
            throw;
        }
    #endif
        sp = other.get_storage();
        other.arr_.~array();
        ::new(&other.nat_.sp_)
            storage_ptr(std::move(sp));
        break;

    case json::kind::string:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&str_) string(
                std::move(other.str_), typename
                string::allocator_type(sp));
    #ifndef BOOST_NO_EXCEPTIONS
        } 
        catch(...)
        {
            kind_ = json::kind::null;
            ::new(&nat_.sp_)
                storage_ptr(std::move(sp));
            throw;
        }
    #endif
        sp = other.get_storage();
        other.str_.~string();
        ::new(&other.nat_.sp_)
            storage_ptr(std::move(sp));
        break;

    case json::kind::number:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        ::new(&nat_.num_) number(
            other.nat_.num_);
        break;

    case json::kind::boolean:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        nat_.bool_ = other.nat_.bool_;
        break;

    case json::kind::null:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        break;
    }
    kind_ = other.kind_;
    other.kind_ = json::kind::null;
}

// unchecked
void
value::
copy(
    storage_ptr sp,
    value const& other)
{
    switch(other.kind_)
    {
    case json::kind::object:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&obj_) object(
                other.obj_, sp);
    #ifndef BOOST_NO_EXCEPTIONS
        } 
        catch(...)
        {
            kind_ = json::kind::null;
            ::new(&nat_.sp_)
                storage_ptr(std::move(sp));
            throw;
        }
    #endif
        break;

    case json::kind::array:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&arr_) array(
                other.arr_, sp);
                
    #ifndef BOOST_NO_EXCEPTIONS
        } 
        catch(...)
        {
            kind_ = json::kind::null;
            ::new(&nat_.sp_)
                storage_ptr(std::move(sp));
            throw;
        }
    #endif
        break;

    case json::kind::string:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&str_) string(
                other.str_, typename
                string::allocator_type(sp));
    #ifndef BOOST_NO_EXCEPTIONS
        } 
        catch(...)
        {
            kind_ = json::kind::null;
            ::new(&nat_.sp_)
                storage_ptr(std::move(sp));
            throw;
        }
    #endif
        break;

    case json::kind::number:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        ::new(&nat_.num_) number(
            other.nat_.num_);
        break;

    case json::kind::boolean:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        nat_.bool_ = other.nat_.bool_;
        break;

    case json::kind::null:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        break;
    }
    kind_ = other.kind_;
}

//------------------------------------------------------------------------------

// friends

std::ostream&
operator<<(std::ostream& os, value const& jv)
{
    switch(jv.kind())
    {
    case json::kind::object:
        os << '{';
        for(auto it = jv.as_object().begin(),
            last = jv.as_object().end();
            it != last;)
        {
            os << '\"' << it->first << "\":";
            os << it->second;
            if(++it != last)
                os << ',';
        }
        os << '}';
        break;
        
    case json::kind::array:
        os << '[';
        for(auto it = jv.as_array().begin(),
            last = jv.as_array().end();
            it != last;)
        {
            os << *it;
            if(++it != last)
                os << ',';
        }
        os << ']';
        break;
        
    case json::kind::string:
        os << '\"' << jv.as_string() << '\"';
        break;
        
    case json::kind::number:
        os << jv.as_number();
        break;
        
    case json::kind::boolean:
        if(jv.as_bool())
            os << "true";
        else
            os << "false";
        break;

    case json::kind::null:
        os << "null";
        break;
    }
    return os;
}

} // json
} // beast
} // boost

#endif
