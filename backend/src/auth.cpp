#include <auth.hpp>
#include <Poco/JWT/Token.h>
#include <Poco/JWT/Signer.h>
#include <crypto.hpp>

namespace auth {
    nlohmann::json User::toJson() const {
        return {
            { "id", id },
            { "login", login },
            { "password", password },
            { "name", name },
            { "", surname }
        };
    }
  
    User User::fromJson(const nlohmann::json& user) {
        User us;
        us.id = user.at("id").get<std::uint32_t>();
        us.login = user.at("login").get<std::string>();
        us.password = user.at("password").get<std::string>();
        us.name = user.at("name").get<std::string>();
        us.surname = user.at("surname").get<std::string>();
        return us;
    }

    SQLiteUserRepository::SQLiteUserRepository() {
        try {
            // Open or create database
            db = DatabaseManager::getInstance().getDatabase();

            initializeDatabase();
        }
        catch (const SQLite::Exception& e) {
            throw std::runtime_error("Failed to open database: " + std::string(e.what()));
        }
    }

    void SQLiteUserRepository::initializeDatabase() {
        db->exec(R"(
            CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                login TEXT NOT NULL,
                password TEXT NOT NULL,
                name TEXT NOT NULL,
                surname TEXT NOT NULL
            )
        )");
    }

    SQLiteUserRepository& SQLiteUserRepository::getInstance() {
        static SQLiteUserRepository instance;
        return instance;
    }

    std::list<User> SQLiteUserRepository::getAll() {
        std::lock_guard<std::mutex> lock(DatabaseManager::getInstance().getMutex());
        std::list<User> passwords;
        
        SQLite::Statement query(*db, "SELECT * FROM users");
        while (query.executeStep()) {
            User u;
            u.id = query.getColumn("id").getUInt();
            u.login = query.getColumn("login").getString();
            u.password = query.getColumn("password").getString();
            u.name = query.getColumn("name").getString();
            u.surname = query.getColumn("surname").getString();
            passwords.push_back(u);
        }
        
        return passwords;
    }

    std::optional<User> SQLiteUserRepository::getById(const std::uint32_t& id) {
        std::lock_guard<std::mutex> lock(DatabaseManager::getInstance().getMutex());
        
        SQLite::Statement query(*db, "SELECT * FROM users WHERE id = ?");
        query.bind(1, static_cast<int64_t>(id));
        
        if (query.executeStep()) {
            User u;
            u.id = query.getColumn("id").getUInt();
            u.login = query.getColumn("login").getString();
            u.password = query.getColumn("password").getString();
            u.name = query.getColumn("name").getString();
            u.surname = query.getColumn("surname").getString();
            
            return u;
        }
        
        return std::nullopt;
    }

    void SQLiteUserRepository::add(User& user) {
        std::lock_guard<std::mutex> lock(DatabaseManager::getInstance().getMutex());
        
        SQLite::Statement query(*db, 
            "INSERT INTO users (login, password, name, surname) VALUES (?, ?, ?, ?)");
        
        query.bind(1, user.login);
        query.bind(2, user.password);
        query.bind(3, user.name);
        query.bind(4, user.surname);
        
        query.exec();
        user.id = static_cast<std::uint32_t>(db->getLastInsertRowid());
    }

    void SQLiteUserRepository::update(const User& user) {
        std::lock_guard<std::mutex> lock(DatabaseManager::getInstance().getMutex());
        
        SQLite::Statement query(*db,
            "UPDATE users SET login = ?, password = ?, name = ?, surname = ? WHERE id = ?");
        
        query.bind(1, user.login);
        query.bind(2, user.password);
        query.bind(3, user.name);
        query.bind(4, user.surname);
        query.exec();
    }

    void SQLiteUserRepository::remove(const std::uint32_t id) {
        std::lock_guard<std::mutex> lock(DatabaseManager::getInstance().getMutex());
        
        SQLite::Statement query(*db, "DELETE FROM users WHERE id = ?");
        query.bind(1, static_cast<int64_t>(id));
        query.exec();
    }

    // AuthenticationManager implementation
    AuthenticationManager::AuthenticationManager() : repo(SQLiteUserRepository::getInstance()) {}

    std::list<User> AuthenticationManager::getAllUsers() {
        return repo.getAll();
    }

    std::optional<User> AuthenticationManager::getUserById(const std::uint32_t& id) {
        return repo.getById(id);
    }

    void AuthenticationManager::addUser(User& user) {
        repo.add(user);
    }

    void AuthenticationManager::updateUser(const User& user) {
        repo.update(user);
    }

    void AuthenticationManager::removeUser(const std::uint32_t id) {
        repo.remove(id);
    }

    std::string AuthenticationManager::secretKey;

    void AuthenticationManager::setPrivateKey(const std::string& key) {
        secretKey = key;
    }

    std::optional<User> AuthenticationManager::checkCredentials(const std::string& login, const std::string& password) {
        AuthenticationManager menager;
        auto users = menager.getAllUsers();
        for (const auto& user : users) {
            Crypto crypto(password);
            auto descryptedLogin = crypto.decrypt(user.login);
            auto descryptedPassword = crypto.decrypt(user.password);
            if (login == descryptedLogin && password == descryptedPassword) {
                User decryptedUser;
                decryptedUser.login = descryptedLogin;
                decryptedUser.password = descryptedPassword;
                decryptedUser.name = crypto.decrypt(user.name);
                decryptedUser.surname = crypto.decrypt(user.surname);
                decryptedUser.id = user.id;
                return decryptedUser;
            }
        }
        return std::nullopt;
    }

    std::string AuthenticationManager::generateJWTToken(const User& user) {
        Poco::JWT::Token token;
        token.setType("JWT");
        token.setSubject("login");
        token.payload().set("app", std::string("Password Fucker"));
        token.payload().set("id", std::to_string(user.id));
        token.payload().set("name", user.name);
        token.payload().set("surname", user.surname);
        token.setIssuedAt(Poco::Timestamp());

        Poco::JWT::Signer signer(secretKey);
        return signer.sign(token, Poco::JWT::Signer::ALGO_HS256);
    }

    std::uint32_t AuthenticationManager::validateJWTToken(const std::string& token) {
        Poco::JWT::Signer signer(secretKey);
        Poco::JWT::Token jwt;
        
        if (signer.tryVerify(token, jwt)) {
            return jwt.payload().get("id");
        }
        throw std::runtime_error("Token is not valid.");
    }
}