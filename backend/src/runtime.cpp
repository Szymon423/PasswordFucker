#include <runtime.hpp>
#include <log.hpp>
#include <stdexcept>

std::atomic_bool Runtime::run = true;
std::atomic_bool Runtime::reload = false;
std::once_flag Runtime::init_flag;

void Runtime::Start() {
    run.store(true, std::memory_order_release);
    reload.store(false, std::memory_order_release);
}

void Runtime::Stop() {
    run.store(false, std::memory_order_release);
}

void Runtime::Reload() {
    reload.store(true, std::memory_order_release);
}

void Runtime::CleanAfterReload() {
    reload.store(false, std::memory_order_release);
}

bool Runtime::ReloadRequired() {
    return reload.load(std::memory_order_acquire);
}

bool Runtime::Run() {
    return run.load(std::memory_order_acquire);
}

BOOL WINAPI Runtime::SignalHandler(DWORD signalType) {
    switch (signalType) {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            Logger::trace("Received terminate signal, stopping.");
            run.store(false, std::memory_order_relaxed);
            return TRUE;
        default:
            Logger::warn("Received unhandled signal: {}.", static_cast<int>(signalType));
            return FALSE;
    }
}

void Runtime::RegisterSignalHandles() {
    std::call_once(init_flag, []() {
        if (!SetConsoleCtrlHandler(SignalHandler, TRUE)) {
            throw std::runtime_error("Failed to register Windows signal handler");
        }
    });
}
