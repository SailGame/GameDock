#pragma once

#include <sailgame/common/types.h>
// #include <texas/core/game_attr.h>

#include <memory>
#include <unordered_map>

#include "../../games/uno/core/game_attr.h"
#include "game_attr.h"

namespace SailGame { namespace Dock {

using Common::GameType;
using Common::Util;

class GameAttrFactory {
public:
    static std::shared_ptr<IGameAttr> Get(GameType gameType) {
        static std::unordered_map<GameType, std::shared_ptr<IGameAttr>> mGames;
        if (mGames.count(gameType) == 0) {
            switch (gameType) {
                case GameType::Uno:
                    mGames.emplace(gameType, std::make_shared<Uno::GameAttr>());
                    break;
                case GameType::Texas:
                    // mGames.emplace(gameType,
                    // std::make_shared<Texas::GameAttr>());
                    break;
                default:
                    throw std::runtime_error("Unsupported game.");
            }
        }

        return mGames.at(gameType);
    }

    static std::shared_ptr<IGameAttr> Get(const std::string &gameName) {
        return Get(Util::GetGameTypeByGameName(gameName));
    }
};

}}  // namespace SailGame::Dock