//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_ERROR_HPP
#define BOOST_BEAST_JSON_ERROR_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/core/error.hpp>

namespace boost {
namespace beast {
namespace json {

/// Error codes returned by JSON operations
enum class error
{
    syntax = 1,

    /// Unexpected extra data encountered while parsing
    extra_data,

    /// A mantissa overflowed while parsing
    mantissa_overflow,

    /// The parser encountered an exponent that overflowed
    exponent_overflow,

    /// The parser's maximum depth limit was reached
    too_deep,

    /// Expected a value of kind object
    expected_object,

    /// Expected a value of kind array
    expected_array,

    /// Expected a value of kind string
    expected_string,

    /// Expected a value of kind signed
    expected_signed,

    /// Expected a value of kind unsigned
    expected_unsigned,

    /// Expected a value of kind floating
    expected_floating,

    /// Expected a value of kind boolean
    expected_bool,

    /// Expected a value of kind boolean
    expected_null,

    /// An integer assignment would overflow
    integer_overflow,

    /// The key was not found in the object
    key_not_found
};

/// Error conditions corresponding to JSON errors
enum class condition
{
    /// A parser-related error
    parse_error = 1,

    /// An error on assignment to or from a JSON value
    assign_error
};

} // websocket
} // beast
} // boost

#include <boost/beast/_experimental/json/impl/error.hpp>
#ifdef BOOST_BEAST_HEADER_ONLY
#include <boost/beast/_experimental/json/impl/error.ipp>
#endif

#endif
