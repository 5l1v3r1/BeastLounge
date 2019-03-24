//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_ITERATOR_HPP
#define BOOST_BEAST_JSON_ITERATOR_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/core/detail/static_const.hpp>
#include <boost/beast/_experimental/json/value.hpp>
#include <boost/beast/_experimental/json/detail/stack.hpp>
#include <iterator>
#include <new>

namespace boost {
namespace beast {
namespace json {

struct end_t
{
};

BOOST_BEAST_INLINE_VARIABLE(end, end_t);

/** A generator to perform a depth-first traversal of a JSON value.
*/
class const_iterator
{
    struct node
    {
        union
        {
            array::const_iterator arr_it;
            object::const_iterator obj_it;
        };
        value const* v;
        string_view  key;
        bool         it;

        BOOST_BEAST_DECL
        ~node();

        BOOST_BEAST_DECL
        node(
            value const& v_,
            bool it_) noexcept;

        BOOST_BEAST_DECL
        node(node const& other) noexcept;

        BOOST_BEAST_DECL
        bool
        last() const noexcept;
    };

    detail::stack<node, 50> stack_;

public:
    struct value_type
    {
        std::size_t         depth;
        string_view         key;
        json::value const&  value;
        bool                last;
        bool                end;

        value_type const*
        operator->() const noexcept
        {
            return this;
        }
    };

    BOOST_BEAST_DECL
    explicit
    const_iterator(
        value const& jv);

    BOOST_BEAST_DECL
    value_type
    operator*() const noexcept;

    BOOST_BEAST_DECL
    const_iterator&
    operator++() noexcept;

    void
    operator++(int) noexcept
    {
        ++*this;
    }

    value_type
    operator->() const noexcept
    {
        return *(*this);
    }

    friend
    bool
    operator==(
        const_iterator const& lhs,
        end_t) noexcept
    {
        return lhs.stack_.empty();
    }

    friend
    bool
    operator==(
        end_t,
        const_iterator const& rhs) noexcept
    {
        return rhs.stack_.empty();
    }

    friend
    bool
    operator!=(
        const_iterator const& lhs,
        end_t) noexcept
    {
        return ! lhs.stack_.empty();
    }

    friend
    bool
    operator!=(
        end_t,
        const_iterator const& rhs) noexcept
    {
        return ! rhs.stack_.empty();
    }
};

} // json
} // beast
} // boost

#include <boost/beast/_experimental/json/impl/iterator.hpp>
#if BOOST_BEAST_HEADER_ONLY
#include <boost/beast/_experimental/json/impl/iterator.ipp>
#endif

#endif
