#include <database-manager.hpp>

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager manager;
    return manager;
}

void DatabaseManager::initialize(const std::filesystem::path& dbPath) {
    std::scoped_lock lock(mtx);
    if (isInitialized) {
        return;
    }

    try {
        // Ensure directory exists
        auto dir = dbPath.parent_path();
        if (!dir.empty()) {
            std::filesystem::create_directories(dir);
        }

        // Open or create database
        db = std::make_shared<SQLite::Database>(
            dbPath.string(), 
            SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE
        );

        isInitialized = true;
    }
    catch (const SQLite::Exception& e) {
        throw std::runtime_error("Failed to open database: " + std::string(e.what()));
    }
    catch (const std::filesystem::filesystem_error& e) {
        throw std::runtime_error("Failed to create database directory: " + std::string(e.what()));
    }
}

std::shared_ptr<SQLite::Database> DatabaseManager::getDatabase() {
    if (!isInitialized) {
        throw std::runtime_error("Database not initialized");
    }
    return db; 
}

std::mutex& DatabaseManager::getMutex() {
    if (!isInitialized) {
        throw std::runtime_error("Database not initialized");
    }
    return mtx; 
}

