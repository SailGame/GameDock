#pragma once

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

namespace SailGame { namespace Test {

MATCHER_P2(LoginArgsMatcher, username, password, "") {
    return arg.username() == username && arg.password() == password;
}

MATCHER_P(ListRoomArgsMatcher, gameName, "") {
    return arg.gamename() == gameName;
}

MATCHER_P(ExitRoomArgsMatcher, token, "") { return arg.token() == token; }

MATCHER_P2(QueryRoomArgsMatcher, token, roomId, "") {
    return arg.token() == token && arg.roomid() == roomId;
}

MATCHER_P2(ReadyMatcher, token, ready, "") {
    return arg.token() == token && arg.ready() == ready;
}

MATCHER_P(ListenArgsMatcher, token, "") { return arg.token() == token; }

}}  // namespace SailGame::Test