#pragma once

#include <spdlog/spdlog.h>
#include <sailgame_pb/core/types.pb.h>

namespace SailGame { namespace Dock {

using namespace ::Core;

class Logger {
public:
    // Args
    static void Log(const LoginArgs &args) {
        spdlog::info("[Login Args] username: {}, password: {}", 
            args.username(), args.password());
    }

    static void Log(const QueryAccountArgs &args) {
        switch (args.key_case()) {
            case QueryAccountArgs::kUserName:
                spdlog::info("[QueryAccount Args] username: {}", 
                    args.username());
                return;
            case QueryAccountArgs::kToken:
                spdlog::info("[QueryAccount Args] token: {}", 
                    args.token());
                return;
            default:
                throw std::runtime_error("Unsupported key.");
        }
    }

    static void Log(const CreateRoomArgs &args) {
        spdlog::info("[CreateRoom Args] token: {}", args.token());
    }

    /// TODO: deal with Any
    static void Log(const ControlRoomArgs &args) {
        spdlog::info("[ControlRoom Args] token: {}, roomId: {}, gameName: {},"
            "roomPassword: {} ", 
            args.token(), args.roomid(), args.gamename(), args.roompassword());
    }

    static void Log(const ListRoomArgs &args) {
        spdlog::info("[ListRoom Args] token: {}", args.gamename());
    }

    static void Log(const JoinRoomArgs &args) {
        spdlog::info("[JoinRoom Args] token: {}, roomId: {}", 
            args.token(), args.roomid());
    }

    static void Log(const ExitRoomArgs &args) {
        spdlog::info("[ExitRoom Args] token: {}", args.token());
    }

    static void Log(const QueryRoomArgs &args) {
        spdlog::info("[QueryRoom Args] token: {}, roomId: {}", 
            args.token(), args.roomid());
    }

    /// TODO: deal with Any
    static void Log(const OperationInRoomArgs &args) {
        switch (args.RoomOperation_case()) {
            case OperationInRoomArgs::kReady:
                spdlog::info("[OperationInRoom Args] token: {}, ready: {}",
                    args.token(), args.ready());
                break;
            case OperationInRoomArgs::kCustom:
                spdlog::info("[OperationInRoom Args] token: {}, custom",
                    args.token());
                break;
            default:
                throw std::runtime_error("Unsupported RoomOperation.");
        }
    }

    static void Log(const MessageArgs &args) {
        spdlog::info("[Message Args] token: {}, message: {}, dstUser: {},"
            "dstRoom{} ",
            args.token(), args.message(), args.dstuser(), args.dstroom());
    }

    // Ret
    static void Log(const LoginRet &ret) {
        spdlog::info("[Login Ret] err: {}, token: {}, username: {}, points: {}",
            ret.err(), ret.token(), ret.account().username(),
            ret.account().points());
    }

    static void Log(const QueryAccountRet &ret) {
        spdlog::info("[QueryAccount Ret] err: {}, username: {}, points: {}",
            ret.err(), ret.account().username(), ret.account().points());
    }

    static void Log(const CreateRoomRet &ret) {
        spdlog::info("[CreateRoom Ret] err: {}, roomId: {}",
            ret.err(), ret.roomid());
    }

    /// TODO: deal with Any
    static void Log(const ControlRoomRet &ret) {
        spdlog::info("[ControlRoom Ret] err: {}", ret.err());
    }

    /// TODO: deal with Repeated
    static void Log(const ListRoomRet &ret) {
        spdlog::info("[ListRoom Ret] err: {}, room size: {}",
            ret.err(), ret.room_size());
    }

    static void Log(const JoinRoomRet &ret) {
        spdlog::info("[JoinRoom Ret] err: {}", ret.err());
    }

    static void Log(const ExitRoomRet &ret) {
        spdlog::info("[ExitRoom Ret] err: {}", ret.err());
    }

    /// TODO: deal with Any
    static void Log(const QueryRoomRet &ret) {
        spdlog::info("[QueryRoom Ret] err: {}, gameName: {}, roomId: {}, "
            "user size: {}",
            ret.err(), ret.room().gamename(), ret.room().roomid(), 
            ret.room().user_size());
    }

    static void Log(const OperationInRoomRet &ret) {
        spdlog::info("[OperationInRoom Ret] err: {}", ret.err());
    }

    static void Log(const MessageRet &ret) {
        spdlog::info("[Message Ret] err: {}", ret.err());
    }

};
}}
