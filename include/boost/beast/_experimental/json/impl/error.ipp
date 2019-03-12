//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_IMPL_ERROR_IPP
#define BOOST_BEAST_JSON_IMPL_ERROR_IPP

#include <boost/beast/_experimental/json/error.hpp>

namespace boost {
namespace beast {
namespace json {

namespace detail {

class error_codes : public error_category
{
public:
    const char*
    name() const noexcept override
    {
        return "boost.beast.json";
    }

    BOOST_BEAST_DECL
    std::string
    message(int ev) const override
    {
        switch(static_cast<error>(ev))
        {
        default:
        case error::syntax: return
            "The serialized JSON object contains a syntax error";
        case error::extra_data: return
            "Unexpected extra data encountered while parsing";
        case error::mantissa_overflow: return
            "A mantissa overflowed while parsing";
        case error::exponent_overflow: return
            "An exponent overflowed while parsing";
        case error::too_deep: return
            "The parser reached the maximum allowed depth";

        case error::integer_overflow: return
            "An integer assignment overflowed";
        case error::expected_object: return
            "Expected a value of kind object";
        case error::expected_array: return
            "Expected a value of kind array";
        case error::expected_string: return
            "Expected a value of kind string";
        case error::expected_signed: return
            "Expected a value of kind signed";
        case error::expected_unsigned: return
            "Expected a value of kind unsigned";
        case error::expected_floating: return
            "Expected a value of kind floating";
        case error::expected_bool: return
            "Expected a value of kind bool";
        case error::expected_null: return
            "Expected a value of kind null";

        case error::key_not_found: return
            "The key was not found in the object";
        }
    }

    BOOST_BEAST_DECL
    error_condition
    default_error_condition(int ev) const noexcept override
    {
        switch(static_cast<error>(ev))
        {
        default:
            return {ev, *this};

        case error::syntax:
        case error::extra_data:
        case error::mantissa_overflow:
        case error::exponent_overflow:
        case error::too_deep:
            return condition::parse_error;

        case error::integer_overflow:
        case error::expected_object:
        case error::expected_array:
        case error::expected_string:
        case error::expected_signed:
        case error::expected_unsigned:
        case error::expected_floating:
        case error::expected_bool:
        case error::expected_null:
            return condition::assign_error;
        }
    }
};

class error_conditions : public error_category
{
public:
    BOOST_BEAST_DECL
    const char*
    name() const noexcept override
    {
        return "boost.beast";
    }

    BOOST_BEAST_DECL
    std::string
    message(int cv) const override
    {
        switch(static_cast<condition>(cv))
        {
        default:
        case condition::parse_error:
            return "A JSON parsing error occurred";
        case condition::assign_error:
            return "An error occurred during assignment";
        }
    }
};

} // detail

error_code
make_error_code(error e)
{
    static detail::error_codes const cat{};
    return error_code{static_cast<
        std::underlying_type<error>::type>(e), cat};
}

error_condition
make_error_condition(condition c)
{
    static detail::error_conditions const cat{};
    return error_condition{static_cast<
        std::underlying_type<condition>::type>(c), cat};
}

} // json
} // beast
} // boost

#endif
