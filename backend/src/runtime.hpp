#pragma once

#include <fix.hpp>
#include <atomic>
#include <csignal>
#include <mutex>


/// @brief Class managing global work flags for controlling program execution
class Runtime {
private:
    /// @brief Flag indicating whether the program should continue running
    static std::atomic_bool run;
    
    /// @brief Flag indicating whether the program should reload configuration
    static std::atomic_bool reload;

    /// @brief Flag to ensure that signals are registered only once
    static std::once_flag init_flag;

public:
    /// @brief Starts the work by setting the run flag to true
    static void Start();

    /// @brief Stops the work by setting the run flag to false
    static void Stop();

    /// @brief Function setting reload flag
    static void Reload();

    /// @brief Function to clean reload flag
    static void CleanAfterReload();

    /// @brief Checks if program should reload
    /// @return True if reload is required, false otherwise
    static bool ReloadRequired();
    
    /// @brief Checks if work should continue
    /// @return True if work should continue, false otherwise
    static bool Run();

    /// @brief Function handling signals recieving
    /// @param signalType recieved signal
    static BOOL WINAPI SignalHandler(DWORD signalType);

    /// @brief Function to register all necessary signal handleres
    /// @note Must be called only once
    /// @throws std::runtime_error on registration failure
    static void RegisterSignalHandles();
};
