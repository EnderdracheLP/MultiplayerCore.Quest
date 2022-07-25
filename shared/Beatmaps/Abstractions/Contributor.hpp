#pragma once
#include "beatsaber-hook/shared/utils/typedefs-string.hpp"
#include <string>

namespace MultiplayerCore::Beatmaps::Abstractions {
    struct Contributor
    {
        Contributor(StringW name, StringW role, StringW iconPath)
            : _name(name)
            , _role(role)
            , _iconPath(iconPath)
        {  }
        Contributor(std::string name, std::string role, std::string iconPath)
            : _name(name)
            , _role(role)
            , _iconPath(iconPath)
        {  }
        std::string _name;
        std::string _role;
        std::string _iconPath;
    };
}