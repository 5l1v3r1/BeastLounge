//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_ASSIGN_VECTOR_HPP
#define BOOST_BEAST_JSON_ASSIGN_VECTOR_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/_experimental/json/error.hpp>
#include <boost/beast/_experimental/json/value.hpp>
#include <type_traits>
#include <vector>

namespace boost {
namespace beast {
namespace json {

template<class T, class A
#if 0
    ,class = typename std::enable_if<
        can_assign_from<T>::value>::type
#endif
>
void
from_json(
    std::vector<T, A>& t,
    value const& v)
{
    if(! v.is_array())
        throw system_error(
            error::expected_array);
    auto& arr = v.as_array();
    t.resize(0);
    t.resize(arr.size());
    auto it = t.begin();
    for(auto const& e : arr)
        e.store(*it++);
}

} // json
} // beast
} // boost

#endif
