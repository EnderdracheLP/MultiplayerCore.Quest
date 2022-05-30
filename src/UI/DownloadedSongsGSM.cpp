#include "main.hpp"
#include "UI/DownloadedSongsGSM.hpp"
#include "Utilities.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "songloader/shared/API.hpp"

#include "GlobalNamespace/IMenuRpcManager.hpp"

#include "HMUI/ModalView.hpp"

#include "UnityEngine/UI/ContentSizeFitter.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"

#include "GlobalFields.hpp"
#include "shared/GlobalFields.hpp"
using namespace GlobalNamespace;
using namespace QuestUI;
using namespace RuntimeSongLoader::API;
using namespace HMUI;
using namespace MultiQuestensions;
using namespace MultiplayerCore;


DEFINE_TYPE(MultiQuestensions::UI, DownloadedSongsGSM);

namespace MultiQuestensions::UI {
    bool cellIsSelected = false;
    DownloadedSongsGSM* DownloadedSongsGSM::instance;
    std::vector<std::string> DownloadedSongsGSM::mapQueue;

    void DownloadedSongsGSM::CreateCell(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* coverTask, CustomPreviewBeatmapLevel* level, System::Action_1<System::Threading::Tasks::Task*>* action) {
        getLogger().debug("CreateCell");
        UnityEngine::Sprite* cover = coverTask->get_Result();
        // If we have a coverImage and a level we add them to the list
        if (cover && level) {
            // "<size=80%><noparse>" + map.GetMetadata().GetSongAuthorName() + "</noparse>" + " <size=90%>[<color=#67c16f><noparse>" + map.GetMetadata().GetLevelAuthorName() + "</noparse></color>]"
            list->data.emplace_back(CustomListTableData::CustomCellInfo{
                level->get_songName() ? to_utf8(csstrtostr(level->get_songName())) : "Error: songName null",
                (level->get_songAuthorName() ? to_utf8(csstrtostr(level->get_songAuthorName())) : std::string()) + " [" + (level->get_levelAuthorName() ? to_utf8(csstrtostr(level->get_levelAuthorName())) : std::string()) + "]",
                cover
                });
        }
        // else if we have the level but no coverImage we add the level and use the default coverImage
        else if (level) {
            list->data.emplace_back(CustomListTableData::CustomCellInfo{
            level->get_songName() ? to_utf8(csstrtostr(level->get_songName())) : "Error: songName null",
            (level->get_songAuthorName() ? to_utf8(csstrtostr(level->get_songAuthorName())) : std::string()) + " [" + (level->get_levelAuthorName() ? to_utf8(csstrtostr(level->get_levelAuthorName())) : std::string()) + "]",
            level->get_defaultCoverImage()
                });
        } else getLogger().error("Nullptr in UI: cover '%p', level '%p'", cover, level);
        if (!mapQueue.empty()) {
            InsertCell(mapQueue.back());
            mapQueue.pop_back();
        }
        // Refresh our view, though this doesn't always work perfectly first try though will at least show the next time our view ie re-enabled
        if (list && list->tableView)
            list->tableView->RefreshCellsContent();
        else getLogger().error("Nullptr in UI: list '%p', list->tableView '%p'", list, list->tableView);
        getLogger().debug("CreateCell Finished, deleting delegate");
        QuestUI::MainThreadScheduler::Schedule([&]{
            MultiplayerCore::Utilities::ClearDelegate(action);
        });
        
    }

    // TODO: Add index check, check if index is out of bounds
    void DownloadedSongsGSM::Delete() {
        try {
            needSongRefresh = false;
            auto level = GetLevelByHash(DownloadedSongIds.at(selectedIdx));
            if (level.has_value()) {
                std::string songPath = to_utf8(csstrtostr(level.value()->get_customLevelPath()));
                getLogger().info("Deleting Song: %s", songPath.c_str());
                DeleteSong(songPath, [&] {
                    if (needSongRefresh) {
                        RefreshSongs(false);
                    }
                    });
                if (lobbyGameStateController) lobbyGameStateController->menuRpcManager->SetIsEntitledToLevel(level.value()->get_levelID(), EntitlementsStatus::NotDownloaded);
            }
            else {
                getLogger().debug("Level to Delete not found");
            }
            needSongRefresh = true;
            DownloadedSongIds.erase(DownloadedSongIds.begin() + selectedIdx);
        }
        catch (il2cpp_utils::RunMethodException const& e) {
            getLogger().error("REPORT TO ENDER: Exception encountered trying to delete song: %s", e.what());
            e.log_backtrace();
        }
        catch (const std::exception& e) {
            getLogger().error("REPORT TO ENDER: Exception encountered trying to delete song: %s", e.what());
        }
        list->tableView->ClearSelection();
        list->data.erase(list->data.begin() + selectedIdx);
        Refresh();
        modal->Hide(true, nullptr);
    }

    // TODO: Add keep all and delete all option
    void DownloadedSongsGSM::DidActivate(bool firstActivation) {
        if (firstActivation) {
            instance = this;

            modal = BeatSaberUI::CreateModal(get_transform(), { 55, 25 }, [this](HMUI::ModalView* self) {
                list->tableView->ClearSelection();
                });
            auto wrapper = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(modal->get_transform());
            auto container = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(wrapper->get_transform());
            container->set_childAlignment(UnityEngine::TextAnchor::MiddleCenter);
            QuestUI::BeatSaberUI::CreateText(container->get_transform(), "Do you want to delete this song?")->set_alignment(TMPro::TextAlignmentOptions::Center);

            auto horizon = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(container->get_transform());

            QuestUI::BeatSaberUI::CreateUIButton(horizon->get_transform(), "<color=#ff0000>Delete</color>", [this]() -> void {
                Delete();
                cellIsSelected = false;
                });

            QuestUI::BeatSaberUI::CreateUIButton(horizon->get_transform(), "<color=#00ff00>Keep</color>", [this]() -> void {
                DownloadedSongIds.erase(DownloadedSongIds.begin() + selectedIdx);
                list->tableView->ClearSelection();
                list->data.erase(list->data.begin() + selectedIdx);
                Refresh();
                modal->Hide(true, nullptr);
                cellIsSelected = false;
            });

            auto vertical = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(get_transform());
            vertical->get_gameObject()->AddComponent<UnityEngine::UI::ContentSizeFitter*>()
                ->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::MinSize);


            list = BeatSaberUI::CreateScrollableList(vertical->get_transform(), { 80, 60 }, [this](int idx) {
                getLogger().debug("Cell with idx %d clicked", idx);
                cellIsSelected = true;
                selectedIdx = idx;
                modal->Show(true, true, nullptr);
            });

            auto autoDelete = QuestUI::BeatSaberUI::CreateToggle(vertical->get_transform(), "Auto-Delete Songs", getConfig().config["autoDelete"].GetBool(), [](bool value) {
                getConfig().config["autoDelete"].SetBool(value);
                getConfig().Write();
            });
            QuestUI::BeatSaberUI::AddHoverHint(autoDelete->get_gameObject(), "Automatically deletes downloaded songs after playing them.");
        }

        getLogger().debug("DownloadedSongsGSM::DidActivate");
        Refresh();
    }

    void DownloadedSongsGSM::InsertCell(std::string hash) {
        std::optional<CustomPreviewBeatmapLevel*> levelOpt = GetLevelByHash(hash);
        if (levelOpt.has_value()) {
            lastDownloaded = levelOpt.value();
            getLogger().info("Song with Hash '%s' added to list", hash.c_str());
            System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* coverTask = lastDownloaded->GetCoverImageAsync(System::Threading::CancellationToken::get_None());
            static System::Action_1<System::Threading::Tasks::Task*>* action;
            action = il2cpp_utils::MakeDelegate<System::Action_1<System::Threading::Tasks::Task*>*>(classof(System::Action_1<System::Threading::Tasks::Task*>*), (std::function<void()>)[coverTask, this] {
                CreateCell(coverTask, lastDownloaded, action);
                }
            );
            reinterpret_cast<System::Threading::Tasks::Task*>(coverTask)->ContinueWith(action);
        }
        else {
            getLogger().error("Song with Hash '%s' not found, was it already deleted?", hash.c_str());
        }
    }

    void DownloadedSongsGSM::Refresh() {
        list->tableView->ReloadData();
        list->tableView->RefreshCellsContent();
    }

    void DownloadedSongsGSM::OnEnable() {
        if (list && list->tableView) {
            if (!mapQueue.empty()) {
                InsertCell(mapQueue.back());
                mapQueue.pop_back();
            }
            if (cellIsSelected) list->tableView->ClearSelection();
            list->tableView->ReloadData();
            list->tableView->RefreshCellsContent();
        }
    }
}