#pragma once

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "System/Threading/Tasks/Task_1.hpp"

namespace MultiplayerCore {
    template<typename Ret, typename T>
    requires(std::is_invocable_r_v<Ret, T>)
    static void task_func(System::Threading::Tasks::Task_1<Ret>* task, T func) {
        task->TrySetResult(std::invoke(func));
    }

    template<typename Ret, typename T>
    requires(std::is_invocable_r_v<Ret, T>)
    static void task_cancel_func(System::Threading::Tasks::Task_1<Ret>* task, T func, System::Threading::CancellationToken cancelToken) {
        auto value = std::invoke(func);
        if (!cancelToken.IsCancellationRequested) {
            task->TrySetResult(std::invoke(func));
        } else {
            task->TrySetCanceled(cancelToken);
        }
    }

    template<typename Ret, typename T>
    requires(!std::is_same_v<Ret, void> && std::is_invocable_r_v<Ret, T>)
    static System::Threading::Tasks::Task_1<Ret>* StartTask(T func) {
        auto t = System::Threading::Tasks::Task_1<Ret>::New_ctor();
        il2cpp_utils::il2cpp_aware_thread(&task_func<Ret, T>, t, func).detach();
        return t;
    }

    template<typename Ret, typename T>
    requires(!std::is_same_v<Ret, void> && std::is_invocable_r_v<Ret, T>)
    static System::Threading::Tasks::Task_1<Ret>* StartTask(T func, System::Threading::CancellationToken cancelToken) {
        auto t = System::Threading::Tasks::Task_1<Ret>::New_ctor();
        il2cpp_utils::il2cpp_aware_thread(&task_cancel_func<Ret, T>, t, func, cancelToken).detach();
        return t;
    }
}
