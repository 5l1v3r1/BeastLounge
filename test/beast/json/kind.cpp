//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

// Test that header file is self-contained.
#include <boost/beast/_experimental/json/kind.hpp>

#include <boost/beast/_experimental/unit_test/suite.hpp>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

class kind_test : public unit_test::suite
{
public:
    BOOST_STATIC_ASSERT(
        std::is_enum<kind>::value);

    void
    testEnum()
    {
        pass();
    }

    void run() override
    {
        testEnum();
    }
};

BEAST_DEFINE_TESTSUITE(beast,json,kind);

} // json
} // beast
} // boost
