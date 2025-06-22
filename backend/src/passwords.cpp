// src/passwords.hpp

#include <passwords.hpp>
#include <utilities.hpp>

namespace pass {
    nlohmann::json Password::toJson() const {
        return {
            { "id", id },
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
        pass.login = password.at("login").get<std::string>();
        pass.password = password.at("password").get<std::string>();
        pass.name = password.at("name").get<std::string>();
        pass.url = password.at("url").get<std::string>();
        pass.notes = password.at("notes").get<std::string>();
        pass.createdAt = util::time::fromString(password.at("createdAt").get<std::string>());
        pass.updatedAt = util::time::fromString(password.at("updatedAt").get<std::string>());
        return pass;
    }

    SQLitePasswordRepository::SQLitePasswordRepository(const std::filesystem::path& dbPath) {
        try {
            // Ensure directory exists
            auto dir = dbPath.parent_path();
            if (!dir.empty()) {
                std::filesystem::create_directories(dir);
            }

            // Open or create database
            db = std::make_unique<SQLite::Database>(
                dbPath.string(), 
                SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE
            );

            initializeDatabase();
        }
        catch (const SQLite::Exception& e) {
            throw std::runtime_error("Failed to open database: " + std::string(e.what()));
        }
        catch (const std::filesystem::filesystem_error& e) {
            throw std::runtime_error("Failed to create database directory: " + std::string(e.what()));
        }
    }

    void SQLitePasswordRepository::initializeDatabase() {
        db->exec(R"(
            CREATE TABLE IF NOT EXISTS passwords (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
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

    SQLitePasswordRepository& SQLitePasswordRepository::getInstance(const std::filesystem::path& dbPath) {
        static SQLitePasswordRepository instance(dbPath);
        return instance;
    }

    std::list<Password> SQLitePasswordRepository::getAll() {
        std::lock_guard<std::mutex> lock(mutex);
        std::list<Password> passwords;
        
        SQLite::Statement query(*db, "SELECT * FROM passwords");
        while (query.executeStep()) {
            Password p;
            p.id = query.getColumn("id").getUInt();
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
        std::lock_guard<std::mutex> lock(mutex);
        
        SQLite::Statement query(*db, "SELECT * FROM passwords WHERE id = ?");
        query.bind(1, static_cast<int64_t>(id));
        
        if (query.executeStep()) {
            Password p;
            p.id = query.getColumn("id").getUInt();
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
        std::lock_guard<std::mutex> lock(mutex);
        
        SQLite::Statement query(*db, 
            "INSERT INTO passwords (login, password, name, url, notes, createdAt, updatedAt) "
            "VALUES (?, ?, ?, ?, ?, ?, ?)");
        
        auto now = std::chrono::system_clock::now();
        password.createdAt = now;
        password.updatedAt = now;
        
        query.bind(1, password.login);
        query.bind(2, password.password);
        query.bind(3, password.name);
        query.bind(4, password.url);
        query.bind(5, password.notes);
        query.bind(6, util::time::toString(password.createdAt));
        query.bind(7, util::time::toString(password.updatedAt));
        
        query.exec();
        password.id = static_cast<std::uint32_t>(db->getLastInsertRowid());
    }

    void SQLitePasswordRepository::update(const Password& password) {
        std::lock_guard<std::mutex> lock(mutex);
        
        SQLite::Statement query(*db,
            "UPDATE passwords SET login = ?, password = ?, name = ?, "
            "url = ?, notes = ?, updatedAt = ? WHERE id = ?");
        
        auto now = std::chrono::system_clock::now();
        
        query.bind(1, password.login);
        query.bind(2, password.password);
        query.bind(3, password.name);
        query.bind(4, password.url);
        query.bind(5, password.notes);
        query.bind(6, util::time::toString(now));
        query.bind(7, static_cast<int64_t>(password.id));
        
        query.exec();
    }

    void SQLitePasswordRepository::remove(const std::uint32_t id) {
        std::lock_guard<std::mutex> lock(mutex);
        
        SQLite::Statement query(*db, "DELETE FROM passwords WHERE id = ?");
        query.bind(1, static_cast<int64_t>(id));
        query.exec();
    }

    std::filesystem::path PasswordManager::dbPath;

    void PasswordManager::initialize(const std::filesystem::path& databasePath) {
        dbPath = databasePath;
    }

    // PasswordManager implementation
    PasswordManager::PasswordManager()
        : repo(SQLitePasswordRepository::getInstance(dbPath)) {}

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