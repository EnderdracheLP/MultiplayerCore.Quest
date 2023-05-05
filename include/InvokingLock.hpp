#pragma once
#include "hooking.hpp"
#include "logging.hpp"

// type to make a base call possible by checking if this call is already happening
// think of it like a mutex but for an object instance instead of a thread
template<typename U, typename T>
requires(std::is_pointer_v<T>)
struct InvokingLock {
    static inline std::list<InvokingLock<U, T>*> currently_invoking{};
    T value;

    InvokingLock(T v) : value(v) { currently_invoking.emplace_back(this); }
    ~InvokingLock() { std::erase(currently_invoking, this); }

    operator bool() const {
        auto other = std::find_if(currently_invoking.begin(), currently_invoking.end(), [this](auto x){
            return x != this && x->value == this->value;
        });
        return other == currently_invoking.end();
    }
};

// we use the hook name to ensure each hook gets its own static list in InvokingLock<U, T>
#define INVOKE_LOCK(name_) InvokingLock<Hook_##name_, decltype(self)> lock(self)
