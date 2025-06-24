#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <filesystem>
#include <memory>
#include <mutex>

class DatabaseManager {
public:
    static DatabaseManager& getInstance();

    // Deleted copy constructor and assignment operator
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    void initialize(const std::filesystem::path& dbPath);
    std::shared_ptr<SQLite::Database> getDatabase();
    std::mutex& getMutex();

private:
    DatabaseManager() = default;  // Private constructor

    std::shared_ptr<SQLite::Database> db;
    std::mutex mtx;
    bool isInitialized = false;
};