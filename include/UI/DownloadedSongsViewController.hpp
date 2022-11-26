#pragma once
#include "custom-types/shared/macros.hpp"
#include "HMUI/ViewController.hpp"
#include "questui/shared/CustomTypes/Components/List/CustomListTableData.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/ModalView.hpp"

#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Threading/Tasks/Task.hpp"
#include "System/Action_1.hpp"
#include "UnityEngine/Sprite.hpp"


DECLARE_CLASS_CODEGEN(MultiplayerCore::UI, DownloadedSongsViewController, HMUI::ViewController,
    DECLARE_INSTANCE_FIELD(HMUI::ModalView*, modal);
    DECLARE_INSTANCE_FIELD(QuestUI::CustomListTableData*, list);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::CustomPreviewBeatmapLevel*, lastDownloaded);
    DECLARE_INSTANCE_FIELD(::System::Threading::Tasks::Task*, continueTask);

    DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::il2cpp_type_check::MetadataGetter<&HMUI::ViewController::DidActivate>::get(), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_INSTANCE_METHOD(void, OnEnable);
    DECLARE_INSTANCE_METHOD(void, Refresh);
    DECLARE_INSTANCE_METHOD(void, CreateCell, System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* coverTask, GlobalNamespace::CustomPreviewBeatmapLevel* level);
    DECLARE_INSTANCE_METHOD(void, Delete);

    static DownloadedSongsViewController* instance;

    int selectedIdx;
    bool needSongRefresh;
    public:
    static DownloadedSongsViewController* get_Instance() { return instance; }

    static std::vector<std::string> mapQueue;

public:
    void InsertCell(std::string hash);
    void InsertCell(GlobalNamespace::CustomPreviewBeatmapLevel* level);
)