#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <sailgame_pb/uno/uno.pb.h>
#include <sailgame/common/util.h>
#include <sailgame/uno/card.h>

namespace SailGame { namespace Test {

using Common::Util;
using Uno::Card;
using ::Uno::UserOperation;

MATCHER_P(DrawMatcher, number, "") {
    if (!arg.custom().template Is<UserOperation>()) {
        return false;
    }
    auto msg = Util::UnpackGrpcAnyTo<UserOperation>(arg.custom());
    return msg.has_draw() && msg.draw().number() == number;
}

MATCHER(SkipMatcher, "") {
    if (!arg.custom().template Is<UserOperation>()) {
        return false;
    }
    auto msg = Util::UnpackGrpcAnyTo<UserOperation>(arg.custom());
    return msg.has_skip();
}

MATCHER_P2(PlayMatcher, card, color, "") {
    if (!arg.custom().template Is<UserOperation>()) {
        return false;
    }
    auto msg = Util::UnpackGrpcAnyTo<UserOperation>(arg.custom());
    return msg.has_play() && 
        Card{msg.play().card()} == card && msg.play().nextcolor() == color;
}
}}