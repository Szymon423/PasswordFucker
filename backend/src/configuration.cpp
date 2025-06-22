#include "Configuration.hpp"
#include <fstream>
#include <stdexcept>
#include <format>

namespace config {

    void Configuration::setDefault() {
        backendServerPort = 1234;
        databasePath = "./definitely-not-password.db";
    }

    nlohmann::json Configuration::toJson() const {
        return nlohmann::json{
            {"backendServerPort", backendServerPort},
            {"databasePath", databasePath.string()}
        };
    }

    Configuration Configuration::fromJson(const nlohmann::json& configuration) {
        Configuration config;
        try {
            config.backendServerPort = configuration.at("backendServerPort").get<std::uint16_t>();
            config.databasePath = configuration.at("databasePath").get<std::string>();
        }
        catch (const nlohmann::json::exception& e) {
            throw std::runtime_error(std::format("Failed to parse configuration: {}", e.what()));
        }
        return config;
    }

    Configuration loadConfiguration(const std::filesystem::path& configPath) {
        if (!std::filesystem::exists(configPath)) {
            throw std::runtime_error(std::format("Configuration file does not exist: {}", configPath.string()));
        }

        try {
            std::ifstream configFile(configPath);
            if (!configFile.is_open()) {
                throw std::runtime_error(std::format("Unable to open configuration file: {}", configPath.string()));
            }

            nlohmann::json jsonConfig;
            configFile >> jsonConfig;
            return Configuration::fromJson(jsonConfig);
        }
        catch (const nlohmann::json::parse_error& e) {
            throw std::runtime_error(std::format("Failed to parse configuration file: {}", e.what()));
        }
        catch (const std::exception& e) {
            throw std::runtime_error(std::format("Error loading configuration: {}", e.what()));
        }
    }

    void saveConfiguration(const Configuration& configuration, const std::filesystem::path& configPath) {
        try {
            std::filesystem::path parentPath = configPath.parent_path();
            if (!parentPath.empty() && !std::filesystem::exists(parentPath)) {
                std::filesystem::create_directories(parentPath);
            }

            std::ofstream configFile(configPath);
            if (!configFile.is_open()) {
                throw std::runtime_error(std::format("Unable to open file for writing: {}", configPath.string()));
            }

            nlohmann::json jsonConfig = configuration.toJson();
            configFile << jsonConfig.dump(4); // Pretty print with 4 spaces indentation
        }
        catch (const std::exception& e) {
            throw std::runtime_error(std::format("Error saving configuration: {}", e.what()));
        }
    }

} // namespace config
