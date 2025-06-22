#pragma once

#include <fix.hpp>
#include <string>
#include <cstddef>
#include <filesystem>
#include <nlohmann/json.hpp>

/// @brief Namespace for configuration related stuff
namespace config {
    /// @brief Class with configuration elements
    class Configuration {
    public:
        std::uint16_t backendServerPort;           // Port with SSH avaliable
        std::filesystem::path databasePath;   // Path to database
        
        /// @brief Function which sets configuration to default values
        void setDefault();

        /// @brief Function which converts Configuration to JSON
        /// @return JSON object
        nlohmann::json toJson() const;

        /// @brief Function which converts JSON to configuration
        /// @param configuration JSON with configuration
        /// @return Confguration object
        static Configuration fromJson(const nlohmann::json& configuration);
    };

    /// @brief Function to load configuration    
    Configuration loadConfiguration(const std::filesystem::path& configPath = "config.json");
    
    /// @brief Function which saves Configuration to JSON file
    /// @param configuration refference to Configuration object
    /// @param configPath path under which save configuration
    void saveConfiguration(const Configuration& configuration, const std::filesystem::path& configPath = "config.json");
}