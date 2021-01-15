#pragma once

#include <memory>
#include <sailgame/common/types.h>

#include "game_attr.h"
#include "../../games/uno/core/game_attr.h"

namespace SailGame { namespace Dock {

using Common::GameType;

class GameAttrFactory {
public:
    static std::unique_ptr<IGameAttr> Create(GameType game) {
        switch (game) {
            case GameType::Uno:
                return std::make_unique<Uno::GameAttr>();
            /// TODO: support other games
            default:
                throw std::runtime_error("Unsupported game.");
        }
    }

    static std::unique_ptr<IGameAttr> Create(const std::string &game) {
        return Create(GetGameTypeByGameName(game));
    }

    static GameType GetGameTypeByGameName(const std::string &gameName) {
        static std::map<std::string, GameType> mapping{
            {"UNO", GameType::Uno},
        };

        auto it = mapping.find(gameName);
        if (it == mapping.end()) {
            throw std::runtime_error("Unsupported game.");
        }
        return it->second;
    }
};

}}