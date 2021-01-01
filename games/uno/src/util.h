#pragma once

namespace SailGame { namespace Game {

class UnoUtil {
public:
    static int Wrap(int numToWrap, int range) {
        int ret = numToWrap % range;
        if (ret < 0) {
            ret += range;
        }
        return ret;
    }
};

}}