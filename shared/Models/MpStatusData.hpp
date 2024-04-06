#pragma once

#include "custom-types/shared/macros.hpp"
#include "GlobalNamespace/MultiplayerStatusData.hpp"

DECLARE_CLASS_CODEGEN(MultiplayerCore::Models, MpStatusData, GlobalNamespace::MultiplayerStatusData,
    DECLARE_CTOR(New, StringW json);
    DECLARE_INSTANCE_FIELD(StringW, maximumAppVersion);
    public:
        struct RequiredMod {
            std::string id;
            std::string version;
            bool required;
        };

        bool get_UseSsl() const { return _useSsl; }
        __declspec(property(get=get_UseSsl)) bool UseSsl;

        std::string_view get_Name() const { return _name; }
        __declspec(property(get=get_Name)) std::string_view Name;

        std::string_view get_Description() const { return _description; }
        __declspec(property(get=get_Description)) std::string_view Description;

        std::string_view get_ImageUrl() const { return _imageUrl; }
        __declspec(property(get=get_ImageUrl)) std::string_view ImageUrl;

        int get_MaxPlayers() const { return _maxPlayers; }
        __declspec(property(get=get_MaxPlayers)) int MaxPlayers;

        bool get_SupportsPPModifiers() const { return _supportsPPModifiers; }
        __declspec(property(get=get_SupportsPPModifiers)) bool SupportsPPModifiers;

        bool get_SupportsPPDifficulties() const { return _supportsPPDifficulties; }
        __declspec(property(get=get_SupportsPPDifficulties)) bool SupportsPPDifficulties;

        bool get_SupportsPPMaps() const { return _supportsPPMaps; }
        __declspec(property(get=get_SupportsPPMaps)) bool SupportsPPMaps;

        std::span<RequiredMod const> get_RequiredMods() const { return _requiredMods; }
        __declspec(property(get=get_RequiredMods)) std::span<RequiredMod const> RequiredMods;
    private:
        std::vector<RequiredMod> _requiredMods;
        bool _useSsl;
        std::string _name;
        std::string _description;
        std::string _imageUrl;
        int _maxPlayers;
        bool _supportsPPModifiers;
        bool _supportsPPDifficulties;
        bool _supportsPPMaps;
)
