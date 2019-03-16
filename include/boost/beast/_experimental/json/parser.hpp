//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_PARSER_HPP
#define BOOST_BEAST_JSON_PARSER_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/_experimental/json/value.hpp>
#include <boost/beast/_experimental/json/basic_parser.hpp>
#include <boost/beast/_experimental/json/storage.hpp>
#include <new>
#include <string>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

class parser : public basic_parser
{
    static std::size_t const
        default_max_depth = 32;

    value jv_;
    detail::stack<value*,
        default_max_depth> stack_;
    std::string s_;
    unsigned long max_depth_ =
        default_max_depth;
    bool obj_;

public:
    BOOST_BEAST_DECL
    parser();

    BOOST_BEAST_DECL
    parser(storage_ptr const& store);

    /** Returns the maximum allowed depth of input JSON.
    */
    std::size_t
    max_depth() const noexcept
    {
        return max_depth_;
    }

    /** Set the maximum allowed depth of input JSON.
    */
    void
    max_depth(unsigned long levels) noexcept
    {
        max_depth_ = levels;
    }

    BOOST_BEAST_DECL
    value const&
    get() const noexcept;

    BOOST_BEAST_DECL
    value
    release() noexcept;

private:
    template<class T>
    void
    assign(T&& t);

    BOOST_BEAST_DECL
    void
    reset();

    BOOST_BEAST_DECL
    void
    on_document_begin(
        error_code& ec) override;

    BOOST_BEAST_DECL
    void
    on_object_begin(
        error_code& ec) override;

    BOOST_BEAST_DECL
    void
    on_object_end(
        error_code& ec) override;

    BOOST_BEAST_DECL
    void
    on_array_begin(
        error_code& ec) override;

    BOOST_BEAST_DECL
    void
    on_array_end(
        error_code& ec) override;

    BOOST_BEAST_DECL
    void
    on_key_data(
        string_view s,
        error_code& ec) override;

    BOOST_BEAST_DECL
    void
    on_key_end(
        string_view s,
        error_code& ec) override;

    BOOST_BEAST_DECL
    void
    on_string_data(
        string_view s,
        error_code& ec) override;

    BOOST_BEAST_DECL
    void
    on_string_end(
        string_view s,
        error_code& ec) override;

    BOOST_BEAST_DECL
    void
    on_number(
        number n,
        error_code& ec) override;

    BOOST_BEAST_DECL
    void
    on_bool(
        bool b,
        error_code& ec) override;

    BOOST_BEAST_DECL
    void
    on_null(error_code&) override;
};

} // json
} // beast
} // boost

#include <boost/beast/_experimental/json/impl/parser.hpp>
#ifdef BOOST_BEAST_HEADER_ONLY
#include <boost/beast/_experimental/json/impl/parser.ipp>
#endif

#endif
