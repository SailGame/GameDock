#pragma once

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/string.hpp>
#include <sailgame_pb/uno/uno.pb.h>
#include <google/protobuf/any.pb.h>
#include <sailgame/common/util.h>

namespace SailGame { namespace Uno {

using ftxui::vbox;
using ftxui::text;
using ftxui::Element;
using ftxui::to_wstring;
using ::Uno::StartGameSettings;
using google::protobuf::Any;

class UnoUtil {
public:
    static Element ShowGameSettings(const Any &any) {
        auto settings = Common::Util::UnpackGrpcAnyTo<StartGameSettings>(any);
        auto vBox = vbox({});
        auto boolToWstring = [](bool b) -> std::wstring {
            return b ? L"true" : L"false"; 
        };
        vBox->children.push_back(text(L"isDraw2Consumed: " + 
            boolToWstring(settings.isdraw2consumed())));
        vBox->children.push_back(text(L"canSkipRespond: " + 
            boolToWstring(settings.canskiprespond())));
        vBox->children.push_back(text(L"hasWildSwapHandsCard: " + 
            boolToWstring(settings.haswildswaphandscard())));
        vBox->children.push_back(text(L"canDoubtDraw4: " + 
            boolToWstring(settings.candoubtdraw4())));
        vBox->children.push_back(text(L"roundTime: " + 
            boolToWstring(settings.roundtime())));
        return vBox;
    }
};

}}