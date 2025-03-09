#pragma once
#include "custom-types/shared/macros.hpp"

#include "Models/MpStatusData.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"

#include "GlobalNamespace/INetworkConfig.hpp"

using MpStatusDictionary = System::Collections::Generic::Dictionary_2<StringW, MultiplayerCore::Models::MpStatusData*>;


DECLARE_CLASS_CODEGEN(MultiplayerCore::Repositories, MpStatusRepository, System::Object) {
    DECLARE_CTOR(New, GlobalNamespace::INetworkConfig* networkConfig);

    DECLARE_INSTANCE_FIELD(MpStatusDictionary*, _statusByUrl);
    // DECLARE_INSTANCE_FIELD(System::Collections::Generic::Dictionary_2<StringW, MultiplayerCore::Models::MpStatusData*>*, _statusByHostname);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::INetworkConfig*, _networkConfig);
    DECLARE_INSTANCE_FIELD_PRIVATE(MultiplayerCore::Models::MpStatusData*, _lastStatus);

    DECLARE_INSTANCE_METHOD(void, ReportStatus, Models::MpStatusData* statusData);
    DECLARE_INSTANCE_METHOD(MultiplayerCore::Models::MpStatusData*, GetLastStatusData);

    public:
        static MpStatusRepository* get_Instance();
        static __declspec(property(get=get_Instance)) MpStatusRepository* Instance;

        UnorderedEventCallback<std::string, const Models::MpStatusData*> statusUpdatedForUrlEvent;

    private:
        static MpStatusRepository* _instance;
};