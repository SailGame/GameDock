#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

namespace SailGame { namespace Test {

MATCHER_P2(LoginArgsMatcher, username, password, "") {
    return arg.username() == username && arg.password() == password;
}

MATCHER_P(ListenArgsMatcher, token, "") { return arg.token() == token; }

}}