// src/passwords.hpp

#include <passwords.hpp>
#include <utilities.hpp>
#include <database-manager.hpp>
#include <tuple>
#include <random>

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

    nlohmann::json PasswordGenerator::Options::toJson() const {
        return {
            { "minimalLength", minimalLength },
            { "includeUppercase", includeUppercase },
            { "includeLowercase", includeLowercase },
            { "includeDigits", includeDigits },
            { "includeSpecialCharacters", includeSpecialCharacters },
            { "uppercaseMinimalNumber", uppercaseMinimalNumber },
            { "lowercaseMinimalNumber", lowercaseMinimalNumber },
            { "digitsMinimalNumber", digitsMinimalNumber },
            { "specialCharactersMinimalNumber", specialCharactersMinimalNumber },
            { "forbiddenCharacters", forbiddenCharacters }
        };
    }

    PasswordGenerator::Options PasswordGenerator::Options::fromJson(const nlohmann::json& options) {
        PasswordGenerator::Options opt;
        opt.minimalLength = options.value("minimalLength", 12);
        opt.includeUppercase = options.value("includeUppercase", true);
        opt.includeLowercase = options.value("includeLowercase", true);
        opt.includeDigits = options.value("includeDigits", true);
        opt.includeSpecialCharacters = options.value("includeSpecialCharacters", true); 
        opt.uppercaseMinimalNumber = options.value("uppercaseMinimalNumber", 1);
        opt.lowercaseMinimalNumber = options.value("lowercaseMinimalNumber", 1);    
        opt.digitsMinimalNumber = options.value("digitsMinimalNumber", 1);
        opt.specialCharactersMinimalNumber = options.value("specialCharactersMinimalNumber", 1);
        opt.forbiddenCharacters = options.value("forbiddenCharacters", "");
        return opt;
    }

    std::string PasswordGenerator::generate(const PasswordGenerator::Options& options) {
        // Define character sets
        static constexpr std::string_view uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        static constexpr std::string_view lowercase = "abcdefghijklmnopqrstuvwxyz";
        static constexpr std::string_view digits = "0123456789";
        static constexpr std::string_view specialCharacters = "!@#$%^&*()-_=+[]{}|;:,.<>?";
        
        // Validate options
        validateOptions(options);

        thread_local std::random_device rd;
        thread_local std::mt19937 gen(rd());

        // Lambda for checing if a character is allowed based on the forbidden characters
        auto checkIfAllowed = [&options](const char c) -> bool {
            return options.forbiddenCharacters.find(c) == std::string::npos;
        };

        std::string uppercaseChars;
        std::string lowercaseChars;
        std::string digitChars;
        std::string specialChars;
        std::array<std::tuple<std::string_view, bool, std::uint8_t, std::string*>, 4> charSets = {
            std::make_tuple(uppercase, options.includeUppercase, options.uppercaseMinimalNumber, &uppercaseChars),
            std::make_tuple(lowercase, options.includeLowercase, options.lowercaseMinimalNumber, &lowercaseChars),
            std::make_tuple(digits, options.includeDigits, options.digitsMinimalNumber, &digitChars),
            std::make_tuple(specialCharacters, options.includeSpecialCharacters, options.specialCharactersMinimalNumber, &specialChars)
        };

        // Prepare final character sets
        std::vector<std::string> finalCharsSets;

        // Check if at least one character set is included
        for (const auto& [charSet, include, minimalNumber, output] : charSets) {
            if (include) {
                bool hasAllowedChars = false;
                for (const char c : charSet) {
                    if (checkIfAllowed(c)) {
                        hasAllowedChars = true;
                        break;
                    }
                }
                
                // If no allowed characters, throw an error
                // This ensures that if a character set is included, there must be at least one allowed character
                if (!hasAllowedChars) {
                    throw std::runtime_error("No allowed characters found in the selected character set.");
                }
                
                // Characters which will be included in the password
                std::uniform_int_distribution<> charDist(0, charSet.size() - 1);
                for (std::uint8_t i = 0; i < minimalNumber; ++i) {
                    auto selectedChar = charSet[charDist(gen)];
                    std::size_t counter = 0;
                    while (!checkIfAllowed(selectedChar)) {
                        selectedChar = charSet[charDist(gen)];
                        if (++counter > 100) {
                            throw std::runtime_error("Failed to find an allowed character after 100 attempts.");
                        }
                    }
                    *output += selectedChar;
                }

                // Add the output to the final character sets
                if (!output->empty()) {
                    finalCharsSets.push_back(*output);
                }
            }
        }

        // Compose password
        std::string password;
        password.reserve(options.minimalLength);

        // Najpierw dodaj wymagane minimalne liczby znaków
        for (const auto& charSet : finalCharsSets) {
            password += charSet;
        }

        // Dopełnij do wymaganej długości losowymi znakami
        std::string allAllowedChars;
        for (const auto& charSet : finalCharsSets) {
            allAllowedChars += charSet;
        }

        std::uniform_int_distribution<> distrib(0, allAllowedChars.size() - 1);
        while (password.length() < options.minimalLength) {
            password += allAllowedChars[distrib(gen)];
        }

        // Mieszanie hasła
        std::shuffle(password.begin(), password.end(), gen);

        return password;
    }

    void PasswordGenerator::validateOptions(const PasswordGenerator::Options& options) {
        // Check if at least one character set is selected
        if (!options.includeUppercase && !options.includeLowercase && 
            !options.includeDigits && !options.includeSpecialCharacters) {
            throw std::invalid_argument("At least one character set must be selected");
        }

        // Check if minimal length is valid
        uint8_t requiredLength = options.uppercaseMinimalNumber + options.lowercaseMinimalNumber +
            options.digitsMinimalNumber + options.specialCharactersMinimalNumber;
    
        if (options.minimalLength < requiredLength) {
            throw std::invalid_argument("Minimal length is less than sum of required characters");
        }

        // Check consistency of minimal numbers
        if (options.uppercaseMinimalNumber > 0 && !options.includeUppercase ||
            options.lowercaseMinimalNumber > 0 && !options.includeLowercase ||
            options.digitsMinimalNumber > 0 && !options.includeDigits ||
            options.specialCharactersMinimalNumber > 0 && !options.includeSpecialCharacters) {
            throw std::invalid_argument("Inconsistent character requirements");
        }
    }
}