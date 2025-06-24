// src/passwords.hpp

#include <passwords.hpp>
#include <utilities.hpp>
#include <database-manager.hpp>

namespace pass {
    nlohmann::json Password::toJson() const {
        return {
            { "id", id },
            { "userId", userId },
            { "login", login },
            { "password", password },
            { "name", name },
            { "url", url },
            { "notes", notes },
            { "createdAt", util::time::toString(createdAt) },
            { "updatedAt", util::time::toString(updatedAt) }
        };
    }
  
    Password Password::fromJson(const nlohmann::json& password) {
        Password pass;
        pass.id = password.at("id").get<std::uint32_t>();
        pass.userId = password.at("userId").get<std::uint32_t>();
        pass.login = password.at("login").get<std::string>();
        pass.password = password.at("password").get<std::string>();
        pass.name = password.at("name").get<std::string>();
        pass.url = password.at("url").get<std::string>();
        pass.notes = password.at("notes").get<std::string>();
        try {
            pass.createdAt = util::time::fromString(password.at("createdAt").get<std::string>());
        }
        catch (const std::runtime_error& e) {
            pass.createdAt = std::chrono::system_clock::now();
        }
        try {
            pass.updatedAt = util::time::fromString(password.at("updatedAt").get<std::string>());
        }
        catch (const std::runtime_error& e) {
            pass.updatedAt = std::chrono::system_clock::now();
        }
        return pass;
    }

    SQLitePasswordRepository::SQLitePasswordRepository() {
        try {
            // Open or create database
            db = DatabaseManager::getInstance().getDatabase();

            initializeDatabase();
        }
        catch (const SQLite::Exception& e) {
            throw std::runtime_error("Failed to open database: " + std::string(e.what()));
        }
    }

    void SQLitePasswordRepository::initializeDatabase() {
        db->exec(R"(
            CREATE TABLE IF NOT EXISTS passwords (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                userId INTEGER NOT NULL,
                login TEXT NOT NULL,
                password TEXT NOT NULL,
                name TEXT NOT NULL,
                url TEXT,
                notes TEXT,
                createdAt TEXT NOT NULL,
                updatedAt TEXT NOT NULL
            )
        )");
    }

    SQLitePasswordRepository& SQLitePasswordRepository::getInstance() {
        static SQLitePasswordRepository instance;
        return instance;
    }

    std::list<Password> SQLitePasswordRepository::getAll() {
        std::lock_guard<std::mutex> lock(DatabaseManager::getInstance().getMutex());
        std::list<Password> passwords;
        
        SQLite::Statement query(*db, "SELECT * FROM passwords");
        while (query.executeStep()) {
            Password p;
            p.id = query.getColumn("id").getUInt();
            p.userId = query.getColumn("userId").getUInt();
            p.login = query.getColumn("login").getString();
            p.password = query.getColumn("password").getString();
            p.name = query.getColumn("name").getString();
            p.url = query.getColumn("url").getString();
            p.notes = query.getColumn("notes").getString();
            p.createdAt = util::time::fromString(query.getColumn("createdAt").getString());
            p.updatedAt = util::time::fromString(query.getColumn("updatedAt").getString());
            passwords.push_back(p);
        }
        
        return passwords;
    }

    std::optional<Password> SQLitePasswordRepository::getById(const std::uint32_t& id) {
        std::lock_guard<std::mutex> lock(DatabaseManager::getInstance().getMutex());
        
        SQLite::Statement query(*db, "SELECT * FROM passwords WHERE id = ?");
        query.bind(1, static_cast<int64_t>(id));
        
        if (query.executeStep()) {
            Password p;
            p.id = query.getColumn("id").getUInt();
            p.userId = query.getColumn("userId").getUInt();
            p.login = query.getColumn("login").getString();
            p.password = query.getColumn("password").getString();
            p.name = query.getColumn("name").getString();
            p.url = query.getColumn("url").getString();
            p.notes = query.getColumn("notes").getString();
            p.createdAt = util::time::fromString(query.getColumn("createdAt").getString());
            p.updatedAt = util::time::fromString(query.getColumn("updatedAt").getString());
            
            return p;
        }
        
        return std::nullopt;
    }

    void SQLitePasswordRepository::add(Password& password) {
        std::lock_guard<std::mutex> lock(DatabaseManager::getInstance().getMutex());
        
        SQLite::Statement query(*db, 
            "INSERT INTO passwords (login, userId, password, name, url, notes, createdAt, updatedAt) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        
        auto now = std::chrono::system_clock::now();
        password.createdAt = now;
        password.updatedAt = now;
        
        query.bind(1, password.login);
        query.bind(2, password.userId);
        query.bind(3, password.password);
        query.bind(4, password.name);
        query.bind(5, password.url);
        query.bind(6, password.notes);
        query.bind(7, util::time::toString(password.createdAt));
        query.bind(8, util::time::toString(password.updatedAt));
        
        query.exec();
        password.id = static_cast<std::uint32_t>(db->getLastInsertRowid());
    }

    void SQLitePasswordRepository::update(const Password& password) {
        std::lock_guard<std::mutex> lock(DatabaseManager::getInstance().getMutex());
        
        SQLite::Statement query(*db,
            "UPDATE passwords SET login = ?, userId = ?, password = ?, name = ?, "
            "url = ?, notes = ?, updatedAt = ? WHERE id = ?");
        
        auto now = std::chrono::system_clock::now();
        
        query.bind(1, password.login);
        query.bind(2, password.userId);
        query.bind(3, password.password);
        query.bind(4, password.name);
        query.bind(5, password.url);
        query.bind(6, password.notes);
        query.bind(7, util::time::toString(now));
        query.bind(8, static_cast<int64_t>(password.id));
        
        query.exec();
    }

    void SQLitePasswordRepository::remove(const std::uint32_t id) {
        std::lock_guard<std::mutex> lock(DatabaseManager::getInstance().getMutex());
        
        SQLite::Statement query(*db, "DELETE FROM passwords WHERE id = ?");
        query.bind(1, static_cast<int64_t>(id));
        query.exec();
    }

    std::filesystem::path PasswordManager::dbPath;

    // PasswordManager implementation
    PasswordManager::PasswordManager() : repo(SQLitePasswordRepository::getInstance()) {}

    std::list<Password> PasswordManager::getAllPasswords() {
        return repo.getAll();
    }

    std::optional<Password> PasswordManager::getPasswordById(const std::uint32_t& id) {
        return repo.getById(id);
    }

    void PasswordManager::addPassword(Password& password) {
        repo.add(password);
    }

    void PasswordManager::updatePassword(const Password& password) {
        repo.update(password);
    }

    void PasswordManager::removePassword(const std::uint32_t id) {
        repo.remove(id);
    }
}