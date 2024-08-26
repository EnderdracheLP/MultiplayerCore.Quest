#pragma once

#include <string>
#include "paper/shared/utfcpp/source/utf8.h"
#include "beatsaber-hook/shared/utils/typedefs-string.hpp"

namespace MultiplayerCore {
    static std::string HashFromLevelID(StringW levelId) {
        if (!levelId.starts_with(u"custom_level_")) return "";
        std::u16string_view hash(levelId);
        hash = hash.substr(13);
        if (hash.ends_with(u" WIP")) hash = hash.substr(0, hash.size() - 4);
        return utf8::utf16to8(hash);
    }

    static std::string HashFromLevelID(std::string_view levelId) {
        if (!levelId.starts_with("custom_level_")) return "";
        auto hash(levelId.substr(13));
        if (hash.ends_with(" WIP")) hash = hash.substr(0, hash.size() - 4);
        return std::string(hash);
    }

    template<typename T>
    std::future<T> finished_future(T& value) {
        std::promise<T> p;
        p.set_value(std::forward<T>(value));
        return p.get_future();
    }
}
